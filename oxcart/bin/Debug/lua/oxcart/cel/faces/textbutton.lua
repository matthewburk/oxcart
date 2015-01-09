local cel = require 'cel'
local M = cel.getface('textbutton')

M.textcolor = 'current'
M.color = cel.color.rgb(.3, .3, .3)
M.bordercolor = cel.color.rgb(.4, .4, .4)
M.bordersize = 4
M.modes = {}

M.layout = {
  padding = {
    l=function(w, h, font) return font.metrics['M'].advance end,
    t = function(w, h, font) return h*.35 end,
  },
}

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

function M.draw(crc, f, t, X, Y)
  if f.color then
    crc.setcolor(f.color)
    crc.rectangle(X, Y, t.w, t.h)
    crc.draw()
  end

  if f.textcolor and t.text then
    crc.setfont(t.font)
    crc.settextcolor(f.textcolor)

    --TODO lineheight is not exact, this may skip things that should be rendered
    local lineheight = t.font.lineheight
    local ymin = t.clip.t - Y - lineheight
    local ymax = t.clip.b - Y + lineheight

    for i, line in ipairs(t.lines) do
      if line.peny > ymin then
        if line.peny > ymax then
          break
        else
          crc.addtext(line.text, X+line.penx, Y+line.peny)
        end
      end
    end
    crc.drawtext()
  end

  if f.bordersize and f.bordercolor then
    crc.setcolor(f.bordercolor)
    crc.rectangleborder(X, Y, t.w, t.h, f.bordersize)
    crc.draw()
  end

  return crc.drawlinks(t)
end

do
  M.modes['pressed'] = M:new {
    color = cel.color.tint(M.color, .5),
    bordercolor = cel.color.rgb(0, 1, 1),
  }

  M.modes['mousefocusin'] = M:new {
    color = cel.color.tint(M.color, .5),
    --bordercolor = cel.color.rgb(0, 1, 1),
  }
 
  M.modes['pressed'] = M.modes['mousefocusin']:new {
    textcolor = cel.color.rgb(.2, .2, .2),
    color = cel.color.rgb(0, .8, .8),
    bordercolor = cel.color.rgb(0, 1, 1),
    bordersize = 2,
  }
end

return M



