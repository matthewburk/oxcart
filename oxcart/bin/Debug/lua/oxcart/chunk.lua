local ffi = require 'ffi'
local gl = require 'opengl'
local bit = require 'bit'
require 'oxcart.geometry'
require 'oxcart.program'
local C = ffi.C

require 'strict'

local math = require('oxcart.math')
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

  vec3 gamma(vec3 color){
      return pow(color, vec3(1.0/1.8));
  }
  

  void main()
  {
    vec3 ambient = l_ambientcoefficient * vs_color.rgb * intensity.rgb;

    float diffusecoefficent = max(0.0, dot(vs_normal, surfacetolight.xyz));
    vec3 diffuse = diffusecoefficent * vs_color.rgb * intensity.rgb;

    fs_color = vec4(gamma(ambient+diffuse), vs_color.a);
  }
  ]],
}

local chunktypes = {}

ffi.cdef[[
typedef struct block {
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t a;
  int8_t value; 
  uint8_t near; //l, r, t, b, f, k
  uint8_t corners; //ltf, rtf, lbf, rbf, ltk, rtk, lbk, rbk 
  uint8_t reserve;
} block_t;
]]

local block_t = ffi.typeof('block_t')

local function makechunktype(size)
  local col_t = ffi.typeof('block_t[$]', size+2)
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

    self.self = grid[icol][iblock].value
    self.top = grid[icol][iblock+1].value
    self.bottom = grid[icol][iblock-1].value
    self.left = gridl[icol][iblock].value
    self.right = gridr[icol][iblock].value
    self.front = grid[icol+1][iblock].value
    self.back = grid[icol-1][iblock].value
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
    self.block[0] = chunk[igrid+1][icol+1][iblock].value
    self.block[1] = chunk[igrid+1][icol  ][iblock].value
    self.block[2] = chunk[igrid+1][icol-1][iblock].value
    self.block[3] = chunk[igrid  ][icol-1][iblock].value
    self.block[4] = chunk[igrid-1][icol-1][iblock].value
    self.block[5] = chunk[igrid-1][icol  ][iblock].value
    self.block[6] = chunk[igrid-1][icol+1][iblock].value
    self.block[7] = chunk[igrid  ][icol+1][iblock].value
  end

  function mt:samplechunkbottom(chunk, igrid, icol, iblock)
    iblock = iblock-1
    self.block[0] = chunk[igrid+1][icol-1][iblock].value
    self.block[1] = chunk[igrid+1][icol  ][iblock].value
    self.block[2] = chunk[igrid+1][icol+1][iblock].value
    self.block[3] = chunk[igrid  ][icol+1][iblock].value
    self.block[4] = chunk[igrid-1][icol+1][iblock].value
    self.block[5] = chunk[igrid-1][icol  ][iblock].value
    self.block[6] = chunk[igrid-1][icol-1][iblock].value
    self.block[7] = chunk[igrid  ][icol-1][iblock].value
  end

  function mt:samplechunkleft(chunk, igrid, icol, iblock)
    igrid = igrid-1
    self.block[0] = chunk[igrid][icol+1][iblock-1].value
    self.block[1] = chunk[igrid][icol+1][iblock  ].value
    self.block[2] = chunk[igrid][icol+1][iblock+1].value
    self.block[3] = chunk[igrid][icol  ][iblock+1].value
    self.block[4] = chunk[igrid][icol-1][iblock+1].value
    self.block[5] = chunk[igrid][icol-1][iblock  ].value
    self.block[6] = chunk[igrid][icol-1][iblock-1].value
    self.block[7] = chunk[igrid][icol  ][iblock-1].value
  end

  function mt:samplechunkright(chunk, igrid, icol, iblock)
    igrid = igrid+1
    self.block[0] = chunk[igrid][icol-1][iblock-1].value
    self.block[1] = chunk[igrid][icol-1][iblock  ].value
    self.block[2] = chunk[igrid][icol-1][iblock+1].value
    self.block[3] = chunk[igrid][icol  ][iblock+1].value
    self.block[4] = chunk[igrid][icol+1][iblock+1].value
    self.block[5] = chunk[igrid][icol+1][iblock  ].value
    self.block[6] = chunk[igrid][icol+1][iblock-1].value
    self.block[7] = chunk[igrid][icol  ][iblock-1].value
  end

  function mt:samplechunkfront(chunk, igrid, icol, iblock)
    icol = icol+1
    self.block[0] = chunk[igrid+1][icol][iblock-1].value
    self.block[1] = chunk[igrid+1][icol][iblock  ].value
    self.block[2] = chunk[igrid+1][icol][iblock+1].value
    self.block[3] = chunk[igrid  ][icol][iblock+1].value
    self.block[4] = chunk[igrid-1][icol][iblock+1].value
    self.block[5] = chunk[igrid-1][icol][iblock  ].value
    self.block[6] = chunk[igrid-1][icol][iblock-1].value
    self.block[7] = chunk[igrid  ][icol][iblock-1].value
  end

  function mt:samplechunkback(chunk, igrid, icol, iblock)
    icol = icol-1
    self.block[0] = chunk[igrid-1][icol][iblock-1].value
    self.block[1] = chunk[igrid-1][icol][iblock  ].value
    self.block[2] = chunk[igrid-1][icol][iblock+1].value
    self.block[3] = chunk[igrid  ][icol][iblock+1].value
    self.block[4] = chunk[igrid+1][icol][iblock+1].value
    self.block[5] = chunk[igrid+1][icol][iblock  ].value
    self.block[6] = chunk[igrid+1][icol][iblock-1].value
    self.block[7] = chunk[igrid  ][icol][iblock-1].value
  end

  ffi.metatype('sidesampler_t', mt)
end

ffi.cdef[[typedef struct chunkvertex {
  point3_t position;
  vec3_t normal;
  color4f_t color;
} chunkvertex_t;
]]

do
  local sidesampler = ffi.new('sidesampler_t')
  local r = .5--half vunit 
  local face = ffi.new('chunkvertex_t[4]')
  local indices = ffi.new('GLuint[6]')
  local blockcolor = ffi.new('color4f_t')
  local ao0 = 1
  local ao1 = 1
  local ao2 = 1
  local ao3 = 1


  local function updateindices(i)
    indices[0]=i+0; indices[1]=i+1; indices[2]=i+2
    indices[3]=i+0; indices[4]=i+2; indices[5]=i+3
  end

  local function calcvertex0ao()
    local a, b, c = sidesampler.block[0], sidesampler.block[1], sidesampler.block[7]
    if b == 1 and c ==1 then
      return .25 -- 1-(3*.25)
    else
      return 1-((a+b+c)*.25)
    end
  end

  local function calcvertex1ao()
    local a, b, c = sidesampler.block[2], sidesampler.block[3], sidesampler.block[1]
    if b == 1 and c ==1 then
      return .25 -- 1-(3*.25)
    else
      return 1-((a+b+c)*.25)
    end
  end

  local function calcvertex2ao()
    local a, b, c = sidesampler.block[4], sidesampler.block[5], sidesampler.block[3]
    if b == 1 and c ==1 then
      return .25 -- 1-(3*.25)
    else
      return 1-((a+b+c)*.25)
    end
  end

  local function calcvertex3ao()
    local a, b, c = sidesampler.block[6], sidesampler.block[7], sidesampler.block[5]
    if b == 1 and c ==1 then
      return .25 -- 1-(3*.25)
    else
      return 1-((a+b+c)*.25)
    end
  end

  local function addcolor()
    ao0 = calcvertex0ao() ao1 = calcvertex1ao() ao2 = calcvertex2ao() ao3 = calcvertex3ao()
    face[0].color.r=blockcolor.r*ao0; face[0].color.g=blockcolor.g*ao0; face[0].color.b=blockcolor.b*ao0; face[0].color.a=1
    face[1].color.r=blockcolor.r*ao1; face[1].color.g=blockcolor.g*ao1; face[1].color.b=blockcolor.b*ao1; face[1].color.a=1
    face[2].color.r=blockcolor.r*ao2; face[2].color.g=blockcolor.g*ao2; face[2].color.b=blockcolor.b*ao2; face[2].color.a=1
    face[3].color.r=blockcolor.r*ao3; face[3].color.g=blockcolor.g*ao3; face[3].color.b=blockcolor.b*ao3; face[3].color.a=1
  end

  local function addfront(i, x, y, z)
    face[0].normal.x=0; face[0].normal.y=0; face[0].normal.z=1
    face[1].normal.x=0; face[1].normal.y=0; face[1].normal.z=1
    face[2].normal.x=0; face[2].normal.y=0; face[2].normal.z=1
    face[3].normal.x=0; face[3].normal.y=0; face[3].normal.z=1

    face[0].position.x=x+r; face[0].position.y=y-r; face[0].position.z=z+r
    face[1].position.x=x+r; face[1].position.y=y+r; face[1].position.z=z+r
    face[2].position.x=x-r; face[2].position.y=y+r; face[2].position.z=z+r
    face[3].position.x=x-r; face[3].position.y=y-r; face[3].position.z=z+r
    addcolor()
    updateindices(i)
  end

  local function addback(i, x, y, z)
    face[0].normal.x=0; face[0].normal.y=0; face[0].normal.z=-1
    face[1].normal.x=0; face[1].normal.y=0; face[1].normal.z=-1
    face[2].normal.x=0; face[2].normal.y=0; face[2].normal.z=-1
    face[3].normal.x=0; face[3].normal.y=0; face[3].normal.z=-1

    face[0].position.x=x-r; face[0].position.y=y-r; face[0].position.z=z-r
    face[1].position.x=x-r; face[1].position.y=y+r; face[1].position.z=z-r
    face[2].position.x=x+r; face[2].position.y=y+r; face[2].position.z=z-r
    face[3].position.x=x+r; face[3].position.y=y-r; face[3].position.z=z-r
    addcolor()
    updateindices(i)
  end

  local function addleft(i, x, y, z)
    face[0].normal.x=-1; face[0].normal.y=0; face[0].normal.z=0
    face[1].normal.x=-1; face[1].normal.y=0; face[1].normal.z=0
    face[2].normal.x=-1; face[2].normal.y=0; face[2].normal.z=0
    face[3].normal.x=-1; face[3].normal.y=0; face[3].normal.z=0

    face[0].position.x=x-r; face[0].position.y=y-r; face[0].position.z=z+r
    face[1].position.x=x-r; face[1].position.y=y+r; face[1].position.z=z+r
    face[2].position.x=x-r; face[2].position.y=y+r; face[2].position.z=z-r
    face[3].position.x=x-r; face[3].position.y=y-r; face[3].position.z=z-r
    addcolor()
    updateindices(i)
  end

  local function addright(i, x, y, z)
    face[0].normal.x=1; face[0].normal.y=0; face[0].normal.z=0
    face[1].normal.x=1; face[1].normal.y=0; face[1].normal.z=0
    face[2].normal.x=1; face[2].normal.y=0; face[2].normal.z=0
    face[3].normal.x=1; face[3].normal.y=0; face[3].normal.z=0

    face[0].position.x=x+r; face[0].position.y=y-r; face[0].position.z=z-r
    face[1].position.x=x+r; face[1].position.y=y+r; face[1].position.z=z-r
    face[2].position.x=x+r; face[2].position.y=y+r; face[2].position.z=z+r
    face[3].position.x=x+r; face[3].position.y=y-r; face[3].position.z=z+r
    addcolor()
    updateindices(i)
  end

  local function addtop(i, x, y, z)
    face[0].normal.x=0; face[0].normal.y=1; face[0].normal.z=0
    face[1].normal.x=0; face[1].normal.y=1; face[1].normal.z=0
    face[2].normal.x=0; face[2].normal.y=1; face[2].normal.z=0
    face[3].normal.x=0; face[3].normal.y=1; face[3].normal.z=0

    face[0].position.x=x+r; face[0].position.y=y+r; face[0].position.z=z+r
    face[1].position.x=x+r; face[1].position.y=y+r; face[1].position.z=z-r
    face[2].position.x=x-r; face[2].position.y=y+r; face[2].position.z=z-r
    face[3].position.x=x-r; face[3].position.y=y+r; face[3].position.z=z+r
    addcolor()
    updateindices(i)
  end

  local function addbottom(i, x, y, z)
    face[0].normal.x=0; face[0].normal.y=-1; face[0].normal.z=0
    face[1].normal.x=0; face[1].normal.y=-1; face[1].normal.z=0
    face[2].normal.x=0; face[2].normal.y=-1; face[2].normal.z=0
    face[3].normal.x=0; face[3].normal.y=-1; face[3].normal.z=0

    face[0].position.x=x+r; face[0].position.y=y-r; face[0].position.z=z-r
    face[1].position.x=x+r; face[1].position.y=y-r; face[1].position.z=z+r
    face[2].position.x=x-r; face[2].position.y=y-r; face[2].position.z=z+r
    face[3].position.x=x-r; face[3].position.y=y-r; face[3].position.z=z-r
    addcolor()
    updateindices(i)
  end

  local handles_t = ffi.typeof[[struct {
    GLuint buffers[2];
    GLuint vao;
  }]]

  ffi.metatype(handles_t, {
    __gc = function(self)
      gl.glDeleteBuffers(2, self.buffers)
      gl.glDeleteVertexArray(self.vao)
    end
  })

  local function genbuffers(vertexarray, indicesarray)
    local handles = handles_t()
    local vao = gl.glGenVertexArray()
    handles.vao = vao
    gl.glBindVertexArray(vao)
    
    local arraybuffer = gl.glGenBuffer()
    handles.buffers[0] = arraybuffer

    gl.glBindBuffer(GL_ARRAY_BUFFER, arraybuffer)
    gl.glBufferData(GL_ARRAY_BUFFER, vertexarray:sizeof(), vertexarray:front(), GL_DYNAMIC_DRAW)

    local size = ffi.sizeof('chunkvertex_t')
    do --positions
      gl.glVertexAttribPointer(oxcart.attrib.position, 3, GL_FLOAT, GL_FALSE, size, ffi.cast('void*', ffi.offsetof('chunkvertex_t', 'position')))
      gl.glEnableVertexAttribArray(oxcart.attrib.position)
    end

    do --normals
      gl.glVertexAttribPointer(oxcart.attrib.normal, 3, GL_FLOAT, GL_FALSE, size, ffi.cast('void*', ffi.offsetof('chunkvertex_t', 'normal')))
      gl.glEnableVertexAttribArray(oxcart.attrib.normal)
    end

    do --colors
      gl.glVertexAttribPointer(oxcart.attrib.color, 4, GL_FLOAT, GL_FALSE, size, ffi.cast('void*', ffi.offsetof('chunkvertex_t', 'color')))
      gl.glEnableVertexAttribArray(oxcart.attrib.color)
    end

    local ibo = gl.glGenBuffer()
    handles.buffers[1] = ibo
    gl.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo)
    gl.glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesarray:sizeof(), indicesarray:front(), GL_DYNAMIC_DRAW);

    gl.glBindVertexArray(0)
    gl.glBindBuffer(GL_ARRAY_BUFFER, 0)
    gl.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0)
    
    return handles
  end

  local sampler = ffi.new('voxelsampler_t')

  local nocolor = function()end
  function M.totriangles(chunk)
    local indicesarray = vector['GLuint'](6)
    local vertexarray = vector['chunkvertex_t'](4)

    local data = chunk.data
    local size = chunk.size
    local midpoint = math.ceil(size/2) --assuming a midpoint, can fix by using * 10 value
    local i = 0 --vertex index

    for x = 1, size do
      local cx = x-.5

      for z = 1, size do
        local cz = z-.5

        for y = 1, size do
          local cy = y-.5

          local block = data[x][z][y]

          if block.value ~= 0 then
            blockcolor.r = block.r/255
            blockcolor.g = block.g/255
            blockcolor.b = block.b/255
            blockcolor.a = (255-block.a)/255

            sampler:samplechunk(data, x, z, y)

            if sampler.left == 0 then 
              sidesampler:samplechunkleft(data, x, z, y) 
              addleft(i, cx, cy, cz) 
              indicesarray:pushbackv(indices, 6)
              vertexarray:pushbackv(face, 4)
              i=i+4  
            end
            if sampler.right == 0 then 
              sidesampler:samplechunkright(data, x, z, y) 
              addright(i, cx, cy, cz) 
              indicesarray:pushbackv(indices, 6)
              vertexarray:pushbackv(face, 4)
              i=i+4  
            end
            if sampler.front == 0 then 
              sidesampler:samplechunkfront(data, x, z, y) 
              addfront(i, cx, cy, cz) 
              indicesarray:pushbackv(indices, 6)
              vertexarray:pushbackv(face, 4)
              i=i+4  
            end
            if sampler.back == 0 then
              sidesampler:samplechunkback(data, x, z, y) 
              addback(i, cx, cy, cz) 
              indicesarray:pushbackv(indices, 6)
              vertexarray:pushbackv(face, 4)
              i=i+4  
            end
            if sampler.top == 0 then
              sidesampler:samplechunktop(data, x, z, y) 
              addtop(i, cx, cy, cz)
              indicesarray:pushbackv(indices, 6)
              vertexarray:pushbackv(face, 4)
              i=i+4 
            end
            if sampler.bottom == 0 then 
              sidesampler:samplechunkbottom(data, x, z, y) 
              addbottom(i, cx, cy, cz)
              indicesarray:pushbackv(indices, 6)
              vertexarray:pushbackv(face, 4)
              i=i+4 
            end
          end
        end
      end
    end

    local nelements = indicesarray:size()

    local handles = genbuffers(vertexarray, indicesarray)

    return {
      transform = C.mat4_identity(),
      cdata = handles,
      program = M.program,
      vao = handles.vao,
      nelements = nelements,
    }
  end
end

oxcart.chunk = M

return M
