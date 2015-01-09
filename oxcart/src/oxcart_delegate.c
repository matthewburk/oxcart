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

#include "oxcart_app.h"
#include "oxcart_delegate.h"
#include "oxcart_scene.h"
#include "oxcart_state.h"
#include "oxcart_lua.h"

/**
 * 
 */
void oxcart_delegate_update(size_t elapsed)
{
  if (L) {
	  size_t total = oxcart_time_tick();
	  lua_rawgeti(L, LUA_REGISTRYINDEX, oxcartref);

	  lua_getfield(L, -1, "update");
	  if (lua_isfunction(L, -1)) {
		  lua_pushnumber(L, total);
		  lua_pushnumber(L, elapsed);
		  oxcart_lua_pcall(L, 2, 0);
	  }
	  else {
		  lua_pop(L, 1); //pop oxcart.update
	  }

	  lua_pop(L, 1); //pop oxcart table
  }

  oxcart_window_swap();
}

/**
 * 
 */
void oxcart_delegate_windowevent(int event)
{
  switch (event)
  {
    case OXCART_WINDOW_EVENT_CREATED:
      oxcart_window_setvisible(1);
      break;
  }

  if (L) {
	  int nargs = 0;
	  lua_rawgeti(L, LUA_REGISTRYINDEX, oxcartref);
	  lua_getfield(L, -1, "onwindowevent");

	  if (lua_isfunction(L, -1)) {
		  lua_pushinteger(L, event); nargs++;
		  oxcart_lua_pcall(L, nargs, 0);

		  lua_pop(L, 1);
	  }
	  else {
		  lua_pop(L, 2);
	  }
  }
}

/**
 * 
 */
int oxcart_delegate_command(int command)
{
	if (L) {
		int nargs = 0;
		lua_rawgeti(L, LUA_REGISTRYINDEX, oxcartref);
		lua_getfield(L, -1, "oncommand");

		if (lua_isfunction(L, -1)) {
			lua_pushinteger(L, command); nargs++;
			oxcart_lua_pcall(L, nargs, 0);

			lua_pop(L, 1);
		}
		else {
			lua_pop(L, 2);
		}
	}
  return(0);
}

/**
 * 
 */
void oxcart_delegate_char(char c)
{
	if (L) {
		int nargs = 0;
		lua_rawgeti(L, LUA_REGISTRYINDEX, oxcartref);
		lua_getfield(L, -1, "onchar");

		if (lua_isfunction(L, -1)) {
			lua_pushfstring(L, "%c", c); nargs++;
			oxcart_lua_pcall(L, nargs, 0);
			lua_pop(L, 1);
		}
		else {
			lua_pop(L, 2);
		}
	}
}

/**
 * 
 */
void oxcart_delegate_keydown(int key, int repeat, int alt, int ctrl, int shift)
{
	if (L) {
		int nargs = 0;
		lua_rawgeti(L, LUA_REGISTRYINDEX, oxcartref);
		lua_getfield(L, -1, "onkeydown");

		if (lua_isfunction(L, -1)) {
			lua_pushinteger(L, key); nargs++;
			lua_pushinteger(L, repeat); nargs++;
			lua_pushboolean(L, alt); nargs++;
			lua_pushboolean(L, ctrl); nargs++;
			lua_pushboolean(L, shift); nargs++;
			oxcart_lua_pcall(L, nargs, 0);

			lua_pop(L, 1);
		}
		else {
			lua_pop(L, 2);
		}
	}
}

/**
 * 
 */
void oxcart_delegate_keyup(int key, int alt, int ctrl, int shift)
{
	if (L) {
		int nargs = 0;
		lua_rawgeti(L, LUA_REGISTRYINDEX, oxcartref);
		lua_getfield(L, -1, "onkeyup");

		if (lua_isfunction(L, -1)) {
			lua_pushinteger(L, key); nargs++;
			lua_pushboolean(L, alt); nargs++;
			lua_pushboolean(L, ctrl); nargs++;
			lua_pushboolean(L, shift); nargs++;
			oxcart_lua_pcall(L, nargs, 0);

			lua_pop(L, 1);
		}
		else {
			lua_pop(L, 2);
		}
	}
}

/**
 * 
 */
void oxcart_delegate_mouseenter()
{
	if (L) {
		int nargs = 0;
		lua_rawgeti(L, LUA_REGISTRYINDEX, oxcartref);
		lua_getfield(L, -1, "onmouseenter");

		if (lua_isfunction(L, -1)) {
			oxcart_lua_pcall(L, nargs, 0);

			lua_pop(L, 1);
		}
		else {
			lua_pop(L, 2);
		}
	}
}

/**
 * 
 */
void oxcart_delegate_mouseexit()
{
	if (L) {
		int nargs = 0;
		lua_rawgeti(L, LUA_REGISTRYINDEX, oxcartref);
		lua_getfield(L, -1, "onmouseexit");

		if (lua_isfunction(L, -1)) {
			oxcart_lua_pcall(L, nargs, 0);

			lua_pop(L, 1);
		}
		else {
			lua_pop(L, 2);
		}
	}
}

/**
 * 
 */
void oxcart_delegate_mousemove(int x, int y, int alt, int ctrl, int shift)
{
	if (L) {
		int nargs = 0;
		lua_rawgeti(L, LUA_REGISTRYINDEX, oxcartref);
		lua_getfield(L, -1, "onmousemove");

		if (lua_isfunction(L, -1)) {
			lua_pushinteger(L, x); nargs++;
			lua_pushinteger(L, y); nargs++;
			lua_pushboolean(L, alt); nargs++;
			lua_pushboolean(L, ctrl); nargs++;
			lua_pushboolean(L, shift); nargs++;
			oxcart_lua_pcall(L, nargs, 0);

			lua_pop(L, 1);
		}
		else {
			lua_pop(L, 2);
		}

	}
}

/**
 * 
 */
void oxcart_delegate_mousemoveraw(int x, int y, int alt, int ctrl, int shift)
{
	if (L) {
		int nargs = 0;
		lua_rawgeti(L, LUA_REGISTRYINDEX, oxcartref);
		lua_getfield(L, -1, "onmousemoveraw");

		if (lua_isfunction(L, -1)) {
			lua_pushinteger(L, x); nargs++;
			lua_pushinteger(L, y); nargs++;
			lua_pushboolean(L, alt); nargs++;
			lua_pushboolean(L, ctrl); nargs++;
			lua_pushboolean(L, shift); nargs++;
			oxcart_lua_pcall(L, nargs, 0);

			lua_pop(L, 1);
		}
		else {
			lua_pop(L, 2);
		}

	}
}

/**
 * 
 */
void oxcart_delegate_mousebuttondblclick(int button, int x, int y, int alt, int ctrl, int shift)
{
	if (L) {
		int nargs = 0;
		lua_rawgeti(L, LUA_REGISTRYINDEX, oxcartref);
		lua_getfield(L, -1, "onmousedoubleclick");

		if (lua_isfunction(L, -1)) {
			lua_pushinteger(L, button); nargs++;
			lua_pushinteger(L, x); nargs++;
			lua_pushinteger(L, y); nargs++;
			lua_pushboolean(L, alt); nargs++;
			lua_pushboolean(L, ctrl); nargs++;
			lua_pushboolean(L, shift); nargs++;
			oxcart_lua_pcall(L, nargs, 0);

			lua_pop(L, 1);
		}
		else {
			lua_pop(L, 2);
		}

	}
}

/**
 * 
 */
void oxcart_delegate_mousebuttondown(int button, int x, int y, int alt, int ctrl, int shift)
{
	if (L) {
		int nargs = 0;
		lua_rawgeti(L, LUA_REGISTRYINDEX, oxcartref);
		lua_getfield(L, -1, "onmousedown");

		if (lua_isfunction(L, -1)) {
			lua_pushinteger(L, button); nargs++;
			lua_pushinteger(L, x); nargs++;
			lua_pushinteger(L, y); nargs++;
			lua_pushboolean(L, alt); nargs++;
			lua_pushboolean(L, ctrl); nargs++;
			lua_pushboolean(L, shift); nargs++;
			oxcart_lua_pcall(L, nargs, 0);

			lua_pop(L, 1);
		}
		else {
			lua_pop(L, 2);
		}

	}
}

/**
 * 
 */
void oxcart_delegate_mousebuttonup(int button, int x, int y, int alt, int ctrl, int shift)
{
	if (L) {
		int nargs = 0;
		lua_rawgeti(L, LUA_REGISTRYINDEX, oxcartref);
		lua_getfield(L, -1, "onmouseup");

		if (lua_isfunction(L, -1)) {
			lua_pushinteger(L, button); nargs++;
			lua_pushinteger(L, x); nargs++;
			lua_pushinteger(L, y); nargs++;
			lua_pushboolean(L, alt); nargs++;
			lua_pushboolean(L, ctrl); nargs++;
			lua_pushboolean(L, shift); nargs++;
			oxcart_lua_pcall(L, nargs, 0);

			lua_pop(L, 1);
		}
		else {
			lua_pop(L, 2);
		}

	}
}

/**
 * 
 */
void oxcart_delegate_mousewheelscroll(int delta, int step, int x, int y, int alt, int ctrl, int shift)
{
	if (L) {
		int nargs = 0;
		lua_rawgeti(L, LUA_REGISTRYINDEX, oxcartref);
		lua_getfield(L, -1, "onmousewheel");

		if (lua_isfunction(L, -1)) {
			lua_pushinteger(L, delta); nargs++;
			lua_pushinteger(L, step); nargs++;
			lua_pushinteger(L, x); nargs++;
			lua_pushinteger(L, y); nargs++;
			lua_pushboolean(L, alt); nargs++;
			lua_pushboolean(L, ctrl); nargs++;
			lua_pushboolean(L, shift); nargs++;
			oxcart_lua_pcall(L, nargs, 0);

			lua_pop(L, 1);
		}
		else {
			lua_pop(L, 2);
		}

	}
}

/**
 * 
 */
void oxcart_delegate_gamepadbuttondown(int button, int repeat)
{
}

/**
 * 
 */
void oxcart_delegate_gamepadbuttonup(int button)
{
}

/**
 * 
 */
void oxcart_delegate_gamepadtrigger(int button, int z)
{
}

/**
 * 
 */
void oxcart_delegate_gamepadthumb(int button, int x, int y)
{
}
