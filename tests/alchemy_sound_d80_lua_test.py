"""Actual packaged callback. Audio engine leaves are spies, never rendered sound.
Uses Lua 5.1 through optional lupa when present; a system Lua 5.4 fallback is
explicitly reported and checks this callback's common Lua syntax/behaviour.
"""
from pathlib import Path
import ctypes, ctypes.util, re
import xml.etree.ElementTree as ET
root=Path(__file__).resolve().parents[1]
source=(root/'companion/source/Scripts/Startup/zzkcd2vr_alchemy_sound_d80.lua').read_text()
names=re.findall(r"'((?:f_he_|f_ge_)[^']+)'",source)
assert len(names)==16
native={e.get('parameter') for e in ET.parse(root/'companion/source/Animations/humans/male/male.animevents').iter('event') if e.get('name')=='audio_trigger'}
assert all(n in native for n in names), 'cue absent from shipped native authored audio'
try:
 from lupa.lua51 import LuaRuntime
 lua=LuaRuntime(unpack_returned_tuples=True)
 execute=lua.execute
 runtime='Lua 5.1 / lupa'
except ImportError:
 name=ctypes.util.find_library('lua5.4')
 if not name: raise SystemExit('Install optional test dependency lupa (Lua 5.1) or system liblua5.4')
 lib=ctypes.CDLL(name); ptr=ctypes.c_void_p
 lib.luaL_newstate.restype=ptr
 lib.luaL_openlibs.argtypes=[ptr]
 lib.luaL_loadstring.argtypes=[ptr,ctypes.c_char_p];lib.luaL_loadstring.restype=ctypes.c_int
 lib.lua_pcallk.argtypes=[ptr,ctypes.c_int,ctypes.c_int,ctypes.c_int,ctypes.c_ssize_t,ptr];lib.lua_pcallk.restype=ctypes.c_int
 lib.lua_tolstring.argtypes=[ptr,ctypes.c_int,ptr];lib.lua_tolstring.restype=ctypes.c_char_p
 lib.lua_settop.argtypes=[ptr,ctypes.c_int]
 state=lib.luaL_newstate();lib.luaL_openlibs(state)
 def execute(text):
  status=lib.luaL_loadstring(state,text.encode())
  if not status: status=lib.lua_pcallk(state,0,0,0,0,None)
  if status: raise AssertionError(lib.lua_tolstring(state,-1,None).decode())
  lib.lua_settop(state,0)
 runtime='system Lua 5.4 (5.1 runtime not available)'
execute(source)
execute("names={"+','.join(repr(n) for n in names)+"}")
execute(r"""
checks=0;calls={};lookups={};function ck(v)checks=checks+1;assert(v,'check '..checks)end
AudioUtils=nil -- optional table is deliberately absent
player={id=42,actor={}}
function player:GetDefaultAuxAudioProxyID()return 7 end
function player:ExecuteAudioTrigger(trigger,proxy)calls[#calls+1]={self,trigger,proxy};return true end
System={GetEntity=function(id)if id==42 then return player end end}
Sound={GetAudioTriggerID=function(name)lookups[#lookups+1]=name;for i,n in ipairs(names)do if n==name then return i+100 end end end}
for cue=1,16 do
 ck(kcd2vr_alchemy_sound_d80('A80|1|2|'..cue..'|42|'..cue)=='A80|'..cue..'|DISPATCHED')
 local c=calls[#calls];ck(c[1]==player and c[2]==cue+100 and c[3]==7)
 ck(lookups[#lookups]==names[cue]);ck(kcd2vr_alchemy_sound_d80('A80|1|2|'..cue..'|42|'..cue)=='A80|'..cue..'|SPENT')
end
ck(#calls==16)
local base=#calls
for _,p in ipairs({'','A80|1|2|0|42|1','A80|1|2|17|0|1','A80|1|2|17|42|17','A80|1|2|17|42|0','A80|1|2|17|42|1\n','A80|1|2|17|42|1|x','A80|1|2|17|16777216|1'})do ck(kcd2vr_alchemy_sound_d80(p)=='A80|REFUSED') end
ck(#calls==base)
System.GetEntity=function()return {}end
ck(kcd2vr_alchemy_sound_d80('A80|1|2|17|42|1')=='A80|17|WRONG_PLAYER');ck(#calls==base)
System.GetEntity=function()return player end
local lookup=Sound.GetAudioTriggerID;Sound.GetAudioTriggerID=function()return 0 end
ck(kcd2vr_alchemy_sound_d80('A80|1|2|18|42|1')=='A80|18|NO_TRIGGER');ck(#calls==base);Sound.GetAudioTriggerID=lookup
local play=player.ExecuteAudioTrigger
player.ExecuteAudioTrigger=function(self,...)play(self,...);error('engine entered then failed')end
ck(kcd2vr_alchemy_sound_d80('A80|1|2|19|42|1')=='A80|19|UNCERTAIN');ck(#calls==base+1)
ck(kcd2vr_alchemy_sound_d80('A80|1|2|19|42|1')=='A80|19|SPENT');ck(#calls==base+1)
player.ExecuteAudioTrigger=play
""")
execute(source)
execute("ck(kcd2vr_alchemy_sound_d80('A80|1|2|19|42|1')=='A80|19|SPENT'); print('D81 actual packaged alchemy audio: '..checks..' checks PASS; engine leaves mocked')")
print(runtime+'; all16 cue names matched shipped animevents; no audio rendered')
