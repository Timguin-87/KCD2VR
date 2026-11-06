"""Small native Lua 5.4 host for source-level adapter tests on Linux.

This executes actual Lua functions, not a Python translation. It is not the
retail game's Lua 5.1 VM or engine ABI. No game APIs are provided implicitly.
"""
import os
import sys
import ctypes as C
import ctypes.util


class LuaHost:
    def __init__(self):
        name = ctypes.util.find_library('lua5.4')
        if not name:
            raise RuntimeError('liblua5.4 is required for this host fixture')
        self.lib = C.CDLL(name)
        signatures = {
            'luaL_newstate': (C.c_void_p, []),
            'luaL_openlibs': (None, [C.c_void_p]),
            'lua_close': (None, [C.c_void_p]),
            'luaL_loadbufferx': (C.c_int, [C.c_void_p, C.c_char_p, C.c_size_t, C.c_char_p, C.c_char_p]),
            'lua_pcallk': (C.c_int, [C.c_void_p, C.c_int, C.c_int, C.c_int, C.c_ssize_t, C.c_void_p]),
            'lua_tolstring': (C.c_void_p, [C.c_void_p, C.c_int, C.POINTER(C.c_size_t)]),
            'lua_getglobal': (C.c_int, [C.c_void_p, C.c_char_p]),
            'lua_tonumberx': (C.c_double, [C.c_void_p, C.c_int, C.POINTER(C.c_int)]),
            'lua_settop': (None, [C.c_void_p, C.c_int]),
        }
        for name, (restype, argtypes) in signatures.items():
            fn = getattr(self.lib, name); fn.restype = restype; fn.argtypes = argtypes
        self.state = self.lib.luaL_newstate()
        if not self.state:
            raise MemoryError('Lua state allocation failed')
        self.lib.luaL_openlibs(self.state)
        self.run('unpack=table.unpack;loadstring=load;math.atan2=function(y,x)return math.atan(y,x)end', 'host_compat.lua')

    def run(self, text, name='test.lua'):
        data = text.encode() if isinstance(text, str) else text
        status = self.lib.luaL_loadbufferx(self.state, data, len(data), ('@'+name).encode(), None)
        if not status:
            status = self.lib.lua_pcallk(self.state, 0, 0, 0, 0, None)
        if status:
            n = C.c_size_t(); ptr = self.lib.lua_tolstring(self.state, -1, C.byref(n))
            error = C.string_at(ptr, n.value).decode(errors='replace') if ptr else 'unknown Lua error'
            self.lib.lua_settop(self.state, 0)
            raise RuntimeError(error)

    def number(self, name):
        self.lib.lua_getglobal(self.state, name.encode()); valid = C.c_int()
        result = self.lib.lua_tonumberx(self.state, -1, C.byref(valid))
        self.lib.lua_settop(self.state, -2)
        if not valid.value:
            raise ValueError('Lua global is not numeric: '+name)
        return result

    def close(self):
        if self.state:
            self.lib.lua_close(self.state); self.state = None

    def __enter__(self): return self
    def __exit__(self, *unused): self.close()

# The Windows review environment already provides Lua 5.1 through lupa.
if os.name == 'nt':
    try:
        from lupa.lua51 import LuaRuntime
    except ImportError:
        sys.path.insert(0, r'C:/Users/turcs/.codex/worktrees/8fd6/KCD2VR/held-map-c7fm1/map-test-deps')
        from lupa.lua51 import LuaRuntime
    class LuaHost:
        def __init__(self): self.state = LuaRuntime(unpack_returned_tuples=True)
        def run(self, text, name='test.lua'):
            if isinstance(text, bytes): text=text.decode('utf-8-sig')
            self.state.execute('assert(loadstring(..., ...))()',text,'@'+name)
        def number(self,name): return self.state.globals()[name]
        def close(self): self.state=None
        def __enter__(self): return self
        def __exit__(self,*unused): self.close()
