require 'oxcart.math'
local ffi = require 'ffi'
local gl = require 'opengl'
local bit = require 'bit'
local C = ffi.C

local M = {}

oxcart.attrib = {
  position = 0,
  texcoord = 1,
  color = 2,
  normal = 4,
}

do
  oxcart.uniformbuffer = {}

  for i, name in ipairs {
    'matrices',
    'light',
  } do
    local t = {}
    local glbuffer = gl.glGenBuffer() 
    gl.glBindBufferBase(GL_UNIFORM_BUFFER, i, glbuffer)

    function t.update(value)
      gl.glBindBuffer(GL_UNIFORM_BUFFER, glbuffer)
      gl.glBufferData(GL_UNIFORM_BUFFER, ffi.sizeof(value), value, GL_DYNAMIC_DRAW)

      t.update = function(value)
        gl.glBindBuffer(GL_UNIFORM_BUFFER, glbuffer)
        gl.glBufferSubData(GL_UNIFORM_BUFFER, 0, ffi.sizeof(value), value)
      end
    end

    function t.bind(programid)
      local blockindex = gl.glGetUniformBlockIndex(programid, name)
      if blockindex ~= GL_INVALID_INDEX then
        gl.glUniformBlockBinding(programid, blockindex, i)
        return blockindex
      else
        return false 
      end
    end

    oxcart.uniformbuffer[name] = t
  end
end


do
  local function newshader(source, shadertype)
    assert(shadertype)

    local shader = gl.glCreateShader(shadertype)
    local strings = ffi.new('const char* [1]')
    strings[0] = source
    local lens = ffi.new('int[1]', #source)

    gl.glShaderSource(shader, 1, strings, lens)
    gl.glCompileShader(shader)

    local status = ffi.new('GLint[1]')
    gl.glGetShaderiv(shader, GL_COMPILE_STATUS, status)
    status = status[0]

    if status == 0 then
      local log = ffi.new('GLchar[1024]')
      gl.glGetShaderInfoLog(shader, 1024, nil, log)
      error(ffi.string(log))
    end 

    return shader
  end

  for name, shadertype in pairs {
    newvertexshader = GL_VERTEX_SHADER,
    newfragmentshader = GL_FRAGMENT_SHADER,
  } do
    M[name] = function(source)
      return newshader(source, shadertype)
    end
  end
end

local mt = {}
mt.__index = mt

function mt:use()
  gl.glUseProgram(self.id)
end

do
  local alinkstatus = ffi.new('GLint[1]')

  function M.new(shaders)
    local program = gl.glCreateProgram()

    for i, shader in ipairs(shaders) do
      gl.glAttachShader(program, shader)
    end

    for name, location in pairs(oxcart.attrib) do
      gl.glBindAttribLocation(program, location, name)
    end

    gl.glLinkProgram(program)
    gl.glGetProgramiv(program, GL_LINK_STATUS, alinkstatus)

    if alinkstatus[0] == 0 then
      local log = ffi.new('GLchar[1024]')
      gl.glGetProgramInfoLog(program, 1024, nil, log)
      error(ffi.string(log))
    end

    for name, t in pairs(oxcart.uniformbuffer) do
      t.bind(program)
    end

    return setmetatable({id=program}, mt)
  end
end

oxcart.program = M

return M
