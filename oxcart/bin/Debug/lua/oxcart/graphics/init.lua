require 'oxcart.math'
local ffi = require 'ffi'

ffi.cdef[[
typedef struct oxcart_matrices {
  mat4_t projection;
  mat4_t view;
  mat4_t model;
  mat3_t normaltransform;
} oxcart_matrices_t;

typedef struct oxcart_light {
  vec4_t surfacetolight;
  vec4_t color;
  float ambientcoefficient;
} oxcart_light_t;

typedef struct color4f {
  float r;
  float g;
  float b;
  float a;
} color4f_t;
]]
