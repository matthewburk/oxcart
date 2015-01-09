local cel = require 'cel'

local M = cel.getface('label')
M.textcolor = 'current'
M.color = false

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
    crc.addtext(t.text, X+t.penx, Y+t.peny)
    crc.drawtext()
  end

  return crc.drawlinks(t)
end

return M



