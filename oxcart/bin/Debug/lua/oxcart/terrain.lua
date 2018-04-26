local ffi = require 'ffi'
local gl = require 'opengl'
local math = require 'oxcart.math'
local vector = require 'oxcart.vector'
local C = ffi.C
local mat4 = math.mat4
require 'oxcart.geometry'
require 'oxcart.program'
require 'oxcart.chunk'

local M = {}

local chunksize = 32 
local nullchunk = oxcart.chunk.new(chunksize)
ffi.fill(nullchunk.data, ffi.sizeof(nullchunk.chunk_t), -1)

local function stitchchunks(chunk, l, r, f, k)
  l = l or nullchunk
  r = r or nullchunk
  f = f or nullchunk
  k = k or nullchunk

  local size = chunk.size
  local data = chunk.data
  l = l.data
  r = r.data
  f = f.data
  k = k.data

  do --left
    local togrid = data[0]
    local fromgrid = l[size]

    for z = 1, size do
      local tocol = togrid[z]
      local fromcol = fromgrid[z]
      for y = 1, size do
        tocol[y].value = fromcol[y].value
      end
    end
  end

  do --right
    local togrid = data[size+1]
    local fromgrid = r[1]

    for z = 1, size do
      local tocol = togrid[z]
      local fromcol = fromgrid[z]
      for y = 1, size do
        tocol[y] .value= fromcol[y].value
      end
    end
  end

  do --front
    for x = 1, size do
      local tocol = data[x][size+1]
      local fromcol = f[x][1]
      for y = 1, size do
        tocol[y] .value= fromcol[y].value
      end
    end
  end

  do --back
    for x = 1, size do
      local tocol = data[x][0]
      local fromcol = k[x][size]
      for y = 1, size do
        tocol[y] .value= fromcol[y].value
      end
    end
  end
end

--integer indices are free chunks
local heap = {}

for i = 1, 20*20 do
  heap[i] = oxcart.chunk.new(chunksize)
  ffi.fill(heap[i].data, ffi.sizeof(heap[i].chunk_t), -1)
end

local function alloc(key, x, y, z)
  local chunk = heap[#heap]

  if not chunk then
    chunk = oxcart.chunk.new(chunksize)
    heap[chunk] = chunk
  else
    heap[#heap] = nil
  end

  ffi.fill(chunk.data, ffi.sizeof(chunk.chunk_t), -1)
  chunk.key = key
  chunk.x = x
  chunk.y = y
  chunk.z = z
    
  return chunk
end

local function free(chunk)
  heap[#heap+1] = chunk
  heap[chunk] = nil
  chunk:reset()
end


local grid = {
  chunks={},
  chunkswap={},
  loadingchunks={},
}

local function getchunk(x, y, z)
  local key = string.format('%d,%d,%d', x, y, z)
  return grid.chunks[key]
end

function setgridaabb(_x1, _y1, _z1, _x2, _y2, _z2) 
  for i = 1, #grid.chunks do
    local chunk = grid.chunks[i]
    grid.chunks[i] = nil
    grid.chunks[chunk.key] = nil

    local x1, y1, z1 = chunk.x, chunk.y, chunk.z
    local x2, y2, z2 = chunk.x+1, chunk.y+1, chunk.z+1

    if x2 < _x1 
    or x1 > _x2 
    or y2 < _y1 
    or y1 > _y2 
    or z2 < _z1 
    or z1 > _z2 
    then
      free(chunk)
    else
      grid.chunkswap[#grid.chunkswap+1] = chunk
      grid.chunkswap[chunk.key] = chunk
    end
  end

  local temp = grid.chunks
  grid.chunks = grid.chunkswap
  grid.chunkswap = temp

  --fill in missing chunks
  for x = _x1, _x2-1 do
    for y = _y1, _y2-1 do
      for z = _z1, _z2-1 do
        local key = string.format('%d,%d,%d', x, y, z)
        if not grid.chunks[key] then
          local chunk = alloc(key, x, y, z)
          grid.loadingchunks[#grid.loadingchunks+1] = chunk
          grid.chunks[key] = chunk
          grid.chunks[#grid.chunks+1] = chunk
        end
      end
    end
  end
end

local function readcolor(blockcolor, blockvalue, x, y, z)
  --local perlin = perlin2d((1000+x)/10, (1000+z)/10)
  blockcolor.g = 1-- math.lerp(.95, 1, perlin) 
  blockcolor.r = .1 --math.lerp(.1, .2, perlin2d((1000+x)/100, (1000+z)/100))
  blockcolor.b = 0
  blockcolor.a = 1
end

local perlin2d = math.noise.simplex2d

local function fillchunk(chunk, x, y, z, left, right, front, back)
  local data = chunk.data
  local size = chunk.size
  local wx = chunk.x*size
  local wy = chunk.y*size
  local wz = chunk.z*size

  for x = 1, size do
    local grid = data[x] 
    local samplex = wx+x

    for z = 1, size do
      local col = grid[z]
      local samplez = wz+z

      local perlin = (perlin2d((1000+samplex)/10, (1000+samplez)/10) + 1)/2
      local r = 0
      local g = math.lerp(200, 255, perlin)
      local b = 0
      --blockcolor.r = .1 --math.lerp(.1, .2, perlin2d((1000+x)/100, (1000+z)/100))
      --blockcolor.b = 0
      --blockcolor.a = 1

      local colh = math.floor(4*size * (perlin2d(samplex/300, samplez/300)+1)/2) - size*y

      for y = 1, colh do
        col[y].value = 1
        col[y].r = r
        col[y].g = g
        col[y].b = b
      end
      for y = colh+1, size do
        col[y].value = 0
      end
    end
  end
end



---[[
local loadchunks = coroutine.wrap(function()
  while true do
    local chunk = grid.loadingchunks[#grid.loadingchunks]

    if chunk then
      grid.loadingchunks[#grid.loadingchunks] = nil
      fillchunk(chunk, chunk.x, chunk.y, chunk.z)
      local l = getchunk(chunk.x-1, chunk.y, chunk.z+0)
      local r = getchunk(chunk.x+1, chunk.y, chunk.z+0)
      local f = getchunk(chunk.x+0, chunk.y, chunk.z+1)
      local k = getchunk(chunk.x+0, chunk.y, chunk.z-1)

      coroutine.yield()
      stitchchunks(chunk, l, r, f, k)
      coroutine.yield()
      oxcart.chunk.totriangles(chunk, readcolor)
      chunk.transform = mat4.translate(chunk.x*chunk.size, chunk.y*chunk.size, chunk.z*chunk.size)
      chunk.aabb:transform(chunk.transform)
      
    end
    coroutine.yield()
  end
end)
--]]
--[[
local loadchunks = function()
  for i = 1, #grid.loadingchunks do
    local chunk = grid.loadingchunks[i]
    grid.loadingchunks[i] = nil
    fillchunk(chunk, chunk.x, chunk.y, chunk.z)
    local l = getchunk(chunk.x-1, chunk.y, chunk.z+0)
    local r = getchunk(chunk.x+1, chunk.y, chunk.z+0)
    local f = getchunk(chunk.x+0, chunk.y, chunk.z+1)
    local k = getchunk(chunk.x+0, chunk.y, chunk.z-1)

    stitchchunks(chunk, l, r, f, k)
    oxcart.chunk.totriangles(chunk, readcolor)
    chunk.transform = mat4.translate(chunk.x*chunk.size, chunk.y*chunk.size, chunk.z*chunk.size)
    chunk.aabb:transform(chunk.transform)
  end
end
--]]

--wx, wy, wz is center of sphere in world coordinates
--radius of sphere chunks
--chunks are loaded and unloaded as necessary
function M.updatechunks(chunks, wx, wy, wz, radius)
  wy = 0 --TODO

  local x = math.floor(wx/chunksize)
  local y = math.floor(wy/chunksize)
  local z = math.floor(wz/chunksize)

  setgridaabb(x-radius, 0, z-radius, x+radius+1, 4, z+radius+1)

  loadchunks()

  local nchunks = 0
  for i = 1, #grid.chunks do
    if grid.chunks[i].triangles then
      nchunks = nchunks + 1
      chunks[nchunks] = grid.chunks[i]
    end
  end

  return chunks, nchunks
end


function M.getelevation(wx, wz)
  local tx=wx
  local tz=wz

  local x = math.floor(tx/chunksize)
  local z = math.floor(tz/chunksize)

  local chunk = getchunk(x, 0, z)
  if chunk then
    x = 1+math.floor(tx%chunksize)
    z = 1+math.floor(tz%chunksize)

    local col = chunk.data[x][z]
    
    for y = chunksize, 1, -1 do
      if col[y].value > 0 then
        return (chunk.y*chunk.size)+y-1
      end
    end
    return 0
  end
end

oxcart.terrain = M

--[[
local L = require 'oxcart.L'

L.new()
--]]


return M
