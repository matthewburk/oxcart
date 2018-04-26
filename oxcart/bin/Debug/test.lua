local ffi = require "ffi"

ffi.cdef[[
	void* malloc(size_t size);
	void* realloc (void* ptr, size_t size);
	void* calloc(size_t num, size_t size);
	void free(void* ptr);
	
	typedef struct _MATRIX4_ {
		float* data;
	} Matrix4;
]]

local Identity = false
local TransposeHelper = false
local TranslateHelper = false
local MultiplyHelper = false
local C = ffi.C

local function donothing() 
  print('donothink')
end
local Create = function(src)
	if src == nil then
		src = Identity
	end
	
	local m = ffi.gc(ffi.cast("Matrix4*", C.malloc(ffi.sizeof("Matrix4"))), C.free)
	m.data = ffi.cast("float*", C.malloc(ffi.sizeof("float[16]")))
	ffi.copy(m.data, src.data, ffi.sizeof("float[16]"))
	
	return m
end

local _metaMatrix4 = {
	__index = {
		Print = function(self, row)
			local d = self.data
			
			if row then
				print(d[00], d[01], d[02], d[03])
				print(d[04], d[05], d[06], d[07])
				print(d[08], d[09], d[10], d[11])
				print(d[12], d[13], d[14], d[15])
			else
				print(d[00], d[04], d[08], d[12])
				print(d[01], d[05], d[09], d[13])
				print(d[02], d[06], d[10], d[14])
				print(d[03], d[07], d[11], d[15])
			end
		end,
		Copy = function(self, to)
			if self ~= to then
				ffi.copy(to.data, self.data, ffi.sizeof("float[16]"))
			end
			
			return to
		end,
		Set = function(self, from)
			if self ~= from then
				ffi.copy(self.data, from.data, ffi.sizeof("float[16]"))
			end
			
			return self
		end,
		Multiply = function(self, right, destination)
			if destination == nil then
				destination = self
			end
			
			local helper = MultiplyHelper
			
			local l = self.data
			local r = right.data
			local h = helper.data
			
			-- print(l[0], r[00], l[4 + 0], r[01], l[8 + 0], r[02], l[12 + 0], r[03])
			-- print(l[1], r[00], l[4 + 1], r[01], l[8 + 1], r[02], l[12 + 1], r[03])
			-- print(l[2], r[00], l[4 + 2], r[01], l[8 + 2], r[02], l[12 + 2], r[03])
			-- print(l[3], r[00], l[4 + 3], r[01], l[8 + 3], r[02], l[12 + 3], r[03])
			
			for i = 0, 3 do
				h[00 + i] = l[i] * r[00] + l[4 + i] * r[01] + l[8 + i] * r[02] + l[12 + i] * r[03]
				h[04 + i] = l[i] * r[04] + l[4 + i] * r[05] + l[8 + i] * r[06] + l[12 + i] * r[07]
				h[08 + i] = l[i] * r[08] + l[4 + i] * r[09] + l[8 + i] * r[10] + l[12 + i] * r[11]
				h[12 + i] = l[i] * r[12] + l[4 + i] * r[13] + l[8 + i] * r[14] + l[12 + i] * r[15]
			end
			
			helper.data = destination.data
			destination.data = h
			
			return destination
		end,
		Identity = function(self)
			ffi.copy(self.data, Identity.data, ffi.sizeof("float[16]"))
			
			return self
		end,
		Translate = function(self, x, y, z, destination)
			if destination == nil then
				destination = self
			end
			
			local helper = TranslateHelper
			local h = helper.data
			
			h[12] = x
			h[13] = y
			h[14] = z
			
			self:Multiply(helper, destination)
			
			return destination
		end,
		TranslateV = function(self, v, destination)
			return self:Translate(v[0], v[1], v[2], destination)
		end,
		Inverse = function(self, destination)
			if destination == nil then
				destination = self
			end
			
			local helper = TransposeHelper
			local transfer = helper.data
			local source = self.data
			
			transfer[00] =  source[05] * source[10] * source[15] - source[05] * source[11] * source[14] - source[09] * source[06] * source[15] + source[09] * source[07] * source[14] + source[13] * source[06] * source[11] - source[13] * source[07] * source[10]
			transfer[04] = -source[04] * source[10] * source[15] + source[04] * source[11] * source[14] + source[08] * source[06] * source[15] - source[08] * source[07] * source[14] - source[12] * source[06] * source[11] + source[12] * source[07] * source[10]
			transfer[08] =  source[04] * source[09] * source[15] - source[04] * source[11] * source[13] - source[08] * source[05] * source[15] + source[08] * source[07] * source[13] + source[12] * source[05] * source[11] - source[12] * source[07] * source[09]
			transfer[12] = -source[04] * source[09] * source[14] + source[04] * source[10] * source[13] + source[08] * source[05] * source[14] - source[08] * source[06] * source[13] - source[12] * source[05] * source[10] + source[12] * source[06] * source[09]
			transfer[01] = -source[01] * source[10] * source[15] + source[01] * source[11] * source[14] + source[09] * source[02] * source[15] - source[09] * source[03] * source[14] - source[13] * source[02] * source[11] + source[13] * source[03] * source[10]
			transfer[05] =  source[00] * source[10] * source[15] - source[00] * source[11] * source[14] - source[08] * source[02] * source[15] + source[08] * source[03] * source[14] + source[12] * source[02] * source[11] - source[12] * source[03] * source[10]
			transfer[09] = -source[00] * source[09] * source[15] + source[00] * source[11] * source[13] + source[08] * source[01] * source[15] - source[08] * source[03] * source[13] - source[12] * source[01] * source[11] + source[12] * source[03] * source[09]
			transfer[13] =  source[00] * source[09] * source[14] - source[00] * source[10] * source[13] - source[08] * source[01] * source[14] + source[08] * source[02] * source[13] + source[12] * source[01] * source[10] - source[12] * source[02] * source[09]
			transfer[02] =  source[01] * source[06] * source[15] - source[01] * source[07] * source[14] - source[05] * source[02] * source[15] + source[05] * source[03] * source[14] + source[13] * source[02] * source[07] - source[13] * source[03] * source[06]
			transfer[06] = -source[00] * source[06] * source[15] + source[00] * source[07] * source[14] + source[04] * source[02] * source[15] - source[04] * source[03] * source[14] - source[12] * source[02] * source[07] + source[12] * source[03] * source[06]
			transfer[10] =  source[00] * source[05] * source[15] - source[00] * source[07] * source[13] - source[04] * source[01] * source[15] + source[04] * source[03] * source[13] + source[12] * source[01] * source[07] - source[12] * source[03] * source[05]
			transfer[14] = -source[00] * source[05] * source[14] + source[00] * source[06] * source[13] + source[04] * source[01] * source[14] - source[04] * source[02] * source[13] - source[12] * source[01] * source[06] + source[12] * source[02] * source[05]
			transfer[03] = -source[01] * source[06] * source[11] + source[01] * source[07] * source[10] + source[05] * source[02] * source[11] - source[05] * source[03] * source[10] - source[09] * source[02] * source[07] + source[09] * source[03] * source[06]
			transfer[07] =  source[00] * source[06] * source[11] - source[00] * source[07] * source[10] - source[04] * source[02] * source[11] + source[04] * source[03] * source[10] + source[08] * source[02] * source[07] - source[08] * source[03] * source[06]
			transfer[11] = -source[00] * source[05] * source[11] + source[00] * source[07] * source[09] + source[04] * source[01] * source[11] - source[04] * source[03] * source[09] - source[08] * source[01] * source[07] + source[08] * source[03] * source[05]
			transfer[15] =  source[00] * source[05] * source[10] - source[00] * source[06] * source[09] - source[04] * source[01] * source[10] + source[04] * source[02] * source[09] + source[08] * source[01] * source[06] - source[08] * source[02] * source[05]
							
			local det = source[0] * transfer[0] + source[1] * transfer[4] + source[2] * transfer[8] + source[3] * transfer[12]

			if det == 0 then
				destination:Copy(self)
				
				return destination
			end

			det = 1 / det

			for i = 1, 15 do
				transfer[i] = transfer[i] * det
			end

			helper.data = destination.data
			destination.data = transfer

			return destination
		end,
		Transpose = function(self, destination)
			if destination == nil then
				destination = self
			end
			
			local helper = TransposeHelper
			local transfer = helper.data
			local source = self.data
			
			transfer[00] = source[00]
			transfer[01] = source[04]
			transfer[02] = source[08]
			transfer[03] = source[12]
			transfer[04] = source[01]
			transfer[05] = source[05]
			transfer[06] = source[09]
			transfer[07] = source[13]
			transfer[08] = source[02]
			transfer[09] = source[06]
			transfer[10] = source[10]
			transfer[11] = source[14]
			transfer[12] = source[03]
			transfer[13] = source[07]
			transfer[14] = source[11]
			transfer[15] = source[15]
			
			helper.data = destination.data
			destination.data = transfer
			
			return destination
		end,
		SetPosition = function(self, x, y, z, destination)
			if destination == nil then
				destination = self
			end
			
			local transfer = destination.data
			
			transfer[12] = x
			transfer[13] = y
			transfer[14] = z
			
			return destination
		end
	}
}

ffi.metatype("Matrix4", _metaMatrix4)

Identity = ffi.cast("Matrix4*", C.malloc(ffi.sizeof("Matrix4")))
Identity.data = ffi.cast("float*", C.calloc(16, ffi.sizeof("float")))
Identity.data[00] = 1
Identity.data[05] = 1
Identity.data[10] = 1
Identity.data[15] = 1

TransposeHelper = Create()
TranslateHelper = Create()
MultiplyHelper = Create()

local test = Create()
local test2 = Create()

for i = 0, 15 do
	test.data[i] = math.random(0, 1)
end

local out
local t = os.clock()

for i = 1, 300000 do
  local inv = test:Inverse(test2, Create())
	out = test:Multiply(test2, inv, Create())
end

print(os.clock() - t)

