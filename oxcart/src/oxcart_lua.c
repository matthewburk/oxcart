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

#include <stdio.h>
#include <physfs.h>
#include "oxcart_assert.h"
#include "oxcart_lua.h"

typedef struct oxcart_fdata_t oxcart_fdata_t;

struct oxcart_fdata_t
{
  PHYSFS_file* file;
  char buffer[LUAL_BUFFERSIZE];
};

static void _lua_doexpr(lua_State* L, const char* expr);
static int _lua_traceback(lua_State* L);
static int _lua_searcher(lua_State* L);
static int _lua_loadfile(lua_State* L);
static const char* _lua_freader(lua_State* L, void* data, size_t* size);

/**
 * 
 */
lua_State* oxcart_lua_newstate()
{
  int i;
  lua_State* L;

  if (!(L = luaL_newstate())) {
    OXCART_ASSERT(!"luaL_newstate() failed");
  }

  luaL_openlibs(L);

  lua_pushcfunction(L, _lua_traceback);
  lua_setglobal(L, "_lua_traceback");

  lua_getglobal(L, "package");
  if (!lua_istable(L, -1)) {
    OXCART_ASSERT(!"lua_istable() failed");
  }

  lua_getfield(L, -1, "loaders");
  if (!lua_istable(L, -1)) {
    OXCART_ASSERT(!"lua_istable() failed");
  }

  i = lua_objlen(L, -1);

  /* insert _lua_searcher at end of package.loaders */
  lua_pushcfunction(L, _lua_searcher);
  lua_rawseti(L, -2, i + 1);
  lua_pop(L, 2);

  return(L);
}

/**
 * 
 */
int oxcart_lua_pcall(lua_State* L, int nargs, int nresults)
{
  int index;
  int status;

  OXCART_ASSERT(L);

  index = lua_gettop(L) - nargs;

  lua_getglobal(L, "_lua_traceback");
  if (!lua_isfunction(L, -1)) {
    OXCART_ASSERT(!"lua_isfunction() failed");
  }

  /* put _lua_traceback under chunk and args */
  lua_insert(L, index);
  status = lua_pcall(L, nargs, nresults, index);
  lua_remove(L, index);

  if (status) {
    #ifdef _DEBUG
      fprintf_s(stderr, "%s\n", lua_tostring(L, -1));
	  OXCART_ASSERT(!status);
    #endif

    lua_pop(L, 1);
  }

  return(status);
}

/**
 * 
 */
int oxcart_lua_loadfile(lua_State* L, const char* filename)
{
  OXCART_ASSERT(L);
  OXCART_ASSERT(filename);

  lua_pushcfunction(L, _lua_loadfile);
  lua_pushstring(L, filename);

  if (oxcart_lua_pcall(L, 1, 1)) {
    return(1);
  }

  return(0);
}

/**
 * 
 */
int oxcart_lua_toboolean(lua_State* L, const char* expr)
{
  OXCART_ASSERT(L);
  OXCART_ASSERT(expr);

  _lua_doexpr(L, expr);

  if (!lua_isboolean(L, -1)) {
    #ifdef _DEBUG
      fprintf_s(stderr, "%s\n", expr);
    #endif

    OXCART_ASSERT(!"lua_isboolean() failed");
  }

  return(lua_toboolean(L, -1));
}

/**
 * 
 */
int oxcart_lua_tointeger(lua_State* L, const char* expr)
{
  OXCART_ASSERT(L);
  OXCART_ASSERT(expr);

  _lua_doexpr(L, expr);

  if (!lua_isnumber(L, -1)) {
    #ifdef _DEBUG
      fprintf_s(stderr, "%s\n", expr);
    #endif

    OXCART_ASSERT(!"lua_isnumber() failed");
  }

  return(lua_tointeger(L, -1));
}

/**
 * 
 */
double oxcart_lua_tonumber(lua_State* L, const char* expr)
{
  OXCART_ASSERT(L);
  OXCART_ASSERT(expr);

  _lua_doexpr(L, expr);

  if (!lua_isnumber(L, -1)) {
    #ifdef _DEBUG
      fprintf_s(stderr, "%s\n", expr);
    #endif

    OXCART_ASSERT(!"lua_isnumber() failed");
  }

  return(lua_tonumber(L, -1));
}

/**
 * 
 */
const char* oxcart_lua_tostring(lua_State* L, const char* expr)
{
  OXCART_ASSERT(L);
  OXCART_ASSERT(expr);

  _lua_doexpr(L, expr);

  if (!lua_isstring(L, -1)) {
    #ifdef _DEBUG
      fprintf_s(stderr, "%s\n", expr);
    #endif

    OXCART_ASSERT(!"lua_isstring() failed");
  }

  return(lua_tostring(L, -1));
}

/**
 * 
 */
static void _lua_doexpr(lua_State* L, const char* expr)
{
  static char buffer[LUAL_BUFFERSIZE];

  OXCART_ASSERT(L);
  OXCART_ASSERT(expr);

  sprintf_s(buffer, LUAL_BUFFERSIZE, "return(%s)", expr);

  if (OXCART_LUA_DOSTRING(L, buffer)) {
    OXCART_ASSERT(!"OXCART_LUA_DOSTRING() failed");
  }
}

/**
 * 
 */
static int _lua_traceback(lua_State* L)
{
  if (!lua_isstring(L, 1)) {
    return(1);
  }

  lua_getglobal(L, "debug");
  if (!lua_istable(L, -1)) {
    lua_pop(L, 1);
    return(1);
  }

  lua_getfield(L, -1, "traceback");
  if (!lua_isfunction(L, -1)) {
    lua_pop(L, 2);
    return(1);
  }

  lua_pushvalue(L, 1);   /* pass error message */
  lua_pushinteger(L, 2); /* skip this function and traceback */
  lua_call(L, 2, 1);     /* call debug.traceback */

  return(1);
}

/**
 * 
 */
static int _lua_searcher(lua_State* L)
{
  const char* module;

  if (!(module = luaL_checkstring(L, 1))) {
    return(lua_error(L));
  }

  lua_pushcfunction(L, _lua_loadfile);
  lua_pushstring(L, "/");
  luaL_gsub(L, module, ".", "/");
  lua_pushstring(L, ".lua");
  lua_concat(L, 3);
  lua_call(L, 1, 1);

  return(1);
}

/**
 * 
 */
static int _lua_loadfile(lua_State* L)
{
  int status;
  const char* filename;
  oxcart_fdata_t fdata;

  if (!(filename = luaL_checkstring(L, 1))) {
    return(lua_error(L));
  }

  if (!(fdata.file = PHYSFS_openRead(filename))) {
    return(luaL_error(L, "%s: %s", filename, PHYSFS_getLastError()));
  }

  status = lua_load(L, _lua_freader, &fdata, filename);
  PHYSFS_close(fdata.file);

  if (status) {
    return(lua_error(L));
  }

  return(1);
}

/**
 * 
 */
static const char* _lua_freader(lua_State* L, void* data, size_t* size)
{
  PHYSFS_sint64 count;
  oxcart_fdata_t* fdata = (oxcart_fdata_t*)data;

  if (0 >= (count = PHYSFS_read(fdata->file, fdata->buffer, 1, LUAL_BUFFERSIZE))) {
    *size = 0;
    return(0);
  }

  *size = (size_t)count;
  return(fdata->buffer);
}

#include "oxcart_math.h"




OXCART_API void vec3_zero(oxcart_vec3_t* r)
{
	*r = oxcart_vec3_zero();
}
OXCART_API void vec3_set(float x, float y, float z, oxcart_vec3_t* r)
{
	*r = oxcart_vec3_set(x, y, z);
}
OXCART_API void vec3_setv2(const oxcart_vec2_t* v1, float z, oxcart_vec3_t* r)
{
	*r = oxcart_vec3_setv2(v1, z);
}
OXCART_API void vec3_add(const oxcart_vec3_t* v1, const oxcart_vec3_t* v2, oxcart_vec3_t* r)
{
	*r = oxcart_vec3_add(v1, v2);
}
OXCART_API void vec3_subtract(const oxcart_vec3_t* v1, const oxcart_vec3_t* v2, oxcart_vec3_t* r)
{
	*r = oxcart_vec3_subtract(v1, v2);
}
OXCART_API void vec3_multiply(const oxcart_vec3_t* v1, const oxcart_vec3_t* v2, oxcart_vec3_t* r)
{
	*r = oxcart_vec3_multiply(v1, v2);
}
OXCART_API void vec3_divide(const oxcart_vec3_t* v1, const oxcart_vec3_t* v2, oxcart_vec3_t* r)
{
	*r = oxcart_vec3_divide(v1, v2);
}
OXCART_API void vec3_scale(const oxcart_vec3_t* v1, float scale, oxcart_vec3_t* r)
{
	*r = oxcart_vec3_scale(v1, scale);
}
OXCART_API void vec3_negate(const oxcart_vec3_t* v1, oxcart_vec3_t* r)
{
	*r = oxcart_vec3_negate(v1);
}
OXCART_API void vec3_sqrt(const oxcart_vec3_t* v1, oxcart_vec3_t* r)
{
	*r = oxcart_vec3_sqrt(v1);
}
OXCART_API void vec3_rsqrt(const oxcart_vec3_t* v1, oxcart_vec3_t* r)
{
	*r = oxcart_vec3_rsqrt(v1);
}
OXCART_API void vec3_normalize(oxcart_vec3_t* v1, oxcart_vec3_t* r) 
{
	*r = oxcart_vec3_normalize(v1);
}
OXCART_API void vec3_cross(const oxcart_vec3_t* v1, const oxcart_vec3_t* v2, oxcart_vec3_t* r)
{
	*r = oxcart_vec3_cross(v1, v2);
}
OXCART_API void vec3_transform(const oxcart_vec3_t* v1, const oxcart_mat3_t* m1, oxcart_vec3_t* r)
{
	*r = oxcart_vec3_transform(v1, m1);
}
OXCART_API float vec3_dot(const oxcart_vec3_t* v1, const oxcart_vec3_t* v2)
{
	return oxcart_vec3_dot(v1, v2);
}
OXCART_API float vec3_length(const oxcart_vec3_t* v1)
{
	return oxcart_vec3_length(v1);
}
OXCART_API float vec3_rlength(const oxcart_vec3_t* v1)
{
	return oxcart_vec3_rlength(v1);
}
OXCART_API float vec3_lengthsq(const oxcart_vec3_t* v1)
{
	return oxcart_vec3_lengthsq(v1);
}
OXCART_API float vec3_angle(const oxcart_vec3_t* v1, const oxcart_vec3_t* v2)
{
	return oxcart_vec3_angle(v1, v2);
}




OXCART_API void vec4_zero(oxcart_vec4_t* r)
{
	*r = oxcart_vec4_zero();
}
OXCART_API void vec4_set(float x, float y, float z, float w, oxcart_vec4_t* r)
{
	*r = oxcart_vec4_set(x, y, z, w);
}
OXCART_API void vec4_setv3(const oxcart_vec3_t* v1, float w, oxcart_vec4_t* r)
{
	*r = oxcart_vec4_setv3(v1, w);
}
OXCART_API void vec4_add(const oxcart_vec4_t* v1, const oxcart_vec4_t* v2, oxcart_vec4_t* r)
{
	*r = oxcart_vec4_add(v1, v2);
}
OXCART_API void vec4_subtract(const oxcart_vec4_t* v1, const oxcart_vec4_t* v2, oxcart_vec4_t* r)
{
	*r = oxcart_vec4_subtract(v1, v2);
}
OXCART_API void vec4_multiply(const oxcart_vec4_t* v1, const oxcart_vec4_t* v2, oxcart_vec4_t* r)
{
	*r = oxcart_vec4_multiply(v1, v2);
}
OXCART_API void vec4_divide(const oxcart_vec4_t* v1, const oxcart_vec4_t* v2, oxcart_vec4_t* r)
{
	*r = oxcart_vec4_divide(v1, v2);
}
OXCART_API void vec4_scale(const oxcart_vec4_t* v1, float scale, oxcart_vec4_t* r)
{
	*r = oxcart_vec4_scale(v1, scale);
}
OXCART_API void vec4_negate(const oxcart_vec4_t* v1, oxcart_vec4_t* r)
{
	*r = oxcart_vec4_negate(v1);
}
OXCART_API void vec4_sqrt(const oxcart_vec4_t* v1, oxcart_vec4_t* r)
{
	*r = oxcart_vec4_sqrt(v1);
}
OXCART_API void vec4_rsqrt(const oxcart_vec4_t* v1, oxcart_vec4_t* r)
{
	*r = oxcart_vec4_rsqrt(v1);
}
OXCART_API void vec4_normalize(oxcart_vec4_t* v1, oxcart_vec4_t* r)
{
	*r = oxcart_vec4_normalize(v1);
}
OXCART_API void vec4_cross(const oxcart_vec4_t* v1, const oxcart_vec4_t* v2, oxcart_vec4_t* r)
{
	*r = oxcart_vec4_cross(v1, v2);
}
OXCART_API void vec4_transform(const oxcart_vec4_t* v1, const oxcart_mat4_t* m1, oxcart_vec4_t* r)
{
	*r = oxcart_vec4_transform(v1, m1);
}
OXCART_API float vec4_dot(const oxcart_vec4_t* v1, const oxcart_vec4_t* v2)
{
	return oxcart_vec4_dot(v1, v2);
}
OXCART_API float vec4_length(const oxcart_vec4_t* v1)
{
	return oxcart_vec4_length(v1);
}
OXCART_API float vec4_rlength(const oxcart_vec4_t* v1)
{
	return oxcart_vec4_rlength(v1);
}
OXCART_API float vec4_lengthsq(const oxcart_vec4_t* v1)
{
	return oxcart_vec4_lengthsq(v1);
}
OXCART_API float vec4_angle(const oxcart_vec4_t* v1, const oxcart_vec4_t* v2)
{
	return oxcart_vec4_angle(v1, v2);
}


OXCART_API void mat3_identity(oxcart_mat3_t* r)
{
	*r = oxcart_mat3_identity();
}


OXCART_API void mat4_to_mat3_normal(const oxcart_mat4_t* m1, oxcart_mat3_t* r) 
{
	r->d[0] = m1->d[0];	r->d[1] = m1->d[1];	r->d[2] = m1->d[2];
	r->d[3] = m1->d[4];	r->d[4] = m1->d[5];	r->d[5] = m1->d[6];
	r->d[6] = m1->d[8];	r->d[7] = m1->d[9];	r->d[8] = m1->d[10];

	oxcart_mat3_t inverse = oxcart_mat3_affineinverse(r);
	*r = oxcart_mat3_transpose(&inverse);
}

OXCART_API void mat4_inverse(const oxcart_mat4_t* m1, oxcart_mat4_t* r) 
{
	float determinate;
	*r = oxcart_mat4_inverse(m1, &determinate);
}

OXCART_API void mat4_multiply(const oxcart_mat4_t* m1, const oxcart_mat4_t* m2, oxcart_mat4_t* r) 
{
	*r = oxcart_mat4_multiply(m1, m2);
}

OXCART_API void mat4_lookat(const oxcart_vec3_t* eye, const oxcart_vec3_t* target, const oxcart_vec3_t* up, oxcart_mat4_t* r) 
{
	*r = oxcart_mat4_lookat(eye, target, up);
}

OXCART_API void mat4_perspective(const float fovy, const float aspect, const float n, const float f, oxcart_mat4_t* r)
{
	*r = oxcart_mat4_perspective(fovy, aspect, n, f);
}

OXCART_API void mat4_rotate(const float x, const float y, const float z, const float angle, oxcart_mat4_t* r)
{
	*r = oxcart_mat4_rotate(x, y, z, angle);
}

OXCART_API void mat4_translate(const float x, const float y, const float z, oxcart_mat4_t* r)
{
	*r = oxcart_mat4_translate(x, y, z);
}

OXCART_API void mat4_orthographic(const float w, const float h, oxcart_mat4_t* r)
{
	*r = oxcart_mat4_orthographic(w, h);
}

OXCART_API void mat4_identity(oxcart_mat4_t* r)
{
	*r = oxcart_mat4_identity();
}

