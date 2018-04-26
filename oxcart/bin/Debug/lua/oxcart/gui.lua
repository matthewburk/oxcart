local ffi = require 'ffi'
local gl = require 'opengl'
local bit = require 'bit'
local C = ffi.C
local cel = require 'oxcart.cel'

local M = {}

local root = cel.new():link(cel.root, 'fill')

M.cameraspeed =.5

M.cameracontrols = cel.row {
  cel.textbutton.new('-'),
  cel.editbox {
    w=100,
    onkeypress = function(self, key, intercepted)
      if key == cel.keyboard.keys.enter then
        M.cameraspeed = tonumber(self:gettext())
      end
    end
  },
  cel.textbutton.new('+'),
}:link(root, 'center.bottom')


local fpslabel = cel.label.new('')
  :link(root, 'center.top')

function M.update(elapsed, deltaelapsed)
  fpslabel:printf('fps:%d', 1/(deltaelapsed/1000))
  cel.root:update(elapsed) --drives cel time and updates animations
end

function M.draw()
  cel.root:draw() --draws cel
end

function M.islinked()
  return root:islinkedto(cel.root)
end

function M.link()
  root:link(cel.root, 'fill')
end

function M.unlink()
  root:unlink()
end

M.keyboard = setmetatable(cel.keyboard, {__index=cel.keyboard.keys})
M.mouse = setmetatable(cel.mouse, {__index=cel.mouse.buttons})

oxcart.gui = M

return M
--TODO fix cel bug that allows a negative h when font ascent is negative
