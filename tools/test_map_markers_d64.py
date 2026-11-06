"""Execute actual D64 map Lua with explicit entity/slot leaves, no game."""
from pathlib import Path
import sys,json
sys.path.insert(0,str(Path(__file__).resolve().parents[2]/'test-deps'))
from lupa.lua51 import LuaRuntime
root=Path(__file__).resolve().parents[1];l=LuaRuntime(unpack_returned_tuples=True)
l.execute('''
n=0;function check(v)assert(v);n=n+1 end
ENTITY_FLAG_NO_SAVE=32768;ENTITY_FLAG_CLIENT_ONLY=256
actor={id=42,GetWorldPos=function()return{x=1000,y=2000,z=0}end};g_localActor=actor
entities={};loads=0;positions=0
System={GetCVar=function()return 'trosecko'end,GetEntity=function(id)if id==42 then return actor end;return entities[id]end,
GetEntityByName=function(name)return entities[name]end,RemoveEntity=function(id)entities[id]=nil end,
SpawnEntity=function(t)local e={id=100,hidden=true,slots={},pos={},draw={}}
 e.Hide=function(self,h)self.hidden=h==1 end;e.IsHidden=function(self)return self.hidden end
 e.LoadObject=function(self,slot,path)loads=loads+1;self.slots[slot]=path end
 e.SetSlotPos=function(self,slot,p)positions=positions+1;self.pos[slot]=p end
 e.DrawSlot=function(self,slot,on)self.draw[slot]=on end
 e.GetSlotPos=function(self,slot)return self.pos[slot]end
 entities[100]=e;entities[t.name]=e;return e end}
Script={ReloadScript=function()end};UIAction={RegisterEventSystemListener=function()end}
KCD2VRMapCatalogM39={levels={trosecko={rotation=0,tiles={3,3},world={496,704,2652,2652},level={4096,4096},crop={0,0,3,3}}},maps={},inside={}}
''')
l.execute((root/'mods/KCD2VRMap/map_runtime.lua').read_text())
l.execute('''
check(loads==0 and positions==0)
check(kcd2vr_map_t3_ready('R3|1|7|42|0'):find('HELPER_PENDING_ID',1,true)~=nil)
check(kcd2vr_map_t3_ready('R3|1|7|42|100'):find('HELPER_READY',1,true)~=nil)
check(loads==2) -- one cached POI batch, one player glyph, no actor per POI
local e=entities[100]
check(e.slots[1]=='Objects/kcd2vr/mapmarkers64/trosecko.cgf')
check(e.slots[2]=='Objects/kcd2vr/mapmarkers64/player.cgf')
check(math.abs(e.pos[2].x-((1000-496)/2652-.5)*.55)<1e-8)
check(math.abs(e.pos[2].z-(.5-(4096-2000-704)/2652)*.55)<1e-8)
local old=positions;check(kcd2vr_map_markers_d64('D64|1|7|42|100')=='D64|HIDDEN' and positions==old)
e:Hide(0);check(kcd2vr_map_markers_d64('D64|1|7|42|100')=='D64|PLAYER' and loads==2)
old=positions;check(kcd2vr_map_markers_d64('D64|1|7|42|100')=='D64|PLAYER' and positions==old)
old=positions;check(kcd2vr_map_markers_d64('D64|2|7|42|100')=='D64|STALE' and positions==old)
check(kcd2vr_map_markers_d64('D64|1|8|42|100')=='D64|STALE' and positions==old)
check(kcd2vr_map_markers_d64('D64|1|7|43|100')=='D64|OWNER' and positions==old)
check(kcd2vr_map_markers_d64('D64|1|7|42|999')=='D64|OWNER' and positions==old)
actor.GetWorldPos=function()return{x=20000,y=20000,z=0}end
check(kcd2vr_map_markers_d64('D64|1|7|42|100')=='D64|UNAVAILABLE' and e.pos[2].z==0 and e.draw[2]==0)
actor.GetWorldPos=function()return{x=1000,y=2000,z=0}end
check(kcd2vr_map_markers_d64('D64|1|7|42|100')=='D64|PLAYER' and e.draw[2]==1)
actor.GetWorldPos=function()return{x=0/0,y=2000,z=0}end
check(kcd2vr_map_markers_d64('D64|1|7|42|100')=='D64|UNAVAILABLE' and e.pos[2].z==0 and e.draw[2]==0)
actor.GetWorldPos=function()error('entity unavailable')end
check(kcd2vr_map_markers_d64('D64|1|7|42|100')=='D64|UNAVAILABLE' and e.pos[2].z==0 and e.draw[2]==0)
check(kcd2vr_map_m31_intent('I31|2|0')=='I31|2|CLOSED' and e.hidden)
old=positions;check(kcd2vr_map_markers_d64('D64|1|7|42|100')=='D64|STALE' and positions==old)
''')
print(json.dumps({'result':'PASS','checks':l.globals().n,'actual_lua':True,'engine_leaves':'SUBSTITUTUTED','headset':'NOT_RUN'}))
