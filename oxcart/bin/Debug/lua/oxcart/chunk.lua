local ffi = require 'ffi'
local gl = require 'opengl'
local bit = require 'bit'
require 'oxcart.geometry'
require 'oxcart.program'
local C = ffi.C

local perlin = require('oxcart.math').perlin
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
]]

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

do
  local mt = {}
  mt.__index = mt

  function mt:samplechunk(chunk, igrid, icol, iblock)
    local gridl, grid, gridr = chunk[igrid-1], chunk[igrid], chunk[igrid+1]

    self.self = grid[icol][iblock]
    self.top = grid[icol][iblock+1]
    self.bottom = grid[icol][iblock-1]
    self.left = gridl[icol][iblock]
    self.right = gridr[icol][iblock]
    self.back = grid[icol-1][iblock]
    self.front = grid[icol+1][iblock]
  end

  ffi.metatype('voxelsampler_t', mt)
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
  local colors = ffi.new('color4f_t[4]', {
    {0, 1, 0, 1},
    {0, 1, 0, 1},
    {0, 1, 0, 1},
    {0, 1, 0, 1},
  })

  local function updatearrays(i, positions, normals, colors)
    indices[0]=i+0; indices[1]=i+1; indices[2]=i+2
    indices[3]=i+0; indices[4]=i+2; indices[5]=i+3
    indicesarray:pushbackv(indices, 6)
    positionsarray:pushbackv(positions, 4)
    normalsarray:pushbackv(normals, 4)

    --[[
    local c =  math.random()
    colors[0].y =c
    colors[1].y =c
    colors[2].y =c
    colors[3].y =c
    --]]
    colorsarray:pushbackv(colors, 4)
  end

  local normals = ffi.new('vec3_t[4]', { {0,  0,  1,}, {0,  0,  1,}, {0,  0,  1,}, {0,  0,  1,}, })
  local function addfront(i, x, y, z)
    p[0].x=x+r; p[0].y=y-r; p[0].z=z+r
    p[1].x=x+r; p[1].y=y+r; p[1].z=z+r
    p[2].x=x-r; p[2].y=y+r; p[2].z=z+r
    p[3].x=x-r; p[3].y=y-r; p[3].z=z+r

    --[[
    colors[0].r=1; colors[0].g=0; colors[0].b=0;
    colors[1].r=1; colors[1].g=0; colors[1].b=0;
    colors[2].r=1; colors[2].g=0; colors[2].b=0;
    colors[3].r=1; colors[3].g=0; colors[3].b=0;
    --]]
    colors[0].r=0; colors[0].g=1; colors[0].b=0;
    colors[1].r=0; colors[1].g=1; colors[1].b=0;
    colors[2].r=0; colors[2].g=1; colors[2].b=0;
    colors[3].r=0; colors[3].g=1; colors[3].b=0;
    updatearrays(i, p, normals, colors)
  end

  local normals = ffi.new('vec3_t[4]', { {0,  0, -1,}, {0,  0, -1,}, {0,  0, -1,}, {0,  0, -1,}, })
  local function addback(i, x, y, z)
    p[0].x=x-r; p[0].y=y-r; p[0].z=z-r
    p[1].x=x-r; p[1].y=y+r; p[1].z=z-r
    p[2].x=x+r; p[2].y=y+r; p[2].z=z-r
    p[3].x=x+r; p[3].y=y-r; p[3].z=z-r
    --[[
    colors[0].r=0; colors[0].g=0; colors[0].b=1;
    colors[1].r=0; colors[1].g=0; colors[1].b=1;
    colors[2].r=0; colors[2].g=0; colors[2].b=1;
    colors[3].r=0; colors[3].g=0; colors[3].b=1;
    --]]
    colors[0].r=0; colors[0].g=1; colors[0].b=0;
    colors[1].r=0; colors[1].g=1; colors[1].b=0;
    colors[2].r=0; colors[2].g=1; colors[2].b=0;
    colors[3].r=0; colors[3].g=1; colors[3].b=0;
    updatearrays(i, p, normals, colors)
  end

  local normals = ffi.new('vec3_t[4]', { {-1,  0, 0,}, {-1,  0, 0,}, {-1,  0, 0,}, {-1,  0, 0,}, })
  local function addleft(i, x, y, z)
    p[0].x=x-r; p[0].y=y-r; p[0].z=z+r
    p[1].x=x-r; p[1].y=y+r; p[1].z=z+r
    p[2].x=x-r; p[2].y=y+r; p[2].z=z-r
    p[3].x=x-r; p[3].y=y-r; p[3].z=z-r
    --[[
    colors[0].r=1; colors[0].g=1; colors[0].b=0;
    colors[1].r=1; colors[1].g=1; colors[1].b=0;
    colors[2].r=1; colors[2].g=1; colors[2].b=0;
    colors[3].r=1; colors[3].g=1; colors[3].b=0;
    --]]
    colors[0].r=0; colors[0].g=1; colors[0].b=0;
    colors[1].r=0; colors[1].g=1; colors[1].b=0;
    colors[2].r=0; colors[2].g=1; colors[2].b=0;
    colors[3].r=0; colors[3].g=1; colors[3].b=0;
    updatearrays(i, p, normals, colors)
  end

  local normals = ffi.new('vec3_t[4]', { {1,  0,  0,}, {1,  0,  0,}, {1,  0,  0,}, {1,  0,  0,}, })
  local function addright(i, x, y, z)
    p[0].x=x+r; p[0].y=y-r; p[0].z=z-r
    p[1].x=x+r; p[1].y=y+r; p[1].z=z-r
    p[2].x=x+r; p[2].y=y+r; p[2].z=z+r
    p[3].x=x+r; p[3].y=y-r; p[3].z=z+r
    --[[
    colors[0].r=0; colors[0].g=1; colors[0].b=1;
    colors[1].r=0; colors[1].g=1; colors[1].b=1;
    colors[2].r=0; colors[2].g=1; colors[2].b=1;
    colors[3].r=0; colors[3].g=1; colors[3].b=1;
    --]]
    colors[0].r=0; colors[0].g=1; colors[0].b=0;
    colors[1].r=0; colors[1].g=1; colors[1].b=0;
    colors[2].r=0; colors[2].g=1; colors[2].b=0;
    colors[3].r=0; colors[3].g=1; colors[3].b=0;
    updatearrays(i, p, normals, colors)
  end

  local normals = ffi.new('vec3_t[4]', { {0,  1,  0,}, {0,  1,  0,}, {0,  1,  0,}, {0,  1,  0,}, })
  local function addtop(i, x, y, z)
    p[0].x=x+r; p[0].y=y+r; p[0].z=z+r
    p[1].x=x+r; p[1].y=y+r; p[1].z=z-r
    p[2].x=x-r; p[2].y=y+r; p[2].z=z-r
    p[3].x=x-r; p[3].y=y+r; p[3].z=z+r

    local cg = math.random(9, 10)/10
    colors[0].r=0; colors[0].g=cg; colors[0].b=0;
    colors[1].r=0; colors[1].g=cg; colors[1].b=0;
    colors[2].r=0; colors[2].g=cg; colors[2].b=0;
    colors[3].r=0; colors[3].g=cg; colors[3].b=0;
    updatearrays(i, p, normals, colors)
  end

  local normals = ffi.new('vec3_t[4]', { {0, -1,  0,}, {0, -1,  0,}, {0, -1,  0,}, {0, -1,  0,}, })
  local function addbottom(i, x, y, z)
    p[0].x=x+r; p[0].y=y-r; p[0].z=z-r
    p[1].x=x+r; p[1].y=y-r; p[1].z=z+r
    p[2].x=x-r; p[2].y=y-r; p[2].z=z+r
    p[3].x=x-r; p[3].y=y-r; p[3].z=z-r
    --[[
    colors[0].r=1; colors[0].g=0; colors[0].b=1;
    colors[1].r=1; colors[1].g=0; colors[1].b=1;
    colors[2].r=1; colors[2].g=0; colors[2].b=1;
    colors[3].r=1; colors[3].g=0; colors[3].b=1;
    --]]
    colors[0].r=0; colors[0].g=1; colors[0].b=0;
    colors[1].r=0; colors[1].g=1; colors[1].b=0;
    colors[2].r=0; colors[2].g=1; colors[2].b=0;
    colors[3].r=0; colors[3].g=1; colors[3].b=0;
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
        local wz = wz + (z-midpoint)

        for y = 1, size do
          local wy = wy + (y-midpoint)

          sampler:samplechunk(data, x, z, y)

          if sampler.self ~= 0 then
            if sampler.top == 0 then addtop(i, wx, wy, wz); i=i+4 end
            if sampler.bottom == 0 then addbottom(i, wx, wy, wz)  i=i+4 end
            if sampler.left <= 0 then addleft(i, wx, wy, wz) i=i+4  end
            if sampler.right <= 0 then addright(i, wx, wy, wz) i=i+4  end
            if sampler.front <= 0 then addfront(i, wx, wy, wz) i=i+4  end
            if sampler.back <= 0 then addback(i, wx, wy, wz) i=i+4  end

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
