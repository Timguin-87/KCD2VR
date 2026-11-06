"""Actual map Lua with retained helper faults/reload; no character API writes."""
import json
from m41_test_support import options,lua_runtime
a=options('map_lifetime_m44');lua=lua_runtime(a);l=lua(unpack_returned_tuples=True)
source=(a.source_root/'mods/KCD2VRMap/map_runtime.lua').read_text()
for forbidden in ['CreateBoneAttachment','SetAttachmentCGF','HideAttachment','DestroyAttachment']:
    assert forbidden not in source,forbidden
l.execute('''
n=0;function check(v)assert(v);n=n+1 end
ENTITY_FLAG_NO_SAVE=32768;ENTITY_FLAG_CLIENT_ONLY=256
actor={id=42,GetWorldPos=function()return{x=1000,y=2000,z=0}end};g_localActor=actor
entities={};nextId=100;spawns=0;loads=0;hides=0;removes=0;listeners=0
hideFault=false;hideNoEffect=false;removeFault=false;removeDeferred=false;lookupFault=false
System={GetCVar=function()return 'trosecko'end,GetEntity=function(id)
 if lookupFault then error('LOOKUP_FAULT')end
 if id==42 then return actor end;return entities[id]end,
 GetEntityByName=function(name)return entities[name]end,
 RemoveEntity=function(id)removes=removes+1;if removeFault then error('REMOVE_FAULT')end;if removeDeferred then return end
  local e=entities[id];entities[id]=nil;if entities.KCD2VR_StaticPaper_M41==e then entities.KCD2VR_StaticPaper_M41=nil end end,
 SpawnEntity=function(t)spawns=spawns+1;local id=nextId;nextId=nextId+1
  local e={id=id,hidden=true,slots={},pos={}}
  e.Hide=function(self,on)hides=hides+1;if hideFault then error('HIDE_FAULT')end;if not hideNoEffect then self.hidden=on==1 end end
  e.IsHidden=function(self)return self.hidden end
  e.DrawSlot=function(self,slot,on)end -- substituted public slot visibility leaf
  e.LoadObject=function(self,slot,path)loads=loads+1;self.slots[slot]=path end
  e.SetSlotPos=function(self,slot,p)self.pos[slot]=p end;e.GetSlotPos=function(self,slot)return self.pos[slot]end
  entities[id]=e;entities[t.name]=e;return e end}
Script={ReloadScript=function()end};UIAction={RegisterEventSystemListener=function()listeners=listeners+1 end}
KCD2VRMapCatalogM39={levels={trosecko={rotation=0,tiles={3,3},world={496,704,2652,2652},level={4096,4096},crop={0,0,3,3}}},maps={},inside={}}
''')
l.execute(source)
l.execute('''
check(listeners==2 and spawns==0)
check(kcd2vr_map_t3_ready('R3|1|7|42|0'):find('HELPER_PENDING_ID',1,true))
check(kcd2vr_map_t3_ready('R3|1|7|42|100'):find('HELPER_READY',1,true))
check(spawns==1 and loads==2)
firstM=KCD2VRMap;firstE=entities[100];check(firstM.entities.KCD2VR_StaticPaper_M41.entity==firstE)
''')
l.execute(source) # actual same-version same-VM reload, not a mocked function
l.execute('''
check(KCD2VRMap==firstM and listeners==2 and entities[100]==firstE)
check(kcd2vr_map_t3_ready('R3|1|7|42|100'):find('HELPER_READY',1,true))
check(spawns==1 and loads==2)
-- Hide failure is PENDING, never a fabricated CLOSED reply.
firstE.hidden=false;hideFault=true
check(kcd2vr_map_m31_intent('I31|2|0')=='I31|2|PENDING')
check(firstM.entities.KCD2VR_StaticPaper_M41.entity==firstE and not firstE.hidden)
hideFault=false;check(kcd2vr_map_m31_intent('I31|2|0')=='I31|2|CLOSED' and firstE.hidden)
-- Legitimate epoch change must finish original helper removal before respawn.
removeFault=true
check(kcd2vr_map_t3_ready('R3|3|8|42|100'):find('HELPER_RETIRE_PENDING',1,true))
check(entities[100]==firstE and firstM.entities.KCD2VR_StaticPaper_M41.entity==firstE and spawns==1)
''')
l.execute(source)
l.execute('''
check(KCD2VRMap==firstM and firstM.retiringM44 and listeners==2)
removeFault=false;removeDeferred=true
check(kcd2vr_map_t3_ready('R3|3|8|42|100'):find('HELPER_RETIRE_PENDING',1,true))
check(entities[100]==firstE and spawns==1)
removeDeferred=false
check(kcd2vr_map_t3_ready('R3|3|8|42|0'):find('HELPER_PENDING_ID',1,true))
check(not entities[100] and spawns==2 and firstM.entities.KCD2VR_StaticPaper_M41.entity==entities[101])
check(kcd2vr_map_t3_ready('R3|3|8|42|101'):find('HELPER_READY',1,true))
-- A same-name foreign entity is never hidden/removed/adopted.
original101=entities[101];foreign={id=999,hidden=false};entities[999]=foreign;entities.KCD2VR_StaticPaper_M41=foreign
beforeForeignHides=hides;beforeForeignRemoves=removes
check(kcd2vr_map_t3_ready('R3|4|8|42|101'):find('HELPER_IDENTITY_CHANGED',1,true))
check(entities[999]==foreign and not foreign.hidden and entities.KCD2VR_StaticPaper_M41==foreign)
check(not entities[101] and next(firstM.entities)==nil)
check(kcd2vr_map_t3_ready('R3|4|8|42|0'):find('FOREIGN_NAME_COLLISION',1,true))
check(entities[999]==foreign and not foreign.hidden)
entities[999]=nil;entities.KCD2VR_StaticPaper_M41=nil
check(kcd2vr_map_t3_ready('R3|5|8|42|0'):find('HELPER_PENDING_ID',1,true))
check(kcd2vr_map_t3_ready('R3|5|8|42|102'):find('HELPER_READY',1,true))
-- World-end fault retains receipt across reload, then WorldStarted can finish it.
removeFault=true;KCD2VRMap.WorldEnded();worldEndE=entities[102]
check(worldEndE and firstM.entities.KCD2VR_StaticPaper_M41.entity==worldEndE and firstM.retiringM44)
''')
l.execute(source)
l.execute('''
check(KCD2VRMap==firstM and firstM.endedActor==actor and listeners==2)
removeFault=false;KCD2VRMap.WorldStarted()
check(kcd2vr_map_t3_ready('R3|6|9|42|0'):find('HELPER_PENDING_ID',1,true))
check(not entities[102] and spawns==4 and not firstM.retiringM44)
check(kcd2vr_map_t3_ready('R3|6|9|42|103'):find('HELPER_READY',1,true))
-- Opaque original helper ID/table owns cleanup; an unreadable lookup is unknown.
lookupFault=true;KCD2VRMap.WorldEnded();check(firstM.entities.KCD2VR_StaticPaper_M41.entity==entities[103])
lookupFault=false;KCD2VRMap.WorldStarted();KCD2VRMap.WorldEnded();check(next(firstM.entities)==nil and not entities[103])
-- CGF updates the deliberately hidden helper ONLY through native-qualified prefixes.
KCD2VRMap.WorldStarted()
check(kcd2vr_map_t3_ready('R3N44|7|10|42|0'):find('R3N44|41.0-static-paper',1,true))
check(kcd2vr_map_t3_ready('R3N44|7|10|42|104'):find('HELPER_READY',1,true))
check(entities[104].hidden and firstM.backendNativeM44==true)
check(kcd2vr_map_markers_d64('D64N44|7|10|42|104')=='D64N44|PLAYER')
check(kcd2vr_map_markers_d64('D64|7|10|42|104')=='D64|BACKEND')
check(kcd2vr_map_t3_ready('R3|7|10|42|104'):find('BACKEND_MISMATCH',1,true))
check(kcd2vr_map_markers_d64('D64N44|6|9|42|104')=='D64N44|STALE')
check(kcd2vr_map_m31_intent('I31|8|0')=='I31|8|CLOSED')
check(kcd2vr_map_t3_ready('R3|9|10|42|104'):find('HELPER_READY',1,true))
check(firstM.backendNativeM44==false and kcd2vr_map_markers_d64('D64|9|10|42|104')=='D64|HIDDEN')
check(kcd2vr_map_markers_d64('D64N44|7|10|42|104')=='D64N44|BACKEND')
''')
# Initial SpawnEntity receipt survives a Hide exception, then retires exactly.
l.execute('''
KCD2VRMap.WorldEnded();check(next(KCD2VRMap.entities)==nil)
KCD2VRMap.WorldStarted();hideFault=true;local before=spawns
check(kcd2vr_map_t3_ready('R3|90|100|42|0'):find('REFUSED',1,true))
check(spawns==before+1 and (KCD2VRMap.entities.KCD2VR_StaticPaper_M41~=nil or entities.KCD2VR_StaticPaper_M41==nil))
hideFault=false
check(kcd2vr_map_t3_ready('R3|90|100|42|0'):find('HELPER_PENDING_ID',1,true))
check(spawns==before+2 and not KCD2VRMap.retiringM44)
''')
# Unknown prior runtime is retained, including its callbacks/listener ownership.
older=lua(unpack_returned_tuples=True)
older.execute('''
old={entities={original={id=777}}};KCD2VRMap=old
Script={ReloadScript=function()end};kcd2vr_map_t3_ready=function()return 'OLD_OWNER'end
''')
older.execute(source)
assert older.eval('KCD2VRMap==old and kcd2vr_map_t3_ready()=="OLD_OWNER"')
result=dict(result='PASS',checks=l.globals().n,actual_source=True,lua_version='5.1',character_attachment_writes=0,native_leaves='SUBSTITUTED',headset='NOT_RUN')
(a.out/'RESULT.json').write_text(json.dumps(result,indent=2));print(json.dumps(result))
