local cel = require 'cel'
local M = cel.getface('button')

M.textcolor = 'current'
M.color = cel.color.rgb(.3, .3, .3)
M.bordercolor = cel.color.rgb(.4, .4, .4)
M.bordersize = 1
M.modes = {}

function M.select(f, t)
  if t.mousefocusin then
    f = f.modes['mousefocusin'] or f
    if t.pressed then
      f = f.modes['pressed'] or f
    end
  elseif t.pressed then
    f = f.modes['pressed'] or f
  end
  return f
end

do
  M.modes['pressed'] = M:new {
    color = cel.color.tint(M.color, .5),
    bordercolor = cel.color.rgb(0, 1, 1),
  }

  M.modes['mousefocusin'] = M:new {
    color = cel.color.tint(M.color, .5),
    bordercolor = cel.color.rgb(0, 1, 1),
  }
  
  M.modes['pressed'] = M.modes['mousefocusin']:new {
    textcolor = cel.color.rgb(.2, .2, .2),
    color = cel.color.rgb(0, .8, .8),
    bordercolor = cel.color.rgb(0, 1, 1),
    bordersize = 2,
  }
end

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

return M


