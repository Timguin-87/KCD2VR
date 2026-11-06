"""Unmodified retail AnimDoor plus C65 hold-to-play/release-to-pause companion.

The native script body is loaded by its exact retail SHA. Native API, external
script helpers, channel and scheduler are explicit doubles; no headset proof.
"""
from pathlib import Path
import argparse
import hashlib
import json
import zipfile
from lupa.lua51 import LuaRuntime
from test_door_retail64 import LEAVES,DOUBLES,PAK,MEMBER,EXPECTED_SCRIPT_SHA
ROOT=Path(__file__).resolve().parents[1]

def main():
 p=argparse.ArgumentParser();p.add_argument('--source',type=Path,default=ROOT)
 p.add_argument('--pak',type=Path,default=PAK);p.add_argument('--companion-pak',type=Path)
 p.add_argument('--out',type=Path,required=True);a=p.parse_args()
 with zipfile.ZipFile(a.pak)as z:retail=z.read(MEMBER)
 assert hashlib.sha256(retail).hexdigest()==EXPECTED_SCRIPT_SHA
 companion=(a.source/'mods/KCD2VRDoors/door_runtime.lua').read_bytes()
 packaged=None
 if a.companion_pak:
  with zipfile.ZipFile(a.companion_pak)as z:member=z.read('Scripts/Startup/zzkcd2vr_doors.lua')
  assert member==companion
  packaged={'archive':str(a.companion_pak),'sha256':hashlib.sha256(a.companion_pak.read_bytes()).hexdigest(),
   'member_sha256':hashlib.sha256(member).hexdigest(),'member_equal_to_source':True}
 leaves=LEAVES.replace('version=1944,companion_46=46','version=1965,companion_65=65')
 cases=[]
 def check(name,code):
  l=LuaRuntime(unpack_returned_tuples=True);l.execute(leaves)
  l.execute(retail.decode('utf-8-sig'));l.execute(DOUBLES)
  l.execute(r'''
local originalGetter=System.GetCVar
System.GetCVar=function(name)
 local state=string.match(name,'^kcd2vr_door_ack_%d+_%d+_(%d+)_%d+$')
 if tonumber(state)==2 then
  assert(doorReports>0 and doorInstance.inUse==0 and doorInstance.bNeedUpdate==0 and doorInstance.curAnim=='')
  if refuseCompletion then return 0 end
 end
 return originalGetter(name)
end
function beat(d,phase,dt)cmd(vars.request,d.raw,phase,0);advance(d,dt or .01)end
''')
  l.execute(companion.decode('utf-8-sig'));l.execute(code);cases.append(name)
 check('ordinary native OnUsed -> hold PLAY -> release PAUSE beyond15s -> regrip/native finish',r'''
d=door(952252);d:OnUsed(player,0)
assert(nativeActions==1 and objectStarts==1 and manipulationStarts==1)
assert(operations[2]=='actor-before-object'and operations[3]=='object-start')
assert(d.curAnim=='door_l_b_o_object'and d.inUse==1 and nativeActorFlags==1)
assert(crimeSounds==1 and crimeVolumes==1);local nativeWrites=writes
advance(d,.01);assert(d.rate==0 and vars.ack_state==1 and claimCount==1)
local first=d.time;beat(d,3);assert(d.rate==1 and d.time==first)
beat(d,3,.1);assert(d.time>first)
beat(d,4);local stopped=d.time;assert(d.rate==0 and d.inUse==1 and doorReports==0)
for i=1,1601 do beat(d,4)end
assert(d.time==stopped and d.rate==0 and d.inUse==1 and doorReports==0 and nativeActions==1)
beat(d,3);beat(d,3,.9)
assert(d.inUse==0 and d.bNeedUpdate==0 and d.curAnim==''and nativeActorFlags==0)
assert(doorReports==1 and vars.ack_state==2 and events[#events]=='report:open')
assert(nativeActions==1 and objectStarts==1 and writes==nativeWrites)
''')
 check('ordinary close uses native close clip and native ReportClosed',r'''
d=door(50704);d.nDirection=1;d:OnUsed(player,0);advance(d,.01)
assert(d.curAnim=='door_l_b_c_object'and d.nDirection==-1 and d.rate==0 and crimeVolumes==0)
beat(d,3);beat(d,3,.2);beat(d,4);local t=d.time
for i=1,10 do beat(d,4)end
assert(d.time==t and d.rate==0 and doorReports==0)
beat(d,3);beat(d,3,.9)
assert(doorReports==1 and vars.ack_state==2 and events[#events]=='report:close')
''')
 check('deferred worker phase1 after an initial native update',r'''
deferredCommand=true;d=door(7);d:OnUsed(player,0);advance(d,.02)
assert(d.rate==1 and claimCount==0)
cmd(pendingRequest,d.raw,1,0);advance(d,.02);assert(d.rate==0 and vars.ack_state==1)
''')
 check('missing accepted request does not seize native clip',"admitRequest=false;d=door(7);d:OnUsed(player,0);advance(d,.03);assert(d.rate==1 and claimCount==0 and nativeActions==1)")
 check('failed native actor acceptance does not claim',"acceptedAction=false;d=door(7);d:OnUsed(player,0);advance(d,.02);assert(nativeActions==1 and objectStarts==1 and d.rate==1 and claimCount==0)")
 check('250ms pending expiry remains a native operation',"deferredCommand=true;d=door(7);d:OnUsed(player,0);advance(d,.13);advance(d,.13);cmd(pendingRequest,d.raw,1,0);advance(d,.01);assert(d.rate==1 and claimCount==0)")
 check('locked key side without key remains refused',"d=door(7);d.bLocked=true;d.Properties.Lock.esLockTypes='key/key';hasKey=false;d:OnUsed(player,0);advance(d,.01);assert(d.bLocked and nativeActions==0 and objectStarts==0 and claimCount==0 and unlockReports==0)")
 check('native key permission unlocks before pause',"d=door(7);d.bLocked=true;d.Properties.Lock.esLockTypes='key/key';hasKey=true;d:OnUsed(player,0);advance(d,.01);assert(not d.bLocked and unlockReports==1 and d.rate==0)")
 check('quest/noninteractive native rejection retained',"interactive=false;d=door(7);d:OnUsed(player,0);advance(d,.01);assert(nativeActions==0 and objectStarts==0 and claimCount==0)")
 check('NPC/native event operations do not receive controller pause',"allowEvent=true;d=door(7);d:OnUsed({id={npc=true}},0);assert(nativeActions==0 and objectStarts==0);d:Event_Open();advance(d,.01);assert(d.rate==1 and objectStarts==1 and claimCount==0)")
 check('foreign userId cannot adopt player actor callback',"d=door(7);d:DoPlayAnimation(1,nil,nil,nil,true,{npc=true});advance(d,.01);assert(nativeActions==1 and objectStarts==1 and d.rate==1 and claimCount==0)")
 check('foreign g_localActor cannot own Henry action',"g_localActor={id={foreign=true}};d=door(7);d:OnUsed(player,0);advance(d,.01);assert(d.rate==1 and claimCount==0)")
 check('forced/load/custom operation remains native',"allowEvent=true;d=door(7);d:DoPlayAnimation(1,1,false,'load_clip',nil,player.id);advance(d,.01);assert(d.rate==1 and claimCount==0 and nativeActions==0)")
 check('programmatic Open stays native',"allowEvent=true;d=door(7);d:Open(false);advance(d,.01);assert(nativeActions==0 and objectStarts==1 and d.rate==1 and claimCount==0)")
 check('claim refused never pauses',"refuseClaim=true;d=door(7);d:OnUsed(player,0);advance(d,.02);assert(d.rate==1 and heldReceipts==0)")
 check('old ASI protocol cannot drive C65 companion',"vars.version=1944;d=door(7);d:OnUsed(player,0);advance(d,.02);assert(d.rate==1 and claimCount==0)")
 check('retired phase2 scrub is rejected with owned cleanup',"d=door(7);d:OnUsed(player,0);advance(d,.01);cmd(1,d.raw,2,.6);advance(d,.01);assert(d.rate==1 and vars.ack_state==0)")
 check('source/heartbeat loss restores native finish',"d=door(7);d:OnUsed(player,0);advance(d,.01);advance(d,.05);advance(d,.05);assert(d.rate==1 and vars.ack_state==0)")
 check('foreign native writer receives no old restore',"d=door(7);d:OnUsed(player,0);advance(d,.01);d.curAnim='foreign';d.rate=.7;beat(d,4);assert(d.rate==.7 and vars.ack_state==0)")
 check('failed cleanup readback retains retry',"d=door(7);d:OnUsed(player,0);advance(d,.01);failRestore=true;cmd(1,d.raw,0,0);advance(d,.01);assert(d.rate==0 and vars.ack_state==1);failRestore=false;advance(d,.01);assert(d.rate==1 and vars.ack_state==0)")
 check('native save restores pause before original native state serialization',r'''
d=door(7);d:OnUsed(player,0);advance(d,.01);beat(d,3);beat(d,3,.2);beat(d,4)
assert(d.rate==0 and d.inUse==1);saved={};d:OnSave(saved)
assert(d.rate==1 and vars.ack_state==0 and saved.nDirection==1 and saved.lastAnim=='door_l_b_o_object')
''')
 check('native after-load completion delay completes before ACK2',r'''
d=door(7);d.nUpdateAfterLoad=2;d:OnUsed(player,0);advance(d,.01);beat(d,3);beat(d,3,1)
assert(d.time==1 and d.inUse==1 and doorReports==0 and vars.ack_state==1)
beat(d,3);assert(d.inUse==1 and doorReports==0)
beat(d,3);assert(d.inUse==0 and doorReports==1 and vars.ack_state==2)
''')
 check('completion ACK retry has no later clock writes',r'''
d=door(7);d:OnUsed(player,0);advance(d,.01);beat(d,3);refuseCompletion=true;beat(d,3,1)
assert(doorReports==1 and d.inUse==0 and vars.ack_state==1);local w=writes;local s=speedWrites
beat(d,4);assert(writes==w and speedWrites==s)
refuseCompletion=false;advance(d,.01);assert(vars.ack_state==2 and writes==w and speedWrites==s and nativeActions==1)
''')
 check('endpoint reached before release is native completion, not a new pause',r'''
d=door(7);d:OnUsed(player,0);advance(d,.01);beat(d,3);beat(d,4,1)
assert(doorReports==1 and d.inUse==0 and vars.ack_state==2 and d.rate==1)
''')
 check('native crime-volume suppression retained',"skipCrime=true;d=door(7);d:OnUsed(player,0);advance(d,.01);assert(d.rate==0 and crimeSounds==1 and crimeVolumes==0)")
 result={'source':str(a.source),'retail_archive':str(a.pak),'retail_member':MEMBER,
  'retail_archive_sha256':hashlib.sha256(a.pak.read_bytes()).hexdigest(),
  'retail_script_sha256':hashlib.sha256(retail).hexdigest(),'companion_sha256':hashlib.sha256(companion).hexdigest(),
  'protocol':1965,'revision':65,'packaged':packaged,'checks_passed':len(cases),'checks':cases,
  'native_execution':False,'headset_acceptance':False,
  'limitations':['Native API/channel/scheduler leaves are doubles.','No hand-to-handle attachment or physics hinge is proved.']}
 a.out.write_text(json.dumps(result,indent=2)+'\n',encoding='utf-8')
 print(f'actual retail AnimDoor + C65 companion: {len(cases)}/{len(cases)} PASS')
if __name__=='__main__':main()
