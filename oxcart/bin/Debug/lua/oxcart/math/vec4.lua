local bit = require 'bit'
local ffi = require 'ffi'
local C = ffi.C

require 'oxcart.math.mat4'

ffi.cdef[[
typedef struct vec4_t{
  float x;
  float y;
  float z;
  float w;
} vec4_t;

void vec4_zero(vec4_t* r);
void vec4_set(float x, float y, float z, float w, vec4_t* r);
void vec4_setv3(const vec3_t* v1, float w, vec4_t* r);
void vec4_add(const vec4_t* v1, const vec4_t* v2, vec4_t* r);
void vec4_subtract(const vec4_t* v1, const vec4_t* v2, vec4_t* r);
void vec4_multiply(const vec4_t* v1, const vec4_t* v2, vec4_t* r);
void vec4_divide(const vec4_t* v1, const vec4_t* v2, vec4_t* r);
void vec4_scale(const vec4_t* v1, float scale, vec4_t* r);
void vec4_negate(const vec4_t* v1, vec4_t* r);
void vec4_sqrt(const vec4_t* v1, vec4_t* r);
void vec4_rsqrt(const vec4_t* v1, vec4_t* r);
void vec4_normalize(vec4_t* v1, vec4_t* r);
void vec4_cross(const vec4_t* v1, const vec4_t* v2, vec4_t* r);
void vec4_transform(const vec4_t* v1, const mat4_t* m1, vec4_t* r);
float vec4_dot(const vec4_t* v1, const vec4_t* v2);
float vec4_length(const vec4_t* v1);
float vec4_rlength(const vec4_t* v1);
float vec4_lengthsq(const vec4_t* v1);
float vec4_angle(const vec4_t* v1, const vec4_t* v2);
]]

local M = {}

local vec4_t = ffi.typeof('vec4_t')

do --vec4_t
  local mt = {}
  mt.__index = mt 

  function mt:zero()
    C.vec4_zero(self)
    return self
  end
  function mt:set(x, y, z, w)
    C.vec4_set(x, y, z, w, self)
    return self
  end
  function mt:add(v1, v2)
    C.vec4_add(v1, v2, self)
    return self
  end
  function mt:subtract(v1, v2)
    C.vec4_subtract(v1, v2, self)
    return self
  end
  function mt:multiply(v1, v2)
    C.vec4_multiply(v1, v2, self)
    return self
  end
  function mt:divide(v1, v2)
    C.vec4_divide(v1, v2, self)
    return self
  end
  function mt:scale(v1, scale)
    C.vec4_scale(v1, scale, self)
    return self
  end
  function mt:negate(v1)
    v1 = v1 or self
    C.vec4_negate(v1, self)
    return self
  end
  function mt:sqrt(v1)
    v1 = v1 or self
    C.vec4_sqrt(v1, self)
    return self
  end
  function mt:rsqrt(v1)
    v1 = v1 or self
    C.vec4_rsqrt(v1, self)
    return self
  end
  function mt:normalize(v1)
    v1 = v1 or self
    C.vec4_normalize(v1, self)
    return self
  end
  function mt:cross(v1, v2)
    C.vec4_cross(v1, v2, self)
    return self
  end
  function mt:transform(v1, m1)
    C.vec4_transform(v1, m1, self)
    return self
  end
  function mt:dot(v2)
    return C.vec4_dot(self, v2)
  end
  function mt:length()
    return C.vec4_length(self)
  end
  function mt:rlength()
    return C.vec4_rlength(self)
  end
  function mt:lengthsq()
    return C.vec4_lengthsq(self)
  end
  function mt:angle(v2)
    return C.vec4_angle(self, v2)
  end

  ffi.metatype(vec4_t, mt)
end

function M.cross(a, b)
  local result = vec4_t()
  C.vec4_cross(a, b, result)
  return result
end


return M

