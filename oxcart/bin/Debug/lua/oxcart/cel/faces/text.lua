local cel = require 'cel'

local M = cel.getface('text')

M.textcolor = 'current'
M.color = false
M.bordercolor = false
M.bordersize = false 

M.layout = {
  padding = {
    l=function(w, h, font) return math.floor(font.metrics[' '].advance/2) end,
    r=function(w, h, font) return math.floor(.9+font.metrics[' '].advance/2) end,
  },
}

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

--[[
function M.print(f, t, pre, facename)
  local s = string.format('%s[%d:%s] { x:%d y:%d w:%d h:%d [refresh:%s]',
    t.metacel, t.id, tostring(facename), t.x, t.y, t.w, t.h, tostring(t.refresh))
  io.write(pre, s)
  if t.text then
    for i, line in ipairs(t.lines) do
      io.write('\n', pre, '  @@', string.format('line[%d %d %s]', line.penx, line.peny, line.text))
    end
  end
  if t.font then
    io.write('\n', pre, '  @@', string.format('font[%s:%d]', t.font.name, t.font.size))
  end
  if f.color then
    io.write('\n', pre, '  @@', string.format('color[%d, %d, %d, %d]', string.byte(f.color, 1, 4)))
  end
  if f.bordercolor and f.borderwidth then
    io.write('\n', pre, '  @@', string.format('bordercolor[%d, %d, %d, %d]', string.byte(f.bordercolor, 1, 4)))
  end
end
--]]


return M

