require 'strict'
require 'oxcart.graphics'
require 'oxcart.geometry'
require 'oxcart.program'
require 'oxcart.font'
local vector = require 'oxcart.vector'
local cel = require 'cel'
local ffi = require 'ffi'
local gl = require 'opengl'
local C = ffi.C
local sizeof, offsetof, copy = ffi.sizeof, ffi.offsetof, ffi.copy

local crc = { --cel render context
  metadescription = false,
}

local vertex_t = ffi.typeof[[ 
struct {
  vec3_t position;
  vec2_t texcoord;
  color4f_t color;
}]]

local color = ffi.new('color4f_t', 1, 1, 1, 1)
local textcolor = ffi.new('color4f_t', 1, 1, 1, 1)

function crc.setcolor(celcolor)
  if not celcolor then error('nil color') end
  if celcolor ~= 'current' then
    local bytes = ffi.cast('uint8_t*', celcolor)
    color.r = bytes[0]/255
    color.g = bytes[1]/255
    color.b = bytes[2]/255
    color.a = bytes[3]/255
  end
end

function crc.settextcolor(celcolor)
  if not celcolor then error('nil color') end
  if celcolor ~= 'current' then
    local bytes = ffi.cast('uint8_t*', celcolor)
    textcolor.r = bytes[0]/255
    textcolor.g = bytes[1]/255
    textcolor.b = bytes[2]/255
    textcolor.a = bytes[3]/255
  end
end

do --crc.addtext crc.drawtext
  local vao = gl.glGenVertexArray()
  local vbo = gl.glGenBuffer()
  local ibo = gl.glGenBuffer()
  local tex = gl.glGenTexture()
  local atlas = oxcart.font.atlas
  local vcapacity = 128 * 4
  local icapacity = 128 * 6
  local vertices = vector[vertex_t](vcapacity)
  local indices = vector['GLuint'](icapacity)

  gl.glBindVertexArray(vao)

  gl.glBindBuffer(GL_ARRAY_BUFFER, vbo)
  gl.glBufferData(GL_ARRAY_BUFFER, vertices:sizeofcapacity(), nil, GL_DYNAMIC_DRAW)

  gl.glVertexAttribPointer(oxcart.attrib.position, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), ffi.cast('void*', offsetof(vertex_t, 'position')))
  gl.glEnableVertexAttribArray(oxcart.attrib.position)
  gl.glVertexAttribPointer(oxcart.attrib.texcoord, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_t),  ffi.cast('void*', offsetof(vertex_t, 'texcoord')))
  gl.glEnableVertexAttribArray(oxcart.attrib.texcoord)
  gl.glVertexAttribPointer(oxcart.attrib.color, 4, GL_FLOAT, GL_FALSE, sizeof(vertex_t),  ffi.cast('void*', offsetof(vertex_t, 'color')))
  gl.glEnableVertexAttribArray(oxcart.attrib.color)

  gl.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo)
  gl.glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices:sizeofcapacity(), nil, GL_DYNAMIC_DRAW)

  gl.glBindTexture(GL_TEXTURE_2D, tex)
  gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE)
  gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE)
  gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR)
  gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR)

  local atlasw, atlash = atlas:properties()
  gl.glTexStorage2D(GL_TEXTURE_2D, 1, GL_R8, atlasw, atlash)

  local program = oxcart.program.new {
    oxcart.program.newvertexshader[[
    #version 400 core
    layout(std140) uniform;

    uniform matrices {
      mat4 m_projection;
      mat4 m_view;
      mat4 m_model;
    };

    in vec4 position;
    in vec2 texcoord;
    in vec4 color;

    out vec2 vs_texcoord;
    out vec4 vs_color;

    void main()
    {
      gl_Position = m_projection * m_view * m_model * position;
      vs_texcoord = texcoord;
      vs_color = color;
    }
    ]],

    oxcart.program.newfragmentshader[[
    #version 400 core
    layout(std140) uniform;

    uniform sampler2D sampler;

    in vec2 vs_texcoord;
    in vec4 vs_color;

    out vec4 fs_color;

    void main()
    {
      float a = texture(sampler, vs_texcoord).r;
      fs_color = vec4(vs_color.rgb, vs_color.a * a);
    }
    ]],
  }

  gl.glProgramUniform1i(program.id, gl.glGetUniformLocation(program.id, 'sampler'), 0) 

  local font = false

  function crc.setfont(celfont)
    if not celfont.textureuploaded then
      gl.glBindTexture(GL_TEXTURE_2D, tex)
      gl.glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, atlasw, atlash, GL_RED, GL_UNSIGNED_BYTE, atlas:data())
      celfont.textureuploaded = true
    end
    font = celfont.oxcartfont
  end

  do
    local _vertices = vertices
    local _indices = indices

    local vertices = ffi.typeof('$[4]', vertex_t)()
    local indices = ffi.new('GLuint[6]')
    local string_byte = string.byte

    local math_floor = math.floor
    function crc.addtext(text, penx, peny)
      penx = penx or 0 --TODO position baseline better by default
      peny = peny or 0 --TODO position baseline better by default

      local index = _vertices:size()

      for i = 1, #text do
        local byte = string_byte(text, i)
        local w, h, bearingx, bearingy, advance = font:getglyphmetrics(byte)

        do
          local x0 = penx + bearingx
          local y0 = peny + bearingy
          local x1 = x0 + w
          local y1 = y0 + h

          vertices[0].position.x = x0
          vertices[0].position.y = y0
          vertices[1].position.x = x0
          vertices[1].position.y = y1
          vertices[2].position.x = x1
          vertices[2].position.y = y1
          vertices[3].position.x = x1
          vertices[3].position.y = y0
        end

        do
          local s0, t0, s1, t1 = font:getglyphtexcoords(byte)
          vertices[0].texcoord.x = s0
          vertices[0].texcoord.y = t0
          vertices[1].texcoord.x = s0
          vertices[1].texcoord.y = t1
          vertices[2].texcoord.x = s1
          vertices[2].texcoord.y = t1
          vertices[3].texcoord.x = s1
          vertices[3].texcoord.y = t0
        end

        do
          vertices[0].color = textcolor
          vertices[1].color = textcolor
          vertices[2].color = textcolor
          vertices[3].color = textcolor
        end

        do
          indices[0] = index + 0
          indices[1] = index + 1
          indices[2] = index + 2
          indices[3] = index + 0
          indices[4] = index + 2
          indices[5] = index + 3
        end

        _vertices:pushbackv(vertices, 4)
        _indices:pushbackv(indices, 6)

        index = index + 4

        --penx = penx + advance
        --hack it to integer positions becuase cel can't handle non-integer advance
        --TODO fix in cel and remove hack 
        penx = penx + math_floor(advance+.5)
      end

      return penx, peny
    end
  end

  function crc.drawtext()
    if vertices:size() > 0 then
      local clip = crc.clip
      gl.glScissor(clip.l, oxcart.window.h-clip.b, clip.r-clip.l, clip.b-clip.t) --TODO put efficent implementation in oxcart C

      gl.glEnable(GL_BLEND)
      gl.glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)

      gl.glUseProgram(program.id)

      gl.glBindVertexArray(vao)
      gl.glBindTexture(GL_TEXTURE_2D, tex)

      gl.glBindBuffer(GL_ARRAY_BUFFER, vbo)
      if vcapacity ~= vertices:capacity() then 
        vcapacity = vertices:capacity()
        gl.glBufferData(GL_ARRAY_BUFFER, vertices:sizeofcapacity(), vertices:front(), GL_DYNAMIC_DRAW)
      else 
        gl.glBufferSubData(GL_ARRAY_BUFFER, 0, vertices:sizeof(), vertices:front())
      end 

      gl.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo)
      if icapacity ~= indices:capacity() then 
        icapacity = indices:capacity()
        gl.glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices:sizeofcapacity(), indices:front(), GL_DYNAMIC_DRAW)
      else 
        gl.glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices:sizeof(), indices:front())
      end 

      gl.glDrawElements(GL_TRIANGLES, indices:size(), GL_UNSIGNED_INT, nil)

      vertices:clear()
      indices:clear()
    end
  end
end

do
  local program = oxcart.program.new {
    oxcart.program.newvertexshader[[
    #version 400 core
    layout(std140) uniform;

    uniform matrices {
      mat4 m_projection;
      mat4 m_view;
      mat4 m_model;
    };

    in vec3 position;
    in vec4 color;

    out vec4 vs_color;

    void main()
    {
      gl_Position = m_projection * m_view * m_model * vec4(position, 1.0);
      vs_color = color;
    }
    ]],

    oxcart.program.newfragmentshader[[
    #version 400 core
    layout(std140) uniform;

    in vec4 vs_color;
    out vec4 fs_color;

    void main()
    {
      fs_color = vs_color;
    }
    ]],
  }

  local vao = gl.glGenVertexArray()
  local vbo = gl.glGenBuffer()
  local ibo = gl.glGenBuffer()
  local vcapacity = 128 * 4
  local icapacity = 128 * 6
  local vertices = vector[vertex_t](vcapacity)
  local indices = vector['GLuint'](icapacity)

  gl.glBindVertexArray(vao)

  gl.glBindBuffer(GL_ARRAY_BUFFER, vbo)
  gl.glBufferData(GL_ARRAY_BUFFER, vertices:sizeofcapacity(), nil, GL_DYNAMIC_DRAW)

  gl.glVertexAttribPointer(oxcart.attrib.position, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), ffi.cast('void*', offsetof(vertex_t, 'position')))
  gl.glEnableVertexAttribArray(oxcart.attrib.position)
  gl.glVertexAttribPointer(oxcart.attrib.color, 4, GL_FLOAT, GL_FALSE, sizeof(vertex_t),  ffi.cast('void*', offsetof(vertex_t, 'color')))
  gl.glEnableVertexAttribArray(oxcart.attrib.color)
  --gl.glVertexAttribPointer(oxcart.attrib.texcoord, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_t),  ffi.cast('void*', offsetof(vertex_t, 'texcoord')))
  --gl.glEnableVertexAttribArray(oxcart.attrib.texcoord)
  
  gl.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo)
  gl.glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices:sizeofcapacity(), nil, GL_DYNAMIC_DRAW)
 
  do
    local _vertices = vertices
    local _indices = indices

    local vertices = ffi.typeof('$[4]', vertex_t)()
    local indices = ffi.new('GLuint[6]')

    local min = math.min
    local max = math.max
    local function rectangle(x, y, w, h)
      do
        local l = max(crc.clip.l, x)
        local r = min(crc.clip.r, x+w)
        local t = max(crc.clip.t, y)
        local b = min(crc.clip.b, y+h)

        vertices[0].position.x = l
        vertices[0].position.y = t
        vertices[1].position.x = l
        vertices[1].position.y = b 
        vertices[2].position.x = r 
        vertices[2].position.y = b 
        vertices[3].position.x = r 
        vertices[3].position.y = t 
      end

      do
        vertices[0].color = color
        vertices[1].color = color
        vertices[2].color = color
        vertices[3].color = color
      end

      do
        local index = _vertices:size()
        indices[0] = index + 0
        indices[1] = index + 1
        indices[2] = index + 2
        indices[3] = index + 0
        indices[4] = index + 2
        indices[5] = index + 3
      end

      _vertices:pushbackv(vertices, 4)
      _indices:pushbackv(indices, 6)
    end

    crc.rectangle = rectangle

    function crc.rectangleborder(x, y, w, h, size)
      rectangle(x, y, size, h) --left
      rectangle(x+w-size, y, size, h) --right
      rectangle(x, y, w, size) --top
      rectangle(x, y+h-size, w, size) --bottom
    end
  end

  function crc.draw()
    if vertices:size() > 0 then
      local clip = crc.clip
      gl.glScissor(clip.l, oxcart.window.h-clip.b, clip.r-clip.l, clip.b-clip.t) --TODO put efficent implementation in oxcart C

      gl.glUseProgram(program.id)
      gl.glBindVertexArray(vao)

      gl.glBindBuffer(GL_ARRAY_BUFFER, vbo)
      if vcapacity ~= vertices:capacity() then 
        vcapacity = vertices:capacity()
        gl.glBufferData(GL_ARRAY_BUFFER, vertices:sizeofcapacity(), vertices:front(), GL_DYNAMIC_DRAW)
      else 
        gl.glBufferSubData(GL_ARRAY_BUFFER, 0, vertices:sizeof(), vertices:front())
      end 

      gl.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo)
      if icapacity ~= indices:capacity() then 
        icapacity = indices:capacity()
        gl.glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices:sizeofcapacity(), indices:front(), GL_DYNAMIC_DRAW)
      else 
        gl.glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices:sizeof(), indices:front())
      end

      
      gl.glDrawElements(GL_TRIANGLES, indices:size(), GL_UNSIGNED_INT, nil)
      vertices:clear()
      indices:clear()
    end
  end

  local window = oxcart.window
  function crc.setclip(clip)
    crc.clip = clip
  end

  local math_max = math.max
  local math_min = math.min
  function crc.addclip(x, y, w, h)
    local clip = crc.clip
    local t = {
      l = math_max(clip.l, x),
      r = math_min(clip.r, x+w),
      t = math_max(clip.t, y),
      b = math_max(clip.b, y+h),
    }
    crc.clip = t 
    return clip
  end

end

do --crc.drawlinks
  --TODO need to fix dirty rect stystem, it does not work in all cases
  local function updaterect(t, r)
    local ur = crc.metadescription.updaterect
    if r.r < ur.l or r.b < ur.t or r.l > ur.r or r.t > ur.b then
      return false
    end
    return true
  end

  local X = 0
  local Y = 0

  local colorstack = vector['color4f_t'](1)
  local textcolorstack = vector['color4f_t'](1)

  function crc.drawlinks(t)
    for i = #t,1,-1 do
      local t = t[i]
      if true or t.refresh or updaterect(t, t.clip) then 
        local face = t.face.select and t.face:select(t) or t.face
        --save
        local x = X
        local y = Y

        --udpate
        X = X + t.x
        Y = Y + t.y
        crc.setclip(t.clip) --TODO do gl.Scissor call when draw is executed some cels will never be drawn
        if face.draw then
          colorstack:pushback(color)
          textcolorstack:pushback(textcolor)

          face.draw(crc, face, t, X, Y) 

          copy(color, colorstack:back(), sizeof(color))
          copy(textcolor, textcolorstack:back(), sizeof(textcolor))

          colorstack:popback()
          textcolorstack:popback()
        else
          crc.drawlinks(t)
        end

        --restore
        X = x
        Y = y
      end
    end
  end
end

local matrices = ffi.new('oxcart_matrices_t')

for i, name in ipairs {
  'cel',
  'label',
  'text',
  'button',
  'textbutton',
  'grip',
  'window',
  'scroll',
  'editbox',
} do
  require(string.format('%s.%s', (...), name))
end

return function()
  local t, metadescription = cel.describe()

  if t then
    matrices.projection = C.mat4_orthographic(t.w, t.h)
    matrices.view = C.mat4_identity()
    matrices.model = C.mat4_identity()

    oxcart.uniformbuffer.matrices.update(matrices)

    gl.glEnable(GL_CULL_FACE)
    gl.glEnable(GL_DEPTH_TEST)
    gl.glDepthFunc(GL_LEQUAL)
    gl.glEnable(GL_MULTISAMPLE)

    gl.glEnable(GL_BLEND)
    gl.glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)

    gl.glClear(GL_DEPTH_BUFFER_BIT)

    gl.glEnable(GL_SCISSOR_TEST)
    crc.setfont(t.face.font)

    crc.metadescription = metadescription
    crc.drawlinks(t)

    gl.glDisable(GL_SCISSOR_TEST)
  end
end
