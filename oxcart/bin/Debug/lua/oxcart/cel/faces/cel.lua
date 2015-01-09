local cel = require 'cel'
local M = cel.getface('cel')

M.font = cel.loadfont('default', 12)
M.draw = nil--default face does not draw

do --@color
  local face = M:new{}:register('@color')
  face.color = false

  function face.draw(crc, f, t, X, Y)
    if f.color then
      crc.setcolor(f.color)
      crc.rectangle(X, Y, t.w, t.h)
      crc.draw()
    end
    return crc.drawlinks(t)
  end
end

return M


