"""Unmodified retail AnimDoor plus C66 motion-owned Door clip companion.

Native API, external helpers, channel and scheduler are explicit doubles. The
retail body is SHA-qualified; this is ordering proof, not native/headset proof.
"""
from pathlib import Path
import argparse
import hashlib
import json
import zipfile
from lupa.lua51 import LuaRuntime
from test_door_retail64 import LEAVES, DOUBLES, PAK, MEMBER, EXPECTED_SCRIPT_SHA
ROOT = Path(__file__).resolve().parents[1]


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
 leaves=LEAVES.replace('version=1944,companion_46=46','version=1966,companion_66=66')
 cases=[]
 def check(name,code):
  l=LuaRuntime(unpack_returned_tuples=True);l.execute(leaves)
  l.execute(retail.decode('utf-8-sig'));l.execute(DOUBLES.replace('_object', '_player'))
  l.execute(r"""
local originalGetter=System.GetCVar
System.GetCVar=function(name)
 local seq,request,clip,t=string.match(name,'^kcd2vr_door_curve_(%d+)_(%d+)_([%w_-]+)_(%d+)$')
 if seq then
  curveRequests=(curveRequests or 0)+1;curveClip=clip;curveTime=tonumber(t)/1000000
  assert(doorInstance and clip==doorInstance.curAnim)
  assert(math.abs(curveTime-doorInstance.time)<.000001)
  assert(doorInstance.rate==0)
  if noCurve or tonumber(seq)~=vars.seq or tonumber(request)~=vars.request then return 0 end
  return pendingCurve and 2 or 1
 end
 local state=string.match(name,'^kcd2vr_door_ack_%d+_%d+_(%d+)_%d+$')
 if tonumber(state)==2 then
  assert(doorReports>0 and doorInstance.inUse==0 and doorInstance.bNeedUpdate==0 and doorInstance.curAnim=='')
  if refuseCompletion then return 0 end
 end
 return originalGetter(name)
end
function beat(d,phase,fraction,dt)cmd(vars.request,d.raw,phase,fraction or 0);advance(d,dt or .01)end
""")
  l.execute(companion.decode('utf-8-sig'));l.execute(code);cases.append(name)
 check('ordinary native OnUsed -> paused claim -> controller forward/reverse -> release pause -> regrip/native finish',r"""
d=door(952252);d:OnUsed(player,0)
assert(nativeActions==1 and objectStarts==1 and manipulationStarts==1)
assert(operations[2]=='actor-before-object'and operations[3]=='object-start')
assert(d.curAnim=='door_l_b_o_player'and d.inUse==1 and nativeActorFlags==1)
assert(crimeSounds==1 and crimeVolumes==1)
advance(d,.01);assert(d.rate==0 and vars.ack_state==1 and claimCount==1)
local t=d.time;beat(d,1,0,.4);assert(d.time==t and d.rate==0)
beat(d,2,.6);assert(d.time==.6 and d.rate==0 and curveTime==t)
beat(d,2,.25);assert(d.time==.25 and d.rate==0 and curveTime==.6)
beat(d,4);local stopped=d.time
for i=1,1601 do beat(d,4)end
assert(d.time==stopped and d.rate==0 and d.inUse==1 and doorReports==0 and nativeActions==1)
beat(d,2,.9995);assert(d.time==.9995 and d.inUse==1 and doorReports==0)
for i=1,10 do beat(d,2,.9995)end
assert(d.time==.9995 and doorReports==0 and d.rate==0)
beat(d,4,.9995);assert(d.inUse==1 and doorReports==0)
beat(d,5,.9995)
assert(d.inUse==0 and d.bNeedUpdate==0 and d.curAnim==''and nativeActorFlags==0)
assert(doorReports==1 and vars.ack_state==2 and events[#events]=='report:open')
assert(nativeActions==1 and objectStarts==1)
""")
 check('ordinary close uses native close clip and ReportClosed only after released endpoint',r"""
d=door(50704);d.nDirection=1;d:OnUsed(player,0);advance(d,.01)
assert(d.curAnim=='door_l_b_c_player'and d.nDirection==-1 and d.rate==0 and crimeVolumes==0)
beat(d,2,.9);beat(d,4);local t=d.time
for i=1,10 do beat(d,4)end
assert(d.time==t and d.rate==0 and doorReports==0)
beat(d,2,.9995);beat(d,5,.9995)
assert(doorReports==1 and vars.ack_state==2 and events[#events]=='report:close')
""")
 check('opening reversed to start delegates opposite native object clip and ReportClosed without ActorUse2',r"""
d=door(7);d:OnUsed(player,0);advance(d,.01);beat(d,2,.7);beat(d,2,0);beat(d,4,0)
assert(d.time==0 and d.inUse==1 and d.nDirection==1 and objectStarts==1)
beat(d,5,0)
assert(objectStarts==2 and nativeActions==1 and d.nDirection==-1 and d.time==1)
assert(manipulationStarts==2 and vars.ack_state==2 and doorReports==1 and events[#events]=='report:close')
""")
 check('closing reversed to start delegates original opposite open clip and ReportOpened',r"""
d=door(7);d.nDirection=1;d:OnUsed(player,0);advance(d,.01);beat(d,2,.3);beat(d,2,0);beat(d,5,0)
assert(objectStarts==2 and nativeActions==1 and d.nDirection==1 and d.time==1)
assert(vars.ack_state==2 and doorReports==1 and events[#events]=='report:open')
""")
 check('single reused animation minimum endpoint forces exact owned stop then native restart',r"""
d=door(7);d.bUseSameAnim=true;d:OnUsed(player,0);advance(d,.01)
beat(d,2,.6);beat(d,2,0);beat(d,5,0)
assert(objectStarts==2 and nativeActions==1 and d.rate==1 and d.time==1 and d.nDirection==-1)
assert(vars.ack_state==2 and events[#events]=='report:close')
""")
 check('curve refusal preserves rate-zero native time without SetAnimationTime',r"""
pendingCurve=true;d=door(7);d:OnUsed(player,0);advance(d,.01);local t=d.time;local w=writes
beat(d,2,.6);assert(d.time==t and writes==w and d.rate==0 and vars.ack_state==1)
pendingCurve=false;beat(d,2,.6);assert(d.time==.6 and writes==w+1 and curveRequests>=3)
""")
 check('permanent first bind pending2 restores on bounded owner cleanup',r"""
pendingCurve=true;d=door(7);d:OnUsed(player,0);advance(d,.01);local t=d.time;local w=writes
for i=1,26 do beat(d,1)end
beat(d,0);assert(d.time==t and d.rate==1 and writes==w and vars.ack_state==0)
""")
 check('lost previously bound curve revokes time owner without controller write',r"""
d=door(7);d:OnUsed(player,0);advance(d,.01);local t=d.time;local w=writes
noCurve=true;beat(d,2,.6);assert(d.time==t and d.rate==1 and writes==w and vars.ack_state==0)
""")
 check('hard first bind refusal restores exact native rate without time write','noCurve=true;d=door(7);d:OnUsed(player,0);advance(d,.01);assert(d.rate==1 and vars.ack_state==0 and claimCount==1 and writes==1)')
 check('pending cannot replace a ready curve','d=door(7);d:OnUsed(player,0);advance(d,.01);pendingCurve=true;local w=writes;beat(d,2,.6);assert(d.rate==1 and vars.ack_state==0 and writes==w)')
 check('same-clip external small seek refuses controller writes',r"""
d=door(7);d:OnUsed(player,0);advance(d,.01);local t=d.time;local w=writes
d.time=t+.02;beat(d,2,.6);assert(d.time==t+.02 and d.rate==1 and writes==w and vars.ack_state==0)
""")
 check('native paused rounding below20us remains valid','d=door(7);d:OnUsed(player,0);advance(d,.01);d.time=d.time+.00001;local w=writes;beat(d,4);assert(d.rate==0 and vars.ack_state==1 and writes==w)')
 check('middle phase5 cannot fabricate native completion',r"""
d=door(7);d:OnUsed(player,0);advance(d,.01);beat(d,2,.5);local w=writes;beat(d,5,.5)
assert(d.rate==1 and writes==w and doorReports==0 and vars.ack_state==0 and nativeActions==1)
""")
 check('deferred phase1 after initial native update remains supported',r"""
deferredCommand=true;d=door(7);d:OnUsed(player,0);advance(d,.02)
assert(d.rate==1 and claimCount==0)
cmd(pendingRequest,d.raw,1,0);advance(d,.02);assert(d.rate==0 and vars.ack_state==1)
""")
 for name,code in [
  ('missing accepted request','admitRequest=false;d=door(7);d:OnUsed(player,0);advance(d,.03);assert(d.rate==1 and claimCount==0 and nativeActions==1)'),
  ('failed native actor acceptance','acceptedAction=false;d=door(7);d:OnUsed(player,0);advance(d,.02);assert(nativeActions==1 and objectStarts==1 and d.rate==1 and claimCount==0)'),
  ('250ms pending expiry','deferredCommand=true;d=door(7);d:OnUsed(player,0);advance(d,.13);advance(d,.13);cmd(pendingRequest,d.raw,1,0);advance(d,.01);assert(d.rate==1 and claimCount==0)'),
  ('locked key side without key','d=door(7);d.bLocked=true;d.Properties.Lock.esLockTypes="key/key";hasKey=false;d:OnUsed(player,0);advance(d,.01);assert(d.bLocked and nativeActions==0 and objectStarts==0 and claimCount==0 and unlockReports==0)'),
  ('native key permission before pause','d=door(7);d.bLocked=true;d.Properties.Lock.esLockTypes="key/key";hasKey=true;d:OnUsed(player,0);advance(d,.01);assert(not d.bLocked and unlockReports==1 and d.rate==0)'),
  ('quest/noninteractive native rejection','interactive=false;d=door(7);d:OnUsed(player,0);advance(d,.01);assert(nativeActions==0 and objectStarts==0 and claimCount==0)'),
  ('NPC native event stays native','allowEvent=true;d=door(7);d:OnUsed({id={npc=true}},0);assert(nativeActions==0 and objectStarts==0);d:Event_Open();advance(d,.01);assert(d.rate==1 and objectStarts==1 and claimCount==0)'),
  ('foreign user callback','d=door(7);d:DoPlayAnimation(1,nil,nil,nil,true,{npc=true});advance(d,.01);assert(nativeActions==1 and objectStarts==1 and d.rate==1 and claimCount==0)'),
  ('foreign local actor','g_localActor={id={foreign=true}};d=door(7);d:OnUsed(player,0);advance(d,.01);assert(d.rate==1 and claimCount==0)'),
  ('forced/load/custom operation','allowEvent=true;d=door(7);d:DoPlayAnimation(1,1,false,"load_clip",nil,player.id);advance(d,.01);assert(d.rate==1 and claimCount==0 and nativeActions==0)'),
  ('programmatic Open stays native','allowEvent=true;d=door(7);d:Open(false);advance(d,.01);assert(nativeActions==0 and objectStarts==1 and d.rate==1 and claimCount==0)'),
  ('claim refused','refuseClaim=true;d=door(7);d:OnUsed(player,0);advance(d,.02);assert(d.rate==1 and heldReceipts==0)'),
  ('old ASI protocol','vars.version=1965;d=door(7);d:OnUsed(player,0);advance(d,.02);assert(d.rate==1 and claimCount==0)'),
  ('retired autonomous phase3','d=door(7);d:OnUsed(player,0);advance(d,.01);cmd(1,d.raw,3,.6);advance(d,.01);assert(d.rate==1 and vars.ack_state==0)'),
  ('source heartbeat expiry','d=door(7);d:OnUsed(player,0);advance(d,.01);advance(d,.05);advance(d,.05);assert(d.rate==1 and vars.ack_state==0)'),
  ('foreign native writer','d=door(7);d:OnUsed(player,0);advance(d,.01);d.curAnim="foreign";d.rate=.7;beat(d,4);assert(d.rate==.7 and vars.ack_state==0)'),
  ('cleanup readback failure retry','d=door(7);d:OnUsed(player,0);advance(d,.01);failRestore=true;cmd(1,d.raw,0,0);advance(d,.01);assert(d.rate==0 and vars.ack_state==1);failRestore=false;advance(d,.01);assert(d.rate==1 and vars.ack_state==0)'),
  ('native crime-volume suppression','skipCrime=true;d=door(7);d:OnUsed(player,0);advance(d,.01);assert(d.rate==0 and crimeSounds==1 and crimeVolumes==0)')
 ]:check(name,code)
 check('native save restores owned rate before original state serialization',r"""
d=door(7);d:OnUsed(player,0);advance(d,.01);beat(d,2,.6);beat(d,4)
assert(d.rate==0 and d.inUse==1);saved={};d:OnSave(saved)
assert(d.rate==1 and vars.ack_state==0 and saved.nDirection==1 and saved.lastAnim=='door_l_b_o_player')
""")
 check('native after-load completion delay runs before ACK2 and never reclaims',r"""
d=door(7);d.nUpdateAfterLoad=2;d:OnUsed(player,0);advance(d,.01);beat(d,2,.9995);beat(d,5,.9995)
assert(d.time==1 and d.inUse==1 and doorReports==0 and vars.ack_state==1 and d.rate==1)
beat(d,4);assert(d.inUse==1 and doorReports==0 and d.rate==1)
beat(d,4);assert(d.inUse==0 and doorReports==1 and vars.ack_state==2)
""")
 check('completion ACK retry has no later clock writes',r"""
d=door(7);d:OnUsed(player,0);advance(d,.01);beat(d,2,.9995);refuseCompletion=true;beat(d,5,.9995)
assert(doorReports==1 and d.inUse==0 and vars.ack_state==1);local w=writes;local s=speedWrites
beat(d,4);assert(writes==w and speedWrites==s)
refuseCompletion=false;advance(d,.01);assert(vars.ack_state==2 and writes==w and speedWrites==s and nativeActions==1)
""")
 result={'source':str(a.source),'retail_archive':str(a.pak),'retail_member':MEMBER,
  'retail_archive_sha256':hashlib.sha256(a.pak.read_bytes()).hexdigest(),
  'retail_script_sha256':hashlib.sha256(retail).hexdigest(),'companion_sha256':hashlib.sha256(companion).hexdigest(),
  'protocol':1966,'revision':66,'packaged':packaged,'checks_passed':len(cases),'checks':cases,
  'native_execution':False,'headset_acceptance':False,
  'limitations':['Native API/channel/scheduler leaves are doubles.',
   'Rigid curve binding and attached native hand presentation are not executed here.',
   'ACK2 retry after the retail scheduler disables FORCE_UPDATE needs a later callback; eventual retry is not proved.']}
 a.out.write_text(json.dumps(result,indent=2)+'\n',encoding='utf-8')
 print(f'actual retail AnimDoor + C66 companion: {len(cases)}/{len(cases)} PASS')
if __name__=='__main__':main()
