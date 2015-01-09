local cel = require 'cel'
local colors = require 'oxcart.cel.colors'

local M = cel.getface('grip')
M.color = colors.gray2
M.bordercolor = colors.gray4
M.bordersize = 1
M.modes = {}

function M.draw(crc, f, t, X, Y)
  if f.color then
    crc.setcolor(f.color)
    crc.rectangle(X, Y, t.w, t.h)

    if f.bordersize and f.bordercolor then
      crc.setcolor(f.bordercolor)
      crc.rectangleborder(X, Y, t.w, t.h, f.bordersize)
    end
    crc.draw()
  elseif f.bordersize and f.bordercolor then
    crc.setcolor(f.bordercolor)
    crc.rectangleborder(X, Y, t.w, t.h, f.bordersize)
    crc.draw()
  end

  return crc.drawlinks(t)
end

function M.select(f, t)
  if t.mousefocusin then
    f = f['mousefocusin'] or f
    if t.isgrabbed then
      f = f['grabbed'] or f
    end
  elseif t.isgrabbed then
    f = f['grabbed'] or f
  end
  return f
end

do
  M.modes['grabbed'] = M:new {
    color = cel.color.rgb(.4, .4, .4),
    bordercolor = cel.color.rgb(0, 1, 1),
  }

  M.modes['mousefocusin'] = M:new {
    color = cel.color.rgb(.4, .4, .4),
    bordercolor = cel.color.rgb(0, 1, 1),
  }
  
  M.modes['grabbed'] = M.modes['mousefocusin']:new {
    color = cel.color.rgb(0, .8, .8),
    bordercolor = cel.color.rgb(0, 1, 1),
    bordersize = 1,
  }
end

return M

