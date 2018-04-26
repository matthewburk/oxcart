require 'strict'

local plane = require 'oxcart.geometry.plane'
local aabb = require 'oxcart.geometry.aabb'

local mt = {}
mt.__index = mt

-- m4 - The combined projection and view matrix
function mt:update(m4)
  self.left.normal.x = m4._41 + m4._11
  self.left.normal.y = m4._42 + m4._12
  self.left.normal.z = m4._43 + m4._13
  self.left.d = m4._44 + m4._14
  self.right.normal.x = m4._41 - m4._11
  self.right.normal.y = m4._42 - m4._12
  self.right.normal.z = m4._43 - m4._13
  self.right.d = m4._44 - m4._14
  self.top.normal.x = m4._41 - m4._21
  self.top.normal.y = m4._42 - m4._22
  self.top.normal.z = m4._43 - m4._23
  self.top.d = m4._44 - m4._24
  self.bottom.normal.x = m4._41 + m4._21
  self.bottom.normal.y = m4._42 + m4._22
  self.bottom.normal.z = m4._43 + m4._23
  self.bottom.d = m4._44 + m4._24
  self.near.normal.x = m4._41 + m4._31
  self.near.normal.y = m4._42 + m4._32
  self.near.normal.z = m4._43 + m4._33
  self.near.d = m4._44 + m4._34
  self.far.normal.x = m4._41 - m4._31
  self.far.normal.y = m4._42 - m4._32
  self.far.normal.z = m4._43 - m4._33
  self.far.d = m4._44 - m4._34
  

  self.left:normalize()
  self.right:normalize()
  self.top:normalize()
  self.bottom:normalize()
  self.near:normalize()
  self.far:normalize()

  return self
end

-- true if aabb is not wholly outside the frustum
function mt:intersectsaabb(box)
  local center = box:center()
  local size = box:size()

  for i=1, 6 do
    local p = self[i]
    local dist = center:dot(p.normal)
    local r = size.x * math.abs(p.normal.x) + size.y * math.abs(p.normal.y) + size.z * math.abs(p.normal.z)

    if dist + r < -p.d then
      return false
    end
  end

  return true
end

local M = {}

function M.new()
  local frustum = {
    plane.new(),
    plane.new(),
    plane.new(),
    plane.new(),
    plane.new(),
    plane.new(),
  }

  frustum.left = frustum[1] 
  frustum.right = frustum[2] 
  frustum.top = frustum[3] 
  frustum.bottom = frustum[4] 
  frustum.near = frustum[5] 
  frustum.far = frustum[6] 

  return setmetatable(frustum, mt)
end

return M
