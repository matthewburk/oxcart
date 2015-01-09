local ffi = require 'ffi'
local gl = require 'opengl'
local bit = require 'bit'
local C = ffi.C
local cel = require 'oxcart.cel'
require 'oxcart.program'
require 'oxcart.camera'

local gui = cel.new():link(cel.root, 'fill')

local program = oxcart.program.new {
  oxcart.program.newvertexshader[[
  #version 400 core
  layout(std140) uniform;

  uniform matrices {
    mat4 m_projection;
    mat4 m_view;
    mat4 m_model;
  };

  const vec4 diffuse = vec4(0, 1, 0, 1);
  const vec4 ambient = vec4(0, .25, 0, 1);
  const vec4 l_dir = normalize(vec4(1, 1, .5, 0));    // world space

  in vec3 position;
  in vec3 normal;
  in vec4 color;

  out vec4 vs_color;

  void main()
  {
    gl_Position = m_projection * m_view * m_model * vec4(position, 1.0);

    // transform normal to world space and normalize it
    vec4 n = normalize(m_model * vec4(normal, 0));

    float intensity = max(dot(n, l_dir), 0.0);
   
    vs_color = max(intensity * diffuse, ambient);
  }
  ]],

  oxcart.program.newfragmentshader[[
  #version 400 core
  layout(std140) uniform;

  in vec4 vs_color;
  out vec4 fs_color;

  void main()
  {
    fs_color = vs_color;
  }
  ]],
}

local mchunk = require 'oxcart.chunk'

local function drawchunk(chunk)
  --gl.glPolygonMode(GL_FRONT_AND_BACK, GL_LINE)

  gl.glUseProgram(program.id)
  gl.glBindVertexArray(chunk.vao)
  gl.glDrawElements(GL_TRIANGLES, chunk.nelements, GL_UNSIGNED_INT, nil)

  --gl.glPolygonMode(GL_FRONT_AND_BACK, GL_FILL)
end

local matrices = ffi.new('oxcart_matrices_t') do
  matrices.projection = C.mat4_perspective(45, oxcart.window.w / oxcart.window.h, 1, 10000)
  matrices.view = C.mat4_identity()
  matrices.model = C.mat4_identity()
end

function oxcart.onwindowresized()
  matrices.projection = C.mat4_perspective(45, oxcart.window.w / oxcart.window.h, 1, 10000)
  gl.glViewport(0, 0, oxcart.window.w, oxcart.window.h)
end

local math = require 'oxcart.math'
local vunit = 2
local chunks = {}
for x = -1, 1 do
  for z = -1, 1 do
    chunks[#chunks+1] = mchunk.new(x*(vunit*31), 0, z*(vunit*31))
  end
end

do --grip
  local grip = cel.grip.new(100, 100)
    :link(gui, 'center') --so it is centered
    :relink() --so it can be moved freely

  function grip:onmousedown()
    grip:raise() --put on top of other cels
  end

  grip:grip(grip) --makes it move itself when dragged with mouse
end

local window do --window
  window = cel.window.new(300, 300)
    :settitle('A window with stuff')
    :adddefaultcontrols()
    :link(gui)

  local scroll = cel.scroll.new()
    :link(window, 'fill')

  cel.col { gap=2, 
    { face='#888888', --make col slots light gray
      link='center',
      
      cel.editbox.new('HELLO', 200),
      cel.editbox.new('hello_again', 200),
      cel.label.new('label'),
      cel.button.new(200, 50),
      cel.text.new('text\nit wraps'),
      cel.textbutton.new('text\nbutton'),
      cel.text.new [[[
      if the text starts with '['
      and a new line then the extra space
      at the beginning of each of these
      lines is removed. The trailing new line
      is also removed.  Handy! 
      ]],
      cel.text.new [[
      Here is one wihout the extra '['.
      Notice all the extra space.
      ]],
      
    },

    { face='#889988', --make col slots light gray with a bit more green
      link = 'width', --so that text will shink and grow 
      cel.text.new([[[
      By deafult text wraps at new lines, 
      but you can make it wrap at word boundaries as well.  Here is the code form the 'scroll' linker.
      M['scroll'] = function(hw, hh, x, y, w, h, fillx, filly)
        if x >= 0 or w <= hw then 
          x = 0
        elseif x + w < hw then
          x = hw - w
        end

        if y >= 0 or h <= hh then
          y = 0
        elseif y + h < hh then
          y = hh - h
        end

        return x, y, fillx and hw or w, filly and hh or h
      end
      ]]):wrap('word'),
    }
  }:link(scroll, 'scroll', true, false) --look at cel.linkers['scroll'] to see what this does

  function window:onmousedown()
    window:raise() --put on top of other cels
  end
end

local fpslabel = cel.label.new('')
  :link(gui, 'center.top')

local camera = oxcart.camera.new()
  camera:moveto(0, 64, 100)
  matrices.view = camera.transform

local keys = cel.keyboard.keys
local function updatecamera()
  local speed = .1
  if cel.keyboard:isdown(keys.w) then
    camera:moveforward(speed)
    matrices.view = camera.transform
  end

  if cel.keyboard:isdown(keys.s) then
    camera:moveforward(-speed)
    matrices.view = camera.transform
  end

  if cel.keyboard:isdown(keys.a) then
    camera:moveright(-speed)
    matrices.view = camera.transform
  end

  if cel.keyboard:isdown(keys.d) then
    camera:moveright(speed)
    matrices.view = camera.transform
  end
end

function oxcart.update(elapsed, deltaelapsed)
  gl.glEnable(GL_CULL_FACE)
  gl.glCullFace(GL_BACK);
  gl.glEnable(GL_DEPTH_TEST)
  gl.glDepthFunc(GL_LEQUAL)
  gl.glEnable(GL_MULTISAMPLE)
  gl.glClearColor(0, 0, 0, 0)
  gl.glClear(bit.bor(GL_COLOR_BUFFER_BIT, GL_DEPTH_BUFFER_BIT))

  updatecamera()
  local rotate = C.mat4_rotate(0, 1, 0, elapsed/100 % 360)
  --oxcart.updatemodel(rotate)
  oxcart.uniformbuffer.matrices.update(matrices)

  for i = 1, #chunks do
    drawchunk(chunks[i])
  end

  fpslabel:printf('fps:%d', 1/(deltaelapsed/1000))

  cel.root:update(elapsed) --drives cel time and updates animations
  cel.root:draw() --draws cel
end

do --TODO pause game?
  local update = oxcart.update
  local idle = function() C.Sleep(100) end

  function oxcart.onwindowdeactivated(event)
    oxcart.update = idle
  end

  function oxcart.onwindowactivated(event)
    oxcart.update = update 
  end
end

do
  do
    local polar = 120 * 10
    local azimuth = 0
    camera:lookat(polar, azimuth)
    matrices.view = camera.transform

    function oxcart.onmousemoveraw(x, y)
      if cel.mouse:isdown(cel.mouse.buttons.right) then
        polar = math.min(170*10, math.max(10*10, polar + y))
        azimuth = (azimuth + x) % (360*10)

        camera:lookat(polar/10, azimuth/10)
        matrices.view = camera.transform
      end
    end
  end

  function oxcart.onkeydown(key, alt, ctrl, shift)
    if key == cel.keyboard.keys.escape then
      if gui:islinkedto(cel.root) then
        gui:unlink()
      else
        gui:link(cel.root, 'fill')
      end
    end
  end
end

--TODO fix cel bug that allows a negative h when font ascent is negative
