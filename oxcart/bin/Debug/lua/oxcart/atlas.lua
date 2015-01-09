local ffi = require 'ffi'
local C = ffi.C

ffi.cdef[[
typedef struct oxcart_atlas_t oxcart_atlas_t;

oxcart_atlas_t* oxcart_atlas_create(int w, int h, int depth);
void oxcart_atlas_destroy(oxcart_atlas_t* atlas);
void oxcart_atlas_properties(oxcart_atlas_t* atlas, int* w, int* h, int* depth);
const unsigned char* oxcart_atlas_data(oxcart_atlas_t* atlas);

int oxcart_atlas_reserve(oxcart_atlas_t* atlas, int w, int h, int* x, int* y);
void oxcart_atlas_fill(oxcart_atlas_t* atlas, int x, int y, int w, int h, int stride, const unsigned char* data);
void oxcart_atlas_clear(oxcart_atlas_t* atlas);
]]

local M = {}

local mt = {}
mt.__index = mt

do
  local aw = ffi.new('int[1]')
  local ah = ffi.new('int[1]')
  local adepth = ffi.new('int[1]')

  function mt:properties() 
    C.oxcart_atlas_properties(self, aw, ah, adepth)
    return aw[0], ah[0], adepth[0]
  end
end

function mt:data() 
  return C.oxcart_atlas_data(self) 
end

function mt:fill(x, y, w, h, stride, data) 
  return C.oxcart_atlas_fill(self, x, y, w, h, stride, data) 
end

function mt:clear() 
  return C.oxcart_atlas_clear(self) 
end

ffi.metatype('oxcart_atlas_t', mt)

function M.new(w, h, depth)
  w = w or 1024
  h = h or 1024
  depth = depth or 1
  return ffi.gc(C.oxcart_atlas_create(w, h, depth), C.oxcart_atlas_destroy)
end

oxcart.atlas = M

return M
