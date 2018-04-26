require 'oxcart.atlas'
require 'oxcart.program'
local oxcart_font = require 'oxcart.font'
local vector = require 'oxcart.vector'
local gl = require 'opengl'
local ffi = require 'ffi'
local C = ffi.C
local math = require 'oxcart.math'
local mat4 = math.mat4

ffi.cdef[[
typedef struct oxcart_text_vertex {
  vec3_t position;
  vec2_t texcoord;
  vec4_t color;
} oxcart_text_vertex_t;
]]

local M = {}

local mt = {}
mt.__index = mt

local function getfont(textbuffer, name, style, size, line)
  local font = oxcart_font.load(name, style, size, line, textbuffer.atlas)

  if not textbuffer.fonts[font] then
    local w, h = textbuffer.atlas:properties()
    gl.glBindTexture(GL_TEXTURE_2D, textbuffer.tex)
    gl.glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, GL_RED, GL_UNSIGNED_BYTE, textbuffer.atlas:data())
    textbuffer.fonts[font] = true
  end

  return font
end

--origin is basline y is positive down
--returns advance, xmin(ink), xmax(ink), ymin(ink), ymax(ink)
function mt:measure(text, markup)
  markup = markup or self.markup
  local font = getfont(self, markup.name, markup.style, markup.size, markup.line)
  return font:measure(text)
end

do
  local vertices = ffi.new('oxcart_text_vertex_t[4]')
  local indices = ffi.new('GLuint[6]')
  local defaultcolor = ffi.new('vec4_t', 1, 1, 1, 1)

  local function append(textbuffer, text, penx, peny, markup)
    penx = penx or 0
    peny = peny or 0
    markup = markup or textbuffer.markup
    local font = getfont(textbuffer, markup.name, markup.style, markup.size, markup.line)

    for i = 1, #text do
      local byte = string.byte(text, i)
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
        local color = markup.color or defaultcolor
        vertices[0].color = color
        vertices[1].color = color
        vertices[2].color = color
        vertices[3].color = color
      end

      do
        local index = textbuffer.vertices:size()
        indices[0] = index + 0
        indices[1] = index + 1
        indices[2] = index + 2
        indices[3] = index + 0
        indices[4] = index + 2
        indices[5] = index + 3
      end

      textbuffer.vertices:pushbackv(vertices, 4)
      textbuffer.indices:pushbackv(indices, 6)

      penx = penx + advance
    end

    textbuffer.changed = true

    return penx, peny
  end

  local function clear(textbuffer)
    textbuffer.changed = false
    textbuffer.vertices:clear()
    textbuffer.indices:clear()
  end

  mt.append = append
  mt.clear = clear
  
  function mt:set(text, markup)
    clear(self)
    return append(self, text, 0, 0, markup)
  end
end



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

program.sampler = gl.glGetUniformLocation(program.id, "sampler")

local draw do
  local matrices = ffi.new('oxcart_matrices_t')

  matrices.projection:orthographic(oxcart.window.w, oxcart.window.h)
  matrices.view:identity()
  matrices.model:identity()

  function mt:draw(x, y, z)
    gl.glEnable(GL_BLEND)
    gl.glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
    gl.glBlendColor(1, 1, 1, 1)

    gl.glUseProgram(program.id)

    matrices.model:translate(x, y, z)

    oxcart.uniformbuffer.matrices.update(matrices)

    gl.glProgramUniform1i(program.id, program.sampler, 0)

    gl.glBindVertexArray(self.vao)
    gl.glBindTexture(GL_TEXTURE_2D, self.tex)

    if self.changed then
      do
        gl.glBindBuffer(GL_ARRAY_BUFFER, self.vbo)
        if self.vcapacity ~= self.vertices:capacity() then 
          self.vcapacity = self.vertices:capacity()
          gl.glBufferData(GL_ARRAY_BUFFER, self.vcapacity * sizeof(self.vertices.itemtype), self.vertices:front(), GL_DYNAMIC_DRAW)
        else 
          gl.glBufferSubData(GL_ARRAY_BUFFER, 0, self.vertices:sizeof(), self.vertices:front())
        end 
      end

      do
        gl.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, self.ibo)
        if self.icapacity ~= self.indices:capacity() then 
          self.icapacity = self.indices:capacity()
          gl.glBufferData(GL_ELEMENT_ARRAY_BUFFER, self.icapacity * sizeof(self.indices.itemtype), self.indices:front(), GL_DYNAMIC_DRAW);
        else 
          gl.glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, self.indices:sizeof(), self.indices:front());
        end 
      end

      self.changed = false
    end 

    gl.glDrawElements(GL_TRIANGLES, self.indices:size(), GL_UNSIGNED_INT, nil)
  end
end

local sizeof = ffi.sizeof
local offsetof = ffi.offsetof
function M.new(t)
  local vcapacity = 128 * 4
  local icapacity = 128 * 6

  local textbuffer = {
    vao = gl.glGenVertexArray(),
    vbo = gl.glGenBuffer(),
    ibo = gl.glGenBuffer(),
    tex = gl.glGenTexture(),
    atlas = oxcart.atlas.new(),
    vertices = vector['oxcart_text_vertex_t'](vcapacity),
    indices = vector['GLuint'](icapacity),
    vcapacity = vcapacity, 
    icapacity = icapacity,
    fonts={},
    changed = false,
    markup = t or {
      name = 'firasans',
      style = 'regular',
      size = 24,
      line = 0,
      color = ffi.new('vec4_t', 1, 1, 1, 1),
    }
  }

  gl.glBindVertexArray(textbuffer.vao)

  gl.glBindBuffer(GL_ARRAY_BUFFER, textbuffer.vbo)
  gl.glBufferData(GL_ARRAY_BUFFER, textbuffer.vertices:capacity() * sizeof(textbuffer.vertices.itemtype), nil, GL_DYNAMIC_DRAW)

  gl.glVertexAttribPointer(oxcart.attrib.position, 3, GL_FLOAT, GL_FALSE, sizeof('oxcart_text_vertex_t'), ffi.cast('void*', offsetof('oxcart_text_vertex_t', 'position')))
  gl.glEnableVertexAttribArray(oxcart.attrib.position)

  gl.glVertexAttribPointer(oxcart.attrib.texcoord, 2, GL_FLOAT, GL_FALSE, sizeof('oxcart_text_vertex_t'),  ffi.cast('void*', offsetof('oxcart_text_vertex_t', 'texcoord')))
  gl.glEnableVertexAttribArray(oxcart.attrib.texcoord)

  gl.glVertexAttribPointer(oxcart.attrib.color, 4, GL_FLOAT, GL_FALSE, sizeof('oxcart_text_vertex_t'),  ffi.cast('void*', offsetof('oxcart_text_vertex_t', 'color')))
  gl.glEnableVertexAttribArray(oxcart.attrib.color)

  gl.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, textbuffer.ibo)
  gl.glBufferData(GL_ELEMENT_ARRAY_BUFFER, textbuffer.indices:capacity() * sizeof(textbuffer.indices.itemtype), nil, GL_DYNAMIC_DRAW)

  gl.glBindTexture(GL_TEXTURE_2D, textbuffer.tex)
  gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE)
  gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE)
  gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR)
  gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR)

  local atlasw, atlash = textbuffer.atlas:properties()
  gl.glTexStorage2D(GL_TEXTURE_2D, 1, GL_R8, atlasw, atlash)

  return setmetatable(textbuffer, mt)
end

oxcart.textbuffer = M

return M
