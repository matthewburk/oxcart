local ffi = require 'ffi'
local math = require 'oxcart.math'

local M = {}

ffi.cdef[[
typedef struct plane {
  vec3_t normal;
  float d;
}plane_t;
]]

local plane_t = ffi.typeof('plane_t')

local mt = {}
mt.__index = mt

function mt:distancefrompoint(x, y, z)
  return self.normal.x*x + self.normal.y*y + self.normal.z*z + self.d
end

function mt:normalize()
  local ln = self.normal:length()
  self.normal.x = self.normal.x/ln
  self.normal.y = self.normal.y/ln
  self.normal.z = self.normal.z/ln
  self.d = self.d/ln
  return self
end


--[[
function mt:intersectsaabb(box)
  local cx, cy, cz = box:getcenter()
  local distance = self:distancefrompoint(cx, cy, cz)

  --project normal onto vector of box half size
  local mdist = math.abs(Vector3.Dot(self.normal, b.Size * 0.5f));
  if (t_distance < -t_mdist) return PlaneSide.Negative;
  else if (t_distance > t_mdist) return PlaneSide.Positive;
  else return PlaneSide.Both;
end
--]]


ffi.metatype(plane_t, mt)

function M.new()
  return plane_t()
end

return M
