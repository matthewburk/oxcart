local ffi = require 'ffi'
local gl = require 'opengl'
local bit = require 'bit'
require 'oxcart.geometry'
require 'oxcart.program'
local C = ffi.C
local M = {}

function M.createcube(x, y, z)
  local r = .5
  local vertices = ffi.new('point3_t[24]', {
    --front
    x+r, y-r, z+r,
    x+r, y+r, z+r,
    x-r, y+r, z+r,
    x-r, y-r, z+r,
    --left
    x-r, y-r, z+r,
    x-r, y+r, z+r,
    x-r, y+r, z-r,
    x-r, y-r, z-r,
    --back
    x-r, y-r, z-r,
    x-r, y+r, z-r,
    x+r, y+r, z-r,
    x+r, y-r, z-r,
    --right
    x+r, y-r, z-r,
    x+r, y+r, z-r,
    x+r, y+r, z+r,
    x+r, y-r, z+r,
    --top
    x+r, y+r, z+r,
    x+r, y+r, z-r,
    x-r, y+r, z-r,
    x-r, y+r, z+r,
    --bottom
    x+r, y-r, z-r,
    x+r, y-r, z+r,
    x-r, y-r, z+r,
    x-r, y-r, z-r,
  })

  local normals = ffi.new('vec3_t[24]', {
     --front
     0,  0,  1,
     0,  0,  1,
     0,  0,  1,
     0,  0,  1,
     --left
     -1,  0, 0,
     -1,  0, 0,
     -1,  0, 0,
     -1,  0, 0,
     --back
     0,  0, -1,
     0,  0, -1,
     0,  0, -1,
     0,  0, -1,
     --right
     1,  0,  0,
     1,  0,  0,
     1,  0,  0,
     1,  0,  0,
     --top
     0,  1,  0,
     0,  1,  0,
     0,  1,  0,
     0,  1,  0,
     --bottom
     0, -1,  0,
     0, -1,  0,
     0, -1,  0,
     0, -1,  0,
  })

  local indices = ffi.new('GLuint[36]', {
    --front
    0, 1, 2,
    0, 2, 3,
    --left
    0+4, 1+4, 2+4,
    0+4, 2+4, 3+4,
    --back
    0+8, 1+8, 2+8,
    0+8, 2+8, 3+8,
    --right
    0+12, 1+12, 2+12,
    0+12, 2+12, 3+12,
    --top
    0+16, 1+16, 2+16,
    0+16, 2+16, 3+16,
    --bottom
    0+20, 1+20, 2+20,
    0+20, 2+20, 3+20,
  })

  local vao = gl.glGenVertexArray()
  gl.glBindVertexArray(vao)
  
  local arraybuffer = gl.glGenBuffer()
  gl.glBindBuffer(GL_ARRAY_BUFFER, arraybuffer)
  gl.glBufferData(GL_ARRAY_BUFFER, ffi.sizeof(vertices), vertices, GL_STATIC_DRAW)
  gl.glVertexAttribPointer(oxcart.attrib.position, 3, GL_FLOAT, GL_FALSE, 0, nil)
  gl.glEnableVertexAttribArray(oxcart.attrib.position)

  do
    local arraybuffer = gl.glGenBuffer()
    gl.glBindBuffer(GL_ARRAY_BUFFER, arraybuffer)
    gl.glBufferData(GL_ARRAY_BUFFER, ffi.sizeof(normals), normals, GL_STATIC_DRAW)
    gl.glVertexAttribPointer(oxcart.attrib.normal, 3, GL_FLOAT, GL_FALSE, 0, nil)
    gl.glEnableVertexAttribArray(oxcart.attrib.normal)
  end

  local ibo = gl.glGenBuffer()
  gl.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo)
  gl.glBufferData(GL_ELEMENT_ARRAY_BUFFER, ffi.sizeof(indices), indices, GL_STATIC_DRAW);
  
  return {
    vao = vao,
    ibo = ibo,
    nindices = 36,
  }
end


local normals = ffi.new('vec3_t[24]', {
   --front
   {0,  0,  1,},
   {0,  0,  1,},
   {0,  0,  1,},
   {0,  0,  1,},
   --left
   {-1,  0, 0,},
   {-1,  0, 0,},
   {-1,  0, 0,},
   {-1,  0, 0,},
   --back
   {0,  0, -1,},
   {0,  0, -1,},
   {0,  0, -1,},
   {0,  0, -1,},
   --right
   {1,  0,  0,},
   {1,  0,  0,},
   {1,  0,  0,},
   {1,  0,  0,},
   --top
   {0,  1,  0,},
   {0,  1,  0,},
   {0,  1,  0,},
   {0,  1,  0,},
   --bottom
   {0, -1,  0,},
   {0, -1,  0,},
   {0, -1,  0,},
   {0, -1,  0,},
})

local colors = ffi.new('vec4_t[24]', {
   --front
    {0, 1, 0, 1},
    {0, 1, 0, 1},
    {0, 1, 0, 1},
    {0, 1, 0, 1},
    --left
    {0, 1, 0, 1},
    {0, 1, 0, 1},
    {0, 1, 0, 1},
    {0, 1, 0, 1},
    --back
    {0, 1, 0, 1},
    {0, 1, 0, 1},
    {0, 1, 0, 1},
    {0, 1, 0, 1},
    --right
    {0, 1, 0, 1},
    {0, 1, 0, 1},
    {0, 1, 0, 1},
    {0, 1, 0, 1},
    --top
    {0, 1, 0, 1},
    {0, 1, 0, 1},
    {0, 1, 0, 1},
    {0, 1, 0, 1},
    --bottom
    {0, 1, 0, 1},
    {0, 1, 0, 1},
    {0, 1, 0, 1},
    {0, 1, 0, 1},
})

--i is iteration, starts at 0
local function addcube(i, x, y, z)
  local r = 1--half vunit 

  local positions = ffi.new('point3_t[24]', {
    --front
    {x+r, y-r, z+r},
    {x+r, y+r, z+r},
    {x-r, y+r, z+r},
    {x-r, y-r, z+r},
    --left
    {x-r, y-r, z+r},
    {x-r, y+r, z+r},
    {x-r, y+r, z-r},
    {x-r, y-r, z-r},
    --back
    {x-r, y-r, z-r},
    {x-r, y+r, z-r},
    {x+r, y+r, z-r},
    {x+r, y-r, z-r},
    --right
    {x+r, y-r, z-r},
    {x+r, y+r, z-r},
    {x+r, y+r, z+r},
    {x+r, y-r, z+r},
    --top
    {x+r, y+r, z+r},
    {x+r, y+r, z-r},
    {x-r, y+r, z-r},
    {x-r, y+r, z+r},
    --bottom
    {x+r, y-r, z-r},
    {x+r, y-r, z+r},
    {x-r, y-r, z+r},
    {x-r, y-r, z-r},
  })

  i = 24*i 
  local elements = ffi.new('GLuint[36]', {
    --front
    i+0, i+1, i+2,
    i+0, i+2, i+3,
    --left
    i+0+4, i+1+4, i+2+4,
    i+0+4, i+2+4, i+3+4,
    --back
    i+0+8, i+1+8, i+2+8,
    i+0+8, i+2+8, i+3+8,
    --right
    i+0+12, i+1+12, i+2+12,
    i+0+12, i+2+12, i+3+12,
    --top
    i+0+16, i+1+16, i+2+16,
    i+0+16, i+2+16, i+3+16,
    --bottom
    i+0+20, i+1+20, i+2+20,
    i+0+20, i+2+20, i+3+20,
  })

  return positions, normals, colors, elements
end

local perlin = require('oxcart.math').perlin
local vector = require 'oxcart.vector'

local sizeof = ffi.sizeof
local vunit = 2
function M.new(tx, ty, tz)
  local nvoxels = 31*31*31
  local nelements = 0
  local npositions = nvoxels*24
  local nnormals = nvoxels*24

  local elements = vector['GLuint'](36)
  local positions = vector['point3_t'](24)
  local normals = vector['vec3_t'](24)
  local colors = vector['vec4_t'](24)

  local i = 0
  for x = tx-vunit*15, tx+vunit*15, vunit do
    for z = tz-vunit*15, tz+vunit*15, vunit do
      local h = math.max(-5, math.floor(perlin(x/300, z/300)*15))*vunit


      for y = math.max(h, ty-vunit*15), h, vunit do
        local voxelpositions, voxelnormals, voxelcolors, voxelelements = addcube(i, x, y, z)

        positions:pushbackv(voxelpositions, sizeof(voxelpositions)/sizeof('point3_t'))
        normals:pushbackv(voxelnormals, sizeof(voxelnormals)/sizeof('vec3_t'))
        colors:pushbackv(voxelcolors, sizeof(voxelcolors)/sizeof('vec4_t'))
        elements:pushbackv(voxelelements, sizeof(voxelelements)/sizeof('GLuint'))

        i = i+1
        nelements = nelements + 36
      end
    end
  end

  local vao = gl.glGenVertexArray()
  gl.glBindVertexArray(vao)
  
  local arraybuffer = gl.glGenBuffer()
  gl.glBindBuffer(GL_ARRAY_BUFFER, arraybuffer)
  gl.glBufferData(GL_ARRAY_BUFFER, positions:sizeof(), positions:front(), GL_STATIC_DRAW)
  gl.glVertexAttribPointer(oxcart.attrib.position, 3, GL_FLOAT, GL_FALSE, 0, nil)
  gl.glEnableVertexAttribArray(oxcart.attrib.position)

  do --normals
    local arraybuffer = gl.glGenBuffer()
    gl.glBindBuffer(GL_ARRAY_BUFFER, arraybuffer)
    gl.glBufferData(GL_ARRAY_BUFFER, normals:sizeof(), normals:front(), GL_STATIC_DRAW)
    gl.glVertexAttribPointer(oxcart.attrib.normal, 3, GL_FLOAT, GL_FALSE, 0, nil)
    gl.glEnableVertexAttribArray(oxcart.attrib.normal)
  end

  do --colors
    local arraybuffer = gl.glGenBuffer()
    gl.glBindBuffer(GL_ARRAY_BUFFER, arraybuffer)
    gl.glBufferData(GL_ARRAY_BUFFER, colors:sizeof(), colors:front(), GL_STATIC_DRAW)
    gl.glVertexAttribPointer(oxcart.attrib.color, 4, GL_FLOAT, GL_FALSE, 0, nil)
    gl.glEnableVertexAttribArray(oxcart.attrib.color)
  end

  local ibo = gl.glGenBuffer()
  gl.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo)
  gl.glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements:sizeof(), elements:front(), GL_STATIC_DRAW);
 
  local program = oxcart.program.new {
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

  return {
    program = program,
    vao = vao,
    nelements = nelements,
  }
end

oxcart.chunk = M

return M
