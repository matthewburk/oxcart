local cel = require 'cel'
local colors = require 'oxcart.cel.colors'

local M = cel.getface('scroll')
M.textcolor = false
M.color = false
M.bordercolor = false
M.bordersize = false

M.flow = {
  scroll = cel.flows.linear(100);
  showybar = cel.flows.linear(500);
  hideybar = cel.flows.linear(100);
  showxbar = cel.flows.linear(500);
  hidexbar = cel.flows.linear(100);
}

local size = 20
M.layout = {
  stepsize = 20,
  ybar = {
    align = 'right',
    size = size, 
    track = {
      size = size,
      link = {'fill.margin', 0, size},
      thumb = {
        minsize = 20,
        size = size,
      };
    },
    decbutton = {
      size = size,
      link = 'width.top',
    },
    incbutton = {
      size = size,
      link = {'width.bottom'},
    },
  },
  xbar = {
    align = 'bottom',
    size = size,
    track = {
      size = size,
      link = {'fill.margin', size, 0},
      thumb = {
        minsize = 10,
        size = size,
      },
    },
    decbutton = {
      size = size,
      link = {'left.height'},
    },
    incbutton = {
      size = size,
      link = {'right.height'},
    },
  },
}

--scrollbar
local scrollbar = cel.getface('scroll.bar')
scrollbar.color = colors.gray4

--track
local track = cel.getface('scroll.bar.track')
track.select = false --override defualt button modes 

--thumb
local thumb = cel.getface('scroll.bar.thumb')
thumb.color = colors.gray2
thumb.bordercolor = cel.color.rgb(0, 1, 1)
thumb.accentcolor = cel.color.rgb(0, 1, 1)

function thumb.draw(crc, f, t, X, Y, size)
  size = size or t.host.host.size/10

  if f.color then
    crc.setcolor(f.color)
    crc.rectangle(X, Y, t.w, t.h)
  end
  if f.bordersize and f.bordercolor then
    crc.setcolor(f.bordercolor)
    crc.rectangleborder(X, Y, t.w, t.h, f.bordersize)
  end
  if f.accentcolor then
    crc.setcolor(f.accentcolor)
    crc.rectangle(X+(t.w/2)-size/2, Y+(t.h/2)-size/2, size, size)
  end

  crc.draw()

  return crc.drawlinks(t)
end

do --incbutton
  local incbutton = cel.getface('scroll.bar.inc')
  incbutton.color = colors.gray2
  incbutton.bordercolor = cel.color.rgb(0, 1, 1)
  incbutton.accentcolor = cel.color.rgb(0, 1, 1)
  incbutton.draw = function(_ENV, cr, f, t) return thumb.cairodraw(_ENV, cr, f, t, t.host.size) end
  incbutton.modes = {}

  incbutton.modes['pressed'] = incbutton:new {
    color = cel.color.rgb(.4, .4, .4),
    bordercolor = cel.color.rgb(0, 1, 1),
  }

  incbutton.modes['mousefocusin'] = incbutton:new {
    color = cel.color.rgb(.4, .4, .4),
    bordercolor = cel.color.rgb(0, 1, 1),
  }
  
  incbutton.modes['pressed'] = incbutton.modes['mousefocusin']:new {
    textcolor = cel.color.rgb(.2, .2, .2),
    color = cel.color.rgb(0, .8, .8),
    bordercolor = cel.color.rgb(0, 1, 1),
    bordersize = 2,
  }

  function incbutton.draw(crc, f, t, X, Y, size)
    return thumb.draw(crc, f, t, X, Y, t.host.size/10)
  end
end

do --decbutton
  local decbutton = cel.getface('scroll.bar.dec')
  decbutton.color = colors.gray2
  decbutton.bordercolor = cel.color.rgb(0, 1, 1)
  decbutton.accentcolor = cel.color.rgb(0, 1, 1)
  decbutton.modes = {}

  decbutton.modes['pressed'] = decbutton:new {
    color = cel.color.rgb(.4, .4, .4),
    bordercolor = cel.color.rgb(0, 1, 1),
  }

  decbutton.modes['mousefocusin'] = decbutton:new {
    color = cel.color.rgb(.4, .4, .4),
    bordercolor = cel.color.rgb(0, 1, 1),
  }
  
  decbutton.modes['pressed'] = decbutton.modes['mousefocusin']:new {
    textcolor = cel.color.rgb(.2, .2, .2),
    color = cel.color.rgb(0, .8, .8),
    bordercolor = cel.color.rgb(0, 1, 1),
    bordersize = 2,
  }

  function decbutton.draw(crc, f, t, X, Y, size)
    return thumb.draw(crc, f, t, X, Y, t.host.size/10)
  end
end


