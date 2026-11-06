"""Actual map bootstrap/request-owner Lua51 with substituted native leaves.
No invented native timer survival, entity rendering or headset acceptance.
"""
from pathlib import Path
import argparse,json,zipfile
import sys
sys.path.insert(0,"C:/Users/turcs/.codex/worktrees/8fd6/KCD2VR/held-map-c7fm1/map-test-deps")
from lupa.lua51 import LuaRuntime
ROOT=Path(__file__).resolve().parents[1]
parser=argparse.ArgumentParser();parser.add_argument('--source',type=Path,default=ROOT/'mods/KCD2VRMap/map_runtime.lua')
parser.add_argument('--pak',type=Path)
args=parser.parse_args()
SOURCE=args.source.read_text();NAV=(ROOT/'mods/KCD2VRMap/map_navigation_t2.lua').read_text()
SUPPLY=(ROOT/'src/vrik/native_supply_t2.lua').read_text()
PRESENT=(ROOT/"mods/KCD2VRMap/map_presentation_m31.lua").read_text()
NATIVE_READ=(ROOT/'mods/KCD2VRMap/map_native_read_h25.lua').read_text()
if args.pak:
 with zipfile.ZipFile(args.pak)as z:
  source=z.read('Scripts/Startup/zzkcd2vr_map.lua');nav=z.read('Scripts/KCD2VR/map_navigation_t2.lua')
  assert source==args.source.read_bytes() and nav==(ROOT/'mods/KCD2VRMap/map_navigation_t2.lua').read_bytes()
  SOURCE=source.decode();NAV=nav.decode()
  native_read=z.read('Scripts/KCD2VR/map_native_read_h25.lua');assert native_read==(ROOT/'mods/KCD2VRMap/map_native_read_h25.lua').read_bytes();NATIVE_READ=native_read.decode()
  supply=z.read('Scripts/KCD2VR/native_supply_t2.lua')
  assert supply==(ROOT/'src/vrik/native_supply_t2.lua').read_bytes();SUPPLY=supply.decode()
def fixture():
 lua=LuaRuntime(unpack_returned_tuples=True)
 lua.execute("""
 calls={};timers={};created=0;spawned=0;papers={};queries=0;actors={};now=10
 names={'KCD2VR_HeldMap_Render_C7FM1','KCD2VR_HeldMap_Render_C32_Kuttenberg'}
 models={'objects/kcd2vr/map32/trosky.cdf','objects/kcd2vr/map32/kuttenberg.cdf'}
 Script={SetTimer=function(ms,fn)timers[#timers+1]=fn;return #timers end,ReloadScript=function(p)return reloadOwned(p)end}
 System={LogAlways=function(s)calls[#calls+1]=s end,GetCurrTime=function()return now end,
 GetCVar=function(n)assert(n=='sv_map');return 'levels/trosecko' end,
 GetEntity=function(id)return actors[id]end,GetEntityByName=function(name)return papers[name]end,
 SpawnEntity=function(p)
  local k=p.name==names[1] and 1 or p.name==names[2] and 2 or nil
  assert(k and p.properties.object_Model==models[k])
  assert(not p.properties.Physics.bPhysicalize and not p.properties.Physics.bRigidBody)
  assert(not p.properties.bSaved_by_game and not p.properties.bAlwaysUpdate)
  if failSecond and k==2 then return nil end
  spawned=spawned+1
  local e={Properties=p.properties,hidden=true,animations={},Hide=function(self,v)self.hidden=v~=0 end,
   IsHidden=function(self)return self.hidden end,
   StopAnimation=function(self,slot,layer)assert(slot==0 and layer==-1);self.stopped=true end,
   GetAnimationLength=function(self,slot,name)assert(slot==0 and (name=='open' or name=='close'));return missingClip and 0 or 2 end,
   GetAnimationTime=function(self) return self.progress or 0 end,
   StartAnimation=function(self,slot,name)assert(slot==0 and (name=='open' or name=='close') and self.stopped);self.stopped=false;self.animations[#self.animations+1]=name;return not refuseStart end,
   SetAnimationTime=function(self,slot,layer,time)self.progress=time end,
   SetAnimationSpeed=function(self,slot,layer,speed)assert(speed==0)end,
   ForceCharacterUpdate=function(self,slot,on)assert(slot==0);self.forced=on end}
  papers[p.name]=e;return e
 end}
 UIAction={RegisterEventSystemListener=function(...)end,CallFunction=function(...)end}
 function actor(id,opaque)
  local key=opaque and {} or id
  local p={id=key,GetWorldPos=function()return{x=1822,y=2066,z=3}end,GetWorldAngles=function()return{z=0}end,
   inventory={GetCountOfClass=function()queries=queries+1;return created end,
    CreateItem=function(self,item,health,n)assert(item=='5f3c2a71-8e4d-4b6a-9c1e-2d7f0b9a4e63'and health==1 and n==1);created=created+1 end}}
  actors[id]=p;actors[key]=p;return p
 end
 function tick()timers[#timers]()end
 function ready(id,epoch,serial)return kcd2vr_map_t3_ready('R3|'..(serial or 1)..'|'..(epoch or 1)..'|'..id)end
 """)
 modules={'Scripts/KCD2VR/map_presentation_m31.lua':PRESENT,'Scripts/KCD2VR/map_navigation_t2.lua':NAV,'Scripts/KCD2VR/native_supply_t2.lua':SUPPLY,'Scripts/KCD2VR/map_native_read_h25.lua':NATIVE_READ}
 lua.globals().reloadOwned=lambda path:lua.execute(modules[path])
 lua.execute(SOURCE)
 assert lua.eval("type(kcd2vr_map_t2_feed)=='function' and type(kcd2vr_supply_native_t2)=='function'")
 return lua

def main():
 lua=fixture();lua.execute("""
 assert(KCD2VRMap.bootstrapRevision=='33.0-request-owner' and created==0 and spawned==0)
 for i=1,200 do tick()end;assert(queries==0 and spawned==0) -- idle is not readiness polling
 assert(ready(42)=='R3|REFUSED|CURRENT_ACTOR_MISMATCH')
 g_localActor=actor(42,true) -- opaque actor.id is NEVER converted to number
 assert(ready(43)=='R3|REFUSED|CURRENT_ACTOR_MISMATCH' and queries==0)
 assert(kcd2vr_map_t3_ready('R3|1|0|42')=='R3|REFUSED|MALFORMED' and queries==0)
 assert(kcd2vr_map_t3_ready('R3|1|1|4294967296')=='R3|REFUSED|MALFORMED')
 timers={} -- simulate ALL startup timers lost on loading; invoke actual private callback only
 assert(ready(42,7,3)=='R3|33.0-request-owner|42|7|3|1|2|HELPERS_READY')
 assert(created==1 and spawned==2 and KCD2VRMap.world==g_localActor.id)
 local q=queries;local n=#papers[names[1]].animations
 assert(n==0 and papers[names[1]].hidden)
 for i=1,30 do assert(string.find(ready(42,7,3),'HELPERS_READY',1,true))end
 assert(queries==q and spawned==2 and #timers==0) -- no inventory rescan / timer needed
 papers[names[1]].hidden=false
 kcd2vr_map_t2_feed('T2|3|0||') -- actual visible feed owns unfolding transition
 assert(KCD2VRMap.open and papers[names[1]].forced and papers[names[1]].animations[n+1]=='open')
 for i=1,20 do now=now+.5;kcd2vr_map_t2_feed('T2|3|0||')end
 assert(#papers[names[1]].animations==n+1) -- never restart opening per feed/frame
 papers[names[1]].hidden=true;kcd2vr_map_t2_feed('T2|3|0||')
 assert(not KCD2VRMap.open and papers[names[1]].forced and #papers[names[1]].animations==n+1) -- transient hide never closes
 papers[names[1]].progress=1
 papers[names[1]].hidden=false;kcd2vr_map_t2_feed('T2|4|0||')
 assert(#papers[names[1]].animations==n+1) -- resume same opened paper, no unfold loop
 for i=1,32 do
  papers[names[1]].hidden=true;kcd2vr_map_t2_feed('T2|4|0||')
  papers[names[1]].hidden=false;kcd2vr_map_t2_feed('T2|4|0||')
 end
 assert(#papers[names[1]].animations==n+1) -- headset visibility churn must never restart it
 papers[names[1]].hidden=true;papers[names[2]].hidden=false;kcd2vr_map_t2_feed('T2|5|0||')
 assert(not papers[names[1]].forced and papers[names[2]].forced)
 KCD2VRMap.WorldEnded();local qEnd=queries;local sEnd=spawned
 assert(papers[names[1]].hidden and papers[names[2]].hidden)
 assert(ready(42,7,6)=='R3|REFUSED|AWAIT_CURRENT_WORLD') -- outgoing SAME epoch never revived
 assert(queries==qEnd and spawned==sEnd)
 assert(string.find(ready(42,8,6),'HELPERS_READY',1,true)) -- fresh native-owned new epoch recovers missing start event
 assert(queries==qEnd+1 and spawned==sEnd)
 g_localActor=actor(43,true);KCD2VRMap.SyncPresentation(true)
 assert(not KCD2VRMap.world and papers[names[2]].hidden)
 assert(string.find(ready(43,9,7),'HELPERS_READY',1,true) and KCD2VRMap.world==g_localActor.id)
 papers[names[1]]=nil;KCD2VRMap.SyncPresentation(true)
 assert(not KCD2VRMap.world and papers[names[2]].hidden)
 assert(string.find(ready(43,10,8),'HELPERS_READY',1,true) and spawned==sEnd+1)
 """)
 lua=fixture();lua.execute("""
 KCD2VRMap.WorldEnded();g_localActor=actor(45)
 for i=1,200 do tick()end;assert(queries==0 and spawned==0)
 assert(string.find(ready(45,25,1),'HELPERS_READY',1,true)) -- no prior epoch: fresh native owner is the start witness
 """)
 lua=fixture();lua.execute("""
 g_localActor=actor(46);failSecond=true
 assert(ready(46)=='R3|REFUSED|HELPER_CREATE_FAILED' and not KCD2VRMap.world)
 assert(spawned==1 and created==1 and papers[names[1]].hidden)
 failSecond=false;assert(string.find(ready(46),'HELPERS_READY',1,true) and spawned==2 and created==1)
 KCD2VRMap.WorldEnded();KCD2VRMap.WorldStarted();papers[names[1]].Properties.object_Model='foreign.cdf'
 assert(ready(46,2)=='R3|REFUSED|HELPER_CREATE_FAILED' and not KCD2VRMap.world)
 """)
 # Native refusals, held-close FIFO, no-progress and hidden progression are
 # intentionally modelled separately from successful Lua call completion.
 lua=fixture();lua.execute("""
 g_localActor=actor(47);assert(string.find(ready(47),'HELPERS_READY',1,true))
 local e=papers[names[1]];missingClip=true;e.hidden=false
 for i=1,20 do KCD2VRMap.SyncPresentation(true)end
 assert(#e.animations==0 and not KCD2VRMap.entities[names[1]].unfolded63)
 assert(KCD2VRMap.entities[names[1]].animationAttemptsM4==3)
 """)
 lua=fixture();lua.execute("""
 g_localActor=actor(48);assert(string.find(ready(48),'HELPERS_READY',1,true))
 local e=papers[names[1]];e.hidden=false;refuseStart=true
 for i=1,20 do KCD2VRMap.SyncPresentation(true)end
 assert(#e.animations==3 and not KCD2VRMap.entities[names[1]].unfolded63)
 """)
 lua=fixture();lua.execute("""
 g_localActor=actor(49);assert(string.find(ready(49),'HELPERS_READY',1,true))
 local e=papers[names[1]];e.hidden=false;KCD2VRMap.SyncPresentation(true)
 e.hidden=true;KCD2VRMap.SyncPresentation(true);assert(e.forced)
 e.progress=.5;KCD2VRMap.SyncPresentation(true)
 assert(not KCD2VRMap.entities[names[1]].unfolded63 and #e.animations==1)
 e.progress=1;KCD2VRMap.SyncPresentation(true)
 assert(KCD2VRMap.entities[names[1]].unfolded63 and not e.forced and e.hidden)
 """)
 lua=fixture();lua.execute("""
 g_localActor=actor(50);assert(string.find(ready(50),'HELPERS_READY',1,true))
 local e=papers[names[1]];e.hidden=false;KCD2VRMap.SyncPresentation(true);e.hidden=true
 for i=1,300 do KCD2VRMap.SyncPresentation(true)end
 assert(#e.animations==1 and not e.forced and not KCD2VRMap.entities[names[1]].unfolded63)
 """)
 print(json.dumps({'actual_request_owner_bootstrap_Lua51':'PASS','timer_loss_no_event_readiness':'PASS',
  'opaque_and_numeric_actor_id':'PASS','current_actor_world_epoch_guards':'PASS',
  'idle_inventory_queries':0,'actual_feed_open_close_reopen_once':'PASS','partial_spawn_foreign_model_fail_closed':'PASS',
  'native_leaves':'SUBSTITUTED','headset_acceptance':'NOT_RUN'},indent=2))
if __name__=='__main__':main()
