local ffi = require 'ffi'
local gl = require 'opengl'
local bit = require 'bit'
local C = ffi.C
local cel = require 'oxcart.cel'

local M = {}

local root = cel.new():link(cel.root, 'fill')

local window do --window
  window = cel.window.new(300, 300)
    :settitle('A window with stuff')
    :adddefaultcontrols()
    :link(root)

  local scroll = cel.scroll.new()
    :link(window, 'fill')

  cel.col { gap=2, 
    { face='#888888', --make col slots light gray
      link='center',
      
      cel.editbox.new('HELLO', 200),
      cel.editbox.new('hello_again', 200),
      cel.label.new('label'),
      cel.button.new(200, 50),
      cel.text.new('text\nit wraps'),
      cel.textbutton.new('text\nbutton'),
      cel.text.new [[[
      if the text starts with '['
      and a new line then the extra space
      at the beginning of each of these
      lines is removed. The trailing new line
      is also removed.  Handy! 
      ]],
      cel.text.new [[
      Here is one wihout the extra '['.
      Notice all the extra space.
      ]],
      
    },

    { face='#889988', --make col slots light gray with a bit more green
      link = 'width', --so that text will shink and grow 
      cel.text.new([[[
      By deafult text wraps at new lines, 
      but you can make it wrap at word boundaries as well.  Here is the code form the 'scroll' linker.
      M['scroll'] = function(hw, hh, x, y, w, h, fillx, filly)
        if x >= 0 or w <= hw then 
          x = 0
        elseif x + w < hw then
          x = hw - w
        end

        if y >= 0 or h <= hh then
          y = 0
        elseif y + h < hh then
          y = hh - h
        end

        return x, y, fillx and hw or w, filly and hh or h
      end
      ]]):wrap('word'),
    }
  }:link(scroll, 'scroll', true, false) --look at cel.linkers['scroll'] to see what this does

  function window:onmousedown()
    window:raise() --put on top of other cels
  end
end

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
