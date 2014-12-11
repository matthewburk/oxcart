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

#ifndef OXCART_LUA_H
#define OXCART_LUA_H

#ifdef __cplusplus
extern "C" {
#endif

/* include lua headers within extern C */
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#define OXCART_LUA_DOFILE(L, fn) (oxcart_lua_loadfile(L, fn) || lua_pcall(L, 0, LUA_MULTRET, 0))

lua_State* oxcart_lua_newstate();
int oxcart_lua_loadfile(lua_State* L, const char* filename);
int oxcart_lua_toboolean(lua_State* L, const char* expr);
int oxcart_lua_tointeger(lua_State* L, const char* expr);
double oxcart_lua_tonumber(lua_State* L, const char* expr);
const char* oxcart_lua_tostring(lua_State* L, const char* expr);

#ifdef __cplusplus
}
#endif

#endif
