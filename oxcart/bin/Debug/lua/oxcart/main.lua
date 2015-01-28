local ffi = require 'ffi'
local gl = require 'opengl'
local bit = require 'bit'
local C = ffi.C
local math = require 'oxcart.math'
require 'oxcart.program'
require 'oxcart.camera'
require 'oxcart.chunk'
require 'oxcart.gui'
require 'oxcart.terrain'
require 'oxcart.creature'

local wc = {
  x = 0,
  y = 0,
  z = 0,
}

dprint('wc', wc.x, wc.y, wc.z)

local mouse = oxcart.gui.mouse
local keyboard = oxcart.gui.keyboard
local camera = oxcart.camera.new()
local cameraspeed = .1
local creature = oxcart.creature.new():moveto(0, 10, 0)
do
  local polar = 120 * 10
  local azimuth = 0
  camera:lookat(polar, azimuth)
  camera:moveto(wc.x, wc.y+64, 10)

  function oxcart.onmousemoveraw(x, y)
    if mouse:isdown(mouse.right) then
      polar = math.min(170*10, math.max(10*10, polar + y))
      azimuth = (azimuth + x) % (360*10)

      camera:lookat(polar/10, azimuth/10)
    end
  end
end

local function updatecamera()
  cameraspeed = oxcart.gui.cameraspeed or .03
  if keyboard:isdown(keyboard.space) then
    if keyboard:isdown(keyboard.shift) then
      camera:moveup(-cameraspeed)
    else
      camera:moveup(cameraspeed)
    end
  end

  if keyboard:isdown(keyboard.w) then
    camera:moveforward(cameraspeed)
  end

  if keyboard:isdown(keyboard.s) then
    camera:moveforward(-cameraspeed)
  end

  if keyboard:isdown(keyboard.a) then
    camera:moveright(-cameraspeed)
  end

  if keyboard:isdown(keyboard.d) then
    camera:moveright(cameraspeed)
  end

  local speed=.01
  if keyboard:isdown(keyboard.left) then creature:moveto(creature.x-speed, creature.y, creature.z) end
  if keyboard:isdown(keyboard.right) then creature:moveto(creature.x+speed, creature.y, creature.z) end
  if keyboard:isdown(keyboard.up) then creature:moveto(creature.x, creature.y, creature.z-speed) end
  if keyboard:isdown(keyboard.down) then creature:moveto(creature.x, creature.y, creature.z+speed) end
end

local mat4_identity = C.mat4_identity()

local matrices = ffi.new('oxcart_matrices_t') do
  matrices.projection = C.mat4_perspective(45, oxcart.window.w / oxcart.window.h, 1, 10000)
  matrices.view = C.mat4_identity()
  matrices.model = C.mat4_identity()
  matrices.normaltransform = C.mat3_identity()
end

function oxcart.onwindowresized()
  matrices.projection = C.mat4_perspective(45, oxcart.window.w / oxcart.window.h, 1, 10000)
  gl.glViewport(0, 0, oxcart.window.w, oxcart.window.h)
end

local function drawbuffers(buffers)
  gl.glEnable(GL_CULL_FACE)
  gl.glCullFace(GL_BACK);
  gl.glEnable(GL_DEPTH_TEST)
  gl.glDepthFunc(GL_LEQUAL)
  gl.glEnable(GL_MULTISAMPLE)

  --gl.glPolygonMode(GL_FRONT_AND_BACK, GL_LINE)


  for i = 1, #buffers do
    local buffer = buffers[i]
    matrices.model = buffer.transform or mat4_identity 
    oxcart.uniformbuffer.matrices.update(matrices)

    gl.glUseProgram(buffer.program.id)
    gl.glBindVertexArray(buffer.vao)
    gl.glDrawElements(GL_TRIANGLES, buffer.nelements, GL_UNSIGNED_INT, nil)
  end

  --gl.glPolygonMode(GL_FRONT_AND_BACK, GL_FILL)
end

local light = ffi.new('oxcart_light_t', {
  surfacetolight = ffi.new('vec4_t', {1, 1, 0, 0}),
  color = ffi.new('vec4_t', {1, 1, 1, 1}),
  ambientcoefficient = .25,
})
C.vec4_normalize(light.surfacetolight)

function oxcart.idleupdate(elapsed, deltaelapsed)
  C.Sleep(100)
end

function oxcart.activeupdate(elapsed, deltaelapsed)
  gl.glClearColor(0, 0, 0, 0)
  gl.glClear(bit.bor(GL_COLOR_BUFFER_BIT, GL_DEPTH_BUFFER_BIT))

  updatecamera()
  local buffers = {}
  oxcart.terrain.updatebuffers(buffers, camera.position.x, 0, camera.position.z, 5)

  local y = oxcart.terrain.getelevation(creature.x, creature.z)
  if y then
    dprint('y', y)
    creature:moveto(creature.x, y+1, creature.z)
  end

  --camera:lookatpoint(creature.x, creature.y, creature.z)
  matrices.view = camera.transform

  oxcart.uniformbuffer.matrices.update(matrices)
  oxcart.uniformbuffer.light.update(light)

  buffers[#buffers+1] = creature.buffer
  drawbuffers(buffers)

  oxcart.gui.update(elapsed, deltaelapsed)
  oxcart.gui.draw()
end

oxcart.update = oxcart.activeupdate

function oxcart.onwindowdeactivated(event)
  oxcart.update = oxcart.idleupdate
end

function oxcart.onwindowactivated(event)
  oxcart.update = oxcart.activeupdate 
end

function oxcart.onkeydown(key, alt, ctrl, shift)
  if key == keyboard.escape then
    if oxcart.gui.islinked() then
      oxcart.gui.unlink()
    else
      oxcart.gui.link()
    end
    collectgarbage()
  elseif key == keyboard.tab then
    collectgarbage()
  end
end
      
collectgarbage('setpause', 101)
--collectgarbage('setstepmul', 1000)



