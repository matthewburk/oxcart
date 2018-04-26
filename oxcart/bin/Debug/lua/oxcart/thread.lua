local ffi = require 'ffi'

ffi.cdef[[
typedef struct lua_State lua_State;

typedef int (*lua_CFunction) (lua_State *L);

]]
