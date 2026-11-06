from pathlib import Path
import sys,zipfile,json
sys.path.insert(0,str(Path(__file__).resolve().parents[2]/'map-test-deps'))
from lupa.lua51 import LuaRuntime
r=Path(__file__).resolve().parents[1];pak=r/'companion/Mods/KCD2VRHeldMap/Data/KCD2VRHeldMap.pak'
lua=LuaRuntime(unpack_returned_tuples=True)
with zipfile.ZipFile(pak) as z:
 def reload(path):lua.execute(z.read(path).decode())
 lua.globals().hostReload=reload
 lua.execute('''
calls={};timers={};created=0;spawned=0;position={x=1800,y=2200,z=100};paper=nil
function note(kind,...) calls[#calls+1]={kind,...} end
Script={ReloadScript=function(path) hostReload(path) end,SetTimer=function(ms,fn)timers[#timers+1]=fn;return #timers end,KillTimer=function()end}
System={LogAlways=function(s)note('log',s)end,GetCVar=function(k)if k=='mapname' then return 'trosecko' end; return nil end,
 GetEntityByName=function(name)return paper end,
 SpawnEntity=function(p)assert(p.properties.Physics.bPhysicalize==false and p.properties.bCanTriggerAreas==false);spawned=spawned+1;
  paper={hidden=true,Hide=function(self,v)self.hidden=v~=0 end,IsHidden=function(self)return self.hidden end,
  StartAnimation=function(self,slot,name,layer,blend,speed,loop,hold)assert(slot==0 and layer==0 and blend==0 and speed==1 and loop==false and hold==1);note('anim',name)end,
  ForceCharacterUpdate=function()end};return paper end}
UIAction={RegisterEventSystemListener=function(...)note('register',...)end,
 CallFunction=function(...)note('flash',...)end,SetArray=function(...)note('array',...)end,
 ShowElement=function()error('must not start a screen overlay')end,SetVisible=function()error('must not write native HUD')end}
Game={GetPlayer=function()return g_localActor end}
''')
 reload('Scripts/Startup/zzKCD2VRHeldMap.lua')
 lua.execute('''
assert(#timers==1 and spawned==0 and created==0 and not KCD2VRHeldMap.heldOpen)
g_localActor={id=42,inventory={GetCountOfClass=function()return created end,CreateItem=function(self,id,health,n)assert(health==1 and n==1);created=created+1 end},
 GetWorldPos=function()return position end,GetWorldAngles=function()return {x=0,y=0,z=0}end}
KCD2VRHeldMap.HeldWorldStarted();timers[#timers]()
assert(created==1 and spawned==1 and paper.hidden and not KCD2VRHeldMap.heldOpen)
minimap_asi('1');minimap_apply('1900,2200,1,quest_main_active,A,quest:objective');minimap_flag('2000,2300')
assert(KCD2VRHeldMap.state.flag.x==2000 and KCD2VRHeldMap.state.flag.y==2300)
paper.hidden=false;timers[#timers]()
assert(KCD2VRHeldMap.heldOpen and KCD2VRHeldMap.state.asiSeen)
assert(KCD2VRHeldMap.state.flag.x==2000 and KCD2VRHeldMap.state.flag.y==2300)
assert(KCD2VRHeldMap.cfg.hires==1 and KCD2VRHeldMap.cfg.radius==160 and KCD2VRHeldMap.cfg.rotateMap==0)
assert(KCD2VRHeldMap.cfg.showRoute==0 and KCD2VRHeldMap.cfg.showMeters==0)
assert(KCD2VRHeldMap.heldError==nil,KCD2VRHeldMap.heldError)
local flash=false;local map=false;local data=false
for i,c in ipairs(calls)do
 if c[1]=='flash' then assert(c[2]=='KCD2VRHeldMap');flash=true;if c[4]=='MinimapSetMap' then map=true end end
 if c[1]=='array' then assert(c[2]=='KCD2VRHeldMap');data=true end
end
assert(flash and map and data)
minimap_flag('');assert(KCD2VRHeldMap.state.flag==nil)
paper.hidden=true;timers[#timers]();assert(not KCD2VRHeldMap.heldOpen)
local before=#calls;timers[#timers]();assert(#calls==before) -- no closed map render/array/HUD work
KCD2VRHeldMap.HeldWorldStarted();timers[#timers]();assert(created==1 and spawned==1)
KCD2VRHeldMap.HeldWorldEnded();assert(paper.hidden and not KCD2VRHeldMap.heldOpen)
''')
 report={'lua_version':lua.eval('_VERSION'),'syntax_and_runtime_spies':'PASS','native_leafs':'SUBSTITUTED','startup_closed':True,'idempotent_one_Map_item':True,'nonphysical_own_helper':True,'native_animation_argument_form':True,'native_flag_feed_set_clear':True,'closest_hires_config':True,'closed_Flash_calls':0,'HUD_writes':0,'screen_ShowElement_calls':0,'headset_acceptance':'NOT_RUN'}
 (r/'review/C7FM1/LUA_TEST.json').write_text(json.dumps(report,indent=2));print(json.dumps(report,indent=2))
