"""R116 packaged swing whoosh callback. Audio engine leaves are spies, never rendered sound.
Lua 5.1 (lupa) when present, else system Lua 5.4 for the common syntax/behaviour.
Also checks every name the C++ table (src/vrik/swing_whoosh_r116.h) can send is an authored
audio_trigger of the shipped male.animevents."""
from pathlib import Path
import ctypes, ctypes.util, re
import xml.etree.ElementTree as ET
root=Path(__file__).resolve().parents[1]
source=(root/'companion/source/Scripts/Startup/zzkcd2vr_swing_whoosh_r116.lua').read_text()
header=(root/'src/vrik/swing_whoosh_r116.h').read_text()
table=set(re.findall(r'"(c_w_[a-z_]+)"',header.split('inline const char* authored')[1].split('inline bool weaponFlick')[0]))
assert len(table)==6,table
native={e.get('parameter') for e in ET.parse(root/'companion/source/Animations/humans/male/male.animevents').iter('event') if e.get('name')=='audio_trigger'}
assert table<=native,'name absent from shipped authored audio: %s'%(table-native)
import sys
sys.path.insert(0,str(root/'tools'))
from companion_sources_d39 import SOURCE_PAIRS,ALLOWED_ADDITIONS
member=SOURCE_PAIRS['companion/source/Scripts/Startup/zzkcd2vr_swing_whoosh_r116.lua']
assert member=='Scripts/Startup/zzkcd2vr_swing_whoosh_r116.lua' and member in ALLOWED_ADDITIONS
try:
 from lupa.lua51 import LuaRuntime
 execute=LuaRuntime(unpack_returned_tuples=True).execute;runtime='Lua 5.1 / lupa'
except ImportError:
 name=ctypes.util.find_library('lua5.4')
 if not name: raise SystemExit('Install optional test dependency lupa (Lua 5.1) or system liblua5.4')
 lib=ctypes.CDLL(name);ptr=ctypes.c_void_p
 lib.luaL_newstate.restype=ptr;lib.luaL_openlibs.argtypes=[ptr]
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
execute(r"""
checks=0;calls={};function ck(v)checks=checks+1;assert(v,'check '..checks)end
ck(kcd2vr_swing_whoosh_r116('W116|1|c_w_sword_flick')=='W116|1|UNAVAILABLE') -- no player yet
player={id=42}
function player:GetDefaultAuxAudioProxyID()return 7 end
function player:ExecuteAudioTrigger(trigger,proxy)calls[#calls+1]={trigger,proxy};return true end
Sound={GetAudioTriggerID=function(name)if name=='c_w_sword_flick' then return 101 end if name=='c_w_mace_flick' then return 102 end end}
ck(kcd2vr_swing_whoosh_r116('W116|2|c_w_sword_flick')=='W116|2|DISPATCHED');ck(#calls==1 and calls[1][1]==101 and calls[1][2]==7)
ck(kcd2vr_swing_whoosh_r116('W116|2|c_w_sword_flick')=='W116|2|SPENT');ck(#calls==1)
ck(kcd2vr_swing_whoosh_r116('W116|3|c_w_halberd_flick')=='W116|3|NO_TRIGGER');ck(#calls==1)
ck(kcd2vr_swing_whoosh_r116('W116|4|c_w_mace_flick')=='W116|4|DISPATCHED');ck(#calls==2)
for _,p in ipairs({'','W116|5|f_he_bowl_put','W116|5|c_w_sword_pommel_hit','W116|0|c_w_sword_flick','W116|5|c_w_sword_flick\n','W116|5|c_w_Sword_flick','W116|x|c_w_sword_flick'})do
 ck(kcd2vr_swing_whoosh_r116(p)=='W116|REFUSED') end
ck(#calls==2)
function player:ExecuteAudioTrigger()error('engine') end
ck(kcd2vr_swing_whoosh_r116('W116|6|c_w_sword_flick')=='W116|6|UNCERTAIN')
print('R116 swing whoosh lua: '..checks..' checks PASS')
""")
print(runtime+'; table names '+', '.join(sorted(table))+' are authored animevents; packaged as '+member)
