local ffi = require 'ffi'

require 'WTypes'

ffi.cdef[[
char *_strdup( const char *strSource );

typedef struct rect {
  int x;
  int y;
  int w;
  int h;
} rect_t;

rect_t oxcart_get_window_rect();

void* malloc(size_t);
void free(void*);

void Sleep(int ms);
]]

return ffi.C
