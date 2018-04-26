local ffi = require 'ffi'
local math = require 'oxcart.math'

ffi.cdef[[
typedef struct aabb {
  vec4_t min;
  vec4_t max;
} aabb_t;
]]

local M = {}

local vec4_t = ffi.typeof('vec4_t')
local vec3_t = ffi.typeof('vec3_t')
local aabb_t = ffi.typeof('aabb_t')

local mt = {}
mt.__index = mt

function mt:size(r)
  r = r or vec3_t()
  r.x = self.max.x - self.min.x
  r.y = self.max.y - self.min.y
  r.z = self.max.z - self.min.z
  return r
end

function mt:center(r)
  r = r or vec3_t()
  r.x = (self.min.x + self.max.x) * 0.5
  r.y = (self.min.y + self.max.y) * 0.5
  r.z = (self.min.z + self.max.z) * 0.5
  return r
end

function mt:transform(m)
  self.min:transform(self.min, m)
  self.max:transform(self.max, m)
end

ffi.metatype(aabb_t, mt)


function M.new(min, max)
  return aabb_t(min, max)
end

return M
