local ffi = require 'ffi'
local gl = require 'opengl'
local math = require 'oxcart.math'
local vector = require 'oxcart.vector'
local C = ffi.C
local mat4 = math.mat4
require 'oxcart.geometry'
require 'oxcart.program'
require 'oxcart.chunk'

local M = {}

local chunksize = 1

local mt = {}
mt.__index = mt

function mt:moveto(wx, wy, wz)
  self.x = wx
  self.y = wy
  self.z = wz
  self.chunk.transform:translate(wx-chunksize/2, wy, wz-chunksize/2)
  self.chunk.aabb:transform(self.chunk.transform)
  return self
end

function M.new()
  local chunk = oxcart.chunk.new(chunksize)
  local block = chunk.data[1][1][1]
  block.value = 1
  block.r = 255

  oxcart.chunk.totriangles(chunk)

  return setmetatable({
    x = 0,
    y = 0, 
    z = 0,
    chunk=chunk,
  }, mt)
end


oxcart.creature = M

return M
