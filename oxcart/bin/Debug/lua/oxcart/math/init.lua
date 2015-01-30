local ffi = require 'ffi'
local bit = require 'bit'

local M = {}

for k, v in pairs(math) do
  M[k]=v
end

ffi.cdef[[
typedef struct vec2_t vec2_t;
typedef struct vec3_t vec3_t;
typedef struct vec4_t vec4_t;
typedef struct mat3_t mat3_t;
typedef struct mat4_t mat4_t;

struct vec2_t {
  float x;
  float y;
};

struct vec3_t {
  float x;
  float y;
  float z;
};

typedef struct vec3_t vec3_t;

struct vec4_t {
  float x;
  float y;
  float z;
  float w;
};

struct mat3_t
{
  float _11, _21, _31;
  float _12, _22, _32;
  float _13, _23, _33;
};

struct mat4_t {
  float _11, _21, _31, _41;
  float _12, _22, _32, _42;
  float _13, _23, _33, _43;
  float _14, _24, _34, _44;
};

void vec3_cross(const vec3_t* v1, const vec3_t* v2, vec3_t* vr);
void vec3_normalize(vec3_t* v);
void vec4_normalize(vec4_t* v);

void mat4_to_mat3_normal(const mat4_t* m1, mat3_t* r);

mat3_t mat3_identity();

//TODO return struct by value prevents jit, change api
mat4_t mat4_lookat(const vec3_t* eye, const vec3_t* target, const vec3_t* up);
mat4_t mat4_perspective(float fovy, float aspect, float n, float f);
mat4_t mat4_orthographic(float w, float h);
mat4_t mat4_rotate(float x, float y, float z, float angle);
mat4_t mat4_translate(float x, float y, float z);
mat4_t mat4_multiply(const mat4_t* m1, const mat4_t* m2);
mat4_t mat4_identity();
]]

local function lerp(a, b, p)
  return a + p * (b - a)
end

local function rlerp(a, b, c)
  return (c - a)/(b - a);
end

M.lerp = lerp
M.rlerp = rlerp

M.smoothstep = function(a, b, p)
  return lerp(a, b, p*p*(3-2*p))
end
    

oxcart.math = M

oxcart.math.noise = require('oxcart.math.noise')

return M
