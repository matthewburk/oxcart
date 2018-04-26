local bit = require 'bit'
local ffi = require 'ffi'
local C = ffi.C

require 'oxcart.math.mat3'

ffi.cdef[[
typedef struct vec3_t{
  float x;
  float y;
  float z;
} vec3_t;

void vec3_zero(vec3_t* r);
void vec3_set(float x, float y, float z, vec3_t* r);
void vec3_setv2(const vec2_t* v1, float z, vec3_t* r);
void vec3_add(const vec3_t* v1, const vec3_t* v2, vec3_t* r);
void vec3_subtract(const vec3_t* v1, const vec3_t* v2, vec3_t* r);
void vec3_multiply(const vec3_t* v1, const vec3_t* v2, vec3_t* r);
void vec3_divide(const vec3_t* v1, const vec3_t* v2, vec3_t* r);
void vec3_scale(const vec3_t* v1, float scale, vec3_t* r);
void vec3_negate(const vec3_t* v1, vec3_t* r);
void vec3_sqrt(const vec3_t* v1, vec3_t* r);
void vec3_rsqrt(const vec3_t* v1, vec3_t* r);
void vec3_normalize(vec3_t* v1, vec3_t* r);
void vec3_cross(const vec3_t* v1, const vec3_t* v2, vec3_t* r);
void vec3_transform(const vec3_t* v1, const mat3_t* m1, vec3_t* r);
float vec3_dot(const vec3_t* v1, const vec3_t* v2);
float vec3_length(const vec3_t* v1);
float vec3_rlength(const vec3_t* v1);
float vec3_lengthsq(const vec3_t* v1);
float vec3_angle(const vec3_t* v1, const vec3_t* v2);
]]

local M = {}

local vec3_t = ffi.typeof('vec3_t')

do --vec3_t
  local mt = {}
  mt.__index = mt 

  function mt:zero()
    C.vec3_zero(self)
    return self
  end
  function mt:set(x, y, z)
    C.vec3_set(x, y, z, self)
    return self
  end
  function mt:add(v1, v2)
    C.vec3_add(v1, v2, self)
    return self
  end
  function mt:subtract(v1, v2)
    C.vec3_subtract(v1, v2, self)
    return self
  end
  function mt:multiply(v1, v2)
    C.vec3_multiply(v1, v2, self)
    return self
  end
  function mt:divide(v1, v2)
    C.vec3_divide(v1, v2, self)
    return self
  end
  function mt:scale(v1, scale)
    C.vec3_scale(v1, scale, self)
    return self
  end
  function mt:negate(v1)
    v1 = v1 or self
    C.vec3_negate(v1, self)
    return self
  end
  function mt:sqrt(v1)
    v1 = v1 or self
    C.vec3_sqrt(v1, self)
    return self
  end
  function mt:rsqrt(v1)
    v1 = v1 or self
    C.vec3_rsqrt(v1, self)
    return self
  end
  function mt:normalize(v1)
    v1 = v1 or self
    C.vec3_normalize(v1, self)
    return self
  end
  function mt:cross(v1, v2)
    C.vec3_cross(v1, v2, self)
    return self
  end
  function mt:transform(v1, m1)
    C.vec3_transform(v1, m1, self)
    return self
  end
  function mt:dot(v2)
    return C.vec3_dot(self, v2)
  end
  function mt:length()
    return C.vec3_length(self)
  end
  function mt:rlength()
    return C.vec3_rlength(self)
  end
  function mt:lengthsq()
    return C.vec3_lengthsq(self)
  end
  function mt:angle(v2)
    return C.vec3_angle(self, v2)
  end

  ffi.metatype(vec3_t, mt)
end

function M.cross(a, b)
  local result = vec3_t()
  C.vec3_cross(a, b, result)
  return result
end


return M
