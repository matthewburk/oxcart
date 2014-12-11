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

typedef struct oxcart_rdata_t oxcart_rdata_t;

struct oxcart_rdata_t
{
  PHYSFS_file* file;
  char buffer[LUAL_BUFFERSIZE];
};

static void _lua_doexpr(lua_State* L, const char* expr);
static int _lua_searcher(lua_State* L);
static int _lua_loadfile(lua_State* L);
static const char* _lua_reader(lua_State* L, void* data, size_t* size);

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

  lua_getglobal(L, "package");
  if (!lua_istable(L, -1)) {
    OXCART_ASSERT(!"lua_istable() failed");
  }

  lua_getfield(L, -1, "loaders");
  if (!lua_istable(L, -1)) {
    OXCART_ASSERT(!"lua_istable() failed");
  }

  /* right shift elements in package.loaders to free second slot */
  for (i = lua_objlen(L, -1); i >= 2; i--) {
    lua_rawgeti(L, -1, i);
    lua_rawseti(L, -2, i + 1);
  }

  /* insert _lua_searcher into second slot of package.loaders */
  lua_pushcfunction(L, _lua_searcher);
  lua_rawseti(L, -2, 2);
  lua_pop(L, 2);

  return(L);
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

  if (lua_pcall(L, 1, 1, 0)) {
    #ifdef _DEBUG
      fprintf_s(stderr, "%s\n", lua_tostring(L, -1));
    #endif

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

  if (luaL_dostring(L, buffer)) {
    #ifdef _DEBUG
      fprintf_s(stderr, "%s\n", lua_tostring(L, -1));
    #endif

    OXCART_ASSERT(!"luaL_dostring() failed");
  }
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
  oxcart_rdata_t rdata;

  if (!(filename = luaL_checkstring(L, 1))) {
    return(lua_error(L));
  }

  if (!(rdata.file = PHYSFS_openRead(filename))) {
    return(luaL_error(L, "%s: %s", filename, PHYSFS_getLastError()));
  }

  status = lua_load(L, _lua_reader, &rdata, filename);
  PHYSFS_close(rdata.file);

  if (status) {
    return(lua_error(L));
  }

  return(1);
}

/**
 * 
 */
static const char* _lua_reader(lua_State* L, void* data, size_t* size)
{
  PHYSFS_sint64 count;
  oxcart_rdata_t* rdata = (oxcart_rdata_t*)data;

  if (0 >= (count = PHYSFS_read(rdata->file, rdata->buffer, 1, LUAL_BUFFERSIZE))) {
    *size = 0;
    return(0);
  }

  *size = (size_t)count;
  return(rdata->buffer);
}
