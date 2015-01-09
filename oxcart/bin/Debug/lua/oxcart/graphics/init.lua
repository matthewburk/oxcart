require 'oxcart.math'
local ffi = require 'ffi'

ffi.cdef[[
typedef struct oxcart_matrices {
  mat4_t projection;
  mat4_t view;
  mat4_t model;
} oxcart_matrices_t;

typedef struct color4f {
  float r;
  float g;
  float b;
  float a;
} color4f_t;
]]
