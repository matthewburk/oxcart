local ffi = require 'ffi'
local gl = require 'opengl'
local bit = require 'bit'
local C = ffi.C
local math = require 'oxcart.math'
require 'oxcart.program'
require 'oxcart.camera'
require 'oxcart.chunk'
require 'oxcart.gui'

local mouse = oxcart.gui.mouse
local keyboard = oxcart.gui.keyboard
local camera = oxcart.camera.new()

do
  local polar = 120 * 10
  local azimuth = 0
  camera:lookat(polar, azimuth)
  camera:moveto(0, 64, 100)

  function oxcart.onmousemoveraw(x, y)
    if mouse:isdown(mouse.right) then
      polar = math.min(170*10, math.max(10*10, polar + y))
      azimuth = (azimuth + x) % (360*10)

      camera:lookat(polar/10, azimuth/10)
    end
  end
end

local function updatecamera()
  local speed = .1

  if keyboard:isdown(keyboard.space) then
    if keyboard:isdown(keyboard.shift) then
      camera:moveup(-speed)
    else
      camera:moveup(speed)
    end
  end

  if keyboard:isdown(keyboard.w) then
    camera:moveforward(speed)
  end

  if keyboard:isdown(keyboard.s) then
    camera:moveforward(-speed)
  end

  if keyboard:isdown(keyboard.a) then
    camera:moveright(-speed)
  end

  if keyboard:isdown(keyboard.d) then
    camera:moveright(speed)
  end
end

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
    gl.glUseProgram(buffer.program.id)
    gl.glBindVertexArray(buffer.vao)
    gl.glDrawElements(GL_TRIANGLES, buffer.nelements, GL_UNSIGNED_INT, nil)
  end

  --gl.glPolygonMode(GL_FRONT_AND_BACK, GL_FILL)
end

local chunks = {}
local buffers = {}
local terrainsize = 5

for x = -terrainsize, terrainsize do
  for z = -terrainsize, terrainsize do
    chunks[#chunks+1] = oxcart.chunk.new(31)
  end
end

do
  local i = 1
  for x = -terrainsize, terrainsize do
    for z = -terrainsize, terrainsize do
      local chunk = chunks[i]
      oxcart.chunk.fillterrain(chunk, x*(chunk.size), 0, z*(chunk.size))
      buffers[i] = oxcart.chunk.totriangles(chunk, x*chunk.size, 0, z*chunk.size)
      i = i+1
    end
  end
end

local light = ffi.new('oxcart_light_t', {
  surfacetolight = ffi.new('vec4_t', {1, 1, 0, 0}),
  color = ffi.new('vec4_t', {.5, .5, .5, 1}),
  ambientcoefficient = .3,
})
C.vec4_normalize(light.surfacetolight)

function oxcart.idleupdate(elapsed, deltaelapsed)
  C.Sleep(100)
end

function oxcart.activeupdate(elapsed, deltaelapsed)
  gl.glClearColor(0, 0, 0, 0)
  gl.glClear(bit.bor(GL_COLOR_BUFFER_BIT, GL_DEPTH_BUFFER_BIT))

  updatecamera()
  matrices.view = camera.transform

  oxcart.uniformbuffer.matrices.update(matrices)
  oxcart.uniformbuffer.light.update(light)

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
  end
end
      
oxcart.gui.unlink()



