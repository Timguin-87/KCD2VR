"""Actual retail AnimDoor + R4 + H40 Lua5.1 composition; only native leaves modeled.
No engine/headset execution is claimed. Runtime source SHA and pinned retail member
are recorded in output. Both module startup orders execute the same assertions.
"""
from pathlib import Path
import argparse, hashlib, json, math, sys, zipfile
try:
    from lupa.lua51 import LuaRuntime
except ImportError:
    sys.path.insert(0, r"C:/Users/turcs/.codex/worktrees/8fd6/KCD2VR/held-map-c7fm1/map-test-deps")
    from lupa.lua51 import LuaRuntime
from test_door_retail64 import LEAVES, DOUBLES, EXPECTED_SCRIPT_SHA, PAK, MEMBER
ROOT=Path(__file__).resolve().parents[1]
H40=ROOT/'mods/KCD2VRDoors/hand_only_h40.lua'
R4=ROOT/'mods/KCD2VRDoors/door_runtime.lua'

EXTERNAL=r"""
allowEvent=true
now=1;timers={};entities={};queries=0;wakeCalls=0;originalUpdates=0
Script.SetTimer=function(ms,fn)timers[#timers+1]=fn end
System.GetCurrTime=function()return now end
System.GetEntitiesInSphere=function(p,r)queries=queries+1;return entities end
local oldGet=System.GetEntity
System.GetEntity=function(id)
 if lookupFail then error('lookup unavailable')end
 if id==10 then return currentPlayerOverride or player end
 if id==player.id then return currentPlayerOverride or player end
 for _,e in ipairs(entities)do if id==e.raw or id==e.id then return e end end
 return nil
end
function player:GetWorldPos(out)
 local p={x=0,y=-1,z=0};if out then out.x=p.x;out.y=p.y;out.z=p.z end;return p
end
g_Vectors={temp_v1={x=0,y=0,z=0}}
VectorUtils={Sum=function(a,b)return{x=a.x+b.x,y=a.y+b.y,z=a.z+b.z}end,
 Scale=function(a,s)return{x=a.x*s,y=a.y*s,z=a.z*s}end,
 Subtract=function(a,b)return{x=a.x-b.x,y=a.y-b.y,z=a.z-b.z}end,
 LengthSquared=function(a)return a.x*a.x+a.y*a.y+a.z*a.z end}
local retailDoor=door
function makeDoor(raw)
 local d=retailDoor(raw);d.class='AnimDoor';d.Properties.fUseDistance=3;d.Properties.object_Model='doors/left'
 d.nUserId=0;d.serverWake=false
 -- Actual object-ADB aliases use _player; the old lifecycle double invented
 -- _object. Production continues to take the native selector's return value.
 d.animDoor.GetObjectAnimName=function(self,id,opened,wanted,front)
  if d.selectedAlias then return (d.selectedAlias:gsub('_[oc]_player$',opened and '_c_player'or '_o_player'))end
  return opened and 'door_l_b_c_player'or 'door_l_b_o_player'
 end
 function d:GetBonePos(name)
  if self.boneFail=='throw'then error('named bone unavailable')end
  if self.boneFail=='missing'then return nil end
  local p=self:GetWorldPos()
  if name:match('^Asset_Door%-')then return{x=p.x,y=p.y,z=p.z}end
  if name:match('^door_.*_pivot$')then return{x=p.x,y=p.y,z=p.z+1}end
  error('unexpected named bone')
 end
 function d:GetWorldBBox()return{x=.15,y=-.05,z=0},{x=.85,y=.05,z=2}end
 function d:Activate(on)assert(on==1);wakeCalls=wakeCalls+1;if failWake then error('wake unknown')end;self.serverWake=true end
 function d:SetAnimationSpeed(slot,layer,rate)
  assert(slot==0 and layer==0);speedWrites=speedWrites+1
  if failSpeed then error('speed failure')end
  if not (failRestore and rate==1)then self.rate=math.max(0,rate)end
 end
 function d:SetAnimationTime(slot,layer,t)
  assert(slot==0 and layer==0);writes=writes+1
  if failTime then error('seek failure')end
  self.time=math.max(0,math.min(.9999899864196777,t))
 end
 local start=d.animDoor.ReportManipulationStart
 d.animDoor.ReportManipulationStart=function(self)start(self);if failStart then error('native start mutated then threw')end end
 local nativeOpen,nativeClose=d.animDoor.ReportOpened,d.animDoor.ReportClosed
 d.animDoor.ReportOpened=function(self,id)if reportHook then reportHook(d,id)end;if failReport then error('report unknown')end;return nativeOpen(self,id)end
 d.animDoor.ReportClosed=function(self,id)if failReport then error('report unknown')end;return nativeClose(self,id)end
 entities[#entities+1]=d;return d
end
local update=AnimDoor.Server.OnUpdate
AnimDoor.Server.OnUpdate=function(self,dt)originalUpdates=originalUpdates+1;return update(self,dt)end
function frame(d,dt)
 now=now+(dt or .01)
 if d.serverWake then
  if d.running then d.time=math.min(.9999899864196777,d.time+(dt or .01)*d.rate)end
  AnimDoor.Server.OnUpdate(d,dt or .01)
 end
end
serial=0;claim0=1;claim1=1
function packet(action,g0,x0,y0,g1,x1,y1,body,req,pl)
 serial=serial+1
 local s=table.concat({serial,action,pl or 10,body or 20,req or 30,serial,g0 or -1,x0 or .5,y0 or 0,1,g1 or -1,x1 or .5,y1 or 0,1,claim0,claim1},'|')
 lastPacket=s;lastReply=kcd2vr_door_hand_h40_t2(s);return lastReply
end
function check(x,msg)assert(x,msg);checks=checks+1 end
checks=0
-- Independent oracle decodes the exact native DBA quaternion tracks in Python;
-- it never calls the production At/Inverse/Map helper.
function leaf(d)return nativeReferenceAngle(d.curAnim,d.time)end
function checkLeaf(d,target,msg)
 local expected=nativeReferenceClamp(d.curAnim,target)
 check(math.abs(leaf(d)-expected)<.0001,msg)
end
function mask()local a=KCD2VRHandDoorsH40.hands;return(a[0]and 1 or 0)+(a[1]and 2 or 0)end
function finish(d)
 for n=1,110 do frame(d,.02);local h=KCD2VRHandDoorsH40.hands;if not(h[0]and h[0].door==d)and not(h[1]and h[1].door==d)then return end end
 error('native finish did not retire')
end
"""

CORE=r"""
check(KCD2VRHandDoorsH40.installed,'installed')
local before=wakeCalls
local p=packet('probe',-1,nil,nil,-1)
check(string.match(p,'^H40R1|')~=nil and mask()==0 and wakeCalls==before,'probe is nonmutating')
d=makeDoor(101)
claim0=claim0+1;local a=packet('feed',.9,.5,0,-1)
check(mask()==1 and objectStarts==1 and nativeActions==0 and d.rate==0,'Grip starts native object-only clip immediately without A or entity update')
check(wakeCalls==before+1 and d.serverWake,'exact idle target woken')
local wake=wakeCalls
check(kcd2vr_door_hand_h40_t2(lastPacket)==a and wakeCalls==wake,'duplicate packet does not wake/start twice')
frame(d,.01)
check(objectStarts==1 and nativeActions==0 and d.rate==0,'first native door update retains the same object-only operation')
check(nativeActorFlags==0 and physicalCalls==0,'no artificial Henry flags acquired')
check(manipulationStarts==1 and crimeSounds==1 and crimeVolumes==1,'actual native OnUsed crime/sound preserved')
local original=originalUpdates
packet('feed',.9,.5,-.5,-1);frame(d,.01)
checkLeaf(d,-math.pi/4,'45degree palm arc drives45degree leaf rather than half animation');check(d.rate==0,'held clip remains paused')
check(originalUpdates==original+1,'composed original update exactly once')
packet('feed',.9,.02,-1,-1);frame(d,.01)
check(d.time<=.9995 and d.curAnim~='' and mask()==1,'held endpoint does not manufacture native completion')
packet('releaseall',-1,nil,nil,-1)
check(mask()==1 and KCD2VRHandDoorsH40.hands[0].retiring and d.rate==0,'release packet only queues handback')
-- An unrelated native actor flag may belong to a different Henry action.
nativeActorFlags=1
finish(d)
check(d.nDirection==1 and d.curAnim=='' and d.inUse==0 and d.bNeedUpdate==0,'native open terminal reached')
check(doorReports==1 and nativeActorFlags==1 and physicalCalls==0,'native report once; unrelated actor flag retained')
check(mask()==0 and KCD2VRHandDoorsH40.ackSeq==serial,'ACK only after native terminal/report')
-- A new native Use after H40 release must select CLOSE, not replay OPEN.
d:OnUsed(player,0)
check(d.nDirection==-1 and d.curAnim=='door_l_b_c_player' and nativeActions==1,'next ordinary native Use picks opposite clip')
-- Complete ordinary native path to ensure per-instance wrappers restored.
for n=1,110 do frame(d,.02)end
check(physicalCalls==2 and nativeActorFlags==0,'ordinary native true/false flag path preserved')
-- Releasing below midpoint continues the selected OPEN rather than reversing it.
claim0=claim0+1;packet('feed',.9,.5,0,-1);frame(d,.01)
packet('feed',.9,.5,-.2,-1);frame(d,.01)
local open=d.time
packet('clear',-1,nil,nil,-1);frame(d,.01)
check(d.nDirection==1 and d.curAnim=='door_l_b_o_player' and d.rate==1,'release retains original opening clip below midpoint')
check(math.abs(d.time-open)<.0001,'release does not replace or seek the acquired clip')
finish(d)
check(d.nDirection==1 and doorReports==3 and mask()==0,'original open report and regrab complete')
"""
NEGATIVES=r"""
-- Invalid second-hand data must not partially publish side0.
d=makeDoor(101)
local bad='1|feed|10|20|30|1|.9|.5|0|1|.9|nan|0|1|1|1'
check(kcd2vr_door_hand_h40_t2(bad)=='BAD|0' and mask()==0,'bad packet atomic')
d.Properties.Lock.esLockTypes='key/key';d.bLocked=true;claim0=claim0+1;packet('feed',.9,.5,0,-1);frame(d,.01)
check(mask()==0 and objectStarts==0 and unlockReports==0,'locked path not custom unlocked')
d.bLocked=false;d.Properties.Lock.esLockTypes='none/none';interactive=false;claim0=claim0+1;packet('feed',.9,.5,0,-1);frame(d,.01)
check(mask()==0 and objectStarts==0,'native interactivity respected')
interactive=true;d.inUse=1;claim0=claim0+1;packet('feed',.9,.5,0,-1);frame(d,.01)
check(mask()==1 and objectStarts==1,'busy bit alone does not veto native Grip opening')
packet('clear',-1,nil,nil,-1);finish(d)
local startsAfterBusy=objectStarts
d.inUse=0;claim0=claim0+1;claim1=claim1+1;packet('feed',.9,.5,0,.9,.5,0);frame(d,.01)
check(mask()==1 and objectStarts==startsAfterBusy+1,'same door cannot be owned by both hands')
local stages=objectStarts
packet('feed',.9,.5,-.4,-1,0,0,21,31)
check(string.match(lastReply,'^IDENT|') and mask()==1 and objectStarts==stages,'changed identity drains original without new acquisition')
finish(d)
check(mask()==0,'original identity cleanup after mismatch')
-- Release before first target update still drains the actual Grip-started clip.
e=makeDoor(102);entities={e};local beforePending=objectStarts
claim0=claim0+1;packet('feed',.9,.5,0,-1,0,0,22,32)
packet('releaseall',-1,nil,nil,-1,0,0,22,32)
check(mask()==1 and objectStarts==beforePending+1 and KCD2VRHandDoorsH40.hands[0].retiring,'immediate release retains original acquired operation until native completion')
finish(e);check(mask()==0,'early release completes through original update')
-- Tracking/side withdrawal does not drop a real mutation debt before native finish.
claim0=claim0+1;packet('feed',.9,.5,0,-1,0,0,22,32);frame(e,.01)
packet('feed',-1,0,0,-1,0,0,22,32)
check(mask()==1 and KCD2VRHandDoorsH40.hands[0].retiring,'tracking withdrawal retains entered owner')
finish(e);check(mask()==0,'tracking withdrawal native cleanup')
-- 80ms feed grace; a timer publishes retirement only, never seeks/resumes.
claim0=claim0+1;packet('feed',.9,.5,0,-1,0,0,22,32);frame(e,.01)
local w,s=writes,speedWrites;now=now+.081
local due=timers;timers={};for _,fn in ipairs(due)do fn()end
check(mask()==1 and KCD2VRHandDoorsH40.hands[0].retiring and writes==w and speedWrites==s,'watchdog retirement has no animation writes')
finish(e);check(mask()==0,'watchdog drains via native update')
-- Pause fault still has an original clip: never false ACK, restore can retry.
failRestore=true
claim0=claim0+1;packet('feed',.9,.5,0,-1,0,0,22,32);frame(e,.01)
packet('feed',.9,.5,-.3,-1,0,0,22,32);frame(e,.01)
packet('clear',-1,nil,nil,-1,0,0,22,32);frame(e,.01)
check(mask()==1 and KCD2VRHandDoorsH40.hands[0].retiring,'failed positive rate restore retained')
failRestore=false;finish(e);check(mask()==0,'restore retry releases only after real finish')
-- Native report throw is once-only/unknown, not a terminal ACK or blind replay.
claim0=claim0+1;packet('feed',.9,.5,0,-1,0,0,22,32);frame(e,.01)
packet('feed',.9,.02,-1,-1,0,0,22,32);frame(e,.01)
packet('releaseall',-1,nil,nil,-1,0,0,22,32);failReport=true
pcall(function()finish(e)end)
check(mask()==1 and KCD2VRHandDoorsH40.hands[0].uncertain,'unknown report retains original ownership')
failReport=false;local r=doorReports
for n=1,5 do frame(e,.01)end
check(mask()==1 and doorReports==r,'unknown report not blindly replayed or accepted')
"""


EXTRA=r"""
-- Native OnUsed keeps key-side unlocking and its original events.
d=makeDoor(101);d.bLocked=true;d.Properties.Lock.esLockTypes='key/key';hasKey=true
claim0=claim0+1;packet('feed',.9,.5,0,-1);frame(d,.01)
check(d.bLocked==false and unlockReports==1 and objectStarts==1 and mask()~=0,'native OnUsed performs key unlock once before hand movement')
check(d.bLocked==false and unlockReports==1 and objectStarts==1 and nativeActions==0,'native unlock restores the same hand-door operation')
packet('releaseall',-1,nil,nil,-1);finish(d)
-- Distinct doors allow the opposite side independently.
d=makeDoor(102);e=makeDoor(103);e.offset=2
local oldPos=e.GetWorldPos
function e:GetWorldPos()return{x=2,y=0,z=0}end
function e:GetWorldBBox()return{x=2.15,y=-.05,z=0},{x=2.85,y=.05,z=2}end
entities={d,e};claim0=claim0+1;claim1=claim1+1;packet('feed',.9,.5,0,.9,2.5,0);frame(d,.01);frame(e,.01)
check(mask()==3 and KCD2VRHandDoorsH40.hands[0].door~=KCD2VRHandDoorsH40.hands[1].door,'two distinct doors have independent side ownership')
-- Feed the still-held opposite hand while the released native door finishes.
-- The previous nearest-endpoint policy completed d in one frame by reversing
-- it; a real ongoing open must not artificially starve e's 80ms source feed.
for n=1,110 do
 packet('feed',-1,0,0,.9,2.5,-.5);frame(d,.02);frame(e,.02)
 if mask()==2 then break end
end
check(mask()==2 and e.rate==0,'one side retirement preserves opposite held door')
packet('clear',-1,nil,nil,-1);finish(e)
check(mask()==0,'dual door original cleanup completed')
-- Door incarnation removal is terminal for this object-only operation.
f=makeDoor(104);entities={f};claim0=claim0+1;packet('feed',.9,.5,0,-1);frame(f,.01)
local writesBefore=writes;entities={};frame(f,.01)
check(mask()==0 and writes==writesBefore,'replaced/deleted door is not written or restored')
-- An original actor replacement cannot be relabeled to the new player object.
g=makeDoor(105);entities={g};claim0=claim0+1;packet('feed',.9,.5,0,-1);frame(g,.01)
local startsBefore=objectStarts;currentPlayerOverride={id=player.id};frame(g,.01)
check(mask()==1 and KCD2VRHandDoorsH40.hands[0].retiring and g.rate==1 and objectStarts==startsBefore,'actor loss resumes only original clip, no new clip/actor write')
finish(g);check(mask()==0 and physicalCalls==0,'actor-lost original door reaches native terminal without actor flags')
currentPlayerOverride=nil
-- Foreign call outside our selected original update keeps its own normal behavior.
h=makeDoor(106);entities={h}
claim0=claim0+1;packet('feed',.9,.5,0,-1);frame(h,.01);nativeActorFlags=1
h:SetPhysicalFlags(player.id,false)
check(nativeActorFlags==0 and physicalCalls>0 and mask()==1 and KCD2VRHandDoorsH40.hands[0].uncertain,'foreign flag call forwarded and cannot ACK our operation')
"""


REENTRY=r"""
d=makeDoor(101)
claim0=claim0+1;packet('feed',.9,.5,0,-1);frame(d,.01)
packet('feed',.9,.01,-1,-1);frame(d,.01)
packet('releaseall',-1,nil,nil,-1)
local attempts=0
reportHook=function(e,id)
 attempts=attempts+1
 if attempts==1 then e.animDoor:ReportOpened(id) end
end
pcall(function()finish(d)end)
check(attempts==1 and doorReports==1,'reentrant original report primitive attempted only once')
check(mask()==1 and KCD2VRHandDoorsH40.hands[0].uncertain,'reentrant report cannot manufacture terminal ACK')
-- Other-side unaffected door remains usable while unknown side is retained.
e=makeDoor(102)
function e:GetWorldPos()return{x=2,y=0,z=0}end
function e:GetWorldBBox()return{x=2.15,y=-.05,z=0},{x=2.85,y=.05,z=2}end
claim1=claim1+1;packet('feed',-1,0,0,.9,2.5,0);frame(e,.01)
check(mask()==3 and KCD2VRHandDoorsH40.hands[1].stage=='HELD','unknown side does not veto opposite new door')
"""
CAPABILITY=r"""
check(KCD2VRHandDoorsH40.installed~=true,'missing timer owner refuses module admission')
local p=packet('probe',-1,nil,nil,-1)
check(string.match(p,'^BAD|')~=nil and mask()==0 and wakeCalls==0,'uninstalled callback cannot advertise readiness')
"""


CLAIMS=r"""
local a=packet('feed',.56,.5,0,-1)
check(string.match(a,'|0|0|'..serial..'|0|0|0$')~=nil and mask()==0,'empty no-door feed ACKs this exact claim')
d=makeDoor(101);claim0=claim0+1
packet('feed',.31,.5,0,-1)
check(mask()==0 and objectStarts==0,'gradual press below genuine threshold does not consume token')
packet('feed',.56,.5,0,-1);frame(d,.01)
check(mask()==1 and objectStarts==1,'same gradual press acquires once above .55')
packet('feed',.30,.5,-.4,-1);frame(d,.01)
check(mask()==1 and not KCD2VRHandDoorsH40.hands[0].retiring,'hold hysteresis .30 remains held')
packet('clear',-1,nil,nil,-1);finish(d)
local starts=objectStarts
packet('feed',.9,.5,0,-1);frame(d,.01)
check(mask()==0 and objectStarts==starts,'same original claim cannot reacquire after native terminal')
packet('clear',-1,nil,nil,-1)
packet('feed',.9,.5,0,-1);frame(d,.01)
check(mask()==0 and objectStarts==starts,'clear does not reset original claim consumption')
claim0=claim0+1;packet('feed',.56,.5,0,-1);frame(d,.01)
check(mask()==1 and objectStarts==starts+1,'next real C++ press token works without Lua observing idle neutral')
packet('releaseall',-1,nil,nil,-1);finish(d)
check(mask()==0,'fresh claim handback finishes normally')
d.bLocked=true;d.Properties.Lock.esLockTypes='key/key';hasKey=false;claim0=claim0+1
packet('feed',.56,.5,0,-1);frame(d,.01)
check(mask()==0,'declined native eligibility leaves no actual clip owner')
local startsDeclined=objectStarts
local declined=packet('feed',.9,.5,0,-1);frame(d,.01)
check(mask()==0 and objectStarts==startsDeclined and string.match(declined,'|0|0|'..serial..'|0|0|0$'),'same declined press yields terminal ACK instead of re-reserving')
"""


INITIAL_OPEN=r"""
d=makeDoor(101);d.nDirection=1
function d:GetWorldBBox()return{x=-.05,y=-.85,z=0},{x=.05,y=-.15,z=2}end
packet('feed',.9,0,-.5,-1);frame(d,.01)
check(mask()==1 and d.curAnim=='door_l_b_c_player' and d.nDirection==-1 and d.rate==0,'initial open door chooses native forward close clip')
packet('feed',.9,.5,-.1,-1);frame(d,.01)
checkLeaf(d,math.atan2(-.1,.5)+math.pi/2,'open door closes by signed palm displacement');check(d.nDirection==-1,'closing native direction retained')
packet('releaseall',-1,nil,nil,-1);finish(d)
check(mask()==0 and d.nDirection==-1 and doorReports==1,'initial-open hand cycle completes closed native state')
d:OnUsed(player,0)
check(d.nDirection==1 and d.curAnim=='door_l_b_o_player' and nativeActions==1,'following ordinary native Use picks open after hand close')
"""


START_FAULT=r"""
d=makeDoor(101);failStart=true
packet('feed',.9,.5,0,-1);frame(d,.01)
check(mask()==1 and d.inUse==1 and manipulationStarts==1 and KCD2VRHandDoorsH40.hands[0].entered,'native start mutation recorded before throw')
frame(d,.01)
check(mask()==1 and KCD2VRHandDoorsH40.hands[0].uncertain and KCD2VRHandDoorsH40.hands[0].retiring,'entered uncertain PENDING cannot be erased as wake-only cancellation')
packet('clear',-1,nil,nil,-1);frame(d,.01)
check(mask()==1 and manipulationStarts==1 and objectStarts==0,'unknown start is retained without blind native replay or ACK')
"""
UNINITIALIZED=r"""
d=makeDoor(101);d.bNoAnims=0
packet('feed',.9,.5,0,-1);frame(d,.01)
check(mask()==0 and objectStarts==0 and manipulationStarts==0,'numeric0 native default is not a usable initialized animated door')
local response=packet('feed',.9,.5,0,-1)
check(mask()==0 and string.match(response,'|0|0|'..serial..'|0|0|0$'),'unsupported initialization declines once with exact terminal receipt')
"""

JOIN_NATIVE=r"""
d=makeDoor(301)
-- The original native Grip starts the operation before hand admission.
AnimDoor.OnUsed(d,player,0)
local starts,manip,flags=objectStarts,manipulationStarts,nativeActorFlags
local clip,direction=d.curAnim,d.nDirection
check(d.inUse==1 and d.lastUsedBy==player.id and d.rate==1,'original native player operation exists')
check(nativeActorFlags==1 and physicalCalls==1,'original player operation acquired native physical flags')
claim0=claim0+1;packet('feed',.9,.5,0,-1);frame(d,.01)
local st=KCD2VRHandDoorsH40.hands[0]
check(mask()==1 and st and st.joinedNativeH42 and st.stage=='HELD','Grip joins original native player episode')
check(objectStarts==starts and manipulationStarts==manip and nativeActorFlags==flags,'join does not replay OnUsed manipulation or actor flags')
check(d.curAnim==clip and d.nDirection==direction and d.rate==0,'join retains original direction and owns paused object clip')
packet('feed',.9,.5,-.4,-1);frame(d,.01)
check(d.time>.1,'current palm motion drives joined native clip')
packet('feed',0,.5,-.4,-1);finish(d)
check(mask()==0 and d.inUse==0 and d.curAnim=='' and doorReports==1,'joined operation releases through original native terminal exactly once')
check(nativeActorFlags==0 and physicalCalls==2,'joined native operation performs its matching original flag cleanup exactly once')
"""

GRIP_START=r"""
d=makeDoor(401)
packet('feed',.9,.5,0,-1)
check(objectStarts==1 and nativeActions==0 and nativeActorFlags==0,'Grip alone starts original object-only animation before A or OnUpdate')
check(mask()==1 and d.rate==0 and KCD2VRHandDoorsH40.hands[0].stage=='HELD','original operation is paused and held in same Grip callback')
local starts,manip=objectStarts,manipulationStarts
check(kcd2vr_door_hand_h40_t2(lastPacket)==lastReply and objectStarts==starts and manipulationStarts==manip,'duplicate same Grip never repeats native OnUsed')
packet('releaseall',-1,nil,nil,-1);finish(d)
check(mask()==0 and doorReports==1 and nativeActions==0,'original endpoint completes once without any Henry player animation')
"""

EARLY_CLOSE=r"""
d=makeDoor(402);d.nDirection=1
function d:GetWorldBBox()return{x=-.05,y=-.85,z=0},{x=.05,y=-.15,z=2}end
packet('feed',.9,0,-.5,-1);frame(d,.01)
-- Close by the relative hand angle: the join is atan2(.5,.01), not exactly90deg.
packet('feed',.9,.1624598481,-.5,-1);frame(d,.01)
local progress=d:GetAnimationTime(0,0)
checkLeaf(d,math.atan2(-.5,.1624598481)+math.pi/2,'selected closing leaf follows actual join-relative angle');check(d.nDirection==-1,'native close retained')
local starts,manip=objectStarts,manipulationStarts
packet('releaseall',-1,nil,nil,-1);frame(d,.01)
check(d.nDirection==-1 and d.curAnim=='door_l_b_c_player' and d.rate==1,'closing release above50percent open never reopens')
check(objectStarts==starts and manipulationStarts==manip,'release starts no opposite animation or repeated manipulation')
finish(d);check(mask()==0 and d.nDirection==-1 and doorReports==1,'original closing endpoint reports once')
"""

WAKE_FAULT=r"""
d=makeDoor(403);failWake=true
packet('feed',.9,.5,0,-1)
local st=KCD2VRHandDoorsH40.hands[0]
check(mask()==1 and manipulationStarts==1 and st.entered and st.uncertain,'original native start that mutates then throws retains its exact operation')
local starts=objectStarts
packet('releaseall',-1,nil,nil,-1)
check(mask()==1 and objectStarts==starts,'unknown original start is not erased or blindly replayed')
"""

RELATIVE_START=r"""
-- A hand starts part-way around the door's angle. Grip starts the actual
-- original clip at0; a stationary hand must not jump it to an absolute angle.
d=makeDoor(410)
packet('feed',.9,.5,-.2,-1)
check(mask()==1 and d.rate==0 and d.time==0,'new original door starts paused at its native clip time')
frame(d,.01)
check(d.time==0,'stationary nonzero join angle does not jump a newly started door')
for n=1,20 do packet('feed',.9,.5,-.2,-1);frame(d,.01)end
check(d.time==0 and objectStarts==1 and manipulationStarts==1,'stationary acquired door has no accumulated drift or repeated native use')
packet('feed',.9,.5,-.5,-1);frame(d,.01)
checkLeaf(d,-math.pi/4-math.atan2(-.2,.5),'opening leaf displacement starts at actual acquisition angle')
packet('feed',.9,.5,-.2,-1);frame(d,.01)
checkLeaf(d,0,'returning hand restores original leaf angle, including a native zero-angle plateau')
packet('releaseall',-1,nil,nil,-1);finish(d)
check(mask()==0 and doorReports==1 and objectStarts==1 and manipulationStarts==1,'relative new-start release preserves one original operation and native terminal')
"""

RELATIVE_JOIN=r"""
-- The native user already opened30percent before Grip acquired this episode.
d=makeDoor(411);AnimDoor.OnUsed(d,player,0);d.time=.3
local starts,manip,flags=objectStarts,manipulationStarts,nativeActorFlags;local joinAngle=leaf(d)
packet('feed',.9,.5,-.2,-1);frame(d,.01)
check(math.abs(d.time-.3)<.0001 and d.rate==0,'joining at a nonzero hand angle preserves actual native30percent progress')
for n=1,20 do packet('feed',.9,.5,-.2,-1);frame(d,.01)end
check(math.abs(d.time-.3)<.0001,'stationary joined hand has no acquisition jump or clock drift')
packet('feed',.9,.5,-.5,-1);frame(d,.01)
local expected=joinAngle-math.pi/4-math.atan2(-.2,.5)
checkLeaf(d,expected,'joined opening follows captured leaf angle plus actual palm arc');local displacedPhase=d.time
for n=1,30 do packet('feed',.9,.5,-.5,-1);frame(d,.01)end
check(d.time==displacedPhase,'stationary displaced hand never re-adds its join delta each frame')
packet('feed',.9,.5,-.2,-1);frame(d,.01)
checkLeaf(d,joinAngle,'returning joined hand restores captured leaf angle')
packet('feed',.9,.5,0,-1);frame(d,.01)
checkLeaf(d,joinAngle-math.atan2(-.2,.5),'hand can reverse to available angle within same clip');check(d.nDirection==1 and d.curAnim=='door_l_b_o_player','reversal does not start opposite operation')
check(objectStarts==starts and manipulationStarts==manip and nativeActorFlags==flags,'held drive does not repeat manipulation or native actor flag acquisition')
packet('releaseall',-1,nil,nil,-1);finish(d)
check(mask()==0 and d.nDirection==1 and doorReports==1 and nativeActorFlags==0 and physicalCalls==2,'joined relative opening retains native terminal and matching flag cleanup exactly once')
"""

RELATIVE_CLOSE=r"""
d=makeDoor(412);d.nDirection=1
function d:GetWorldBBox()return{x=-.05,y=-.85,z=0},{x=.05,y=-.15,z=2}end
AnimDoor.OnUsed(d,player,0);d.time=.3
local starts,manip=objectStarts,manipulationStarts;local joinAngle=leaf(d)
packet('feed',.9,.2,-.5,-1);frame(d,.01)
check(math.abs(d.time-.3)<.0001 and d.nDirection==-1,'joining a closing episode preserves its native30percent clip time')
packet('feed',.9,.5,-.5,-1);frame(d,.01)
local expected=joinAngle-math.pi/4-math.atan2(-.5,.2)
checkLeaf(d,expected,'closing leaf follows signed join-relative palm arc');local displacedPhase=d.time;check(d.curAnim=='door_l_b_c_player','same native closing clip')
for n=1,30 do packet('feed',.9,.5,-.5,-1);frame(d,.01)end
check(d.time==displacedPhase,'stationary displaced closing hand does not run away')
packet('feed',.9,.2,-.5,-1);frame(d,.01)
checkLeaf(d,joinAngle,'closing hand returned to join restores original leaf angle')
packet('feed',.9,0,-.5,-1);frame(d,.01)
checkLeaf(d,joinAngle-math.pi/2-math.atan2(-.5,.2),'reverse palm displacement stays within available close curve');check(d.nDirection==-1,'same native close direction')
packet('releaseall',-1,nil,nil,-1);frame(d,.01)
check(d.rate==1 and d.nDirection==-1 and d.curAnim=='door_l_b_c_player','relative close release resumes its original positive-rate closing clip')
check(objectStarts==starts and manipulationStarts==manip,'relative close release never reopens or repeats native use')
finish(d)
check(mask()==0 and d.nDirection==-1 and doorReports==1 and nativeActorFlags==0 and physicalCalls==2,'relative closing completes and performs exact original flag cleanup once')
"""

RELATIVE_NO_RUNAWAY=r"""
-- Fixed join: a stationary45degree hand displacement must never accumulate.
-- Nonlinear selected clip time is checked through the independent native leaf oracle.
d=makeDoor(413);packet('feed',.9,.5,0,-1);frame(d,.01)
packet('feed',.9,.5,-.5,-1);frame(d,.01)
checkLeaf(d,-math.pi/4,'first45degree displacement drives45degree leaf');local displacedPhase=d.time
for n=1,30 do packet('feed',.9,.5,-.5,-1);frame(d,.01)end
check(d.time==displacedPhase,'stationary displaced hand does not accumulate toward endpoint')
packet('releaseall',-1,nil,nil,-1);finish(d)
check(mask()==0 and doorReports==1,'stationary displaced episode still releases through one native terminal')
"""

RELATIVE_CLAMP_LIMITS=r"""
-- Signed palm arcs cross the old projected quadrant boundaries. Only the
-- actual selected native curve's available leaf angles limit the seek.
d=makeDoor(414);AnimDoor.OnUsed(d,player,0);d.time=.3;local joinAngle=leaf(d)
packet('feed',.9,.5,.02,-1);frame(d,.01)
check(math.abs(d.time-.3)<.0001,'acquisition at across clamp preserves native clip time')
packet('feed',.9,.5,.3,-1);frame(d,.01)
checkLeaf(d,joinAngle+math.atan2(.3,.5)-math.atan2(.02,.5),'crossing old across boundary uses signed arc and actual curve limits')
packet('feed',.9,.5,-.2,-1);frame(d,.01)
checkLeaf(d,joinAngle+math.atan2(-.2,.5)-math.atan2(.02,.5),'cross-quadrant return drives actual selected leaf angle')
packet('releaseall',-1,nil,nil,-1);finish(d)
check(mask()==0 and doorReports==1,'across-clamped episode still releases through original native completion')
entities={};e=makeDoor(415);e.nDirection=1
function e:GetWorldBBox()return{x=-.05,y=-.85,z=0},{x=.05,y=-.15,z=2}end
AnimDoor.OnUsed(e,player,0);e.time=.3;local joinClose=leaf(e);claim0=claim0+1
packet('feed',.9,0,-.5,-1);frame(e,.01)
check(math.abs(e.time-.3)<.0001,'acquisition near along clamp preserves native closing clip time')
packet('feed',.9,-.3,-.5,-1);frame(e,.01)
checkLeaf(e,joinClose+math.atan2(-.5,-.3)+math.pi/2,'old along floor does not discard signed hand excursion')
packet('feed',.9,.5,-.5,-1);frame(e,.01)
checkLeaf(e,joinClose+math.pi/4,'return across quadrant drives original close by45degrees')
packet('releaseall',-1,nil,nil,-1);finish(e)
check(mask()==0 and e.nDirection==-1 and doorReports==2,'along-clamped episode preserves original close and once-only native report')
"""

GEOMETRY_HANDBACK=r"""
-- A valid current slot hinge does not need the retired entity-horizontal plane.
do
 local d=makeDoor(500)
 function d:GetWorldDir()return{x=0,y=0,z=1}end
 local reports,starts=doorReports,manipulationStarts
 claim0=claim0+1;packet('feed',.9,.5,0,-1);frame(d,.01)
 check(mask()==1 and d.rate==0 and manipulationStarts==starts+1,'native Grip with valid named hinge is not vetoed by obsolete entity XY model')
 packet('releaseall',-1,nil,nil,-1);finish(d)
 check(mask()==0 and doorReports==reports+1 and nativeActions==0,'retired plane removal retains one original native terminal')
end
for _,case in ipairs({'missing_join','throw_join','undefined_join','missing_held','throw_held','undefined_held','ordinary_release'})do
 entities={};local d=makeDoor(501)
 local reports,starts,crime=doorReports,manipulationStarts,crimeSounds
 if case=='missing_join'then d.boneFail='missing'end
 if case=='throw_join'then d.boneFail='throw'end
 claim0=claim0+1;packet('feed',.9,case=='undefined_join'and 0 or .5,0,-1)
 if case=='missing_held'then d.boneFail='missing'end
 if case=='throw_held'then d.boneFail='throw'end
 if case:match('_held$')then packet('feed',.9,case=='undefined_held'and 0 or .5,case=='undefined_held'and 0 or -.1,-1);frame(d,.01)end
 packet('releaseall',-1,nil,nil,-1);finish(d)
 check(mask()==0 and d.rate==1 and d.curAnim=='','read-only geometry loss resumes original clip: '..case)
 check(doorReports==reports+1 and manipulationStarts==starts+1 and crimeSounds==crime+1,'one original operation/report/cost: '..case)
 check(d.inUse==0 and d.bNeedUpdate==0 and nativeActions==0,'native object-only terminal clears original flags: '..case)
end
"""

SELECTED_CURVES=r"""
for _,alias in ipairs(nativeReferenceAliases)do
 entities={};local d=makeDoor(601);d.selectedAlias=alias
 local closing=alias:match('_c_player$')~=nil
 if closing then d.nDirection=1;function d:GetWorldBBox()return{x=-.05,y=-.85,z=0},{x=.05,y=-.15,z=2}end end
 local x,y=closing and 0 or .5,closing and -.5 or 0
 local initial=math.atan2(y,x);local delta=nativeReferenceDirection(alias)*math.pi/4
 local reports,starts=doorReports,manipulationStarts
 claim0=claim0+1;packet('feed',.9,x,y,-1);frame(d,.01)
 check(d.curAnim==alias and d.rate==0 and mask()==1,'join retains exact native selected alias: '..alias..' clip='..tostring(d.curAnim)..' rate='..tostring(d.rate)..' mask='..mask()..' reply='..lastReply)
 packet('feed',.9,.5*math.cos(initial+delta),.5*math.sin(initial+delta),-1);frame(d,.01)
 checkLeaf(d,delta,'selected native clip maps signed45degree palm arc: '..alias)
 local phase=d.time
 for i=1,6 do packet('feed',.9,.5*math.cos(initial+delta),.5*math.sin(initial+delta),-1);frame(d,.01)end
 check(d.time==phase and manipulationStarts==starts+1,'no drift or repeated use: '..alias)
 packet('releaseall',-1,nil,nil,-1);finish(d)
 check(mask()==0 and doorReports==reports+1 and d.nDirection==(closing and -1 or 1),'original native selected operation completes once: '..alias)
end
"""

def chunk(lua,code,name):
    lua.eval('function(s,n)assert(loadstring(s,n))()end')(code,'@'+name)

def main():
    ap=argparse.ArgumentParser();ap.add_argument('--output',required=True);ap.add_argument('--runtime');ap.add_argument('--scenario',action='append');args=ap.parse_args()
    global H40
    if args.runtime:H40=Path(args.runtime)
    out=Path(args.output);out.mkdir(parents=True,exist_ok=True)
    with zipfile.ZipFile(PAK) as z:retail=z.read(MEMBER)
    assert hashlib.sha256(retail).hexdigest()==EXPECTED_SCRIPT_SHA
    from verify_door_clip_angle_h40 import decoded_curves,DBA_MEMBER,DBA_SHA
    with zipfile.ZipFile(PAK.parent/'Animations.pak')as z:curves=decoded_curves(z.read(DBA_MEMBER))
    reference={alias:(end,angle,min(values),max(values))for alias,end,keys,values,angle in curves}
    def native_angle(alias,phase):
        end,angle,lo,hi=reference[alias];return angle(end*phase)
    def native_clamp(alias,target):
        end,angle,lo,hi=reference[alias];return min(hi,max(lo,target))
    results=[]
    scenarios={'core':CORE,'negatives':NEGATIVES,'extra':EXTRA,'no_debug':CORE,'reentry':REENTRY,'no_debug_reentry':REENTRY,'missing_capability':CAPABILITY,'claims':CLAIMS,'initial_open':INITIAL_OPEN,'start_fault':START_FAULT,'uninitialized':UNINITIALIZED,'join_native':JOIN_NATIVE,'grip_start':GRIP_START,'early_close':EARLY_CLOSE,'wake_fault':WAKE_FAULT,'relative_start':RELATIVE_START,'relative_join':RELATIVE_JOIN,'relative_close':RELATIVE_CLOSE,'relative_no_runaway':RELATIVE_NO_RUNAWAY,'relative_clamp_limits':RELATIVE_CLAMP_LIMITS}
    scenarios.update(geometry_handback=GEOMETRY_HANDBACK,selected_curves=SELECTED_CURVES)
    for order in ('r4_first','h40_first'):
      for scenario in (args.scenario or scenarios):
        L=LuaRuntime(unpack_returned_tuples=True)
        L.globals().nativeReferenceAngle=native_angle;L.globals().nativeReferenceClamp=native_clamp
        L.globals().nativeReferenceAliases=L.table_from(list(reference))
        L.globals().nativeReferenceDirection=lambda alias:1 if reference[alias][3]>abs(reference[alias][2])else -1
        chunk(L,LEAVES,'test/native_leaves.lua');chunk(L,retail.decode('utf-8-sig'),MEMBER)
        chunk(L,DOUBLES,'test/native_object_leaves.lua');chunk(L,EXTERNAL,'test/h40_external_leaves.lua')
        if scenario in ('no_debug','no_debug_reentry'):chunk(L,'debug=nil','test/no_debug.lua')
        if scenario=='missing_capability':chunk(L,'Script.SetTimer=nil','test/no_timer.lua')
        modules=[R4,H40] if order=='r4_first' else [H40,R4]
        for p in modules:chunk(L,p.read_text(),('mods/KCD2VRDoors/hand_only_h40.lua' if p==H40 else str(p.relative_to(ROOT)).replace('\\','/')))
        chunk(L,scenarios[scenario],'test/'+scenario+'.lua')
        results.append({'order':order,'scenario':scenario,'checks':L.globals().checks,'status':'PASS'})
    receipt={'results':results,'assertions':sum(x['checks'] for x in results),'lua':'Lua5.1 via lupa.lua51',
      'source':{str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in (H40,R4,Path(__file__))},
      'retail':EXPECTED_SCRIPT_SHA,'angle_reference_dba_sha256':DBA_SHA,'limitations':['Actual pinned retail script and decoded quaternion key reference; native bone/animation/scheduler/component leaves controlled, no game/headset execution.','Sixteen exact-retail selected aliases; physical bone convention, engine sampler and hand/door synchronization require live verification.']}
    (out/'RECEIPT.json').write_text(json.dumps(receipt,indent=2)+'\n');print(json.dumps(receipt,indent=2))
if __name__=='__main__':main()
