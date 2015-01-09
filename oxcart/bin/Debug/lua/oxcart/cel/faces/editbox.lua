local cel = require 'cel'
local colors = require 'oxcart.cel.colors'

cel.getface('cel', colors.themecomplement):register('@caret')

local M = cel.getface('editbox')
M.color = cel.color.setalpha(colors.white, .4)
M.bordersize = 1
M.bordercolor = colors.oceanblue

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

do
  M.text = cel.getface('editbox.text')
  M.text.textcolor = colors.oceanblue
  M.text.selectedtextcolor = colors.themecomplement
  M.text.color = false 
  M.text.bordercolor = false
  M.text.bordersize = false 

  M.text.layout = {
    padding = {
      l=function(w, h, font) return math.floor(font.emadvance/2) end,
      t=function(w, h, font) return font.emheight/2 end,
    },
  }

  function M.text.draw(crc, f, t, X, Y)
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

      if t.selectionx then
        local clip = crc.addclip(X+t.selectionx, Y+t.selectiony, t.selectionw, t.selectionh)

        crc.setcolor(f.textcolor)
        crc.rectangle(X+t.selectionx, Y+t.selectiony, t.selectionw, t.selectionh)
        crc.draw()

        crc.settextcolor(f.selectedtextcolor)
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
        crc.setclip(clip)
      end
      --[[
      if t.selectionx then
        cr:save()
        cr:rectangle(t.selectionx, t.selectiony, t.selectionw, t.selectionh)
        cr:clip()
        cr:paint()
        cr:set_source_color(f.selectedtextcolor)

        for i, line in ipairs(t.lines) do
          if line.peny > ymin then
            if line.peny > ymax then
              break
            else
              cr:drawstring(t.font, line.penx, line.peny, line.text)
            end
          end
        end

        cr:restore()
      end
      --]]
    end

    if f.bordersize and f.bordercolor then
      crc.setcolor(f.bordercolor)
      crc.rectangleborder(X, Y, t.w, t.h, f.bordersize)
      crc.draw()
    end

    return crc.drawlinks(t)
  end
end

return M



