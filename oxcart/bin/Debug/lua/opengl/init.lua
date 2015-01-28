local ffi = require "ffi"

local libs = {
  OSX     = { x86 = "OpenGL.framework/OpenGL", x64 = "OpenGL.framework/OpenGL" },
  Windows = { x86 = "OPENGL32.DLL",            x64 = "OPENGL32.DLL" },
}

local gl = ffi.load( libs[ ffi.os ][ ffi.arch ] )

require "WTypes"

ffi.cdef[[
typedef unsigned int GLenum;
typedef unsigned char GLboolean;
typedef unsigned int GLbitfield;
typedef signed char GLbyte;
typedef int GLint;
typedef int GLsizei;
typedef unsigned char GLubyte;
typedef char GLchar;
typedef char GLcharARB;
typedef short GLshort;
typedef unsigned short GLushort;
typedef unsigned int GLuint;
typedef int64_t GLint64;
typedef uint64_t GLuint64;
typedef int64_t GLint64EXT;
typedef uint64_t GLuint64EXT;

typedef unsigned short GLhalfNV;

typedef float GLfloat;
typedef float GLclampf;

typedef double GLdouble;
typedef double GLclampd;

typedef void GLvoid;
typedef long GLintptr;
typedef long GLsizeiptr;
typedef void *GLhandleARB;
typedef long GLintptrARB;
typedef long GLsizeiptrARB;
typedef unsigned short GLhalfARB;
typedef unsigned short GLhalf;
]]


require 'gl'
require 'glext'
require 'wglext'

local function getwglfunction(name)
	local funcptr = gl.wglGetProcAddress(name);

	if funcptr == nil then
		return nil
	end

  return ffi.cast('PFN'..name:upper()..'PROC', funcptr)
end

local M = setmetatable({}, {
	__index = function(t, k)
		local v = getwglfunction(k) or gl[k]

		rawset(t, k, v)
		return v
	end
})


for i, name in ipairs {
  'glGenVertexArray',
  'glGenBuffer',
  'glGenTexture',
} do
  local plural = name..'s'
  local naked = M[plural]

  M[plural] = function(n, out)
    if out then
      naked(n, out)
      return out, n
    else
      local array = ffi.new('GLuint[?]', n)
      gl[plural](n, array)
      return array, n
    end
  end

  M[name] = function()
    local array = ffi.new('GLuint[1]')
    naked(1, array)
    return array[0]
  end
end

for i, name in ipairs {
  'glDeleteVertexArray',
  'glDeleteBuffer',
} do
  local plural = name..'s'
  local naked = M[plural]

  M[name] = function(oneitem)
    local array = ffi.new('GLuint[1]')
    array[0] = oneitem
    M[plural](1, array)
  end
end

return M 
