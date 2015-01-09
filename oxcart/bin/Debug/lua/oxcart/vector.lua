local ffi = require 'ffi'
local C = ffi.C

ffi.cdef[[
typedef struct oxcart_vector_t oxcart_vector_t;

oxcart_vector_t* oxcart_vector_create(int itemsize, int capacity);
void oxcart_vector_destroy(oxcart_vector_t* vector);
int oxcart_vector_capacity(oxcart_vector_t* vector);
int oxcart_vector_empty(oxcart_vector_t* vector);
int oxcart_vector_size(oxcart_vector_t* vector);
void oxcart_vector_reserve(oxcart_vector_t* vector, int capacity);
void oxcart_vector_resize(oxcart_vector_t* vector, int capacity);
void oxcart_vector_shrink(oxcart_vector_t* vector);
void oxcart_vector_clear(oxcart_vector_t* vector);
void oxcart_vector_popback(oxcart_vector_t* vector);
void oxcart_vector_erase(oxcart_vector_t* vector, int index);
void oxcart_vector_eraserange(oxcart_vector_t* vector, int first, int last);

void* oxcart_vector_item(oxcart_vector_t* vector, int index);
void* oxcart_vector_front(oxcart_vector_t* vector);
void* oxcart_vector_back(oxcart_vector_t* vector);
void* oxcart_vector_pushback(oxcart_vector_t* vector, const void* item);
void* oxcart_vector_pushbackv(oxcart_vector_t* vector, const void* items, int count);
void* oxcart_vector_insert(oxcart_vector_t* vector, int index, const void* item);
void* oxcart_vector_insertv(oxcart_vector_t* vector, int index, const void* items, int count);
void* oxcart_vector_replace(oxcart_vector_t* vector, int index, const void* item);
]]

--callbacks, apparently low performace
ffi.cdef[[
int oxcart_vector_contains(oxcart_vector_t* vector, const void* item, int (*compare)(const void*, const void*));
void oxcart_vector_sort(oxcart_vector_t* vector, int (*compare)(const void*, const void*));
]]

local M = {}

local function makevector(ct)
  local vectoroftype = ffi.typeof('struct { oxcart_vector_t* vector; }')
  local pointertype = ffi.typeof('$*', ct)

  local mt = {itemtype=ct}
  mt.__index = mt

  mt.__new = function(ctype, capacity)
    return ffi.new(ctype, C.oxcart_vector_create(ffi.sizeof(ct), capacity))
  end

  mt.__gc = function(self) 
    C.oxcart_vector_destroy(self.vector) 
  end

  local ctsize = ffi.sizeof(ct)
  local cast = ffi.cast
  --TODO oxcart_vector_empty return value seems backwards
  function mt:isempty() return C.oxcart_vector_empty(self.vector) == 0 end
  function mt:capacity() return C.oxcart_vector_capacity(self.vector) end
  function mt:lastindex() return C.oxcart_vector_size(self.vector)-1 end
  function mt:size() return C.oxcart_vector_size(self.vector) end
  function mt:sizeof() return C.oxcart_vector_size(self.vector) * ctsize end
  function mt:sizeofcapacity() return C.oxcart_vector_capacity(self.vector) * ctsize end
  function mt:reserve(capacity) return C.oxcart_vector_reserve(self.vector, capacity) end
  function mt:resize(capacity) return C.oxcart_vector_resize(self.vector, capacity) end
  function mt:shrink() return C.oxcart_vector_shrink(self.vector) end
  function mt:clear() return C.oxcart_vector_clear(self.vector) end
  function mt:item(index) return cast(pointertype, C.oxcart_vector_item(self.vector, index)) end
  function mt:front() return cast(pointertype, C.oxcart_vector_front(self.vector)) end
  function mt:back() return cast(pointertype, C.oxcart_vector_back(self.vector)) end
  function mt:pushback(item) return cast(pointertype, C.oxcart_vector_pushback(self.vector, item)) end
  function mt:pushbackv(items, count) return cast(pointertype, C.oxcart_vector_pushbackv(self.vector, items, count)) end
  function mt:insert(index, item) return cast(pointertype, C.oxcart_vector_insert(self.vector, index, item)) end
  function mt:insertv(index, items, count) return cast(pointertype, C.oxcart_vector_insertv(self.vector, index, items, count)) end
  function mt:replace(index, item) return cast(pointertype, C.oxcart_vector_replace(self.vector, index, item)) end
  function mt:popback() return C.oxcart_vector_popback(self.vector) end
  function mt:erase(index) return C.oxcart_vector_erase(self.vector, index) end
  function mt:eraserange(first, last) return C.oxcart_vector_eraserange(self.vector, first, last) end

  return ffi.metatype(vectoroftype, mt)
end

--TODO document that this vector is not a container, but copys the item data to its own buffer
--so you have to copy back out
--should probably rename it to membuffer and change the names of the functions to make it clear

M.__index = function(M, k)
  local vectorofct
  if type(k) == 'string' then
    vectorofct = makevector(ffi.typeof(k))
  else
    vectorofct = makevector(k)
  end
  M[k] = function(capacity) return vectorofct(capacity) end
  return M[k]
end

setmetatable(M, M)

oxcart.vector = M

return M
