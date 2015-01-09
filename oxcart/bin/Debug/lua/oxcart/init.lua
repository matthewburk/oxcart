return function()
  require 'stdinit' {
    oxcart.path..'/lua/opengl',
    oxcart.path..'/lua/win32',
  }

  local _oxcart = oxcart
  local function null() end
  oxcart = {
    oncommand=null,
    onchar=null,
    onkeydown=null,
    onkeyup=null,
    onmouseenter=null,
    onmouseexit=null,
    onmousemove=null,
    onmousedoubleclick=null,
    onmousedown=null,
    onmouseup=null,
    onmousewheel=null,
    update=null,
    onwindowcreated=null,
    onwindowdestroyed=null,
    onwindowactivated=null,
    onwindowdeactivated=null,
    onwindowshown=null,
    onwindowhidden=null,
    onwindowmoved=null,
    onwindowmaximized=null,
    onwindowminimized=null,
    onwindowresized=null,
    onmousemoveraw=null,
  }

  local C = require 'oxcart.C'
  local rect = C.oxcart_get_window_rect()

  oxcart.window = { x=rect.x, y=rect.y, w=rect.w, h=rect.h }

  

  local cel = require 'cel'

  do --cel driver
    local driver = cel.installdriver(
    { --mouse
      states = { up = 'up', down = 'down' },
      buttons = { left = 0x01, middle = 0x02, right = 0x04 },
      wheel = { up = 1, down = -1 },
    },
    { --keyboard
      states = { up = 'up', down = 'down' },
      keys = {
        ['backspace'] = 0x01,
        ['tab'] = 0x02,
        ['enter'] = 0x03,
        ['shift'] = 0x04,
        ['ctrl'] = 0x05,
        ['alt'] = 0x06,
        ['pause'] = 0x07,
        ['capslock'] = 0x08,
        ['escape'] = 0x09,
        ['space'] = 0x0A,
        ['pageup'] = 0x0B,
        ['pagedown'] = 0x0C,
        ['end'] = 0x0D,
        ['home'] = 0x0E,
        ['left'] = 0x0F,
        ['up'] = 0x10,
        ['right'] = 0x11,
        ['down'] = 0x12,
        ['printscreen'] = 0x13,
        ['insert'] = 0x14,
        ['delete'] = 0x15,
        ['0'] = 0x16, ['1'] = 0x17, ['2'] = 0x18, ['3'] = 0x19, ['4'] = 0x1A, ['5'] = 0x1B, ['6'] = 0x1C,
        ['7'] = 0x1D, ['8'] = 0x1E, ['9'] = 0x1F, ['a'] = 0x20, ['b'] = 0x21, ['c'] = 0x22, ['d'] = 0x23,
        ['e'] = 0x24, ['f'] = 0x25, ['g'] = 0x26, ['h'] = 0x27, ['i'] = 0x28, ['j'] = 0x29, ['k'] = 0x2A,
        ['l'] = 0x2B, ['m'] = 0x2C, ['n'] = 0x2D, ['o'] = 0x2E, ['p'] = 0x2F, ['q'] = 0x30, ['r'] = 0x31,
        ['s'] = 0x32, ['t'] = 0x33, ['u'] = 0x34, ['v'] = 0x35, ['w'] = 0x36, ['x'] = 0x37, ['y'] = 0x38,
        ['z'] = 0x39,
        ['numpad0'] = 0x3A,
        ['numpad1'] = 0x3B, ['numpad2'] = 0x3C, ['numpad3'] = 0x3D,
        ['numpad4'] = 0x3E, ['numpad5'] = 0x3F, ['numpad6'] = 0x40,
        ['numpad7'] = 0x41, ['numpad8'] = 0x42, ['numpad9'] = 0x43,
        ['multiply'] = 0x44,
        ['add'] = 0x45,
        ['subtract'] = 0x46,
        ['decimal'] = 0x47,
        ['divide'] = 0x48,
        ['f1'] = 0x49, ['f2'] = 0x4A, ['f3'] = 0x4B, ['f4'] = 0x4C,
        ['f5'] = 0x4D, ['f6'] = 0x4E, ['f7'] = 0x4F, ['f8'] = 0x50,
        ['f9'] = 0x51, ['f10'] = 0x52, ['f11'] = 0x53, ['f12'] = 0x54,
        ['numlock'] = 0x55,
        ['scrolllock'] = 0x56,
        ['equal'] = 0x57,
        ['comma'] = 0x58,
        ['minus'] = 0x59,
        ['period'] = 0x5A,
        ['semicolon'] = 0x5B,
        ['slash'] = 0x5C,
        ['graveaccent'] = 0x5D,
        ['openbracket'] = 0x5E,
        ['backslash'] = 0x5F,
        ['closebracket'] = 0x60,
        ['apostrophe'] = 0x61,
      },
    })

    function driver.getface(metaface, key)
      if type(key) == 'string' then
        if #key == 9 and key:sub(1,1) == "#" then
          local r = tonumber(key:sub(2,3), 16)
          local g = tonumber(key:sub(4,5), 16)
          local b = tonumber(key:sub(6,7), 16)
          local a = tonumber(key:sub(8,9), 16)
          local color = cel.color.rgb8(r, g, b, a)
          return cel.getface('cel', '@color'):new {
            color=color
          }:weakregister(key):weakregister(color)
        elseif #key == 7 and key:sub(1,1) == "#" then
          local r = tonumber(key:sub(2,3), 16)
          local g = tonumber(key:sub(4,5), 16)
          local b = tonumber(key:sub(6,7), 16)
          local color = cel.color.rgb8(r, g, b)
          return cel.getface('cel', '@color'):new {
            color=color
          }:weakregister(key):weakregister(color)
        elseif #key == 4 then
          return cel.getface('cel', '@color'):new {
            color=key
          }:weakregister(key)
        end
        --do not register, so it can be collected when no longer used
      end
    end

    do
      require 'oxcart.font'

      local names = {
        code = 'firamono',
        monospace = 'firamono',
        serif = 'firasans',
        sansserif = 'firasans',
        default ='firasans',
      }

      function driver.loadfont(name, size)
        name = names[name] or name 

        local oxcartfont = oxcart.font.load(name, 'regular', size)

        local _, _, _, _, emadvance = oxcartfont:getglyphmetrics(string.byte(' '))

        local font = {
          oxcartfont = oxcartfont,
          textureuploaded = false,
          name = name,
          size = size,
          lineheight = math.ceil(oxcartfont.metrics.lineheight),
          ascent = math.ceil(-oxcartfont.metrics.ascent), --cel ascent is positive up TODO correct it
          descent = math.ceil(oxcartfont.metrics.descent), --cel descent is positive down
          emwidth = size, 
          emheight = size,
          emadvance = emadvance,
          metrics = setmetatable({}, {__index = function(glyphmetrics, utf8codepoint)
            --TODO support utf8
            local w, h, xbr, ybr, advance = oxcartfont:getglyphmetrics(string.byte(utf8codepoint))
            glyphmetrics[utf8codepoint] = {
              utf8codepoint = utf8codepoint,
              advance = math.floor(advance+.5), --TODO make cel work with floating point advance
              xmin = xbr,
              xmax = xbr + w,
              ymin = ybr,
              ymax = ybr + h,
            }
            --[[
            local t = glyphmetrics[utf8codepoint]
            dprint('xmin, xmax, ymin, ymax, advance', t.xmin, t.xmax, t.ymin, t.ymax, t.advance)
            --]]
            return glyphmetrics[utf8codepoint]
          end})
        }

        font.metrics['\n'] = {
          utf8codepoint = '\n',
          advance = 0,
          xmin = 0,
          xmax = 0,
          ymin = 0,
          ymax = 0,
        }--oxcart does not provide metrics for '\n' TODO fix cel, becuase shouldn't need metrics for '\n'

        return font 
      end
    end

    require 'oxcart.cel'

    driver.root:resize(oxcart.window.w, oxcart.window.h)

    oxcart.cel.root:link(driver.root, 'fill')

    function oxcart.cel.root:update(millis)
      driver.tick(millis)
    end

    do
      local events = {
        [0x01] = function() oxcart.onwindowcreated () end,
        [0x02] = function() oxcart.onwindowdestroyed () end,
        [0x03] = function() oxcart.onwindowactivated () end,
        [0x04] = function() oxcart.onwindowdeactivated () end,
        [0x05] = function() oxcart.onwindowshown () end,
        [0x06] = function() oxcart.onwindowhidden () end,
        [0x07] = function() oxcart.onwindowmoved () end,
        [0x08] = function() oxcart.onwindowmaximized () end,
        [0x09] = function() oxcart.onwindowminimized () end,
        [0x0A] = function()
          rect = C.oxcart_get_window_rect()
          oxcart.window.x = rect.x
          oxcart.window.y = rect.y
          oxcart.window.w = rect.w
          oxcart.window.h = rect.h
          driver.root:resize(rect.w, rect.h)
          oxcart.onwindowresized()
        end,
      }

      function _oxcart.onwindowevent(e)
        if events[e] then return events[e]() end
      end
    end

    function _oxcart.oncommand(command)
      driver.command(command)
      oxcart.oncommand(command)
    end

    function _oxcart.onchar(c)
      driver.char(c)
      oxcart.onchar(c)
    end

    function _oxcart.onkeydown(key, rep, alt, ctrl, shift)
      if not rep then
        driver.keydown(key, alt, ctrl, shift)
      end
      driver.keypress(key, alt, ctrl, shift)
      oxcart.onkeydown(key, rep, alt, ctrl, shift)
    end

    function _oxcart.onkeyup(key, alt, ctrl, shift)
      driver.keyup(key, alt, ctrl, shift)
      oxcart.onkeyup(key, alt, ctrl, shift)
    end

    function _oxcart.onmouseenter()
      oxcart.onmouseenter()
    end

    function _oxcart.onmouseexit()
      oxcart.onmouseexit()
    end

    function _oxcart.onmousemoveraw(x, y, alt, ctrl, shift)
      oxcart.onmousemoveraw(x, y, alt, ctrl, shift)
    end

    function _oxcart.onmousemove(x, y, alt, ctrl, shift)
      driver.mousemove(x, y)
      oxcart.onmousemove(x, y, alt, ctrl, shift)
    end

    function _oxcart.onmousedoubleclick(button, x, y, alt, ctrl, shift)
      oxcart.onmousedoubleclick(button, x, y, alt, ctrl, shift)
    end

    function _oxcart.onmousedown(button, x, y, alt, ctrl, shift)
      driver.mousedown(x, y, button, alt, ctrl, shift)
      oxcart.onmousedown(button, x, y, alt, ctrl, shift)
    end

    function _oxcart.onmouseup(button, x, y, alt, ctrl, shift)
      driver.mouseup(x, y, button, alt, ctrl, shift)
      oxcart.onmouseup(button, x, y, alt, ctrl, shift)
    end

    function _oxcart.onmousewheel(delta, step, x, y, alt, ctrl, shift)
      local direction 

      if delta > 0 then
        direction = cel.mouse.wheel.up
      else
        direction =  cel.mouse.wheel.down
      end

      delta = math.abs(delta)
      for i=1,delta do
        driver.mousewheel(x, y, direction, step, alt, ctrl, shift)
      end
      oxcart.onmousewheel(delta, step, x, y, alt, ctrl, shift)
    end

    function _oxcart.update(elapsed, frameelapsed)
      oxcart.update(elapsed, frameelapsed)
    end
  end

  function oxcart.iskeydown(key)
    return C.GetKeyState(key) > 0
  end

  require 'oxcart.main'
end
