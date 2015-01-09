local C = require 'oxcart.C'
local cel = require 'cel'

local M = setmetatable({}, {
  __call = function(M, t) return cel(t) end,
  __index = cel,
})

M.root = cel.new(800, 600)
M.colors = require 'oxcart.cel.colors'
local draw = require 'oxcart.cel.faces'

function M.root:draw()
  return draw()
end

oxcart.cel = M

return M
