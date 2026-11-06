"""Production Lua51 presentation, explicit intent and native-pose-time contract.
Native calls are substituted; actual game animation/render acceptance is not.
"""
from pathlib import Path
import sys,json
sys.path.insert(0,'C:/Users/turcs/.codex/worktrees/8fd6/KCD2VR/held-map-c7fm1/map-test-deps')
from lupa.lua51 import LuaRuntime
root=Path(__file__).resolve().parents[1]
lua=LuaRuntime(unpack_returned_tuples=True)
lua.execute('''
t=10;starts=0;logs={};reject=false;badClock=false
g_localActor={id=42};KCD2VRMap={world=42,awaitStart=false,entities={}}
System={GetCurrTime=function()return t end,GetEntity=function(id)return id==42 and g_localActor or nil end,LogAlways=function(s)logs[#logs+1]=s end}
paper={hidden=false,time=0,forced=false}
function paper:GetAnimationLength(slot,clip)assert(slot==0 and (clip=='open' or clip=='close'));return 1 end
function paper:StopAnimation(slot,layer)assert(slot==0 and layer==-1)end
function paper:StartAnimation(slot,clip,layer,blend,speed,loop,hold)
 assert(slot==0 and layer==0 and blend==0 and speed==0 and loop==false and hold==1);starts=starts+1;self.clip=clip;return not reject
end
function paper:SetAnimationSpeed(slot,layer,speed)assert(slot==0 and layer==0 and speed==0)end
function paper:SetAnimationTime(slot,layer,time)assert(slot==0 and layer==0 and time>=0 and time<=1);self.time=time end
function paper:GetAnimationTime(slot,layer)return badClock and 0 or self.time end
function paper:ForceCharacterUpdate(slot,on)assert(slot==0);self.forced=on end
function paper:Hide(v)self.hidden=v~=0 end
state={entity=paper};KCD2VRMap.entities.paper=state
function KCD2VRMap.SyncPresentation()KCD2VRMap.AdvancePaper31(state,paper,not paper.hidden)end
function intent(serial,on)return kcd2vr_map_m31_intent('I31|'..serial..'|'..on)end
function tick(dt)t=t+dt;KCD2VRMap.SyncPresentation()end
''')
lua.execute((root/'mods/KCD2VRMap/map_presentation_m31.lua').read_text())
lua.execute('''
assert(intent(1,1)=='I31|1|OPENING' and starts==1 and paper.clip=='open')
tick(.5);assert(state.paper31.progress==.5 and KCD2VRMap.Paper31Animating())
paper.hidden=true;tick(.2);paper.hidden=false;tick(.3)
assert(state.paper31.phase=='OPEN' and starts==1 and paper.time==1)
for i=1,100 do tick(.033)end
assert(starts==1 and paper.time==1 and state.paper31.phase=='OPEN')
assert(intent(2,0)=='I31|2|CLOSING' and starts==2 and paper.clip=='close')
tick(.5);assert(state.paper31.phase=='CLOSING')
tick(.5);assert(intent(2,0)=='I31|2|CLOSED' and starts==2)
assert(intent(3,1)=='I31|3|OPENING' and starts==3) -- repeat opens, not an already-unfolded forever helper
tick(.25);assert(intent(4,0)=='I31|4|CLOSING');assert(math.abs(paper.time-.75)<1e-6)
tick(.25);assert(state.paper31.phase=='CLOSED')
assert(intent(3,1)=='I31|REFUSED') -- stale command cannot reopen
reject=true;assert(intent(5,1)=='I31|5|WAIT_OPEN');tick(2.1);assert(state.paper31.phase=='FAULT' and paper.hidden);reject=false
paper.hidden=false;assert(intent(6,1)=='I31|6|OPENING')
badClock=true;tick(3);assert(state.paper31.phase=='FAULT' and paper.hidden) -- a returned StartAnimation is not completion
badClock=false;paper.hidden=false;assert(intent(7,1)=='I31|7|OPENING')
KCD2VRMap.WorldEnded();assert(not state.paper31 and paper.hidden and not paper.forced)
KCD2VRMap.awaitStart=true;assert(intent(8,1)=='I31|NO_CURRENT_PAPER')
assert(kcd2vr_map_m31_intent({})=='I31|REFUSED')
KCD2VRMap.awaitStart=false;paper.hidden=false;reject=true
assert(intent(8,1)=='I31|8|WAIT_OPEN');assert(intent(9,0)=='I31|9|CLOSED')
assert(not KCD2VRMap.Paper31Animating());reject=false
''')
print(json.dumps({'result':'PASS','production_Lua51':True,'repeat_open_close':True,'transient_hide_does_not_restart':True,'explicit_native_pose_clock_checked':True,'native_game_animation_and_render':'NOT_RUN'},indent=2))
