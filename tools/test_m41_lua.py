from pathlib import Path
import sys,json,hashlib
from m41_test_support import options,lua_runtime
args=options('lua');ROOT=args.source_root;out=args.out
LuaRuntime=lua_runtime(args)
lua=LuaRuntime(unpack_returned_tuples=True)
lua.execute(r'''
checks=0;spawns=0;loads=0;removes=0;reloads={};entities={};events={};level='trosecko';badEntity=false
function check(v)assert(v);checks=checks+1 end
ENTITY_FLAG_NO_SAVE=32768;ENTITY_FLAG_CLIENT_ONLY=256
actor={id=42,GetWorldPos=function()return{x=100,y=900,z=0}end};g_localActor=actor
System={GetEntity=function(id)if badEntity then return nil end;if id==actor.id then return actor end;for _,e in pairs(entities)do if e.id==id or e.nativeId==id then return e end end end,
 GetCVar=function(n)assert(n=='sv_map');return level end,
 GetEntityByName=function(n)return entities[n]end,
 RemoveEntity=function(id)for n,e in pairs(entities)do if e.id==id then entities[n]=nil;removes=removes+1 end end end,
 SpawnEntity=function(t)
  assert(t.class=='BasicEntity'and not t.properties.Physics.bPhysicalize)
  assert(t.flags==32768+256 and t.properties.bSaved_by_game==false)
  assert(t.properties.Physics.bRigidBody==false and t.properties.MultiplayerOptions.bNetworked==false)
  spawns=spawns+1;local e={id=100+spawns,nativeId=100+spawns,name=t.name,hidden=true,model=t.properties.object_Model}
  e.Hide=function(self,h)self.hidden=h==1 end
  e.LoadObject=function(self,slot,path)assert(slot==0);loads=loads+1;self.model=path end
  entities[t.name]=e;return e
 end}
Script={ReloadScript=function(s)reloads[s]=true end}
UIAction={RegisterEventSystemListener=function(m,_,event,fn)events[event]=function()m[fn]()end end}
KCD2VRMapCatalogM39={levels={trosecko={rotation=0,tiles={1,1},world={0,0,1000,1000},level={1000,1000},crop={0,0,1,1}},kutnohorsko={rotation=0,tiles={1,1},world={0,0,1000,1000},level={1000,1000},crop={0,0,1,1}}},maps={
 {region='trosecko',name='troskovice',active={0,0,500,500},rect={0,0,500,500},priority=1,ellipse=false,inside=false},
 {region='trosecko',name='trosky',active={0,0,500,500},rect={0,0,500,500},priority=2,ellipse=false,inside=false}
},inside={}}
''')
runtime=(ROOT/'mods/KCD2VRMap/map_runtime.lua').read_bytes();lua.execute(runtime.decode())
lua.execute(r'''
-- Fixture native presenter supplies CEntity+0C after the hidden spawn; it never
-- converts the Lua opaque entity ID. Both actual callback invocations execute.
function requestReadyM41(packet)
 local reply=kcd2vr_map_t3_ready(packet..'|0')
 if reply:find('|HELPER_PENDING_ID|',1,true)then
  local e=entities.KCD2VR_StaticPaper_M41
  return kcd2vr_map_t3_ready(packet..'|'..e.nativeId)
 end
 return reply
end
''')
lua.execute(r'''
check(reloads['Scripts/KCD2VR/native_supply_t2.lua'])
check(reloads['Scripts/KCD2VR/map_catalog_static_m41.lua'])
check(reloads['Scripts/KCD2VR/map_navigation_t2.lua']and reloads['Scripts/KCD2VR/map_native_live_h28.lua']and reloads['Scripts/KCD2VR/map_native_read_h25.lua'])
check(spawns==0 and loads==0) -- no Startup inventory, timer, UI/movie or spawn
check(requestReadyM41('bad')=='R3|REFUSED|MALFORMED')
check(requestReadyM41('R3|0|7|42')=='R3|REFUSED|MALFORMED')
check(requestReadyM41('R3|1|0|42')=='R3|REFUSED|MALFORMED')
check(requestReadyM41('R3|1|7|43')=='R3|REFUSED|CURRENT_ACTOR_MISMATCH')
local reply=requestReadyM41('R3|1|7|42')
check(reply=='R3|41.0-static-paper|42|7|1|1|1|HELPER_READY|trosky|101')
check(spawns==1 and KCD2VRMap.entities.KCD2VR_StaticPaper_M41.art=='trosky')
check(kcd2vr_map_t3_ready('R3|1|7|42|999')=='R3|REFUSED|HELPER_IDENTITY_MISMATCH')
entities.KCD2VR_StaticPaper_M41.id={}
check(requestReadyM41('R3|1|7|42')==reply) -- opaque Lua ID, native integer wire join
check(entities.KCD2VR_StaticPaper_M41.hidden)
check(entities.KCD2VR_StaticPaper_M41.model=='Objects/kcd2vr/staticmap40/trosky.cgf')
check(requestReadyM41('R3|1|7|42')==reply and spawns==1 and loads==0)
check(kcd2vr_map_m31_intent('I31|1|1')=='I31|REFUSED')
entities.KCD2VR_StaticPaper_M41:Hide(0)
check(kcd2vr_map_m31_intent('I31|2|0')=='I31|2|CLOSED')
check(entities.KCD2VR_StaticPaper_M41.hidden)
check(requestReadyM41('R3|1|7|42')=='R3|REFUSED|STALE_INTENT')
check(kcd2vr_map_m31_intent('I31|1|0')=='I31|REFUSED')
level='kutnohorsko'
check(requestReadyM41('R3|3|7|42'):find('|HELPER_READY|kutnohorsko',1,true)~=nil)
check(spawns==1 and loads==1 and KCD2VRMap.entities.KCD2VR_StaticPaper_M41.art=='kutnohorsko')
check(requestReadyM41('R3|4|8|42'):find('HELPER_READY',1,true)~=nil)
check(removes==1 and spawns==2)
check(requestReadyM41('R3|4|7|42')=='R3|REFUSED|STALE_BODY')
entities.KCD2VR_StaticPaper_M41={id=999}
check(requestReadyM41('R3|5|8|42'):find('REFUSED',1,true)~=nil)
check(entities.KCD2VR_StaticPaper_M41.id==999) -- foreign replacement never deleted
entities.KCD2VR_StaticPaper_M41=nil
check(requestReadyM41('R3|6|8|42'):find('HELPER_READY',1,true)~=nil)
events.OnGameplayEnded();check(KCD2VRMap.world==nil and next(KCD2VRMap.entities)==nil)
check(requestReadyM41('R3|7|8|42')=='R3|REFUSED|CURRENT_ACTOR_MISMATCH')
events.OnGameplayEnded();check(requestReadyM41('R3|8|8|42')=='R3|REFUSED|CURRENT_ACTOR_MISMATCH')
-- Original current-native caller may qualify a NEW body/epoch without relying
-- on an optional gameplay event. The old epoch never grants the new paper.
check(requestReadyM41('R3|9|9|42'):find('HELPER_READY',1,true)~=nil)
events.OnGameplayEnded();events.OnGameplayStarted()
check(requestReadyM41('R3|10|10|42'):find('HELPER_READY',1,true)~=nil)
level='unknown';local before=spawns
check(requestReadyM41('R3|11|10|42'):find('REFUSED',1,true)~=nil and spawns==before)
check(next(KCD2VRMap.entities)==nil) -- unknown region never reuses old printed art
level='trosecko';ENTITY_FLAG_NO_SAVE=nil
check(requestReadyM41('R3|12|10|42'):find('NATIVE_NO_SAVE_FLAG_UNAVAILABLE',1,true)~=nil)
ENTITY_FLAG_NO_SAVE=32768;badEntity=true
check(requestReadyM41('R3|12|10|42')=='R3|REFUSED|CURRENT_ACTOR_MISMATCH')
''')
checks=lua.globals().checks
# Execute the exact packaged native catalog too; chosen art must be one of the
# actual supplied resource names, including current town/region selection.
lua.execute((ROOT/'mods/KCD2VRMap/map_catalog_static_m41.lua').read_text())
lua.execute(runtime.decode())
lua.execute("badEntity=false;events.OnGameplayStarted();level='trosecko'")
names={p.stem for p in (ROOT/'companion/source/Objects/kcd2vr/staticmap40').glob('*.cgf')}
actual=[]
for region in ('trosecko','kutnohorsko','klaster'):
 lua.globals().level=region
 reply=lua.globals().requestReadyM41('R3|20|20|42')
 assert '|HELPER_READY|'in reply,reply
 art=reply.rsplit('|',2)[1];assert art in names
 actual.append(dict(region=region,artwork=art));checks+=1
# Real town rectangles -> inverse native coordinate transform -> real selection.
import math
catalog=lua.globals().KCD2VRMapCatalogM39
for chosen in ('troskovice','kutna_hora'):
 m=next(v for _,v in catalog.maps.items()if v['name']==chosen)
 r=m['region'];lev=catalog.levels[r];a=m['active'];px=a[1]+a[3]/2;py=a[2]+a[4]/2
 rad=-lev.rotation*math.pi/180;c=math.cos(rad);si=math.sin(rad)
 xs=[0,c,c-si,-si];ys=[0,si,si+c,c]
 sx=lev.tiles[1]*2048/(max(xs)-min(xs));sy=lev.tiles[2]*2048/(max(ys)-min(ys))
 u=px+min(xs)*sx+lev.crop[1]*2048;v=py+min(ys)*sy+lev.crop[2]*2048
 nx=(c*u+si*v)/sx;ny=(-si*u+c*v)/sy
 x=lev.world[1]+nx*lev.world[3];y=lev.level[2]-lev.world[2]-ny*lev.world[4]
 lua.globals().fixtureX=x;lua.globals().fixtureY=y;lua.globals().level=r
 lua.execute('actor.GetWorldPos=function()return{x=fixtureX,y=fixtureY,z=0}end')
 reply=lua.globals().requestReadyM41('R3|30|20|42')
 art=reply.rsplit('|',2)[1];assert art==chosen,(chosen,reply,x,y)
 actual.append(dict(region=r,artwork=art,native_town=chosen));checks+=1
report=dict(checks=checks,production_lua_sha256=hashlib.sha256(runtime).hexdigest(),actual_catalog_cases=actual,
 native_leaves='SUBSTITUTED',headset='NOT_RUN',image_rendering='NOT_RUN')
(out/'RESULT.json').write_text(json.dumps(report,indent=2));print(json.dumps(report,indent=2))
