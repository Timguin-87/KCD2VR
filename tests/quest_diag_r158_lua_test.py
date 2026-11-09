"""R158 Shift+F5 quest recorder companion Lua (zzkcd2vr_quest_diag_r158.lua) with spy game objects.
checks: packet validation, Henry row, NPC rows nearest first within the sphere answer, only read methods are called
(no Request*/Do*/Start*/Accept*), a missing or throwing method reads '?', an engine error never escapes, the reply
matches the C++ parser format (src/diagnostics/quest_diag_r158.h ParseReply), and the file is packaged.
Lua 5.1 (lupa) when present, else system Lua 5.4 (the script uses only the common 5.1 subset)."""
from pathlib import Path
import ctypes, ctypes.util, re, sys
root=Path(__file__).resolve().parents[1]
path=root/'companion/source/Scripts/Startup/zzkcd2vr_quest_diag_r158.lua'
source=path.read_text()
sys.path.insert(0,str(root/'tools'))
from companion_sources_d39 import SOURCE_PAIRS,ALLOWED_ADDITIONS
member=SOURCE_PAIRS['companion/source/Scripts/Startup/zzkcd2vr_quest_diag_r158.lua']
assert member=='Scripts/Startup/zzkcd2vr_quest_diag_r158.lua' and member in ALLOWED_ADDITIONS
# every engine method the script names must be one the game's own BasicAIActions.lua (or an existing companion
# script) already calls - no guessed API.
known=(root/'companion/source/Scripts/Entities/AI/Shared/BasicAIActions.lua').read_text()+\
      (root/'companion/source/Scripts/Startup/zzkcd2vr_life_d73.lua').read_text()+\
      (root/'companion/source/zzkcd2vr_corpse_d72.lua').read_text()+\
      (root/'mods/KCD2VRGore/r93r3/zzkcd2vr_gore.lua').read_text()
names=set(re.findall(r"call\([^,]+,'([A-Za-z]+)'",source))
assert names, 'no engine calls found'
for n in sorted(names): assert re.search(r'[:.]'+n+r'\b',known), 'unverified engine method '+n
assert 'GetEntitiesInSphere' in known
for bad in ('Request','DoChat','StartFollow','AcceptChat','OpenChat','AddLink','RemoveLink','Set','Kill'):
  assert not re.search(r"'"+bad,source), 'write/action call '+bad
outputs=[]
try:
 from lupa.lua51 import LuaRuntime
 lua=LuaRuntime(unpack_returned_tuples=True)
 def execute(text): lua.execute(text)
 def evaluate(text): return lua.eval(text)
 runtime='Lua 5.1 / lupa'
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
 def run(text,results):
  status=lib.luaL_loadstring(state,text.encode())
  if not status: status=lib.lua_pcallk(state,0,results,0,0,None)
  if status: raise AssertionError(lib.lua_tolstring(state,-1,None).decode())
  value=lib.lua_tolstring(state,-1,None) if results else None
  lib.lua_settop(state,0);return value.decode() if value is not None else None
 def execute(text): run(text,0)
 def evaluate(text): return run('return '+text,1)
 runtime='system Lua 5.4 (5.1 runtime not available)'
execute(source)
execute(r"""
calls={}
local function spy(tag,v) return function(self,...) calls[#calls+1]=tag; return v end end
local function npc(id,name,x,dead,chatReq)
  return {id=id,class='NPC',GetName=function() return name end,GetWorldPos=function() return {x=x,y=0,z=0} end,
    soul={IsInCombatMode=spy('combatMode',true),IsInCombatDanger=spy('npcDanger',true),
          HasScriptContext=function(self,c) calls[#calls+1]='ctx:'..c; return c=='combat_arrangedFight' end,
          IsDialogRestricted=spy('restricted',false)},
    actor={IsDead=spy('dead',dead),IsUnconscious=spy('unc',false),GetHealth=spy('hp',87.6),CanTalk=spy('talk',false),
           CanChat=spy('chat',false),HasChatRequest=spy('req',chatReq),HasAcceptedChat=spy('acc',false),
           IsWaitingForDialogueReply=function() error('engine fault') end}}
end
player={id=1,class='Player',GetWorldPos=function() return {x=0,y=0,z=0} end,
  soul={IsInCombatDanger=spy('pDanger',true),IsInTenseCircumstance=spy('pTense',false),IsInCombatMode=spy('pMode',true)},
  actor={IsDead=spy('pDead',false),CanFollow=spy('canFollow',true),IsFollowing=spy('following',false)},
  player={IsCombatChatTarget=spy('chatTarget',false)}}
local far=npc(30,'far_one',20,false,false)
local hans=npc(10,'hans_capon',3,false,false)
local rosa=npc(20,'rosa|x y',9.5,false,true)
local item={id=40,class='PickableItem',GetWorldPos=function() return {x=1,y=0,z=0} end}
System={GetEntitiesInSphere=function(p,r) sphere=r; return {far,item,player,hans,rosa} end}
""")
ck=0
def check(cond,msg):
  global ck
  assert cond,msg;ck+=1
check(evaluate("kcd2vr_quest_diag_r158('x')")=='Q158|BAD','bad packet');check(evaluate("kcd2vr_quest_diag_r158('Q158|0')")=='Q158|BAD','serial 0')
check(evaluate("kcd2vr_quest_diag_r158(5)")=='Q158|BAD','not a string')
reply=evaluate("kcd2vr_quest_diag_r158('Q158|12')")
lines=reply.split('\n')
check(lines[0]=='Q158|12','header carries the serial')
check(lines[1]=='P|combatDanger=1 tenseCircumstance=0 combatMode=1 dead=0 npcs25m=3 query=OK','henry row: '+lines[1])
check(evaluate('sphere')=='25','25 m sphere')
check(lines[2].startswith('N|10|hans_capon|d=3.0 dead=0 unconscious=0 hp=88 combatMode=1 combatDanger=1 arrangedFight=1 suppressedDialogInFight=0 canTalk=0'),'nearest first: '+lines[2])
check('waitingReply=?' in lines[2],'throwing method reads ?')
check(lines[3].startswith('N|20|rosa_x_y|d=9.5 ') and 'chatRequest=1' in lines[3],'name cleaned, chat request: '+lines[3])
check(lines[4].startswith('N|30|far_one|d=20.0'),'far one last');check(len(lines)==5,'items and the player are not NPC rows')
check('combatChatTarget=0 canFollow=1 following=0' in lines[2],'player-side follow/chat target reads')
check(all('|' not in l.split('|',3)[3] for l in lines[2:]),'no | inside fields (C++ parser format)')
execute("player=nil")
check(evaluate("kcd2vr_quest_diag_r158('Q158|13')")=='Q158|13\nP|player=NONE','no player yet')
execute("player={id=1,soul={},actor={},GetWorldPos=function() error('boom') end}")
check(evaluate("kcd2vr_quest_diag_r158('Q158|14')")=='Q158|14\nP|combatDanger=? tenseCircumstance=? combatMode=? dead=? npcs25m=0 query=NO_PLAYER_POS','faults read ?')
execute("player={id=1,soul={},actor={},GetWorldPos=function() return {x=0,y=0,z=0} end};System={GetEntitiesInSphere=function() error('x') end}")
check(evaluate("kcd2vr_quest_diag_r158('Q158|15')").endswith('query=QUERY_FAILED'),'query fault contained')
execute("local many={} for i=1,20 do many[i]={id=100+i,class='NPC',soul={},actor={},GetName=function() return 'n'..i end,GetWorldPos=function() return {x=i,y=0,z=0} end} end System={GetEntitiesInSphere=function() return many end}")
r=evaluate("kcd2vr_quest_diag_r158('Q158|16')").split('\n')
check(len([l for l in r if l.startswith('N|')])==12 and r[-1]=='E|8_more_npcs_beyond_the_nearest_12','bounded to the nearest 12 + a count')
check(max(len(l) for l in reply.split('\n'))<480,'rows fit the C++ row (512)')
print('R158 quest recorder lua: %d checks PASS (%s); packaged as %s'%(ck,runtime,member))
