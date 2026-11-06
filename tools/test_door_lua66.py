"""C66 controller-time/release-pause Lua5.1; native API leaves are doubles."""
from pathlib import Path
import argparse
import hashlib
import json
import zipfile
from lupa.lua51 import LuaRuntime
ROOT=Path(__file__).resolve().parents[1]
LUA=(ROOT/'mods/KCD2VRDoors/door_runtime.lua').read_text(encoding='utf-8-sig')

def fixture():
 l=LuaRuntime(unpack_returned_tuples=True)
 l.execute(r'''
vars={seq=2,version=1966,companion_66=66,request=0,target_hi=0,target_lo=0,phase=0,fraction=0}
logs={};getterCalls=0;torn=false;plays=0;updates=0;finishes=0;writes=0;speeds=0
System={GetCVar=function(name)
 getterCalls=getterCalls+1;if allNil then return nil end
 local k=string.sub(name,13);local cs,cr=string.match(k,'^claim_(%d+)_(%d+)$')
 if cs then
  if noClaim then return 0 end
  assert(tonumber(cs)==vars.seq and tonumber(cr)==vars.request and vars.phase==1)
  claimed=true;if disableAfterClaim then vars.seq=0 end;return 1
 end
 local seq,request,state,fraction=string.match(k,'^ack_(%d+)_(%d+)_(%d+)_(%d+)$')
 if seq then
  state=tonumber(state);if state==2 then assert(finishes>0)end
  if (denyAck0 and state==0)or(denyAck2 and state==2)then return 0 end
  vars.ack_source=tonumber(seq);vars.ack_request=tonumber(request)
  vars.ack_state=state;vars.ack_fraction=tonumber(fraction)/1000000;return 1
 end
 local cs,cr,clip,time=string.match(k,'^curve_(%d+)_(%d+)_(.+)_(%d+)$')
 if cs then
  curveRequests=(curveRequests or 0)+1;curveClip=clip;curveTime=tonumber(time)/1000000
  if tonumber(cr)~=vars.request or tonumber(cs)~=vars.seq then return 0 end
  return noCurve and 0 or(pendingCurve and 2 or 1)
 end
 if k=='seq'and torn then vars.seq=vars.seq+2 end
 return vars[k]
end,LogAlways=function(message)table.insert(logs,message)end}
function cmd(request,target,phase,fraction)
 vars.seq=vars.seq+2;vars.request=request;vars.target_hi=math.floor(target/65536)
 vars.target_lo=target%65536;vars.phase=phase;vars.fraction=math.floor((fraction or 0)*1000000+.5)
end
g_localActor={id={nativeHandle=1}};AnimDoor={Server={}}
function AnimDoor:DoPlayAnimation(direction,forceTime,useSound,customAnim,usePlayerAnim,userId)
 plays=plays+1;self.lastUsedBy=userId;self.inUse=1;self.bNeedUpdate=1;self.bNoAnims=false
 if not(self.bUseSameAnim and self.running)then
  self.curAnim=nativeAlias or('native_'..plays);self.time=forceTime or .1;self.rate=1;self.running=true
  if usePlayerAnim then actorUses=(actorUses or 0)+1;if self.onNative then self:onNative()end end
 else self.rate=direction end
 self.nDirection=direction;return 17
end
function AnimDoor.Server:OnUpdate(dt)
 updates=updates+1
 if self.bNeedUpdate==1 and(not self.running or self.time>=.9999)then
  self.inUse=0;self.bNeedUpdate=0;self.curAnim='';finishes=finishes+1
  if finishForeignUser then self.lastUsedBy={foreign=true}end
 end
 return 23
end
function AnimDoor:OnSave(t)self.savedRate=self.rate;return 31 end
function AnimDoor:OnLoad(t)self.loaded=true;return 32 end
function AnimDoor:OnReset()self.reset=true;return 33 end
function door(id)
 local d={id={opaque=id},raw=id,time=0,rate=1,curAnim='',bNeedUpdate=0,inUse=0}
 setmetatable(d,{__index=AnimDoor})
 function d:GetRawId()return self.raw end
 function d:GetAnimationTime(a,b)assert(a==0 and b==0);return self.time end
 function d:GetAnimationSpeed(a,b)assert(a==0 and b==0);if badRate then error('unreadable rate')end;return self.rate end
 function d:IsAnimationRunning(a,b)assert(a==0 and b==0);return self.running end
 function d:SetAnimationSpeed(a,b,v)
  assert(a==0 and b==0)
  if not((failRestore and v~=0)or(failPause and v==0))then self.rate=v end;speeds=speeds+1
 end
 function d:SetAnimationTime(a,b,v)assert(a==0 and b==0);if not failTime then self.time=v end;writes=writes+1 end
 function d:StopAnimation(a,b)assert(a==0 and b==0);self.running=false;stops=(stops or 0)+1 end
 return d
end
function accept(d,serial)
 d.onNative=function()cmd(serial,d.raw,1,0)end
 assert(d:DoPlayAnimation(1,nil,nil,nil,true,g_localActor.id)==17);d.onNative=nil
end
function tick(d,dt)
 if d.running then d.time=math.max(0,math.min(1,d.time+dt*d.rate))end
 assert(AnimDoor.Server.OnUpdate(d,dt)==23)
end
function freshTick(d,phase,fraction,dt)cmd(vars.request,d.raw,phase,fraction or 0);tick(d,dt or .01)end
''')
 l.execute(LUA);return l

def main():
 p=argparse.ArgumentParser();p.add_argument('--pak',type=Path);p.add_argument('--out',type=Path);a=p.parse_args()
 packaged=None
 if a.pak:
  with zipfile.ZipFile(a.pak)as z:b=z.read('Scripts/Startup/zzkcd2vr_doors.lua')
  assert b==(ROOT/'mods/KCD2VRDoors/door_runtime.lua').read_bytes()
  packaged={'path':str(a.pak),'sha256':hashlib.sha256(a.pak.read_bytes()).hexdigest(),'member_equal_to_source':True}
 cases=[]
 def run(name,code):l=fixture();l.execute(code);cases.append(name)
 run('exact1966/revision66 initial native pause and actual curve request',r'''
assert(KCD2VRDoors.installed and KCD2VRDoors.revision==66 and #logs==1)
assert(string.find(logs[1],'revision=66 protocol=1966',1,true));assert(KCD2VRDoors.Install()and #logs==1)
d=door(7);accept(d,1);tick(d,.01)
assert(KCD2VRDoors.bridge66 and d.rate==0 and claimed and vars.ack_state==1)
assert(curveRequests==1 and curveClip==d.curAnim and curveTime==d.time)
''')
 run('controller forward/reverse/stop and release pause beyond15s without ActorUse2',r'''
d=door(7);accept(d,1);tick(d,.01);assert(actorUses==1)
freshTick(d,2,.6);assert(d.time==.6 and d.rate==0 and vars.ack_fraction==.6)
freshTick(d,2,.2);assert(d.time==.2 and d.rate==0)
local w=writes;for i=1,1601 do freshTick(d,4,.9)end
assert(d.time==.2 and d.rate==0 and writes==w and finishes==0 and plays==1 and actorUses==1)
freshTick(d,2,.7);assert(d.time==.7 and d.rate==0 and plays==1 and actorUses==1)
''')
 run('actual curve callback precedes controller time write',"d=door(7);accept(d,1);tick(d,.01);local t=d.time;freshTick(d,2,.7);assert(curveTime==t and d.time==.7 and curveRequests==2)")
 run('initial pending curve2 can later bind without time writes', 'pendingCurve=true;d=door(7);accept(d,1);tick(d,.01);local t=d.time;freshTick(d,2,.8);assert(d.time==t and d.rate==0 and writes==0 and vars.ack_state==1);pendingCurve=false;freshTick(d,2,.8);assert(d.time==.8 and d.rate==0 and writes==1)')
 run('permanent pending2 restores on bounded owner cleanup', 'pendingCurve=true;d=door(7);accept(d,1);tick(d,.01);local t=d.time;for i=1,26 do freshTick(d,1,0)end;cmd(1,d.raw,0,0);tick(d,.01);assert(d.time==t and d.rate==1 and writes==0 and vars.ack_state==0)')
 run('lost previously bound curve retires time owner', 'd=door(7);accept(d,1);tick(d,.01);local t=d.time;noCurve=true;freshTick(d,2,.8);assert(d.time==t and d.rate==1 and writes==0 and vars.ack_state==0)')
 run('held maximum never completes until explicit released endpoint',r'''
d=door(7);accept(d,1);tick(d,.01);freshTick(d,2,.9995)
for i=1,20 do freshTick(d,2,.9995)end
assert(d.rate==0 and finishes==0 and d.inUse==1)
freshTick(d,5,.9995);assert(finishes==1 and vars.ack_state==2 and plays==1 and actorUses==1)
''')
 run('returned start uses original opposite object clip and native finish',r'''
d=door(7);accept(d,1);tick(d,.01);freshTick(d,2,.8);freshTick(d,2,0)
assert(finishes==0 and d.nDirection==1 and d.rate==0)
freshTick(d,5,0);assert(finishes==1 and vars.ack_state==2 and d.nDirection==-1 and plays==2 and actorUses==1)
''')
 run('same-clip native reuse is stopped only at released start endpoint',r'''
d=door(7);accept(d,1);tick(d,.01);d.bUseSameAnim=true;freshTick(d,2,0)
assert(not stops and d.rate==0);freshTick(d,5,0)
assert(stops==1 and plays==2 and actorUses==1 and finishes==1 and vars.ack_state==2 and d.nDirection==-1)
''')
 run('close action returned to start natively reports original open state',r'''
d=door(7);d.onNative=function()cmd(1,d.raw,1,0)end
assert(d:DoPlayAnimation(-1,nil,nil,nil,true,g_localActor.id)==17);d.onNative=nil;tick(d,.01)
freshTick(d,2,0);freshTick(d,5,0)
assert(d.nDirection==1 and plays==2 and actorUses==1 and finishes==1 and vars.ack_state==2)
''')
 run('malformed middle finalization never invents ACK2','d=door(7);accept(d,1);tick(d,.01);freshTick(d,2,.4);freshTick(d,5,.4);assert(vars.ack_state==0 and finishes==0 and d.rate==1 and plays==1)')
 run('true cleanup restores native finish at current controller time','d=door(7);accept(d,1);tick(d,.01);freshTick(d,2,.6);cmd(1,d.raw,0,0);tick(d,.01);assert(d.rate==1 and vars.ack_state==0);tick(d,.9);assert(finishes==1 and vars.ack_state==0)')
 for loss in ['cmd(1,8,4,0)','cmd(2,d.raw,4,0)','torn=true','vars.version=1965','vars.fraction=0/0','vars.seq=3','cmd(1,d.raw,3,.6)']:
  run('context/protocol loss: '+loss,'d=door(7);accept(d,1);tick(d,.01);'+loss+';tick(d,.01);assert(d.rate==1 and vars.ack_state==0 and writes==0)')
 run('80ms command loss cleans native pause','d=door(7);accept(d,1);tick(d,.01);tick(d,.05);tick(d,.05);assert(d.rate==1 and vars.ack_state==0)')
 run('foreign clip/rate never receives old time or rate','d=door(7);accept(d,1);tick(d,.01);d.curAnim="foreign";d.rate=.7;freshTick(d,2,.8);assert(d.rate==.7 and d.time<.2 and vars.ack_state==0 and writes==0)')
 run('foreign native user cannot inherit same-looking clip','d=door(7);accept(d,1);tick(d,.01);d.lastUsedBy={other=true};d.rate=.7;freshTick(d,5,0);assert(d.rate==.7 and writes==0 and plays==1 and vars.ack_state==0)')
 run('foreign paused time writer not overwritten','d=door(7);accept(d,1);tick(d,.01);d.time=.8;freshTick(d,2,.5);assert(d.time==.8 and writes==0 and vars.ack_state==0)')
 run('same-clip external20ms seek revokes paused owner','d=door(7);accept(d,1);tick(d,.01);local t=d.time;d.time=t+.02;freshTick(d,2,.5);assert(d.time==t+.02 and writes==0 and d.rate==1 and vars.ack_state==0)')
 run('native paused float rounding within10us remains valid','d=door(7);accept(d,1);tick(d,.01);d.time=d.time+.00001;freshTick(d,4);assert(d.rate==0 and vars.ack_state==1 and writes==0)')
 run('G2 legacy mode works without a spatial curve','vars.mode69=3;vars.companion_69=69;noCurve=true;d=door(7);accept(d,1);tick(d,.01);assert(d.rate==0 and vars.ack_state==1 and not curveRequests);freshTick(d,2,.6);assert(math.abs(d.time-.6)<.00001 and d.rate==0 and not curveRequests);cmd(vars.request,7,0,0);tick(d,.01);assert(d.rate==1 and vars.ack_state==0)')
 run('G2 legacy reverse-rate clip uses operation progress','vars.mode69=3;vars.companion_69=69;noCurve=true;d=door(7);d.bUseSameAnim=true;d.running=true;d.time=.8;d.curAnim="native_legacy_reverse";d.onNative=function()cmd(1,d.raw,1,0)end;d:DoPlayAnimation(-1,nil,nil,nil,true,g_localActor.id);d.onNative=nil;cmd(1,d.raw,1,0);tick(d,.01);freshTick(d,2,.6);assert(math.abs(d.time-.4)<.00001 and math.abs(vars.ack_fraction-.6)<.00001);cmd(vars.request,7,0,0);tick(d,.01);assert(d.rate==-1 and vars.ack_state==0)')
 run('G2 mode change cannot convert a claimed door','vars.mode69=3;vars.companion_69=69;noCurve=true;d=door(7);accept(d,1);tick(d,.01);vars.mode69=2;freshTick(d,2,.6);assert(d.rate==1 and vars.ack_state==0 and writes==0)')
 run('failed controller time readback retires authority','d=door(7);accept(d,1);tick(d,.01);failTime=true;freshTick(d,2,.5);assert(writes==1 and d.rate==1 and vars.ack_state==0)')
 run('failed endpoint setter never fabricates completion','d=door(7);accept(d,1);tick(d,.01);failTime=true;freshTick(d,5,.9995);assert(finishes==0 and vars.ack_state==0 and d.rate==1)')
 for method,result in [('OnSave({})',31),('OnLoad({})',32),('OnReset()',33)]:
  run('native lifecycle cleanup '+method,'d=door(7);accept(d,1);tick(d,.01);assert(d:'+method+'=='+str(result)+');assert(d.rate==1 and vars.ack_state==0)')
 run('NPC remains native','d=door(7);cmd(1,d.raw,1,0);d:DoPlayAnimation(1,nil,nil,nil,true,{npc=true});tick(d,.01);assert(d.rate==1 and not claimed)')
 run('forced/load stays native','d=door(7);cmd(1,d.raw,1,0);d:DoPlayAnimation(1,1,nil,nil,false,g_localActor.id);tick(d,.01);assert(d.rate==1 and not claimed)')
 run('preposted request cannot seize later action','d=door(7);cmd(1,d.raw,1,0);d:DoPlayAnimation(1,nil,nil,nil,true,g_localActor.id);tick(d,.01);assert(d.rate==1 and not claimed)')
 run('bad dt cleans up and original still updates','d=door(7);accept(d,1);tick(d,.01);local u=updates;freshTick(d,4,0,2);assert(d.rate==1 and updates==u+1)')
 run('claim refusal waits without pause','d=door(7);accept(d,1);noClaim=true;tick(d,.01);assert(d.rate==1 and not claimed);noClaim=false;freshTick(d,1);assert(d.rate==0 and claimed)')
 run('target outside protocol exact range never pauses','d=door(16777216);accept(d,1);tick(d,.01);assert(d.rate==1 and not claimed and speeds==0)')
 run('Stop after claim retains cleanup','d=door(7);accept(d,1);disableAfterClaim=true;tick(d,.01);assert(d.rate==1 and claimed and vars.ack_state==0 and writes==0);tick(d,.01);assert(d.rate==1 and vars.ack_state==0)')
 run('failed restore is cleanup-only until readback','d=door(7);accept(d,1);tick(d,.01);failRestore=true;cmd(1,d.raw,0,0);tick(d,.01);assert(d.rate==0 and vars.ack_state==1);failRestore=false;tick(d,.01);assert(d.rate==1 and vars.ack_state==0)')
 run('failed initial pause grants no lasting authority','d=door(7);accept(d,1);failPause=true;tick(d,.01);assert(d.rate==1 and vars.ack_state==0)')
 run('unreadable restore retains original updater','d=door(7);accept(d,1);tick(d,.01);badRate=true;cmd(1,d.raw,0,0);local u=updates;tick(d,.01);assert(updates==u+1 and d.rate==0);badRate=false;tick(d,.01);assert(d.rate==1 and vars.ack_state==0)')
 run('native negative rate retained only for true cleanup',r'''
d=door(7);d.onNative=function()d.rate=-1;d.time=.9;cmd(1,d.raw,1,0)end
d:DoPlayAnimation(-1,nil,nil,nil,true,g_localActor.id);d.onNative=nil;tick(d,.01)
freshTick(d,2,.4);assert(d.time==.4 and d.rate==0);cmd(1,d.raw,0,0);tick(d,.01)
assert(d.rate==-1 and vars.ack_state==0)
''')
 run('ACK0 failure remains receipt-only','d=door(7);accept(d,1);tick(d,.01);denyAck0=true;cmd(1,d.raw,0,0);tick(d,.01);local w=writes;freshTick(d,2,.8);assert(d.rate==1 and writes==w);denyAck0=false;tick(d,.01);assert(vars.ack_state==0)')
 run('ACK2 retry never restarts animation or clock',r'''
d=door(7);accept(d,1);tick(d,.01);denyAck2=true;freshTick(d,5,.9995)
assert(finishes==1 and vars.ack_state==1);local n=speeds;local w=writes
freshTick(d,4);assert(speeds==n and writes==w and plays==1)
denyAck2=false;tick(d,.01);assert(vars.ack_state==2 and speeds==n and writes==w)
''')
 run('native replacement preserves separate cleanup identity','d=door(7);accept(d,1);tick(d,.01);accept(d,2);tick(d,.01);assert(d.rate==0 and plays==2 and vars.ack_request==2)')
 run('unconfirmed old cleanup cannot adopt another session',r'''
d=door(7);accept(d,1);tick(d,.01);denyAck0=true;accept(d,2);assert(plays==2 and d.rate==1)
local w=writes;freshTick(d,2,.7);assert(writes==w and d.rate==1)
denyAck0=false;tick(d,.01);assert(vars.ack_state==0)
''')
 run('inactive native doors stay cheap','d=door(7);local n=getterCalls;for i=1,200 do tick(d,.01)end;assert(getterCalls==n and updates==200 and speeds==0 and writes==0)')
 run('missing ASI cannot acquire','allNil=true;d=door(7);accept(d,1);local n=getterCalls;tick(d,.01);assert(d.rate==1 and not claimed and getterCalls==n)')
 run('hard initial curve0 retires without controller write', 'noCurve=true;d=door(7);accept(d,1);tick(d,.01);assert(d.rate==1 and vars.ack_state==0 and writes==0)')
 run('pending2 cannot revoke a ready curve', 'd=door(7);accept(d,1);tick(d,.01);pendingCurve=true;freshTick(d,2,.8);assert(d.rate==1 and vars.ack_state==0 and writes==0)')
 for alias in ['../native','native.space','a'*81]:
  run('invalid curve alias stays paused: '+alias, 'nativeAlias="'+alias+'";d=door(7);accept(d,1);tick(d,.01);freshTick(d,2,.7);assert(d.rate==1 and writes==0 and not curveRequests and vars.ack_state==0)')
 result={'checks_passed':len(cases),'cases':cases,'protocol':1966,'revision':66,
  'lua_sha256':hashlib.sha256((ROOT/'mods/KCD2VRDoors/door_runtime.lua').read_bytes()).hexdigest(),
  'packaged':packaged,'native_execution':False,'headset_acceptance':False}
 if a.out:a.out.write_text(json.dumps(result,indent=2)+'\n',encoding='utf-8')
 print(f'actual C66 door companion Lua5.1: {len(cases)} scenario groups PASS (native leaves replaced)')
if __name__=='__main__':main()
