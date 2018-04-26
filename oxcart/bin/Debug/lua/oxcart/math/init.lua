local bit = require 'bit'
local ffi = require 'ffi'
local C = ffi.C

local M = {}

for k, v in pairs(math) do
  M[k]=v
end

local function lerp(a, b, p)
  return a + p * (b - a)
end

local function rlerp(a, b, c)
  return (c - a)/(b - a);
end

M.lerp = lerp
M.rlerp = rlerp

M.smoothstep = function(a, b, p)
  return lerp(a, b, p*p*(3-2*p))
end
    

oxcart.math = M
oxcart.math.noise = require('oxcart.math.noise')
oxcart.math.vec2 = require('oxcart.math.vec2')
oxcart.math.vec3 = require('oxcart.math.vec3')
oxcart.math.vec4 = require('oxcart.math.vec4')
oxcart.math.mat3 = require('oxcart.math.mat3')
oxcart.math.mat4 = require('oxcart.math.mat4')

return M
