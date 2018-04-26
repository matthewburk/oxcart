local bit = require 'bit'
local ffi = require 'ffi'
local C = ffi.C

ffi.cdef[[
typedef struct mat3_t{
  float _11, _21, _31;
  float _12, _22, _32;
  float _13, _23, _33;
} mat3_t;

void mat3_identity(mat3_t* r);
]]

local M = {}

local mat3_t = ffi.typeof('mat3_t')

do --mat3_t
  local mt = {}
  mt.__index = mt 

  local size = ffi.sizeof(mat3_t)

  do
    local identity = mat3_t()
    C.mat3_identity(identity)
    function mt:identity()
      ffi.copy(self, identity, size)
    end
  end

  ffi.metatype(mat3_t, mt)
end

function M.identity()
  local result = mat3_t()
  C.mat3_identity(result)
  return result
end

return M
