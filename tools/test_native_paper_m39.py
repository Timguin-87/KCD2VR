"""Production Lua, native animator leaf simulation. Does not prove rendering."""
from pathlib import Path
import json,sys
sys.path.insert(0,'C:/Users/turcs/.codex/worktrees/8fd6/KCD2VR/held-map-c7fm1/map-test-deps')
from lupa.lua51 import LuaRuntime
root=Path(__file__).resolve().parents[1]
l=LuaRuntime(unpack_returned_tuples=True)
l.execute('''
t=10;starts=0;resets=0;header=false;refuse=false;stalled=false
g_localActor={id=42};KCD2VRMap={world=42,awaitStart=false,entities={}}
System={GetCurrTime=function()return t end,GetEntity=function(id)return id==42 and g_localActor end,LogAlways=function()end}
paper={time=0,speed=0,hidden=false,active=false}
function paper:GetAnimationLength(slot,clip)return header and 1 or 0 end
function paper:StopAnimation(slot,layer)assert(slot==0 and layer==-1)end
function paper:ResetAnimation(slot,layer)assert(slot==0 and layer==-1);resets=resets+1;self.fifo=0 end
function paper:StartAnimation(slot,clip,layer,blend,speed,loop,hold)
 assert(self.fifo==0 and slot==0 and layer==0 and blend==0 and not loop and hold==1)
 starts=starts+1;self.clip=clip;self.speed=speed;self.time=0
 if refuse then return false end;self.fifo=1;return true
end
function paper:SetAnimationSpeed(slot,layer,speed)assert(speed==0 or speed==1);self.speed=speed end
function paper:SetAnimationTime(slot,layer,time)assert(self.fifo==1);self.time=time end
function paper:GetAnimationTime(slot,layer)return self.time end
function paper:ForceCharacterUpdate(slot,on)self.forced=on end
function paper:Activate(on)self.active=on==1 end
function paper:Hide(on)self.hidden=on==1 end
state={entity=paper};KCD2VRMap.entities.paper=state
function KCD2VRMap.SyncPresentation()KCD2VRMap.AdvancePaper31(state,paper,not paper.hidden)end
function intent(serial,on)return kcd2vr_map_m31_intent('I31|'..serial..'|'..on)end
function tick(dt,engine)
 t=t+dt
 if engine and paper.active and not stalled then paper.time=math.min(1,paper.time+dt*paper.speed)end
 KCD2VRMap.SyncPresentation()
end
''')
l.execute((root/'mods/KCD2VRMap/map_presentation_m31.lua').read_text())
l.execute('''
assert(intent(1,1)=='I31|1|WAIT_OPEN' and starts==1) -- request BEFORE missing duration
tick(.1,false);assert(starts==1)
header=true;tick(.1,false);assert(state.paper31.phase=='OPENING' and paper.speed==1 and starts==1)
tick(.5,false);assert(state.paper31.progress==0) -- wall clock is NOT animation completion
tick(.5,true);assert(state.paper31.progress==.5)
paper.hidden=true;tick(.2,true);paper.hidden=false;tick(.3,true)
assert(state.paper31.phase=='OPEN' and starts==1)
for i=1,100 do tick(.033,true)end;assert(starts==1 and paper.time==1)
assert(intent(2,0)=='I31|2|CLOSING' and starts==2)
tick(.5,false);assert(intent(2,0)=='I31|2|CLOSING') -- never ACK CLOSING as CLOSED
tick(.5,true);assert(state.paper31.phase=='CLOSING');tick(.5,true)
assert(intent(2,0)=='I31|2|CLOSED' and not paper.active)
assert(intent(3,1)=='I31|3|OPENING');tick(1,true)
assert(kcd2vr_map_fold_m39('F39|3|0.5')=='F39|OK')
assert(state.paper31.phase=='MANUAL' and math.abs(paper.time-.5)<1e-6 and paper.speed==0)
tick(.05,true);assert(paper.time==.5)
assert(kcd2vr_map_fold_m39('F39|3|-1')=='F39|OK');assert(paper.speed==1)
tick(.5,true);assert(state.paper31.phase=='OPEN')
assert(kcd2vr_map_fold_m39('F39|3|0.1')=='F39|OK');assert(state.paper31.phase=='MANUAL')
local before=paper.time;assert(intent(4,0)=='I31|4|CLOSING')
assert(math.abs(paper.time-(1-before))<1e-6);tick(.3,true);assert(state.paper31.phase=='CLOSED')
assert(kcd2vr_map_fold_m39('F39|3|0')=='F39|REFUSED')
assert(intent(5,1)=='I31|5|OPENING');tick(.2,true)
assert(intent(6,0)=='I31|6|CLOSING');tick(.2,true);assert(state.paper31.phase=='CLOSED')
assert(intent(5,1)=='I31|REFUSED')
header=false;assert(intent(7,1)=='I31|7|WAIT_OPEN');assert(intent(8,0)=='I31|8|CLOSED')
header=true;paper.hidden=false;stalled=true;assert(intent(9,1)=='I31|9|OPENING')
tick(3.1,false);assert(state.paper31.phase=='FAULT' and paper.hidden)
stalled=false;paper.hidden=false;assert(intent(10,1)=='I31|10|OPENING')
KCD2VRMap.WorldEnded();assert(not state.paper31 and paper.hidden and not paper.forced and not paper.active)
assert(intent(11,1)=='I31|NO_CURRENT_PAPER')
KCD2VRMap.awaitStart=false;paper.hidden=false;header=false;refuse=true
assert(intent(11,1)=='I31|11|WAIT_OPEN');local n=starts
tick(.5,false);assert(starts==n);header=true;tick(.1,false)
assert(state.paper31.phase=='FAULT' and starts==n+1) -- one bounded ready retry
''')
print(json.dumps({'production_Lua51':'PASS','native_clock_not_elapsed_completion':'PASS','stream_request_before_length':'PASS','B_and_hand_fold_reversal':'PASS','transient_hide_repeat_close_stall_world_end':'PASS','headset_render':'NOT_RUN'},indent=2))
