/**
 * oxcart is licensed under the terms of the MIT license reproduced below.
 * 
 * Copyright © 2014 Thomas J. Schaefer
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of 
 * this software and associated documentation files (the "Software"), to deal in 
 * the Software without restriction, including without limitation the rights to 
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER 
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN 
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef OXCART_MATH_H
#define OXCART_MATH_H

#include <float.h>
#include <math.h>
#include "oxcart_assert.h"
#include "oxcart_util.h"

#ifdef __cplusplus
extern "C" {
#endif

#define OXCART_PI 3.141592654f
#define OXCART_2PI 6.283185307f
#define OXCART_1DIVPI 0.318309886f
#define OXCART_1DIV2PI 0.159154943f
#define OXCART_PIDIV2 1.570796327f
#define OXCART_PIDIV4 0.785398163f
#define OXCART_DEG2RAD(deg) (deg * (OXCART_PI / 180.0f))
#define OXCART_RAD2DEG(rad) (rad * (180.0f / OXCART_PI))
#define OXCART_ISZEROF(val) (fabsf(val) <= FLT_EPSILON)
#define OXCART_ISEQUALF(a, b, e) (oxcart_isequalf(a, b, e))
#define OXCART_VEC_X(vec) ((vec).d[0])
#define OXCART_VEC_Y(vec) ((vec).d[1])
#define OXCART_VEC_Z(vec) ((vec).d[2])
#define OXCART_VEC_W(vec) ((vec).d[3])

typedef struct oxcart_vec2_t oxcart_vec2_t;
typedef struct oxcart_vec3_t oxcart_vec3_t;
typedef struct oxcart_vec4_t oxcart_vec4_t;
typedef struct oxcart_mat3_t oxcart_mat3_t;
typedef struct oxcart_mat4_t oxcart_mat4_t;

struct oxcart_vec2_t
{
  float d[2];
};

struct oxcart_vec3_t
{
  float d[3];
};

struct oxcart_vec4_t
{
  float d[4];
};

struct oxcart_mat3_t
{
  float d[9];
};

struct oxcart_mat4_t
{
  float d[16];
};

OXCART_INLINE oxcart_vec2_t OXCART_VCALL oxcart_vec2_zero();
OXCART_INLINE oxcart_vec2_t OXCART_VCALL oxcart_vec2_set(float x, float y);
OXCART_INLINE oxcart_vec2_t OXCART_VCALL oxcart_vec2_add(const oxcart_vec2_t* v1, const oxcart_vec2_t* v2);
OXCART_INLINE oxcart_vec2_t OXCART_VCALL oxcart_vec2_subtract(const oxcart_vec2_t* v1, const oxcart_vec2_t* v2);
OXCART_INLINE oxcart_vec2_t OXCART_VCALL oxcart_vec2_multiply(const oxcart_vec2_t* v1, const oxcart_vec2_t* v2);
OXCART_INLINE oxcart_vec2_t OXCART_VCALL oxcart_vec2_divide(const oxcart_vec2_t* v1, const oxcart_vec2_t* v2);
OXCART_INLINE oxcart_vec2_t OXCART_VCALL oxcart_vec2_scale(const oxcart_vec2_t* v1, float scale);
OXCART_INLINE oxcart_vec2_t OXCART_VCALL oxcart_vec2_negate(const oxcart_vec2_t* v1);
OXCART_INLINE oxcart_vec2_t OXCART_VCALL oxcart_vec2_sqrt(const oxcart_vec2_t* v1);
OXCART_INLINE oxcart_vec2_t OXCART_VCALL oxcart_vec2_rsqrt(const oxcart_vec2_t* v1);
OXCART_INLINE oxcart_vec2_t OXCART_VCALL oxcart_vec2_normalize(const oxcart_vec2_t* v1);
OXCART_INLINE float OXCART_VCALL oxcart_vec2_dot(const oxcart_vec2_t* v1, const oxcart_vec2_t* v2);
OXCART_INLINE float OXCART_VCALL oxcart_vec2_length(const oxcart_vec2_t* v1);
OXCART_INLINE float OXCART_VCALL oxcart_vec2_rlength(const oxcart_vec2_t* v1);
OXCART_INLINE float OXCART_VCALL oxcart_vec2_lengthsq(const oxcart_vec2_t* v1);
OXCART_INLINE float OXCART_VCALL oxcart_vec2_angle(const oxcart_vec2_t* v1, const oxcart_vec2_t* v2);

OXCART_INLINE oxcart_vec3_t OXCART_VCALL oxcart_vec3_zero();
OXCART_INLINE oxcart_vec3_t OXCART_VCALL oxcart_vec3_set(float x, float y, float z);
OXCART_INLINE oxcart_vec3_t OXCART_VCALL oxcart_vec3_add(const oxcart_vec3_t* v1, const oxcart_vec3_t* v2);
OXCART_INLINE oxcart_vec3_t OXCART_VCALL oxcart_vec3_subtract(const oxcart_vec3_t* v1, const oxcart_vec3_t* v2);
OXCART_INLINE oxcart_vec3_t OXCART_VCALL oxcart_vec3_multiply(const oxcart_vec3_t* v1, const oxcart_vec3_t* v2);
OXCART_INLINE oxcart_vec3_t OXCART_VCALL oxcart_vec3_divide(const oxcart_vec3_t* v1, const oxcart_vec3_t* v2);
OXCART_INLINE oxcart_vec3_t OXCART_VCALL oxcart_vec3_scale(const oxcart_vec3_t* v1, float scale);
OXCART_INLINE oxcart_vec3_t OXCART_VCALL oxcart_vec3_negate(const oxcart_vec3_t* v1);
OXCART_INLINE oxcart_vec3_t OXCART_VCALL oxcart_vec3_sqrt(const oxcart_vec3_t* v1);
OXCART_INLINE oxcart_vec3_t OXCART_VCALL oxcart_vec3_rsqrt(const oxcart_vec3_t* v1);
OXCART_INLINE oxcart_vec3_t OXCART_VCALL oxcart_vec3_normalize(const oxcart_vec3_t* v1);
OXCART_INLINE oxcart_vec3_t OXCART_VCALL oxcart_vec3_cross(const oxcart_vec3_t* v1, const oxcart_vec3_t* v2);
OXCART_INLINE oxcart_vec3_t OXCART_VCALL oxcart_vec3_transform(const oxcart_vec3_t* v1, const oxcart_mat3_t* m1);
OXCART_INLINE float OXCART_VCALL oxcart_vec3_dot(const oxcart_vec3_t* v1, const oxcart_vec3_t* v2);
OXCART_INLINE float OXCART_VCALL oxcart_vec3_length(const oxcart_vec3_t* v1);
OXCART_INLINE float OXCART_VCALL oxcart_vec3_rlength(const oxcart_vec3_t* v1);
OXCART_INLINE float OXCART_VCALL oxcart_vec3_lengthsq(const oxcart_vec3_t* v1);
OXCART_INLINE float OXCART_VCALL oxcart_vec3_angle(const oxcart_vec3_t* v1, const oxcart_vec3_t* v2);

OXCART_INLINE oxcart_vec4_t OXCART_VCALL oxcart_vec4_zero();
OXCART_INLINE oxcart_vec4_t OXCART_VCALL oxcart_vec4_set(float x, float y, float z, float w);
OXCART_INLINE oxcart_vec4_t OXCART_VCALL oxcart_vec4_add(const oxcart_vec4_t* v1, const oxcart_vec4_t* v2);
OXCART_INLINE oxcart_vec4_t OXCART_VCALL oxcart_vec4_subtract(const oxcart_vec4_t* v1, const oxcart_vec4_t* v2);
OXCART_INLINE oxcart_vec4_t OXCART_VCALL oxcart_vec4_multiply(const oxcart_vec4_t* v1, const oxcart_vec4_t* v2);
OXCART_INLINE oxcart_vec4_t OXCART_VCALL oxcart_vec4_divide(const oxcart_vec4_t* v1, const oxcart_vec4_t* v2);
OXCART_INLINE oxcart_vec4_t OXCART_VCALL oxcart_vec4_scale(const oxcart_vec4_t* v1, float scale);
OXCART_INLINE oxcart_vec4_t OXCART_VCALL oxcart_vec4_negate(const oxcart_vec4_t* v1);
OXCART_INLINE oxcart_vec4_t OXCART_VCALL oxcart_vec4_sqrt(const oxcart_vec4_t* v1);
OXCART_INLINE oxcart_vec4_t OXCART_VCALL oxcart_vec4_rsqrt(const oxcart_vec4_t* v1);
OXCART_INLINE oxcart_vec4_t OXCART_VCALL oxcart_vec4_normalize(const oxcart_vec4_t* v1);
OXCART_INLINE oxcart_vec4_t OXCART_VCALL oxcart_vec4_cross(const oxcart_vec4_t* v1, const oxcart_vec4_t* v2);
OXCART_INLINE oxcart_vec4_t OXCART_VCALL oxcart_vec4_transform(const oxcart_vec4_t* v1, const oxcart_mat4_t* m1);
OXCART_INLINE float OXCART_VCALL oxcart_vec4_dot(const oxcart_vec4_t* v1, const oxcart_vec4_t* v2);
OXCART_INLINE float OXCART_VCALL oxcart_vec4_length(const oxcart_vec4_t* v1);
OXCART_INLINE float OXCART_VCALL oxcart_vec4_rlength(const oxcart_vec4_t* v1);
OXCART_INLINE float OXCART_VCALL oxcart_vec4_lengthsq(const oxcart_vec4_t* v1);
OXCART_INLINE float OXCART_VCALL oxcart_vec4_angle(const oxcart_vec4_t* v1, const oxcart_vec4_t* v2);

OXCART_INLINE oxcart_mat3_t OXCART_VCALL oxcart_mat3_identity();
OXCART_INLINE oxcart_mat3_t OXCART_VCALL oxcart_mat3_rotate(float angle);
OXCART_INLINE oxcart_mat3_t OXCART_VCALL oxcart_mat3_translate(float x, float y);
OXCART_INLINE oxcart_mat3_t OXCART_VCALL oxcart_mat3_translatev(const oxcart_vec2_t* v1);
OXCART_INLINE oxcart_mat3_t OXCART_VCALL oxcart_mat3_scale(float x, float y);
OXCART_INLINE oxcart_mat3_t OXCART_VCALL oxcart_mat3_scalev(const oxcart_vec2_t* v1);
OXCART_INLINE oxcart_mat3_t OXCART_VCALL oxcart_mat3_multiply(const oxcart_mat3_t* m1, const oxcart_mat3_t* m2);
OXCART_INLINE oxcart_mat3_t OXCART_VCALL oxcart_mat3_transpose(const oxcart_mat3_t* m1);

OXCART_INLINE oxcart_mat4_t OXCART_VCALL oxcart_mat4_identity();
OXCART_INLINE oxcart_mat4_t OXCART_VCALL oxcart_mat4_rotate(float x, float y, float z, float angle);
OXCART_INLINE oxcart_mat4_t OXCART_VCALL oxcart_mat4_rotatev(const oxcart_vec3_t* v1, float angle);
OXCART_INLINE oxcart_mat4_t OXCART_VCALL oxcart_mat4_translate(float x, float y, float z);
OXCART_INLINE oxcart_mat4_t OXCART_VCALL oxcart_mat4_translatev(const oxcart_vec3_t* v1);
OXCART_INLINE oxcart_mat4_t OXCART_VCALL oxcart_mat4_scale(float x, float y, float z);
OXCART_INLINE oxcart_mat4_t OXCART_VCALL oxcart_mat4_scalev(const oxcart_vec3_t* v1);
OXCART_INLINE oxcart_mat4_t OXCART_VCALL oxcart_mat4_multiply(const oxcart_mat4_t* m1, const oxcart_mat4_t* m2);
OXCART_INLINE oxcart_mat4_t OXCART_VCALL oxcart_mat4_transpose(const oxcart_mat4_t* m1);
OXCART_INLINE oxcart_mat4_t OXCART_VCALL oxcart_mat4_orthographic(float w, float h);
OXCART_INLINE oxcart_mat4_t OXCART_VCALL oxcart_mat4_perspective(float fovy, float aspect, float n, float f);
OXCART_INLINE oxcart_mat4_t OXCART_VCALL oxcart_mat4_lookat(const oxcart_vec3_t* eye, const oxcart_vec3_t* target, const oxcart_vec3_t* up);

OXCART_INLINE int OXCART_VCALL oxcart_isequalf(float a, float b, float epsilon);

#include "oxcart_math.inl"

#ifdef __cplusplus
}
#endif

#endif
