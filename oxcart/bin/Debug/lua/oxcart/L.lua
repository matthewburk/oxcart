local ffi = require 'ffi'
local C = ffi.C

ffi.cdef[[
typedef struct lua_State lua_State;
lua_State *luaL_newstate(void);
void luaL_openlibs(lua_State *L);
void lua_close(lua_State *L);
int luaL_loadstring(lua_State *L, const char *s);
int lua_pcall(lua_State *L, int nargs, int nresults, int errfunc);
]]

local M = {}

function threadFuncToAddress(thread_entry)
	return tonumber(ffi.cast('intptr_t', ffi.cast('void *(*)(void *)', thread_entry)))
end



-- Destroys all objects in the given Lua state
-- if Lua state is still running you WILL get crash
function luaStateDelete(luaState)
	C.lua_close(luaState)
end

function threadToIdString(thread)
	return tostring(thread)
end

	function threadSelf()
		local id = C.GetCurrentThreadId()
		return threadToIdString(id)
	end

	-- then use pthread_create() from the original state, passing the callback address of the other state
	function luaThreadCreate(func_ptr, arg, flags)
		flags = flags or 0
		local thread_c = ffi.new("DWORD[1]")
		local arg_c = ffi.cast("void *", arg) -- necessary if arg is not cstr, should we we check arg type?
		local func_ptr_c = ffi.cast("void *", func_ptr)
		local handle = C.CreateThread(nil, 0, func_ptr_c, arg_c, flags, thread_c)
		assert(handle)
		return thread_c
	end

	function threadJoin(thread_id)
		local thread = ffi.cast('void *', thread_id)
		local return_val = C.WaitForSingleObject(thread, C.INFINITE)
		return return_val
	end


  -- create a separate Lua state first
-- define a callback function in *that* created state
function luaStateCreate(lua_code, thread_entry_address_name)
	local thread_entry_address = thread_entry_address_name or "thread_entry_address"
	local L = C.luaL_newstate()
	assert(L ~= nil)

	-- http://williamaadams.wordpress.com/2012/04/03/step-by-step-inch-by-inch/
	C.lua_gc(L, C.LUA_GCSTOP, 0) -- stop collector during initialization
	C.luaL_openlibs(L)
	C.lua_gc(L, C.LUA_GCRESTART, -1)

	assert(C.luaL_loadstring(L, lua_code) == 0)
	local res = C.lua_pcall(L, 0, 1, 0) -- runs code
	-- defines functions and variables, we need thread_entry_address -variable
	-- that is the thread_entry() -function memory pointer Lua-number
	assert(res == 0)

	-- get function thread_entry() address from calling thread
	-- http://pgl.yoyo.org/luai/i/lua_call
	C.lua_getfield(L, C.LUA_GLOBALSINDEX, thread_entry_address) 
	-- thread_entry_address = function or (usually) global variable to be called
	local func_ptr = C.lua_tointeger(L, -1); -- lua_getfield value
	C.lua_settop(L, -2); -- set stack to correct (prev call params -1?)
	return L, func_ptr
end

function M.new(threadmain)
  local L = C.luaL_newstate()
  assert(L ~= nil)

  C.lua_gc(L, C.LUA_GCSTOP, 0)
  C.luaL_openlibs(L)
  C.lua_gc(L, C.LUA_GCRESTART, -1)

  assert(C.luaL_loadstring(L, threadmain) == 0)
  assert(C.lua_pcall(L, 0, 1, 0) == 0)

  function luaThreadCreate(func_ptr, arg, flags)
		flags = flags or 0
		local thread_c = ffi.new("DWORD[1]")
		local arg_c = ffi.cast("void *", arg) -- necessary if arg is not cstr, should we we check arg type?
		local func_ptr_c = ffi.cast("void *", func_ptr)
		local handle = C.CreateThread(nil, 0, func_ptr_c, arg_c, flags, thread_c)
		assert(handle)
		return thread_c
	end

  C.lua_close(L)
end

return M

