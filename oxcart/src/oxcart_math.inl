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

/**
 * 
 */
OXCART_INLINE oxcart_vec2_t OXCART_VCALL oxcart_vec2_zero()
{
  oxcart_vec2_t vec2 = {0.0f, 0.0f};

  return(vec2);
}

/**
 * 
 */
OXCART_INLINE oxcart_vec2_t OXCART_VCALL oxcart_vec2_set(float x, float y)
{
  oxcart_vec2_t vec2;

  vec2.d[0] = x;
  vec2.d[1] = y;

  return(vec2);
}

/**
 * 
 */
OXCART_INLINE oxcart_vec2_t OXCART_VCALL oxcart_vec2_add(const oxcart_vec2_t* v1, const oxcart_vec2_t* v2)
{
  oxcart_vec2_t vec2;

  OXCART_ASSERT(v1);
  OXCART_ASSERT(v2);

  vec2.d[0] = v1->d[0] + v2->d[0];
  vec2.d[1] = v1->d[1] + v2->d[1];

  return(vec2);
}

/**
 * 
 */
OXCART_INLINE oxcart_vec2_t OXCART_VCALL oxcart_vec2_subtract(const oxcart_vec2_t* v1, const oxcart_vec2_t* v2)
{
  oxcart_vec2_t vec2;

  OXCART_ASSERT(v1);
  OXCART_ASSERT(v2);

  vec2.d[0] = v1->d[0] - v2->d[0];
  vec2.d[1] = v1->d[1] - v2->d[1];

  return(vec2);
}

/**
 * 
 */
OXCART_INLINE oxcart_vec2_t OXCART_VCALL oxcart_vec2_multiply(const oxcart_vec2_t* v1, const oxcart_vec2_t* v2)
{
  oxcart_vec2_t vec2;

  OXCART_ASSERT(v1);
  OXCART_ASSERT(v2);

  vec2.d[0] = v1->d[0] * v2->d[0];
  vec2.d[1] = v1->d[1] * v2->d[1];

  return(vec2);
}

/**
 * 
 */
OXCART_INLINE oxcart_vec2_t OXCART_VCALL oxcart_vec2_divide(const oxcart_vec2_t* v1, const oxcart_vec2_t* v2)
{
  oxcart_vec2_t vec2;

  OXCART_ASSERT(v1);
  OXCART_ASSERT(v2);

  vec2.d[0] = v1->d[0] / v2->d[0];
  vec2.d[1] = v1->d[1] / v2->d[1];

  return(vec2);
}

/**
 * 
 */
OXCART_INLINE oxcart_vec2_t OXCART_VCALL oxcart_vec2_scale(const oxcart_vec2_t* v1, float scale)
{
  oxcart_vec2_t vec2;

  OXCART_ASSERT(v1);

  vec2.d[0] = v1->d[0] * scale;
  vec2.d[1] = v1->d[1] * scale;

  return(vec2);
}

/**
 * 
 */
OXCART_INLINE oxcart_vec2_t OXCART_VCALL oxcart_vec2_negate(const oxcart_vec2_t* v1)
{
  oxcart_vec2_t vec2;

  OXCART_ASSERT(v1);

  vec2.d[0] = -v1->d[0];
  vec2.d[1] = -v1->d[1];

  return(vec2);
}

/**
 * 
 */
OXCART_INLINE oxcart_vec2_t OXCART_VCALL oxcart_vec2_sqrt(const oxcart_vec2_t* v1)
{
  oxcart_vec2_t vec2;

  OXCART_ASSERT(v1);

  vec2.d[0] = sqrtf(v1->d[0]);
  vec2.d[1] = sqrtf(v1->d[1]);

  return(vec2);
}

/**
 * 
 */
OXCART_INLINE oxcart_vec2_t OXCART_VCALL oxcart_vec2_rsqrt(const oxcart_vec2_t* v1)
{
  oxcart_vec2_t vec2;

  OXCART_ASSERT(v1);

  vec2.d[0] = 1.0f / sqrtf(v1->d[0]);
  vec2.d[1] = 1.0f / sqrtf(v1->d[1]);

  return(vec2);
}

/**
 * 
 */
OXCART_INLINE oxcart_vec2_t OXCART_VCALL oxcart_vec2_normalize(const oxcart_vec2_t* v1)
{
  float length;
  oxcart_vec2_t vec2;

  OXCART_ASSERT(v1);

  length = oxcart_vec2_length(v1);

  if (length > 0.0f) {
    length = 1.0f / length;
  }

  vec2.d[0] = v1->d[0] * length;
  vec2.d[1] = v1->d[1] * length;

  return(vec2);
}

/**
 * 
 */
OXCART_INLINE float OXCART_VCALL oxcart_vec2_dot(const oxcart_vec2_t* v1, const oxcart_vec2_t* v2)
{
  OXCART_ASSERT(v1);
  OXCART_ASSERT(v2);

  return((v1->d[0] * v2->d[0]) + (v1->d[1] * v2->d[1]));
}

/**
 * 
 */
OXCART_INLINE float OXCART_VCALL oxcart_vec2_length(const oxcart_vec2_t* v1)
{
  OXCART_ASSERT(v1);

  return(sqrtf(oxcart_vec2_dot(v1, v1)));
}

/**
 * 
 */
OXCART_INLINE float OXCART_VCALL oxcart_vec2_rlength(const oxcart_vec2_t* v1)
{
  OXCART_ASSERT(v1);

  return(1.0f / sqrtf(oxcart_vec2_dot(v1, v1)));
}

/**
 * 
 */
OXCART_INLINE float OXCART_VCALL oxcart_vec2_lengthsq(const oxcart_vec2_t* v1)
{
  OXCART_ASSERT(v1);

  return(oxcart_vec2_dot(v1, v1));
}

/**
 * 
 */
OXCART_INLINE float OXCART_VCALL oxcart_vec2_angle(const oxcart_vec2_t* v1, const oxcart_vec2_t* v2)
{
  float angle;

  OXCART_ASSERT(v1);
  OXCART_ASSERT(v2);

  angle = oxcart_vec2_dot(v1, v2) * oxcart_vec2_rlength(v1) * oxcart_vec2_rlength(v2);

  if (angle < -1.0f) {
    angle = -1.0f;
  }

  if (angle > 1.0f) {
    angle = 1.0f;
  }

  return(OXCART_RAD2DEG(acosf(angle)));
}

/**
 * 
 */
OXCART_INLINE oxcart_vec3_t OXCART_VCALL oxcart_vec3_zero()
{
  oxcart_vec3_t vec3 = {0.0f, 0.0f, 0.0f};

  return(vec3);
}

/**
 * 
 */
OXCART_INLINE oxcart_vec3_t OXCART_VCALL oxcart_vec3_set(float x, float y, float z)
{
  oxcart_vec3_t vec3;

  vec3.d[0] = x;
  vec3.d[1] = y;
  vec3.d[2] = z;

  return(vec3);
}

/**
 * 
 */
OXCART_INLINE oxcart_vec3_t OXCART_VCALL oxcart_vec3_add(const oxcart_vec3_t* v1, const oxcart_vec3_t* v2)
{
  oxcart_vec3_t vec3;

  OXCART_ASSERT(v1);
  OXCART_ASSERT(v2);

  vec3.d[0] = v1->d[0] + v2->d[0];
  vec3.d[1] = v1->d[1] + v2->d[1];
  vec3.d[2] = v1->d[2] + v2->d[2];

  return(vec3);
}

/**
 * 
 */
OXCART_INLINE oxcart_vec3_t OXCART_VCALL oxcart_vec3_subtract(const oxcart_vec3_t* v1, const oxcart_vec3_t* v2)
{
  oxcart_vec3_t vec3;

  OXCART_ASSERT(v1);
  OXCART_ASSERT(v2);

  vec3.d[0] = v1->d[0] - v2->d[0];
  vec3.d[1] = v1->d[1] - v2->d[1];
  vec3.d[2] = v1->d[2] - v2->d[2];

  return(vec3);
}

/**
 * 
 */
OXCART_INLINE oxcart_vec3_t OXCART_VCALL oxcart_vec3_multiply(const oxcart_vec3_t* v1, const oxcart_vec3_t* v2)
{
  oxcart_vec3_t vec3;

  OXCART_ASSERT(v1);
  OXCART_ASSERT(v2);

  vec3.d[0] = v1->d[0] * v2->d[0];
  vec3.d[1] = v1->d[1] * v2->d[1];
  vec3.d[2] = v1->d[2] * v2->d[2];

  return(vec3);
}

/**
 * 
 */
OXCART_INLINE oxcart_vec3_t OXCART_VCALL oxcart_vec3_divide(const oxcart_vec3_t* v1, const oxcart_vec3_t* v2)
{
  oxcart_vec3_t vec3;

  OXCART_ASSERT(v1);
  OXCART_ASSERT(v2);

  vec3.d[0] = v1->d[0] / v2->d[0];
  vec3.d[1] = v1->d[1] / v2->d[1];
  vec3.d[2] = v1->d[2] / v2->d[2];

  return(vec3);
}

/**
 * 
 */
OXCART_INLINE oxcart_vec3_t OXCART_VCALL oxcart_vec3_scale(const oxcart_vec3_t* v1, float scale)
{
  oxcart_vec3_t vec3;

  OXCART_ASSERT(v1);

  vec3.d[0] = v1->d[0] * scale;
  vec3.d[1] = v1->d[1] * scale;
  vec3.d[2] = v1->d[2] * scale;

  return(vec3);
}

/**
 * 
 */
OXCART_INLINE oxcart_vec3_t OXCART_VCALL oxcart_vec3_negate(const oxcart_vec3_t* v1)
{
  oxcart_vec3_t vec3;

  OXCART_ASSERT(v1);

  vec3.d[0] = -v1->d[0];
  vec3.d[1] = -v1->d[1];
  vec3.d[2] = -v1->d[2];

  return(vec3);
}

/**
 * 
 */
OXCART_INLINE oxcart_vec3_t OXCART_VCALL oxcart_vec3_sqrt(const oxcart_vec3_t* v1)
{
  oxcart_vec3_t vec3;

  OXCART_ASSERT(v1);

  vec3.d[0] = sqrtf(v1->d[0]);
  vec3.d[1] = sqrtf(v1->d[1]);
  vec3.d[2] = sqrtf(v1->d[2]);

  return(vec3);
}

/**
 * 
 */
OXCART_INLINE oxcart_vec3_t OXCART_VCALL oxcart_vec3_rsqrt(const oxcart_vec3_t* v1)
{
  oxcart_vec3_t vec3;

  OXCART_ASSERT(v1);

  vec3.d[0] = 1.0f / sqrtf(v1->d[0]);
  vec3.d[1] = 1.0f / sqrtf(v1->d[1]);
  vec3.d[2] = 1.0f / sqrtf(v1->d[2]);

  return(vec3);
}

/**
 * 
 */
OXCART_INLINE oxcart_vec3_t OXCART_VCALL oxcart_vec3_normalize(const oxcart_vec3_t* v1)
{
  float length;
  oxcart_vec3_t vec3;

  OXCART_ASSERT(v1);

  length = oxcart_vec3_length(v1);

  if (length > 0.0f) {
    length = 1.0f / length;
  }

  vec3.d[0] = v1->d[0] * length;
  vec3.d[1] = v1->d[1] * length;
  vec3.d[2] = v1->d[2] * length;

  return(vec3);
}

/**
 * 
 */
OXCART_INLINE oxcart_vec3_t OXCART_VCALL oxcart_vec3_cross(const oxcart_vec3_t* v1, const oxcart_vec3_t* v2)
{
  oxcart_vec3_t vec3;

  OXCART_ASSERT(v1);
  OXCART_ASSERT(v2);

  vec3.d[0] = (v1->d[1] * v2->d[2]) - (v1->d[2] * v2->d[1]);
  vec3.d[1] = (v1->d[2] * v2->d[0]) - (v1->d[0] * v2->d[2]);
  vec3.d[2] = (v1->d[0] * v2->d[1]) - (v1->d[1] * v2->d[0]);

  return(vec3);
}

/**
 * 
 */
OXCART_INLINE oxcart_vec3_t OXCART_VCALL oxcart_vec3_transform(const oxcart_vec3_t* v1, const oxcart_mat3_t* m1)
{
  oxcart_vec3_t vec3;

  OXCART_ASSERT(v1);
  OXCART_ASSERT(m1);

  vec3.d[0] = (m1->d[0] * v1->d[0]) + (m1->d[3] * v1->d[1]) + (m1->d[6] * v1->d[2]);
  vec3.d[1] = (m1->d[1] * v1->d[0]) + (m1->d[4] * v1->d[1]) + (m1->d[7] * v1->d[2]);
  vec3.d[2] = (m1->d[2] * v1->d[0]) + (m1->d[5] * v1->d[1]) + (m1->d[8] * v1->d[2]);

  return(vec3);
}

/**
 * 
 */
OXCART_INLINE float OXCART_VCALL oxcart_vec3_dot(const oxcart_vec3_t* v1, const oxcart_vec3_t* v2)
{
  OXCART_ASSERT(v1);
  OXCART_ASSERT(v2);

  return((v1->d[0] * v2->d[0]) + (v1->d[1] * v2->d[1]) + (v1->d[2] * v2->d[2]));
}

/**
 * 
 */
OXCART_INLINE float OXCART_VCALL oxcart_vec3_length(const oxcart_vec3_t* v1)
{
  OXCART_ASSERT(v1);

  return(sqrtf(oxcart_vec3_dot(v1, v1)));
}

/**
 * 
 */
OXCART_INLINE float OXCART_VCALL oxcart_vec3_rlength(const oxcart_vec3_t* v1)
{
  OXCART_ASSERT(v1);

  return(1.0f / sqrtf(oxcart_vec3_dot(v1, v1)));
}

/**
 * 
 */
OXCART_INLINE float OXCART_VCALL oxcart_vec3_lengthsq(const oxcart_vec3_t* v1)
{
  OXCART_ASSERT(v1);

  return(oxcart_vec3_dot(v1, v1));
}

/**
 * 
 */
OXCART_INLINE float OXCART_VCALL oxcart_vec3_angle(const oxcart_vec3_t* v1, const oxcart_vec3_t* v2)
{
  float angle;

  OXCART_ASSERT(v1);
  OXCART_ASSERT(v2);

  angle = oxcart_vec3_dot(v1, v2) * oxcart_vec3_rlength(v1) * oxcart_vec3_rlength(v2);

  if (angle < -1.0f) {
    angle = -1.0f;
  }

  if (angle > 1.0f) {
    angle = 1.0f;
  }

  return(OXCART_RAD2DEG(acosf(angle)));
}

/**
 * 
 */
OXCART_INLINE oxcart_vec4_t OXCART_VCALL oxcart_vec4_zero()
{
  oxcart_vec4_t vec4 = {0.0f, 0.0f, 0.0f, 0.0f};

  return(vec4);
}

/**
 * 
 */
OXCART_INLINE oxcart_vec4_t OXCART_VCALL oxcart_vec4_set(float x, float y, float z, float w)
{
  oxcart_vec4_t vec4;

  vec4.d[0] = x;
  vec4.d[1] = y;
  vec4.d[2] = z;
  vec4.d[3] = w;

  return(vec4);
}

/**
 * 
 */
OXCART_INLINE oxcart_vec4_t OXCART_VCALL oxcart_vec4_add(const oxcart_vec4_t* v1, const oxcart_vec4_t* v2)
{
  oxcart_vec4_t vec4;

  OXCART_ASSERT(v1);
  OXCART_ASSERT(v2);

  vec4.d[0] = v1->d[0] + v2->d[0];
  vec4.d[1] = v1->d[1] + v2->d[1];
  vec4.d[2] = v1->d[2] + v2->d[2];
  vec4.d[3] = v1->d[3] + v2->d[3];

  return(vec4);
}

/**
 * 
 */
OXCART_INLINE oxcart_vec4_t OXCART_VCALL oxcart_vec4_subtract(const oxcart_vec4_t* v1, const oxcart_vec4_t* v2)
{
  oxcart_vec4_t vec4;

  OXCART_ASSERT(v1);
  OXCART_ASSERT(v2);

  vec4.d[0] = v1->d[0] - v2->d[0];
  vec4.d[1] = v1->d[1] - v2->d[1];
  vec4.d[2] = v1->d[2] - v2->d[2];
  vec4.d[3] = v1->d[3] - v2->d[3];

  return(vec4);
}

/**
 * 
 */
OXCART_INLINE oxcart_vec4_t OXCART_VCALL oxcart_vec4_multiply(const oxcart_vec4_t* v1, const oxcart_vec4_t* v2)
{
  oxcart_vec4_t vec4;

  OXCART_ASSERT(v1);
  OXCART_ASSERT(v2);

  vec4.d[0] = v1->d[0] * v2->d[0];
  vec4.d[1] = v1->d[1] * v2->d[1];
  vec4.d[2] = v1->d[2] * v2->d[2];
  vec4.d[3] = v1->d[3] * v2->d[3];

  return(vec4);
}

/**
 * 
 */
OXCART_INLINE oxcart_vec4_t OXCART_VCALL oxcart_vec4_divide(const oxcart_vec4_t* v1, const oxcart_vec4_t* v2)
{
  oxcart_vec4_t vec4;

  OXCART_ASSERT(v1);
  OXCART_ASSERT(v2);

  vec4.d[0] = v1->d[0] / v2->d[0];
  vec4.d[1] = v1->d[1] / v2->d[1];
  vec4.d[2] = v1->d[2] / v2->d[2];
  vec4.d[3] = v1->d[3] / v2->d[3];

  return(vec4);
}

/**
 * 
 */
OXCART_INLINE oxcart_vec4_t OXCART_VCALL oxcart_vec4_scale(const oxcart_vec4_t* v1, float scale)
{
  oxcart_vec4_t vec4;

  OXCART_ASSERT(v1);

  vec4.d[0] = v1->d[0] * scale;
  vec4.d[1] = v1->d[1] * scale;
  vec4.d[2] = v1->d[2] * scale;
  vec4.d[3] = v1->d[3] * scale;

  return(vec4);
}

/**
 * 
 */
OXCART_INLINE oxcart_vec4_t OXCART_VCALL oxcart_vec4_negate(const oxcart_vec4_t* v1)
{
  oxcart_vec4_t vec4;

  OXCART_ASSERT(v1);

  vec4.d[0] = -v1->d[0];
  vec4.d[1] = -v1->d[1];
  vec4.d[2] = -v1->d[2];
  vec4.d[3] = -v1->d[3];

  return(vec4);
}

/**
 * 
 */
OXCART_INLINE oxcart_vec4_t OXCART_VCALL oxcart_vec4_sqrt(const oxcart_vec4_t* v1)
{
  oxcart_vec4_t vec4;

  OXCART_ASSERT(v1);

  vec4.d[0] = sqrtf(v1->d[0]);
  vec4.d[1] = sqrtf(v1->d[1]);
  vec4.d[2] = sqrtf(v1->d[2]);
  vec4.d[3] = sqrtf(v1->d[3]);

  return(vec4);
}

/**
 * 
 */
OXCART_INLINE oxcart_vec4_t OXCART_VCALL oxcart_vec4_rsqrt(const oxcart_vec4_t* v1)
{
  oxcart_vec4_t vec4;

  OXCART_ASSERT(v1);

  vec4.d[0] = 1.0f / sqrtf(v1->d[0]);
  vec4.d[1] = 1.0f / sqrtf(v1->d[1]);
  vec4.d[2] = 1.0f / sqrtf(v1->d[2]);
  vec4.d[3] = 1.0f / sqrtf(v1->d[3]);

  return(vec4);
}

/**
 * 
 */
OXCART_INLINE oxcart_vec4_t OXCART_VCALL oxcart_vec4_normalize(const oxcart_vec4_t* v1)
{
  float length;
  oxcart_vec4_t vec4;

  OXCART_ASSERT(v1);

  length = oxcart_vec4_length(v1);

  if (length > 0.0f) {
    length = 1.0f / length;
  }

  vec4.d[0] = v1->d[0] * length;
  vec4.d[1] = v1->d[1] * length;
  vec4.d[2] = v1->d[2] * length;
  vec4.d[3] = v1->d[3] * length;

  return(vec4);
}

/**
 * 
 */
OXCART_INLINE oxcart_vec4_t OXCART_VCALL oxcart_vec4_cross(const oxcart_vec4_t* v1, const oxcart_vec4_t* v2)
{
  oxcart_vec4_t vec4;

  OXCART_ASSERT(v1);
  OXCART_ASSERT(v2);

  vec4.d[0] = (v1->d[1] * v2->d[2]) - (v1->d[2] * v2->d[1]);
  vec4.d[1] = (v1->d[2] * v2->d[0]) - (v1->d[0] * v2->d[2]);
  vec4.d[2] = (v1->d[0] * v2->d[1]) - (v1->d[1] * v2->d[0]);
  vec4.d[3] = 0.0f;

  return(vec4);
}

/**
 * 
 */
OXCART_INLINE oxcart_vec4_t OXCART_VCALL oxcart_vec4_transform(const oxcart_vec4_t* v1, const oxcart_mat4_t* m1)
{
  oxcart_vec4_t vec4;

  OXCART_ASSERT(v1);
  OXCART_ASSERT(m1);

  vec4.d[0] = (m1->d[ 0] * v1->d[0]) + (m1->d[ 4] * v1->d[1]) + (m1->d[ 8] * v1->d[2]) + (m1->d[12] * v1->d[3]);
  vec4.d[1] = (m1->d[ 1] * v1->d[0]) + (m1->d[ 5] * v1->d[1]) + (m1->d[ 9] * v1->d[2]) + (m1->d[13] * v1->d[3]);
  vec4.d[2] = (m1->d[ 2] * v1->d[0]) + (m1->d[ 6] * v1->d[1]) + (m1->d[10] * v1->d[2]) + (m1->d[14] * v1->d[3]);
  vec4.d[3] = (m1->d[ 3] * v1->d[0]) + (m1->d[ 7] * v1->d[1]) + (m1->d[11] * v1->d[2]) + (m1->d[15] * v1->d[3]);

  return(vec4);
}

/**
 * 
 */
OXCART_INLINE float OXCART_VCALL oxcart_vec4_dot(const oxcart_vec4_t* v1, const oxcart_vec4_t* v2)
{
  OXCART_ASSERT(v1);
  OXCART_ASSERT(v2);

  return((v1->d[0] * v2->d[0]) + (v1->d[1] * v2->d[1]) + (v1->d[2] * v2->d[2]) + (v1->d[3] * v2->d[3]));
}

/**
 * 
 */
OXCART_INLINE float OXCART_VCALL oxcart_vec4_length(const oxcart_vec4_t* v1)
{
  OXCART_ASSERT(v1);

  return(sqrtf(oxcart_vec4_dot(v1, v1)));
}

/**
 * 
 */
OXCART_INLINE float OXCART_VCALL oxcart_vec4_rlength(const oxcart_vec4_t* v1)
{
  OXCART_ASSERT(v1);

  return(1.0f / sqrtf(oxcart_vec4_dot(v1, v1)));
}

/**
 * 
 */
OXCART_INLINE float OXCART_VCALL oxcart_vec4_lengthsq(const oxcart_vec4_t* v1)
{
  OXCART_ASSERT(v1);

  return(oxcart_vec4_dot(v1, v1));
}

/**
 * 
 */
OXCART_INLINE float OXCART_VCALL oxcart_vec4_angle(const oxcart_vec4_t* v1, const oxcart_vec4_t* v2)
{
  float angle;

  OXCART_ASSERT(v1);
  OXCART_ASSERT(v2);

  angle = oxcart_vec4_dot(v1, v2) * oxcart_vec4_rlength(v1) * oxcart_vec4_rlength(v2);

  if (angle < -1.0f) {
    angle = -1.0f;
  }

  if (angle > 1.0f) {
    angle = 1.0f;
  }

  return(OXCART_RAD2DEG(acosf(angle)));
}

/**
 * 
 */
OXCART_INLINE oxcart_mat3_t OXCART_VCALL oxcart_mat3_identity()
{
  oxcart_mat3_t mat3 = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};

  return(mat3);
}

/**
 * 
 */
OXCART_INLINE oxcart_mat3_t OXCART_VCALL oxcart_mat3_rotate(float angle)
{
  float rad;
  float s, c;
  oxcart_mat3_t mat3;

  rad = OXCART_DEG2RAD(angle);
  s = sinf(rad);
  c = cosf(rad);

  if (OXCART_ISZEROF(s)) {
    s = 0.0f;
  }

  if (OXCART_ISZEROF(c)) {
    c = 0.0f;
  }

  mat3.d[0] = c;
  mat3.d[1] = s;
  mat3.d[2] = 0.0f;
  mat3.d[3] = -s;
  mat3.d[4] = c;
  mat3.d[5] = 0.0f;
  mat3.d[6] = 0.0f;
  mat3.d[7] = 0.0f;
  mat3.d[8] = 1.0f;

  return(mat3);
}

/**
 * 
 */
OXCART_INLINE oxcart_mat3_t OXCART_VCALL oxcart_mat3_translate(float x, float y)
{
  oxcart_vec2_t vec2;

  vec2.d[0] = x;
  vec2.d[1] = y;

  return(oxcart_mat3_translatev(&vec2));
}

/**
 * 
 */
OXCART_INLINE oxcart_mat3_t OXCART_VCALL oxcart_mat3_translatev(const oxcart_vec2_t* v1)
{
  oxcart_mat3_t mat3;

  OXCART_ASSERT(v1);

  mat3.d[0] = 1.0f;
  mat3.d[1] = 0.0f;
  mat3.d[2] = 0.0f;
  mat3.d[3] = 0.0f;
  mat3.d[4] = 1.0f;
  mat3.d[5] = 0.0f;
  mat3.d[6] = v1->d[0];
  mat3.d[7] = v1->d[1];
  mat3.d[8] = 1.0f;

  return(mat3);
}

/**
 * 
 */
OXCART_INLINE oxcart_mat3_t OXCART_VCALL oxcart_mat3_scale(float x, float y)
{
  oxcart_vec2_t vec2;

  vec2.d[0] = x;
  vec2.d[1] = y;

  return(oxcart_mat3_scalev(&vec2));
}

/**
 * 
 */
OXCART_INLINE oxcart_mat3_t OXCART_VCALL oxcart_mat3_scalev(const oxcart_vec2_t* v1)
{
  oxcart_mat3_t mat3;

  OXCART_ASSERT(v1);

  mat3.d[0] = v1->d[0];
  mat3.d[1] = 0.0f;
  mat3.d[2] = 0.0f;
  mat3.d[3] = 0.0f;
  mat3.d[4] = v1->d[1];
  mat3.d[5] = 0.0f;
  mat3.d[6] = 0.0f;
  mat3.d[7] = 0.0f;
  mat3.d[8] = 1.0f;

  return(mat3);
}

/**
 * 
 */
OXCART_INLINE oxcart_mat3_t OXCART_VCALL oxcart_mat3_multiply(const oxcart_mat3_t* m1, const oxcart_mat3_t* m2)
{
  oxcart_mat3_t mat3;

  OXCART_ASSERT(m1);
  OXCART_ASSERT(m2);

  mat3.d[0] = (m1->d[0] * m2->d[0]) + (m1->d[3] * m2->d[1]) + (m1->d[6] * m2->d[2]);
  mat3.d[1] = (m1->d[1] * m2->d[0]) + (m1->d[4] * m2->d[1]) + (m1->d[7] * m2->d[2]);
  mat3.d[2] = (m1->d[2] * m2->d[0]) + (m1->d[5] * m2->d[1]) + (m1->d[8] * m2->d[2]);
  mat3.d[3] = (m1->d[0] * m2->d[3]) + (m1->d[3] * m2->d[4]) + (m1->d[6] * m2->d[5]);
  mat3.d[4] = (m1->d[1] * m2->d[3]) + (m1->d[4] * m2->d[4]) + (m1->d[7] * m2->d[5]);
  mat3.d[5] = (m1->d[2] * m2->d[3]) + (m1->d[5] * m2->d[4]) + (m1->d[8] * m2->d[5]);
  mat3.d[6] = (m1->d[0] * m2->d[6]) + (m1->d[3] * m2->d[7]) + (m1->d[6] * m2->d[8]);
  mat3.d[7] = (m1->d[1] * m2->d[6]) + (m1->d[4] * m2->d[7]) + (m1->d[7] * m2->d[8]);
  mat3.d[8] = (m1->d[2] * m2->d[6]) + (m1->d[5] * m2->d[7]) + (m1->d[8] * m2->d[8]);

  return(mat3);
}

/**
 * 
 */
OXCART_INLINE oxcart_mat3_t OXCART_VCALL oxcart_mat3_transpose(const oxcart_mat3_t* m1)
{
  oxcart_mat3_t mat3;

  OXCART_ASSERT(m1);

  mat3.d[0] = m1->d[0];
  mat3.d[1] = m1->d[3];
  mat3.d[2] = m1->d[6];
  mat3.d[3] = m1->d[1];
  mat3.d[4] = m1->d[4];
  mat3.d[5] = m1->d[7];
  mat3.d[6] = m1->d[2];
  mat3.d[7] = m1->d[5];
  mat3.d[8] = m1->d[8];

  return(mat3);
}

/**
 * 
 */
OXCART_INLINE float OXCART_VCALL oxcart_mat3_determinant(const oxcart_mat3_t* m1)
{
  oxcart_vec3_t vec3;

  OXCART_ASSERT(m1);

  vec3.d[0] = m1->d[0] * ((m1->d[8] * m1->d[4]) - (m1->d[7] * m1->d[5]));
  vec3.d[1] = m1->d[3] * ((m1->d[8] * m1->d[1]) - (m1->d[7] * m1->d[2]));
  vec3.d[2] = m1->d[6] * ((m1->d[5] * m1->d[1]) - (m1->d[4] * m1->d[2]));

  return(vec3.d[0] - vec3.d[1] + vec3.d[2]);
}

/**
 * 
 */
OXCART_INLINE int OXCART_VCALL oxcart_mat3_inverse(const oxcart_mat3_t* m1, oxcart_mat3_t* out)
{
  float det;

  OXCART_ASSERT(m1);
  OXCART_ASSERT(out);

  if (0.0f == (det = oxcart_mat3_determinant(m1))) {
    return(0);
  }

  det = 1.0f / det;

  out->d[0] =  ((m1->d[4] * m1->d[8]) - (m1->d[7] * m1->d[5])) * det;
  out->d[1] = -((m1->d[1] * m1->d[8]) - (m1->d[7] * m1->d[2])) * det;
  out->d[2] =  ((m1->d[1] * m1->d[5]) - (m1->d[4] * m1->d[2])) * det;
  out->d[3] = -((m1->d[3] * m1->d[8]) - (m1->d[6] * m1->d[5])) * det;
  out->d[4] =  ((m1->d[0] * m1->d[8]) - (m1->d[6] * m1->d[2])) * det;
  out->d[5] = -((m1->d[0] * m1->d[5]) - (m1->d[3] * m1->d[2])) * det;
  out->d[6] =  ((m1->d[3] * m1->d[7]) - (m1->d[6] * m1->d[4])) * det;
  out->d[7] = -((m1->d[0] * m1->d[7]) - (m1->d[6] * m1->d[1])) * det;
  out->d[8] =  ((m1->d[0] * m1->d[4]) - (m1->d[3] * m1->d[1])) * det;

  return(1);
}

/**
 * 
 */
OXCART_INLINE oxcart_mat3_t OXCART_VCALL oxcart_mat3_affineinverse(const oxcart_mat3_t* m1)
{
  oxcart_vec3_t tran;
  oxcart_mat3_t mat3;

  OXCART_ASSERT(m1);

  mat3 = *m1;
  mat3.d[6] = 0.0f;
  mat3.d[7] = 0.0f;
  mat3.d[8] = 1.0f;
  mat3 = oxcart_mat3_transpose(&mat3);

  tran = oxcart_vec3_set(-m1->d[6], -m1->d[7], m1->d[8]);
  tran = oxcart_vec3_transform(&tran, &mat3);

  mat3.d[6] = tran.d[0];
  mat3.d[7] = tran.d[1];
  mat3.d[8] = tran.d[2];

  return(mat3);
}

/**
 * 
 */
OXCART_INLINE oxcart_mat4_t OXCART_VCALL oxcart_mat4_identity()
{
  oxcart_mat4_t mat4 = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};

  return(mat4);
}

/**
 * 
 */
OXCART_INLINE oxcart_mat4_t OXCART_VCALL oxcart_mat4_rotate(float x, float y, float z, float angle)
{
  oxcart_vec3_t vec3;

  vec3.d[0] = x;
  vec3.d[1] = y;
  vec3.d[2] = z;
  vec3 = oxcart_vec3_normalize(&vec3);

  return(oxcart_mat4_rotatev(&vec3, angle));
}

/**
 * 
 */
OXCART_INLINE oxcart_mat4_t OXCART_VCALL oxcart_mat4_rotatev(const oxcart_vec3_t* v1, float angle)
{
  float rad;
  float s, c, omc;
  oxcart_mat4_t mat4;

  OXCART_ASSERT(v1);

  rad = OXCART_DEG2RAD(angle);
  s = sinf(rad);
  c = cosf(rad);

  if (OXCART_ISZEROF(s)) {
    s = 0.0f;
  }

  if (OXCART_ISZEROF(c)) {
    c = 0.0f;
  }

  omc = 1.0f - c;

  mat4.d[ 0] = (v1->d[0] * v1->d[0] * omc) + c;
  mat4.d[ 1] = (v1->d[0] * v1->d[1] * omc) + (v1->d[2] * s);
  mat4.d[ 2] = (v1->d[0] * v1->d[2] * omc) - (v1->d[1] * s);
  mat4.d[ 3] = 0.0f;
  mat4.d[ 4] = (v1->d[1] * v1->d[0] * omc) - (v1->d[2] * s);
  mat4.d[ 5] = (v1->d[1] * v1->d[1] * omc) + c;
  mat4.d[ 6] = (v1->d[1] * v1->d[2] * omc) + (v1->d[0] * s);
  mat4.d[ 7] = 0.0f;
  mat4.d[ 8] = (v1->d[2] * v1->d[0] * omc) + (v1->d[1] * s);
  mat4.d[ 9] = (v1->d[2] * v1->d[1] * omc) - (v1->d[0] * s);
  mat4.d[10] = (v1->d[2] * v1->d[2] * omc) + c;
  mat4.d[11] = 0.0f;
  mat4.d[12] = 0.0f;
  mat4.d[13] = 0.0f;
  mat4.d[14] = 0.0f;
  mat4.d[15] = 1.0f;

  return(mat4);
}

/**
 * 
 */
OXCART_INLINE oxcart_mat4_t OXCART_VCALL oxcart_mat4_translate(float x, float y, float z)
{
  oxcart_vec3_t vec3;

  vec3.d[0] = x;
  vec3.d[1] = y;
  vec3.d[2] = z;

  return(oxcart_mat4_translatev(&vec3));
}

/**
 * 
 */
OXCART_INLINE oxcart_mat4_t OXCART_VCALL oxcart_mat4_translatev(const oxcart_vec3_t* v1)
{
  oxcart_mat4_t mat4;

  OXCART_ASSERT(v1);

  mat4.d[ 0] = 1.0f;
  mat4.d[ 1] = 0.0f;
  mat4.d[ 2] = 0.0f;
  mat4.d[ 3] = 0.0f;
  mat4.d[ 4] = 0.0f;
  mat4.d[ 5] = 1.0f;
  mat4.d[ 6] = 0.0f;
  mat4.d[ 7] = 0.0f;
  mat4.d[ 8] = 0.0f;
  mat4.d[ 9] = 0.0f;
  mat4.d[10] = 1.0f;
  mat4.d[11] = 0.0f;
  mat4.d[12] = v1->d[0];
  mat4.d[13] = v1->d[1];
  mat4.d[14] = v1->d[2];
  mat4.d[15] = 1.0f;

  return(mat4);
}

/**
 * 
 */
OXCART_INLINE oxcart_mat4_t OXCART_VCALL oxcart_mat4_scale(float x, float y, float z)
{
  oxcart_vec3_t vec3;

  vec3.d[0] = x;
  vec3.d[1] = y;
  vec3.d[2] = z;

  return(oxcart_mat4_scalev(&vec3));
}

/**
 * 
 */
OXCART_INLINE oxcart_mat4_t OXCART_VCALL oxcart_mat4_scalev(const oxcart_vec3_t* v1)
{
  oxcart_mat4_t mat4;

  OXCART_ASSERT(v1);

  mat4.d[ 0] = v1->d[0];
  mat4.d[ 1] = 0.0f;
  mat4.d[ 2] = 0.0f;
  mat4.d[ 3] = 0.0f;
  mat4.d[ 4] = 0.0f;
  mat4.d[ 5] = v1->d[1];
  mat4.d[ 6] = 0.0f;
  mat4.d[ 7] = 0.0f;
  mat4.d[ 8] = 0.0f;
  mat4.d[ 9] = 0.0f;
  mat4.d[10] = v1->d[2];
  mat4.d[11] = 0.0f;
  mat4.d[12] = 0.0f;
  mat4.d[13] = 0.0f;
  mat4.d[14] = 0.0f;
  mat4.d[15] = 1.0f;

  return(mat4);
}

/**
 * 
 */
OXCART_INLINE oxcart_mat4_t OXCART_VCALL oxcart_mat4_multiply(const oxcart_mat4_t* m1, const oxcart_mat4_t* m2)
{
  oxcart_mat4_t mat4;

  OXCART_ASSERT(m1);
  OXCART_ASSERT(m2);

  mat4.d[ 0] = (m1->d[ 0] * m2->d[ 0]) + (m1->d[ 4] * m2->d[ 1]) + (m1->d[ 8] * m2->d[ 2]) + (m1->d[12] * m2->d[ 3]);
  mat4.d[ 1] = (m1->d[ 1] * m2->d[ 0]) + (m1->d[ 5] * m2->d[ 1]) + (m1->d[ 9] * m2->d[ 2]) + (m1->d[13] * m2->d[ 3]);
  mat4.d[ 2] = (m1->d[ 2] * m2->d[ 0]) + (m1->d[ 6] * m2->d[ 1]) + (m1->d[10] * m2->d[ 2]) + (m1->d[14] * m2->d[ 3]);
  mat4.d[ 3] = (m1->d[ 3] * m2->d[ 0]) + (m1->d[ 7] * m2->d[ 1]) + (m1->d[11] * m2->d[ 2]) + (m1->d[15] * m2->d[ 3]);
  mat4.d[ 4] = (m1->d[ 0] * m2->d[ 4]) + (m1->d[ 4] * m2->d[ 5]) + (m1->d[ 8] * m2->d[ 6]) + (m1->d[12] * m2->d[ 7]);
  mat4.d[ 5] = (m1->d[ 1] * m2->d[ 4]) + (m1->d[ 5] * m2->d[ 5]) + (m1->d[ 9] * m2->d[ 6]) + (m1->d[13] * m2->d[ 7]);
  mat4.d[ 6] = (m1->d[ 2] * m2->d[ 4]) + (m1->d[ 6] * m2->d[ 5]) + (m1->d[10] * m2->d[ 6]) + (m1->d[14] * m2->d[ 7]);
  mat4.d[ 7] = (m1->d[ 3] * m2->d[ 4]) + (m1->d[ 7] * m2->d[ 5]) + (m1->d[11] * m2->d[ 6]) + (m1->d[15] * m2->d[ 7]);
  mat4.d[ 8] = (m1->d[ 0] * m2->d[ 8]) + (m1->d[ 4] * m2->d[ 9]) + (m1->d[ 8] * m2->d[10]) + (m1->d[12] * m2->d[11]);
  mat4.d[ 9] = (m1->d[ 1] * m2->d[ 8]) + (m1->d[ 5] * m2->d[ 9]) + (m1->d[ 9] * m2->d[10]) + (m1->d[13] * m2->d[11]);
  mat4.d[10] = (m1->d[ 2] * m2->d[ 8]) + (m1->d[ 6] * m2->d[ 9]) + (m1->d[10] * m2->d[10]) + (m1->d[14] * m2->d[11]);
  mat4.d[11] = (m1->d[ 3] * m2->d[ 8]) + (m1->d[ 7] * m2->d[ 9]) + (m1->d[11] * m2->d[10]) + (m1->d[15] * m2->d[11]);
  mat4.d[12] = (m1->d[ 0] * m2->d[12]) + (m1->d[ 4] * m2->d[13]) + (m1->d[ 8] * m2->d[14]) + (m1->d[12] * m2->d[15]);
  mat4.d[13] = (m1->d[ 1] * m2->d[12]) + (m1->d[ 5] * m2->d[13]) + (m1->d[ 9] * m2->d[14]) + (m1->d[13] * m2->d[15]);
  mat4.d[14] = (m1->d[ 2] * m2->d[12]) + (m1->d[ 6] * m2->d[13]) + (m1->d[10] * m2->d[14]) + (m1->d[14] * m2->d[15]);
  mat4.d[15] = (m1->d[ 3] * m2->d[12]) + (m1->d[ 7] * m2->d[13]) + (m1->d[11] * m2->d[14]) + (m1->d[15] * m2->d[15]);

  return(mat4);
}

/**
 * 
 */
OXCART_INLINE oxcart_mat4_t OXCART_VCALL oxcart_mat4_transpose(const oxcart_mat4_t* m1)
{
  oxcart_mat4_t mat4;

  OXCART_ASSERT(m1);

  mat4.d[ 0] = m1->d[ 0];
  mat4.d[ 1] = m1->d[ 4];
  mat4.d[ 2] = m1->d[ 8];
  mat4.d[ 3] = m1->d[12];
  mat4.d[ 4] = m1->d[ 1];
  mat4.d[ 5] = m1->d[ 5];
  mat4.d[ 6] = m1->d[ 9];
  mat4.d[ 7] = m1->d[13];
  mat4.d[ 8] = m1->d[ 2];
  mat4.d[ 9] = m1->d[ 6];
  mat4.d[10] = m1->d[10];
  mat4.d[11] = m1->d[14];
  mat4.d[12] = m1->d[ 3];
  mat4.d[13] = m1->d[ 7];
  mat4.d[14] = m1->d[11];
  mat4.d[15] = m1->d[15];

  return(mat4);
}

/**
 * 
 */
OXCART_INLINE float OXCART_VCALL oxcart_mat4_determinant(const oxcart_mat4_t* m1)
{
  float subfactor[6];
  oxcart_vec4_t vec4;

  OXCART_ASSERT(m1);

  subfactor[0] = (m1->d[10] * m1->d[15]) - (m1->d[14] * m1->d[11]);
  subfactor[1] = (m1->d[ 9] * m1->d[15]) - (m1->d[13] * m1->d[11]);
  subfactor[2] = (m1->d[ 9] * m1->d[14]) - (m1->d[13] * m1->d[10]);
  subfactor[3] = (m1->d[ 8] * m1->d[15]) - (m1->d[12] * m1->d[11]);
  subfactor[4] = (m1->d[ 8] * m1->d[14]) - (m1->d[12] * m1->d[10]);
  subfactor[5] = (m1->d[ 8] * m1->d[13]) - (m1->d[12] * m1->d[ 9]);

  vec4.d[0] = (m1->d[5] * subfactor[0]) - (m1->d[6] * subfactor[1]) + (m1->d[7] * subfactor[2]);
  vec4.d[1] = (m1->d[4] * subfactor[0]) - (m1->d[6] * subfactor[3]) + (m1->d[7] * subfactor[4]);
  vec4.d[2] = (m1->d[4] * subfactor[1]) - (m1->d[5] * subfactor[3]) + (m1->d[7] * subfactor[5]);
  vec4.d[3] = (m1->d[4] * subfactor[2]) - (m1->d[5] * subfactor[4]) + (m1->d[6] * subfactor[5]);

  return((m1->d[0] * vec4.d[0]) - (m1->d[1] * vec4.d[1]) + (m1->d[2] * vec4.d[2]) - (m1->d[3] * vec4.d[3]));
}

/**
 * 
 */
OXCART_INLINE int OXCART_VCALL oxcart_mat4_inverse(const oxcart_mat4_t* m1, oxcart_mat4_t* out)
{
  int i;
  float det;

  OXCART_ASSERT(m1);
  OXCART_ASSERT(out);

  out->d[ 0] = (m1->d[5] * ((m1->d[10] * m1->d[15]) - (m1->d[11] * m1->d[14]))) - (m1->d[9] * ((m1->d[6] * m1->d[15]) - (m1->d[7] * m1->d[14]))) - (m1->d[13] * ((m1->d[7] * m1->d[10]) - (m1->d[6] * m1->d[11])));
  out->d[ 1] = (m1->d[1] * ((m1->d[11] * m1->d[14]) - (m1->d[10] * m1->d[15]))) - (m1->d[9] * ((m1->d[3] * m1->d[14]) - (m1->d[2] * m1->d[15]))) - (m1->d[13] * ((m1->d[2] * m1->d[11]) - (m1->d[3] * m1->d[10])));
  out->d[ 2] = (m1->d[1] * ((m1->d[ 6] * m1->d[15]) - (m1->d[ 7] * m1->d[14]))) - (m1->d[5] * ((m1->d[2] * m1->d[15]) - (m1->d[3] * m1->d[14]))) - (m1->d[13] * ((m1->d[3] * m1->d[ 6]) - (m1->d[2] * m1->d[ 7])));
  out->d[ 3] = (m1->d[1] * ((m1->d[ 7] * m1->d[10]) - (m1->d[ 6] * m1->d[11]))) - (m1->d[5] * ((m1->d[3] * m1->d[10]) - (m1->d[2] * m1->d[11]))) - (m1->d[ 9] * ((m1->d[2] * m1->d[ 7]) - (m1->d[3] * m1->d[ 6])));
  out->d[ 4] = (m1->d[4] * ((m1->d[11] * m1->d[14]) - (m1->d[10] * m1->d[15]))) - (m1->d[8] * ((m1->d[7] * m1->d[14]) - (m1->d[6] * m1->d[15]))) - (m1->d[12] * ((m1->d[6] * m1->d[11]) - (m1->d[7] * m1->d[10])));
  out->d[ 5] = (m1->d[0] * ((m1->d[10] * m1->d[15]) - (m1->d[11] * m1->d[14]))) - (m1->d[8] * ((m1->d[2] * m1->d[15]) - (m1->d[3] * m1->d[14]))) - (m1->d[12] * ((m1->d[3] * m1->d[10]) - (m1->d[2] * m1->d[11])));
  out->d[ 6] = (m1->d[0] * ((m1->d[ 7] * m1->d[14]) - (m1->d[ 6] * m1->d[15]))) - (m1->d[4] * ((m1->d[3] * m1->d[14]) - (m1->d[2] * m1->d[15]))) - (m1->d[12] * ((m1->d[2] * m1->d[ 7]) - (m1->d[3] * m1->d[ 6])));
  out->d[ 7] = (m1->d[0] * ((m1->d[ 6] * m1->d[11]) - (m1->d[ 7] * m1->d[10]))) - (m1->d[4] * ((m1->d[2] * m1->d[11]) - (m1->d[3] * m1->d[10]))) - (m1->d[ 8] * ((m1->d[3] * m1->d[ 6]) - (m1->d[2] * m1->d[ 7])));
  out->d[ 8] = (m1->d[4] * ((m1->d[ 9] * m1->d[15]) - (m1->d[11] * m1->d[13]))) - (m1->d[8] * ((m1->d[5] * m1->d[15]) - (m1->d[7] * m1->d[13]))) - (m1->d[12] * ((m1->d[7] * m1->d[ 9]) - (m1->d[5] * m1->d[11])));
  out->d[ 9] = (m1->d[0] * ((m1->d[11] * m1->d[13]) - (m1->d[ 9] * m1->d[15]))) - (m1->d[8] * ((m1->d[3] * m1->d[13]) - (m1->d[1] * m1->d[15]))) - (m1->d[12] * ((m1->d[1] * m1->d[11]) - (m1->d[3] * m1->d[ 9])));
  out->d[10] = (m1->d[0] * ((m1->d[ 5] * m1->d[15]) - (m1->d[ 7] * m1->d[13]))) - (m1->d[4] * ((m1->d[1] * m1->d[15]) - (m1->d[3] * m1->d[13]))) - (m1->d[12] * ((m1->d[3] * m1->d[ 5]) - (m1->d[1] * m1->d[ 7])));
  out->d[11] = (m1->d[0] * ((m1->d[ 7] * m1->d[ 9]) - (m1->d[ 5] * m1->d[11]))) - (m1->d[4] * ((m1->d[3] * m1->d[ 9]) - (m1->d[1] * m1->d[11]))) - (m1->d[ 8] * ((m1->d[1] * m1->d[ 7]) - (m1->d[3] * m1->d[ 5])));
  out->d[12] = (m1->d[4] * ((m1->d[10] * m1->d[13]) - (m1->d[ 9] * m1->d[14]))) - (m1->d[8] * ((m1->d[6] * m1->d[13]) - (m1->d[5] * m1->d[14]))) - (m1->d[12] * ((m1->d[5] * m1->d[10]) - (m1->d[6] * m1->d[ 9])));
  out->d[13] = (m1->d[0] * ((m1->d[ 9] * m1->d[14]) - (m1->d[10] * m1->d[13]))) - (m1->d[8] * ((m1->d[1] * m1->d[14]) - (m1->d[2] * m1->d[13]))) - (m1->d[12] * ((m1->d[2] * m1->d[ 9]) - (m1->d[1] * m1->d[10])));
  out->d[14] = (m1->d[0] * ((m1->d[ 6] * m1->d[13]) - (m1->d[ 5] * m1->d[14]))) - (m1->d[4] * ((m1->d[2] * m1->d[13]) - (m1->d[1] * m1->d[14]))) - (m1->d[12] * ((m1->d[1] * m1->d[ 6]) - (m1->d[2] * m1->d[ 5])));
  out->d[15] = (m1->d[0] * ((m1->d[ 5] * m1->d[10]) - (m1->d[ 6] * m1->d[ 9]))) - (m1->d[4] * ((m1->d[1] * m1->d[10]) - (m1->d[2] * m1->d[ 9]))) - (m1->d[ 8] * ((m1->d[2] * m1->d[ 5]) - (m1->d[1] * m1->d[ 6])));

  if (0.0f == (det = ((m1->d[0] * out->d[0]) + (m1->d[1] * out->d[4]) + (m1->d[2] * out->d[8]) + (m1->d[3] * out->d[12])))) {
    return(0);
  }

  det = 1.0f / det;

  for (i = 0; i < 16; i++) {
    out->d[i] *= det;
  }

  return(1);
}

/**
 * 
 */
OXCART_INLINE oxcart_mat4_t OXCART_VCALL oxcart_mat4_affineinverse(const oxcart_mat4_t* m1)
{
  oxcart_vec4_t tran;
  oxcart_mat4_t mat4;

  OXCART_ASSERT(m1);

  mat4 = *m1;
  mat4.d[12] = 0.0f;
  mat4.d[13] = 0.0f;
  mat4.d[14] = 0.0f;
  mat4.d[15] = 1.0f;
  mat4 = oxcart_mat4_transpose(&mat4);

  tran = oxcart_vec4_set(-m1->d[12], -m1->d[13], -m1->d[14], m1->d[15]);
  tran = oxcart_vec4_transform(&tran, &mat4);

  mat4.d[12] = tran.d[0];
  mat4.d[13] = tran.d[1];
  mat4.d[14] = tran.d[2];
  mat4.d[15] = tran.d[3];

  return(mat4);
}

/**
 * 
 */
OXCART_INLINE oxcart_mat4_t OXCART_VCALL oxcart_mat4_orthographic(float w, float h)
{
  oxcart_mat4_t mat4;

  OXCART_ASSERT(w > 0.0f);
  OXCART_ASSERT(h > 0.0f);

  mat4.d[ 0] = 2.0f / w;
  mat4.d[ 1] = 0.0f;
  mat4.d[ 2] = 0.0f;
  mat4.d[ 3] = 0.0f;
  mat4.d[ 4] = 0.0f;
  mat4.d[ 5] = 2.0f / -h;
  mat4.d[ 6] = 0.0f;
  mat4.d[ 7] = 0.0f;
  mat4.d[ 8] = 0.0f;
  mat4.d[ 9] = 0.0f;
  mat4.d[10] = -1.0f;
  mat4.d[11] = 0.0f;
  mat4.d[12] = -1.0f;
  mat4.d[13] = 1.0f;
  mat4.d[14] = 0.0f;
  mat4.d[15] = 1.0f;

  return(mat4);
}

/**
 * 
 */
OXCART_INLINE oxcart_mat4_t OXCART_VCALL oxcart_mat4_perspective(float fovy, float aspect, float n, float f)
{
  float w, h;
  oxcart_mat4_t mat4;

  OXCART_ASSERT(fovy > 0.0f);
  OXCART_ASSERT(aspect > 0.0f);
  OXCART_ASSERT(f > n);

  h = n * tanf(OXCART_DEG2RAD(fovy * 0.5f));
  w = h * aspect;

  mat4.d[ 0] = n / w;
  mat4.d[ 1] = 0.0f;
  mat4.d[ 2] = 0.0f;
  mat4.d[ 3] = 0.0f;
  mat4.d[ 4] = 0.0f;
  mat4.d[ 5] = n / h;
  mat4.d[ 6] = 0.0f;
  mat4.d[ 7] = 0.0f;
  mat4.d[ 8] = 0.0f;
  mat4.d[ 9] = 0.0f;
  mat4.d[10] = (n + f) / (n - f);
  mat4.d[11] = -1.0f;
  mat4.d[12] = 0.0f;
  mat4.d[13] = 0.0f;
  mat4.d[14] = (2.0f * n * f) / (n - f);
  mat4.d[15] = 0.0f;

  return(mat4);
}

/**
 * 
 */
OXCART_INLINE oxcart_mat4_t OXCART_VCALL oxcart_mat4_lookat(const oxcart_vec3_t* eye, const oxcart_vec3_t* target, const oxcart_vec3_t* up)
{
  oxcart_vec3_t x;
  oxcart_vec3_t y;
  oxcart_vec3_t z;
  oxcart_mat4_t mat4;

  OXCART_ASSERT(eye);
  OXCART_ASSERT(target);
  OXCART_ASSERT(up);

  z = oxcart_vec3_subtract(eye, target);
  z = oxcart_vec3_normalize(&z);
  x = oxcart_vec3_cross(up, &z);
  x = oxcart_vec3_normalize(&x);
  y = oxcart_vec3_cross(&z, &x);

  mat4.d[ 0] = x.d[0];
  mat4.d[ 1] = y.d[0];
  mat4.d[ 2] = z.d[0];
  mat4.d[ 3] = 0.0f;
  mat4.d[ 4] = x.d[1];
  mat4.d[ 5] = y.d[1];
  mat4.d[ 6] = z.d[1];
  mat4.d[ 7] = 0.0f;
  mat4.d[ 8] = x.d[2];
  mat4.d[ 9] = y.d[2];
  mat4.d[10] = z.d[2];
  mat4.d[11] = 0.0f;
  mat4.d[12] = -oxcart_vec3_dot(&x, eye);
  mat4.d[13] = -oxcart_vec3_dot(&y, eye);
  mat4.d[14] = -oxcart_vec3_dot(&z, eye);
  mat4.d[15] = 1.0f;

  return(mat4);
}

/**
 * 
 */
OXCART_INLINE int OXCART_VCALL oxcart_isequalf(float a, float b, float epsilon)
{
  float diff;
  float aa, ab;

  if (a == b) {
    return(1);
  }

  diff = fabsf(a - b);

  if ((a == 0.0f) || (b == 0.0f) || (diff < FLT_MIN)) {
    return(diff < (epsilon * FLT_MIN));
  }

  aa = fabsf(a);
  ab = fabsf(b);

  return((diff / (aa + ab)) < epsilon);
}
