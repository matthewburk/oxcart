local cel = require 'cel'
local colors = require 'oxcart.cel.colors'

local M = cel.getface('window')
M.textcolor = false
M.color = false
M.bordercolor = colors.gray2
M.bordersize = 4
M.modes = {}

local border = 5
local handle = 24

M.layout = {
  minw = border + (handle*3) + border,
  minh = border + handle + border,
  maxw = nil,
  maxh = nil,

  border = { 
    size = border,
    face = colors.oceanblue,
  },
  corner = { 
    size = border * 2,
    face = cel.color.tint(colors.oceanblue, .3),
  },
  handle = {
    w = handle,
    h = handle,    
    link = {'width.top', border, border},
  },
  client = {    
    w = 0,
    h = 0,
    link = {cel.rcomposelinker('fill.topmargin', 'fill.margin'), {handle,  border}},
  }
}

M.flow = {
  minimize = cel.flows.smooth(300),
  maximize = cel.flows.smooth(300),
  restore = cel.flows.smooth(300),
}

M.modes['focus'] = M:new {
  bordercolor = colors.gray2,
}

M.modes['mousefocusin'] = M:new {
  bordercolor = colors.gray4,
}

function M.select(f, t)
  if t.focus then
    f = f['focus'] or f
  elseif t.mousefocusin then
    f = f['mousefocusin'] or f
  end
  return f
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

do --window.handle
  local handle = cel.getface('window.handle')
  handle.color = colors.gray2
  handle.textcolor = colors.oceanblue
  handle.modes = {}

  handle.modes['focus'] = handle:new {
    textcolor = colors.gray2,
    color = colors.oceanblue,
  }

  function handle.select(f, t)
    if t.host.focus then
      f = f['focus'] or f
    end
    return f
  end

  function handle.draw(crc, f, t, X, Y)
    if f.color then
      crc.setcolor(f.color)
      crc.rectangle(X, Y, t.w, t.h)
      crc.draw()
    end

    if t.host.title and f.textcolor then
      crc.setfont(f.font)
      crc.settextcolor(f.textcolor)
      --a little hacky but will work unless a crazy font is used
      crc.addtext(t.host.title, X+f.font.metrics[' '].advance, Y+f.font.ascent)
      crc.drawtext()
    end

    if f.bordersize and f.bordercolor then
      crc.setcolor(f.bordercolor)
      crc.rectangleborder(X, Y, t.w, t.h, f.bordersize)
      crc.draw()
    end

    return crc.drawlinks(t)
  end
end

do --window.client
  local client = cel.getface('window.client')
  client.color = colors.gray4
  client.bordercolor = false
  client.bordersize = false

  client.draw = M.draw
end

return M
