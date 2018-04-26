local bit = require 'bit'
local ffi = require 'ffi'
local C = ffi.C

ffi.cdef[[
typedef struct mat4_t{
  float _11, _21, _31, _41; //col1
  float _12, _22, _32, _42; //col2
  float _13, _23, _33, _43; //col3
  float _14, _24, _34, _44; //col4
} mat4_t;

void mat4_identity(mat4_t* r);
void mat4_lookat(const vec3_t* eye, const vec3_t* target, const vec3_t* up, mat4_t* r);
void mat4_perspective(float fovy, float aspect, float n, float f, mat4_t* r);
void mat4_orthographic(float w, float h, mat4_t* r);
void mat4_multiply(const mat4_t* m1, const mat4_t* m2, mat4_t* r);
void mat4_inverse(const mat4_t* m, mat4_t* r);
void mat4_rotate(const float x, const float y, const float z, const float angle, mat4_t* r);
void mat4_translate(const float x, const float y, const float z, mat4_t* r);
void mat4_to_mat3_normal(const mat4_t* m1, mat3_t* r);
]]

local M = {}

local mat4_t = ffi.typeof('mat4_t')

do --mat4_t
  local mt = {}
  mt.__index = mt 
  
  local size = ffi.sizeof(mat4_t)

  do
    local identity = mat4_t()
    C.mat4_identity(identity)
    function mt:identity()
      ffi.copy(self,identity, size)
    return self
    end
  end

  function mt:multiply(m1, m2)
    C.mat4_multiply(m1, m2, self)
    return self
  end

  function mt:inverse(m)
    m = m or self
    C.mat4_inverse(m, self)
    return self
  end

  function mt:translate(x, y, z)
    C.mat4_translate(x, y, z, self)
    return self
  end

  function mt:orthographic(w, h)
    C.mat4_orthographic(w, h, self)
    return self
  end

  function mt:perspective(fovy, aspect, near, far)
    C.mat4_perspective(fovy, aspect, near, far, self)
    return self
  end

  function mt:copy(b)
    ffi.copy(self, b, size)
    return self
  end

  ffi.metatype(mat4_t, mt)
end

function M.identity()
  local result = mat4_t()
  C.mat4_identity(result)
  return result
end

function M.inverse(m)
  local result = mat4_t()
  C.mat4_inverse(m, result)
  return result
end

function M.orthographic(w, h)
  local result = mat4_t()
  C.mat4_orthographic(w, h, result)
  return result
end

function M.perspective(fovy, aspect, near, far)
  local result = mat4_t()
  C.mat4_perspective(fovy, aspect, near, far, result)
  return result
end

function M.lookat(eye, target, up)
  local result = mat4_t()
  C.mat4_lookat(eye, target, up, result)
  return result
end

function M.rotate(x, y, z, angle)
  local result = mat4_t()
  C.mat4_rotate(x, y, z, angle, result)
  return result
end

function M.translate(x, y, z)
  local result = mat4_t()
  C.mat4_translate(x, y, z, result)
  return result
end

return M
