local bit = require 'bit'
local ffi = require 'ffi'
local C = ffi.C

ffi.cdef[[
typedef struct vec2_t{
  float x;
  float y;
} vec2_t;
]]

local M = {}

do --vec2_t
  local mt = {}
  mt.__index = mt 

  ffi.metatype('vec2_t', mt)
end

return M
