local ffi = require("ffi")
local  new = ffi.new

--[[
All numeric calculations are performed with doubles. Using
floats for storage saves memory (for big arrays). But arithmetic
is usually slower due to the required float<->double conversions.
]]
ffi.cdef([[
  struct Matrix {
    float entries[16];
  };
  void* malloc(size_t);
  void free(void*);
]])

local C = ffi.C

local Matrix = ffi.typeof("struct Matrix")
local Matrix_p = ffi.typeof("struct Matrix*")
local MatrixMetatable = {}

function MatrixMetatable:__new()
  local m = ffi.cast(Matrix_p, C.malloc(ffi.sizeof(Matrix)))
  ffi.fill(m, ffi.sizeof(Matrix))
  return m
end

function MatrixMetatable:__gc()
  C.free(self)
end

MatrixMetatable.__index = MatrixMetatable

function MatrixMetatable:identity()
  for i=0, 15, 5 do self.entries[i] = 1 end
end

function MatrixMetatable:mul(m)
  local ret = Matrix()
  for i=0, 12, 4 do
    for j=0, 3 do
      ret.entries[i+j] = m.entries[j] * self.entries[i] + m.entries[j+4] * self.entries[i+1] + m.entries[j+8] * self.entries[i+2] + m.entries[j+12] * self.entries[i+3]
    end
  end
  return ret
end
function MatrixMetatable:clone()
  local ret = Matrix()
  for i=0, 15 do ret.entries[i] = self.entries[i] end
  return ret
end
function MatrixMetatable:inverse()
  local ret = Matrix()
  ret.entries[0] =  self.entries[5]  * self.entries[10] * self.entries[15] -
            self.entries[5]  * self.entries[11] * self.entries[14] -
            self.entries[9] * self.entries[6]  * self.entries[15] +
            self.entries[9] * self.entries[7]  * self.entries[14] +
            self.entries[13] * self.entries[6]  * self.entries[11] -
            self.entries[13] * self.entries[7]  * self.entries[10]
  ret.entries[4] = -self.entries[4]  * self.entries[10] * self.entries[15] +
            self.entries[4]  * self.entries[11] * self.entries[14] +
            self.entries[8]  * self.entries[6]  * self.entries[15] -
            self.entries[8]  * self.entries[7]  * self.entries[14] -
            self.entries[12] * self.entries[6]  * self.entries[11] +
            self.entries[12] * self.entries[7]  * self.entries[10]
  ret.entries[8] =  self.entries[4]  * self.entries[9] * self.entries[15] -
            self.entries[4]  * self.entries[11] * self.entries[13] -
            self.entries[8]  * self.entries[5]  * self.entries[15] +
            self.entries[8]  * self.entries[7]  * self.entries[13] +
            self.entries[12] * self.entries[5]  * self.entries[11] -
            self.entries[12] * self.entries[7]  * self.entries[9]
  ret.entries[12] = -self.entries[4]  * self.entries[9] * self.entries[14] +
            self.entries[4]  * self.entries[10] * self.entries[13] +
            self.entries[8]  * self.entries[5]  * self.entries[14] -
            self.entries[8]  * self.entries[6]  * self.entries[13] -
            self.entries[12] * self.entries[5]  * self.entries[10] +
            self.entries[12] * self.entries[6]  * self.entries[9]
  ret.entries[1] = -self.entries[1]  * self.entries[10] * self.entries[15] +
            self.entries[1]  * self.entries[11] * self.entries[14] +
            self.entries[9] * self.entries[2]  * self.entries[15] -
            self.entries[9] * self.entries[3]  * self.entries[14] -
            self.entries[13] * self.entries[2]  * self.entries[11] +
            self.entries[13] * self.entries[3]  * self.entries[10]
  ret.entries[5] =  self.entries[0]  * self.entries[10] * self.entries[15] -
            self.entries[0]  * self.entries[11] * self.entries[14] -
            self.entries[8]  * self.entries[2] * self.entries[15] +
            self.entries[8]  * self.entries[3] * self.entries[14] +
            self.entries[12] * self.entries[2] * self.entries[11] -
            self.entries[12] * self.entries[3] * self.entries[10]
  ret.entries[9] = -self.entries[0]  * self.entries[9] * self.entries[15] +
            self.entries[0]  * self.entries[11] * self.entries[13] +
            self.entries[8]  * self.entries[1]  * self.entries[15] -
            self.entries[8]  * self.entries[3]  * self.entries[13] -
            self.entries[12] * self.entries[1]  * self.entries[11] +
            self.entries[12] * self.entries[3]  * self.entries[9]
  ret.entries[13] = self.entries[0]  * self.entries[9] * self.entries[14] -
            self.entries[0]  * self.entries[10] * self.entries[13] -
            self.entries[8]  * self.entries[1] * self.entries[14] +
            self.entries[8]  * self.entries[2] * self.entries[13] +
            self.entries[12] * self.entries[1] * self.entries[10] -
            self.entries[12] * self.entries[2] * self.entries[9]
  ret.entries[2] = self.entries[1]  * self.entries[6] * self.entries[15] -
            self.entries[1]  * self.entries[7] * self.entries[14] -
            self.entries[5]  * self.entries[2] * self.entries[15] +
            self.entries[5]  * self.entries[3] * self.entries[14] +
            self.entries[13] * self.entries[2] * self.entries[7] -
            self.entries[13] * self.entries[3] * self.entries[6]
  ret.entries[6] = -self.entries[0]  * self.entries[6] * self.entries[15] +
            self.entries[0]  * self.entries[7] * self.entries[14] +
            self.entries[4]  * self.entries[2] * self.entries[15] -
            self.entries[4]  * self.entries[3] * self.entries[14] -
            self.entries[12] * self.entries[2] * self.entries[7] +
            self.entries[12] * self.entries[3] * self.entries[6]
  ret.entries[10] = self.entries[0]  * self.entries[5] * self.entries[15] -
            self.entries[0]  * self.entries[7] * self.entries[13] -
            self.entries[4]  * self.entries[1] * self.entries[15] +
            self.entries[4]  * self.entries[3] * self.entries[13] +
            self.entries[12] * self.entries[1] * self.entries[7] -
            self.entries[12] * self.entries[3] * self.entries[5]
  ret.entries[14] = -self.entries[0]  * self.entries[5] * self.entries[14] +
            self.entries[0]  * self.entries[6] * self.entries[13] +
            self.entries[4]  * self.entries[1] * self.entries[14] -
            self.entries[4]  * self.entries[2] * self.entries[13] -
            self.entries[12] * self.entries[1] * self.entries[6] +
            self.entries[12] * self.entries[2] * self.entries[5]
  ret.entries[3] = -self.entries[1]  * self.entries[6] * self.entries[11] +
            self.entries[1]  * self.entries[7] * self.entries[10] +
            self.entries[5]  * self.entries[2] * self.entries[11] -
            self.entries[5]  * self.entries[3] * self.entries[10] -
            self.entries[9] * self.entries[2] * self.entries[7] +
            self.entries[9] * self.entries[3] * self.entries[6]
  ret.entries[7] = self.entries[0] * self.entries[6] * self.entries[11] -
            self.entries[0] * self.entries[7] * self.entries[10] -
            self.entries[4] * self.entries[2] * self.entries[11] +
            self.entries[4] * self.entries[3] * self.entries[10] +
            self.entries[8] * self.entries[2] * self.entries[7] -
            self.entries[8] * self.entries[3] * self.entries[6]
  ret.entries[11] = -self.entries[0] * self.entries[5] * self.entries[11] +
            self.entries[0] * self.entries[7] * self.entries[9] +
            self.entries[4] * self.entries[1] * self.entries[11] -
            self.entries[4] * self.entries[3] * self.entries[9] -
            self.entries[8] * self.entries[1] * self.entries[7] +
            self.entries[8] * self.entries[3] * self.entries[5]
  ret.entries[15] = self.entries[0] * self.entries[5] * self.entries[10] -
            self.entries[0] * self.entries[6] * self.entries[9] -
            self.entries[4] * self.entries[1] * self.entries[10] +
            self.entries[4] * self.entries[2] * self.entries[9] +
            self.entries[8] * self.entries[1] * self.entries[6] -
            self.entries[8] * self.entries[2] * self.entries[5]
  local det = self.entries[0] * ret.entries[0] + self.entries[1] * ret.entries[4] + self.entries[2] * ret.entries[8] + self.entries[3] * ret.entries[12]
  if det == 0 then return self:clone() end -- << must return new matrix not self
  det = 1.0 / det
  for i=0, 15 do ret.entries[i] = ret.entries[i] * det end
  return ret
end

Matrix = ffi.metatype(Matrix, MatrixMetatable)


local mOut
local m = Matrix()
m:identity()

local ITERATIONS = 300000

local t = os.clock()
for i=1, ITERATIONS do
  mOut = m:mul(m:inverse())
end
local time = os.clock() - t
print("time", time, "iterations per sec:", ITERATIONS / time)

