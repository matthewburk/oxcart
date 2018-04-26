local ffi = require 'ffi'
local math = require 'oxcart.math'
local C = ffi.C
local mat4 = math.mat4



local M = {}
local mt = {}
mt.__index = mt

local vec3_t = ffi.typeof('vec3_t')
local radtodeg = 180/math.pi
local degtorad = math.pi/180

local up = vec3_t(0, 1, 0)
local target = vec3_t(0, 0, 0)

--using opengl coordinate system
--polar angle is angle from (0, 1, 0) to camera forward vector
--azimuth is angle from (0, 0, -1) clockwise
--Z positive behind camera
--X is positive to right of camera
function mt:lookat(polar, azimuth)
  polar = polar*degtorad
  azimuth = azimuth*degtorad

  local r2 = math.sin(polar)
  local Z = self.axes.z
  Z.x = -r2*math.sin(azimuth) 
  Z.y = -math.cos(polar)
  Z.z = -r2*-math.cos(azimuth) 

  C.vec3_cross(up, Z, self.axes.x)
  C.vec3_cross(self.axes.z, self.axes.x, self.axes.y)

  local p = self.position

  target.x = p.x - Z.x
  target.y = p.y - Z.y
  target.z = p.z - Z.z
  self.transform = mat4.lookat(p, target, up)
  return self
end

function mt:lookatpoint(x, y, z)
  local p = self.position
  local Z = self.axes.z

  Z.x = p.x - x 
  Z.y = p.y - y
  Z.z = p.z - z

  Z:normalize()
  self.axes.x:cross(up, Z)
  self.axes.y:cross(Z, self.axes.x)

  target.x = x
  target.y = y
  target.z = z
  self.transform = mat4.lookat(p, target, up)
  return self
end

function mt:moveto(x, y, z)
  local p = self.position
  local Z = self.axes.z
  p.x = x
  p.y = y
  p.z = z

  target.x = p.x - Z.x
  target.y = p.y - Z.y
  target.z = p.z - Z.z
  self.transform = mat4.lookat(p, target, up)
  return self
end

function mt:moveforward(scalar)
  local p = self.position
  local Z = self.axes.z
  p.x = p.x - Z.x*scalar
  p.y = p.y - Z.y*scalar
  p.z = p.z - Z.z*scalar

  target.x = p.x - Z.x
  target.y = p.y - Z.y
  target.z = p.z - Z.z
  self.transform = mat4.lookat(p, target, up)

  return self
end

function mt:moveright(scalar)
  local p = self.position
  local X = self.axes.x
  local Z = self.axes.z
  p.x = p.x + X.x*scalar
  p.y = p.y + X.y*scalar
  p.z = p.z + X.z*scalar

  target.x = p.x - Z.x
  target.y = p.y - Z.y
  target.z = p.z - Z.z
  self.transform = mat4.lookat(p, target, up)

  return self
end

function mt:moveup(scalar)
  local p = self.position
  local Y = self.axes.y
  local Z = self.axes.z
  p.x = p.x + Y.x*scalar
  p.y = p.y + Y.y*scalar
  p.z = p.z + Y.z*scalar

  target.x = p.x - Z.x
  target.y = p.y - Z.y
  target.z = p.z - Z.z
  self.transform = mat4.lookat(p, target, up)

  return self
end


local axes_t = ffi.typeof[[ struct { vec3_t x; vec3_t y; vec3_t z; } ]]

function M.new()
  local camera =  {
    axes = axes_t(),
    transform = mat4.identity(),
    position = vec3_t(0, 0, 0),
  }
  return setmetatable(camera, mt)
end

oxcart.camera = M

return M
