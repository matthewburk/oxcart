local ffi = require 'ffi'
local gl = require 'opengl'
local bit = require 'bit'
require 'oxcart.geometry'
require 'oxcart.program'
local C = ffi.C

local math = require('oxcart.math')
local perlin = math.perlin
local vector = require 'oxcart.vector'

local M = {}

M.program = oxcart.program.new {
  oxcart.program.newvertexshader[[
  #version 420 core
  layout(std140) uniform;

  uniform matrices {
    mat4 m_projection;
    mat4 m_view;
    mat4 m_model;
    mat3 m_normaltransform;
  };

  in vec4 position;
  in vec3 normal;
  in vec4 color;

  out vec3 vs_position;
  out vec3 vs_normal;
  out vec4 vs_color;

  void main()
  {
    vs_color = color;
    vs_normal = normalize(mat3(m_model) * normal);
    vs_position = vec3(m_model * position);

    gl_Position = m_projection * m_view * m_model * position;
  }
  ]],

  oxcart.program.newfragmentshader[[
  #version 420 core
  layout(std140) uniform;

  uniform matrices {
    mat4 m_projection;
    mat4 m_view;
    mat4 m_model;
    mat3 m_normaltransform;
  };

  uniform light {
    vec4 surfacetolight; 
    vec4 intensity; 
    float l_ambientcoefficient;
  };

  in vec4 vs_color;
  in vec3 vs_normal;

  out vec4 fs_color;

  void main()
  {
    vec3 ambient = l_ambientcoefficient * vs_color.rgb * intensity.rgb;

    float diffusecoefficent = max(0.0, dot(vs_normal, surfacetolight.xyz));
    vec3 diffuse = diffusecoefficent * vs_color.rgb * intensity.rgb;

    fs_color = vec4(ambient+diffuse, vs_color.a);
  }
  ]],
}



local chunktypes = {}

local function makechunktype(size)
  local col_t = ffi.typeof('int8_t[$]', size+2)
  local grid_t = ffi.typeof('$[$]', col_t, size+2)
  local chunk_t = ffi.typeof('$[$]', grid_t, size+2)

  local mt = {}
  mt.__index = mt
  mt.chunk_t = chunk_t
  mt.grid_t = grid_t
  mt.col_t = col_t
  mt.size = size

  local function new()
    local data = chunk_t()
    ffi.fill(data, ffi.sizeof(chunk_t), -1)

    local chunk = {
      data=data,
    }

    return setmetatable(chunk, mt)
  end

  chunktypes[size] = {
    size = size,
    col_t = col_t,
    grid_t = grid_t,
    chunk_t = chunk_t,
    new = new,
  }

  return chunktypes[size]
end

function M.new(size)
  return (chunktypes[size] or makechunktype(size)).new()
end

ffi.cdef[[
typedef struct voxelsampler {
  int8_t top;
  int8_t bottom;
  int8_t left;
  int8_t right;
  int8_t front;
  int8_t back;
  int8_t self;
  int8_t dunno;
} voxelsampler_t;
]] do

  local mt = {}
  mt.__index = mt

  function mt:samplechunk(chunk, igrid, icol, iblock)
    local gridl, grid, gridr = chunk[igrid-1], chunk[igrid], chunk[igrid+1]

    self.self = grid[icol][iblock]
    self.top = grid[icol][iblock+1]
    self.bottom = grid[icol][iblock-1]
    self.left = gridl[icol][iblock]
    self.right = gridr[icol][iblock]
    self.front = grid[icol-1][iblock]
    self.back = grid[icol+1][iblock]
  end

  ffi.metatype('voxelsampler_t', mt)
end

ffi.cdef[[
typedef struct sidesampler {
  int8_t block[8];
}sidesampler_t;
]] do

  local mt = {}
  mt.__index = mt

  function mt:samplechunktop(chunk, igrid, icol, iblock)
    iblock = iblock+1
    self.block[0] = chunk[igrid+1][icol-1][iblock]
    self.block[1] = chunk[igrid+1][icol  ][iblock]
    self.block[2] = chunk[igrid+1][icol+1][iblock]
    self.block[3] = chunk[igrid  ][icol+1][iblock]
    self.block[4] = chunk[igrid-1][icol+1][iblock]
    self.block[5] = chunk[igrid-1][icol  ][iblock]
    self.block[6] = chunk[igrid-1][icol-1][iblock]
    self.block[7] = chunk[igrid  ][icol-1][iblock]
  end

  function mt:samplechunkbottom(chunk, igrid, icol, iblock)
    iblock = iblock-1
    self.block[0] = chunk[igrid+1][icol+1][iblock]
    self.block[1] = chunk[igrid+1][icol  ][iblock]
    self.block[2] = chunk[igrid+1][icol-1][iblock]
    self.block[3] = chunk[igrid  ][icol-1][iblock]
    self.block[4] = chunk[igrid-1][icol-1][iblock]
    self.block[5] = chunk[igrid-1][icol  ][iblock]
    self.block[6] = chunk[igrid-1][icol+1][iblock]
    self.block[7] = chunk[igrid  ][icol+1][iblock]
  end

  function mt:samplechunkleft(chunk, igrid, icol, iblock)
    igrid = igrid-1
    self.block[0] = chunk[igrid][icol-1][iblock-1]
    self.block[1] = chunk[igrid][icol-1][iblock  ]
    self.block[2] = chunk[igrid][icol-1][iblock+1]
    self.block[3] = chunk[igrid][icol  ][iblock+1]
    self.block[4] = chunk[igrid][icol+1][iblock+1]
    self.block[5] = chunk[igrid][icol+1][iblock  ]
    self.block[6] = chunk[igrid][icol+1][iblock-1]
    self.block[7] = chunk[igrid][icol  ][iblock-1]
  end

  function mt:samplechunkright(chunk, igrid, icol, iblock)
    igrid = igrid+1
    self.block[0] = chunk[igrid][icol+1][iblock-1]
    self.block[1] = chunk[igrid][icol+1][iblock  ]
    self.block[2] = chunk[igrid][icol+1][iblock+1]
    self.block[3] = chunk[igrid][icol  ][iblock+1]
    self.block[4] = chunk[igrid][icol-1][iblock+1]
    self.block[5] = chunk[igrid][icol-1][iblock  ]
    self.block[6] = chunk[igrid][icol-1][iblock-1]
    self.block[7] = chunk[igrid][icol  ][iblock-1]
  end

  function mt:samplechunkfront(chunk, igrid, icol, iblock)
    icol = icol-1
    self.block[0] = chunk[igrid+1][icol][iblock-1]
    self.block[1] = chunk[igrid+1][icol][iblock  ]
    self.block[2] = chunk[igrid+1][icol][iblock+1]
    self.block[3] = chunk[igrid  ][icol][iblock+1]
    self.block[4] = chunk[igrid-1][icol][iblock+1]
    self.block[5] = chunk[igrid-1][icol][iblock  ]
    self.block[6] = chunk[igrid-1][icol][iblock-1]
    self.block[7] = chunk[igrid  ][icol][iblock-1]
  end

  function mt:samplechunkback(chunk, igrid, icol, iblock)
    icol = icol+1
    self.block[0] = chunk[igrid-1][icol][iblock-1]
    self.block[1] = chunk[igrid-1][icol][iblock  ]
    self.block[2] = chunk[igrid-1][icol][iblock+1]
    self.block[3] = chunk[igrid  ][icol][iblock+1]
    self.block[4] = chunk[igrid+1][icol][iblock+1]
    self.block[5] = chunk[igrid+1][icol][iblock  ]
    self.block[6] = chunk[igrid+1][icol][iblock-1]
    self.block[7] = chunk[igrid  ][icol][iblock-1]
  end

  ffi.metatype('sidesampler_t', mt)
end


function M.fillterrain(chunk, wx, wy, wz)
  local data = chunk.data
  local size = chunk.size
  local halfsize = math.floor(size/2)

  for x = 1, size do
    local grid = data[x] 
    local samplex = wx+x
    for z = 1, size do
      local col = grid[z]
      local samplez = wz+z

      local colh = math.floor(size * (perlin(samplex/100, samplez/100)+1)/2)

      for y = 1, colh do
        col[y] = 1
      end
      for y = colh+1, size do
        col[y] = 0
      end
    end
  end
end

do
  local indicesarray = vector['GLuint'](6)
  local positionsarray = vector['point3_t'](4)
  local normalsarray = vector['vec3_t'](4)
  local colorsarray = vector['color4f_t'](4)

  local r = .5--half vunit 
  local p = ffi.new('point3_t[4]')
  local indices = ffi.new('GLuint[6]')
  local blockcolor = ffi.new('color4f_t')
  local colors = ffi.new('color4f_t[4]', { {0, 1, 0, 1}, {0, 1, 0, 1}, {0, 1, 0, 1}, {0, 1, 0, 1}, })

  local function updatearrays(i, positions, normals, colors)
    indices[0]=i+0; indices[1]=i+1; indices[2]=i+2
    indices[3]=i+0; indices[4]=i+2; indices[5]=i+3
    indicesarray:pushbackv(indices, 6)
    positionsarray:pushbackv(positions, 4)
    normalsarray:pushbackv(normals, 4)
    colorsarray:pushbackv(colors, 4)
  end

  local function calcvertex0ao(sidesampler)
    local a, b, c = sidesampler.block[0], sidesampler.block[1], sidesampler.block[7]
    if b == 1 and c ==1 then
      return .25 -- 1-(3*.25)
    else
      return 1-((a+b+c)*.25)
    end
  end

  local function calcvertex1ao(sidesampler)
    local a, b, c = sidesampler.block[2], sidesampler.block[3], sidesampler.block[1]
    if b == 1 and c ==1 then
      return .25 -- 1-(3*.25)
    else
      return 1-((a+b+c)*.25)
    end
  end

  local function calcvertex2ao(sidesampler)
    local a, b, c = sidesampler.block[4], sidesampler.block[5], sidesampler.block[3]
    if b == 1 and c ==1 then
      return .25 -- 1-(3*.25)
    else
      return 1-((a+b+c)*.25)
    end
  end

  local function calcvertex3ao(sidesampler)
    local a, b, c = sidesampler.block[6], sidesampler.block[7], sidesampler.block[5]
    if b == 1 and c ==1 then
      return .25 -- 1-(3*.25)
    else
      return 1-((a+b+c)*.25)
    end
  end

  local normals = ffi.new('vec3_t[4]', { {0,  0,  1,}, {0,  0,  1,}, {0,  0,  1,}, {0,  0,  1,}, })
  local function addfront(i, x, y, z, sidesampler)
    p[0].x=x+r; p[0].y=y-r; p[0].z=z+r
    p[1].x=x+r; p[1].y=y+r; p[1].z=z+r
    p[2].x=x-r; p[2].y=y+r; p[2].z=z+r
    p[3].x=x-r; p[3].y=y-r; p[3].z=z+r

    local ao0 = calcvertex0ao(sidesampler)
    local ao1 = calcvertex1ao(sidesampler)
    local ao2 = calcvertex2ao(sidesampler)
    local ao3 = calcvertex3ao(sidesampler)
    colors[0].r=blockcolor.r*ao0; colors[0].g=blockcolor.g*ao0; colors[0].b=blockcolor.b*ao0;
    colors[1].r=blockcolor.r*ao1; colors[1].g=blockcolor.g*ao1; colors[1].b=blockcolor.b*ao1;
    colors[2].r=blockcolor.r*ao2; colors[2].g=blockcolor.g*ao2; colors[2].b=blockcolor.b*ao2;
    colors[3].r=blockcolor.r*ao3; colors[3].g=blockcolor.g*ao3; colors[3].b=blockcolor.b*ao3;
    
    updatearrays(i, p, normals, colors)
  end

  local normals = ffi.new('vec3_t[4]', { {0,  0, -1,}, {0,  0, -1,}, {0,  0, -1,}, {0,  0, -1,}, })
  local function addback(i, x, y, z, sidesampler)
    p[0].x=x-r; p[0].y=y-r; p[0].z=z-r
    p[1].x=x-r; p[1].y=y+r; p[1].z=z-r
    p[2].x=x+r; p[2].y=y+r; p[2].z=z-r
    p[3].x=x+r; p[3].y=y-r; p[3].z=z-r
    local ao0 = calcvertex0ao(sidesampler)
    local ao1 = calcvertex1ao(sidesampler)
    local ao2 = calcvertex2ao(sidesampler)
    local ao3 = calcvertex3ao(sidesampler)
    colors[0].r=blockcolor.r*ao0; colors[0].g=blockcolor.g*ao0; colors[0].b=blockcolor.b*ao0;
    colors[1].r=blockcolor.r*ao1; colors[1].g=blockcolor.g*ao1; colors[1].b=blockcolor.b*ao1;
    colors[2].r=blockcolor.r*ao2; colors[2].g=blockcolor.g*ao2; colors[2].b=blockcolor.b*ao2;
    colors[3].r=blockcolor.r*ao3; colors[3].g=blockcolor.g*ao3; colors[3].b=blockcolor.b*ao3;
    
    updatearrays(i, p, normals, colors)
  end

  local normals = ffi.new('vec3_t[4]', { {-1,  0, 0,}, {-1,  0, 0,}, {-1,  0, 0,}, {-1,  0, 0,}, })
  local function addleft(i, x, y, z, sidesampler)
    p[0].x=x-r; p[0].y=y-r; p[0].z=z+r
    p[1].x=x-r; p[1].y=y+r; p[1].z=z+r
    p[2].x=x-r; p[2].y=y+r; p[2].z=z-r
    p[3].x=x-r; p[3].y=y-r; p[3].z=z-r
    local ao0 = calcvertex0ao(sidesampler)
    local ao1 = calcvertex1ao(sidesampler)
    local ao2 = calcvertex2ao(sidesampler)
    local ao3 = calcvertex3ao(sidesampler)
    colors[0].r=blockcolor.r*ao0; colors[0].g=blockcolor.g*ao0; colors[0].b=blockcolor.b*ao0;
    colors[1].r=blockcolor.r*ao1; colors[1].g=blockcolor.g*ao1; colors[1].b=blockcolor.b*ao1;
    colors[2].r=blockcolor.r*ao2; colors[2].g=blockcolor.g*ao2; colors[2].b=blockcolor.b*ao2;
    colors[3].r=blockcolor.r*ao3; colors[3].g=blockcolor.g*ao3; colors[3].b=blockcolor.b*ao3;
    
    updatearrays(i, p, normals, colors)
  end

  local normals = ffi.new('vec3_t[4]', { {1,  0,  0,}, {1,  0,  0,}, {1,  0,  0,}, {1,  0,  0,}, })
  local function addright(i, x, y, z, sidesampler)
    p[0].x=x+r; p[0].y=y-r; p[0].z=z-r
    p[1].x=x+r; p[1].y=y+r; p[1].z=z-r
    p[2].x=x+r; p[2].y=y+r; p[2].z=z+r
    p[3].x=x+r; p[3].y=y-r; p[3].z=z+r
    local ao0 = calcvertex0ao(sidesampler)
    local ao1 = calcvertex1ao(sidesampler)
    local ao2 = calcvertex2ao(sidesampler)
    local ao3 = calcvertex3ao(sidesampler)
    colors[0].r=blockcolor.r*ao0; colors[0].g=blockcolor.g*ao0; colors[0].b=blockcolor.b*ao0;
    colors[1].r=blockcolor.r*ao1; colors[1].g=blockcolor.g*ao1; colors[1].b=blockcolor.b*ao1;
    colors[2].r=blockcolor.r*ao2; colors[2].g=blockcolor.g*ao2; colors[2].b=blockcolor.b*ao2;
    colors[3].r=blockcolor.r*ao3; colors[3].g=blockcolor.g*ao3; colors[3].b=blockcolor.b*ao3;
    
    updatearrays(i, p, normals, colors)
  end

  local normals = ffi.new('vec3_t[4]', { {0,  1,  0,}, {0,  1,  0,}, {0,  1,  0,}, {0,  1,  0,}, })
  local function addtop(i, x, y, z, sidesampler)
    p[0].x=x+r; p[0].y=y+r; p[0].z=z+r
    p[1].x=x+r; p[1].y=y+r; p[1].z=z-r
    p[2].x=x-r; p[2].y=y+r; p[2].z=z-r
    p[3].x=x-r; p[3].y=y+r; p[3].z=z+r
    local ao0 = calcvertex0ao(sidesampler)
    local ao1 = calcvertex1ao(sidesampler)
    local ao2 = calcvertex2ao(sidesampler)
    local ao3 = calcvertex3ao(sidesampler)
    colors[0].r=blockcolor.r*ao0; colors[0].g=blockcolor.g*ao0; colors[0].b=blockcolor.b*ao0;
    colors[1].r=blockcolor.r*ao1; colors[1].g=blockcolor.g*ao1; colors[1].b=blockcolor.b*ao1;
    colors[2].r=blockcolor.r*ao2; colors[2].g=blockcolor.g*ao2; colors[2].b=blockcolor.b*ao2;
    colors[3].r=blockcolor.r*ao3; colors[3].g=blockcolor.g*ao3; colors[3].b=blockcolor.b*ao3;
    

    updatearrays(i, p, normals, colors)
  end

  local normals = ffi.new('vec3_t[4]', { {0, -1,  0,}, {0, -1,  0,}, {0, -1,  0,}, {0, -1,  0,}, })
  local function addbottom(i, x, y, z, sidesampler)
    p[0].x=x+r; p[0].y=y-r; p[0].z=z-r
    p[1].x=x+r; p[1].y=y-r; p[1].z=z+r
    p[2].x=x-r; p[2].y=y-r; p[2].z=z+r
    p[3].x=x-r; p[3].y=y-r; p[3].z=z-r
    local ao0 = calcvertex0ao(sidesampler)
    local ao1 = calcvertex1ao(sidesampler)
    local ao2 = calcvertex2ao(sidesampler)
    local ao3 = calcvertex3ao(sidesampler)
    colors[0].r=blockcolor.r*ao0; colors[0].g=blockcolor.g*ao0; colors[0].b=blockcolor.b*ao0;
    colors[1].r=blockcolor.r*ao1; colors[1].g=blockcolor.g*ao1; colors[1].b=blockcolor.b*ao1;
    colors[2].r=blockcolor.r*ao2; colors[2].g=blockcolor.g*ao2; colors[2].b=blockcolor.b*ao2;
    colors[3].r=blockcolor.r*ao3; colors[3].g=blockcolor.g*ao3; colors[3].b=blockcolor.b*ao3;
    
    updatearrays(i, p, normals, colors)
  end

  local function genbuffers()
    local vao = gl.glGenVertexArray()
    gl.glBindVertexArray(vao)
    
    local arraybuffer = gl.glGenBuffer()
    gl.glBindBuffer(GL_ARRAY_BUFFER, arraybuffer)
    gl.glBufferData(GL_ARRAY_BUFFER, positionsarray:sizeof(), positionsarray:front(), GL_STATIC_DRAW)
    gl.glVertexAttribPointer(oxcart.attrib.position, 3, GL_FLOAT, GL_FALSE, 0, nil)
    gl.glEnableVertexAttribArray(oxcart.attrib.position)

    do --normals
      local arraybuffer = gl.glGenBuffer()
      gl.glBindBuffer(GL_ARRAY_BUFFER, arraybuffer)
      gl.glBufferData(GL_ARRAY_BUFFER, normalsarray:sizeof(), normalsarray:front(), GL_STATIC_DRAW)
      gl.glVertexAttribPointer(oxcart.attrib.normal, 3, GL_FLOAT, GL_FALSE, 0, nil)
      gl.glEnableVertexAttribArray(oxcart.attrib.normal)
    end

    do --colors
      local arraybuffer = gl.glGenBuffer()
      gl.glBindBuffer(GL_ARRAY_BUFFER, arraybuffer)
      gl.glBufferData(GL_ARRAY_BUFFER, colorsarray:sizeof(), colorsarray:front(), GL_STATIC_DRAW)
      gl.glVertexAttribPointer(oxcart.attrib.color, 4, GL_FLOAT, GL_FALSE, 0, nil)
      gl.glEnableVertexAttribArray(oxcart.attrib.color)
    end

    local ibo = gl.glGenBuffer()
    gl.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo)
    gl.glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesarray:sizeof(), indicesarray:front(), GL_STATIC_DRAW);

    return vao, ibo
  end

  local sampler = ffi.new('voxelsampler_t')
  local sidesampler = ffi.new('sidesampler_t')

  function M.totriangles(chunk, wx, wy, wz)
    indicesarray:clear()
    positionsarray:clear()
    normalsarray:clear()
    colorsarray:clear()

    local data = chunk.data
    local size = chunk.size
    local midpoint = math.ceil(size/2) --assuming a midpoint, can fix by using * 10 value
    local i = 0 --vertex index

    for x = 1, size do
      local wx = wx + (x-midpoint)

      for z = 1, size do
        local wz = -(wz + (z-midpoint)) --start at +z count forward to -z

        local perlin = math.perlin((1000+wx)/300, (1000+wz)/100)
        local cg = math.lerp(.9, 1, perlin) 
        local cr = math.lerp(.3, .5, perlin)
        local cb = math.lerp(.3, .1, perlin)

        blockcolor.r=cr; blockcolor.g=cg; blockcolor.b=cb;

        for y = 1, size do
          local wy = wy + (y-midpoint)

          sampler:samplechunk(data, x, z, y)

          if sampler.self ~= 0 then
            if sampler.left <= 0 then 
              sidesampler:samplechunkleft(data, x, z, y) 
              addleft(i, wx, wy, wz, sidesampler) 
              i=i+4  
            end
            if sampler.right <= 0 then 
              sidesampler:samplechunkright(data, x, z, y) 
              addright(i, wx, wy, wz, sidesampler) 
              i=i+4  
            end
            if sampler.front <= 0 then 
              sidesampler:samplechunkfront(data, x, z, y) 
              addfront(i, wx, wy, wz, sidesampler) 
              i=i+4  
            end
            if sampler.back <= 0 then
              sidesampler:samplechunkback(data, x, z, y) 
              addback(i, wx, wy, wz, sidesampler) 
              i=i+4  
            end
            if sampler.top == 0 then
              sidesampler:samplechunktop(data, x, z, y) 
              addtop(i, wx, wy, wz, sidesampler)
              i=i+4 
            end
            if sampler.bottom == 0 then 
              sidesampler:samplechunkbottom(data, x, z, y) 
              addbottom(i, wx, wy, wz, sidesampler)
              i=i+4 
            end
          end
        end
      end
    end

    local nelements = indicesarray:size()

    local vao, ibo = genbuffers()

    return {
      program = M.program,
      vao = vao,
      nelements = nelements,
    }

  end
end

oxcart.chunk = M

return M
