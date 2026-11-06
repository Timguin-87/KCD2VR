"""C65 hold-to-play/release-to-pause Lua5.1; native API leaves are doubles."""
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
vars={seq=2,version=1965,companion_65=65,request=0,target_hi=0,target_lo=0,phase=0,fraction=0}
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
 if k=='seq'and torn then vars.seq=vars.seq+2 end
 return vars[k]
end,LogAlways=function(message)table.insert(logs,message)end}
function cmd(request,target,phase,fraction)
 vars.seq=vars.seq+2;vars.request=request;vars.target_hi=math.floor(target/65536)
 vars.target_lo=target%65536;vars.phase=phase;vars.fraction=math.floor((fraction or 0)*1000000+.5)
end
g_localActor={id={nativeHandle=1}};AnimDoor={Server={}}
function AnimDoor:DoPlayAnimation(direction,forceTime,useSound,customAnim,usePlayerAnim,userId)
 plays=plays+1;self.curAnim='native_'..plays;self.nDirection=direction;self.lastUsedBy=userId
 self.inUse=1;self.bNeedUpdate=1;self.bNoAnims=false;self.time=.1;self.rate=1;self.running=true
 if self.onNative then self:onNative()end;return 17
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
 function d:SetAnimationTime(a,b,v)error('C65 must never scrub animation time')end
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
function freshTick(d,phase,dt)cmd(vars.request,d.raw,phase,0);tick(d,dt or .01)end
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
 run('exact protocol/revision and native accepted pause',r'''
assert(KCD2VRDoors.installed and KCD2VRDoors.revision==65 and #logs==1)
assert(string.find(logs[1],'revision=65 protocol=1965',1,true))
assert(KCD2VRDoors.Install()and #logs==1)
d=door(7);accept(d,1);tick(d,.01)
assert(KCD2VRDoors.bridge65 and d.rate==0 and claimed and vars.ack_state==1)
''')
 run('held PLAY/release PAUSE beyond15s/regrip/native completion',r'''
d=door(7);accept(d,1);tick(d,.01);local first=d.time
freshTick(d,3);assert(d.rate==1 and d.time==first)
freshTick(d,3,.1);assert(d.time>first and d.rate==1)
freshTick(d,4);local stopped=d.time;assert(d.rate==0 and d.inUse==1 and finishes==0)
for i=1,1600 do freshTick(d,4)end
assert(d.rate==0 and d.time==stopped and finishes==0 and plays==1)
freshTick(d,3);assert(d.rate==1)
freshTick(d,3,.9);assert(finishes==1 and vars.ack_state==2 and plays==1 and writes==0)
''')
 run('pause fraction cannot scrub native time',"d=door(7);accept(d,1);tick(d,.01);local t=d.time;cmd(1,d.raw,4,.9);tick(d,.01);assert(d.time==t and d.rate==0);cmd(1,d.raw,4,.1);tick(d,.01);assert(d.time==t and writes==0)")
 run('true cleanup restores native finish',"d=door(7);accept(d,1);tick(d,.01);freshTick(d,3,.1);freshTick(d,4,.1);cmd(1,d.raw,0,0);tick(d,.01);assert(d.rate==1 and vars.ack_state==0);tick(d,.9);assert(finishes==1 and vars.ack_state==0 and plays==1)")
 for loss in ['cmd(1,8,4,0)','cmd(2,d.raw,4,0)','torn=true','vars.version=1944','vars.fraction=0/0','vars.seq=3','cmd(1,d.raw,2,.6)']:
  run('context/protocol loss: '+loss,'d=door(7);accept(d,1);tick(d,.01);assert(d.rate==0);'+loss+';tick(d,.01);assert(d.rate==1 and vars.ack_state==0 and writes==0)')
 run('stale80ms heartbeat restores pause','d=door(7);accept(d,1);tick(d,.01);tick(d,.05);tick(d,.05);assert(d.rate==1 and vars.ack_state==0)')
 run('foreign clip/rate survives','d=door(7);accept(d,1);tick(d,.01);d.curAnim="foreign";d.rate=.7;freshTick(d,4);assert(d.rate==.7 and vars.ack_state==0)')
 run('foreign user loses same-looking clip','d=door(7);accept(d,1);tick(d,.01);d.lastUsedBy={other=true};d.rate=.7;freshTick(d,4);assert(d.rate==.7 and vars.ack_state==0)')
 run('foreign paused time writer not overwritten','d=door(7);accept(d,1);tick(d,.01);d.time=.8;freshTick(d,4);assert(d.rate==1 and writes==0 and vars.ack_state==0)')
 for method,result in [('OnSave({})',31),('OnLoad({})',32),('OnReset()',33)]:
  run('native lifecycle cleanup '+method,'d=door(7);accept(d,1);tick(d,.01);assert(d:'+method+'=='+str(result)+');assert(d.rate==1 and vars.ack_state==0)')
 run('NPC remains native','d=door(7);cmd(1,d.raw,1,0);assert(d:DoPlayAnimation(1,nil,nil,nil,true,{npc=true})==17);tick(d,.01);assert(d.rate==1 and not claimed)')
 run('forced/load remains native','d=door(7);cmd(1,d.raw,1,0);assert(d:DoPlayAnimation(1,1,nil,nil,false,g_localActor.id)==17);tick(d,.01);assert(d.rate==1 and not claimed)')
 run('preposted request cannot acquire later action','d=door(7);cmd(1,d.raw,1,0);assert(d:DoPlayAnimation(1,nil,nil,nil,true,g_localActor.id)==17);tick(d,.01);assert(d.rate==1 and not claimed)')
 run('bad dt cleans up while original update runs','d=door(7);accept(d,1);tick(d,.01);local u=updates;freshTick(d,4,2);assert(d.rate==1 and updates==u+1)')
 run('claim refused until actual lease','d=door(7);accept(d,1);noClaim=true;tick(d,.01);assert(d.rate==1 and not claimed);noClaim=false;freshTick(d,1);assert(d.rate==0 and claimed)')
 run('target outside exact float32 protocol range is not claimed','d=door(16777216);accept(d,1);tick(d,.01);assert(d.rate==1 and not claimed and speeds==0)')
 run('Stop after CLAIM before pause retains restore','d=door(7);accept(d,1);disableAfterClaim=true;tick(d,.01);assert(d.rate==0 and claimed);tick(d,.01);assert(d.rate==1 and vars.ack_state==0)')
 run('failed restore cleanup retries','d=door(7);accept(d,1);tick(d,.01);failRestore=true;cmd(1,d.raw,0,0);tick(d,.01);assert(d.rate==0 and vars.ack_state==1);failRestore=false;tick(d,.01);assert(d.rate==1 and vars.ack_state==0)')
 run('failed pause does not retain hand authority','d=door(7);accept(d,1);failPause=true;tick(d,.01);assert(d.rate==1 and vars.ack_state==0)')
 run('unreadable restore retains original update','d=door(7);accept(d,1);tick(d,.01);badRate=true;cmd(1,d.raw,0,0);local u=updates;tick(d,.01);assert(d.rate==0 and updates==u+1);badRate=false;tick(d,.01);assert(d.rate==1 and vars.ack_state==0)')
 run('stopped FIFO completes natively beforeACK2','d=door(7);accept(d,1);tick(d,.01);d.running=false;freshTick(d,3);assert(finishes==1 and vars.ack_state==2)')
 run('negative native speed resumes without scrubbing',r'''
d=door(7);d.onNative=function()d.rate=-1;d.time=.9;cmd(1,d.raw,1,0)end
assert(d:DoPlayAnimation(-1,nil,nil,nil,true,g_localActor.id)==17);d.onNative=nil
tick(d,.01);assert(d.rate==0);freshTick(d,3);assert(d.rate==-1)
freshTick(d,4,.1);assert(d.rate==0 and d.time<.89)
cmd(1,d.raw,0,0);tick(d,.01);assert(d.rate==-1 and vars.ack_state==0 and writes==0)
''')
 run('ACK0 refusal remains cleanup-only','d=door(7);accept(d,1);tick(d,.01);denyAck0=true;cmd(1,d.raw,0,0);tick(d,.01);assert(d.rate==1 and vars.ack_state==1);freshTick(d,3);assert(d.rate==1 and writes==0);denyAck0=false;tick(d,.01);assert(vars.ack_state==0)')
 run('ACK2 retry has no clock writes',r'''
d=door(7);accept(d,1);tick(d,.01);freshTick(d,3);denyAck2=true;freshTick(d,3,.9)
assert(finishes==1 and vars.ack_state==1);local n=speeds;local u=updates
cmd(1,d.raw,4,0);tick(d,.01);assert(speeds==n and updates==u+1 and plays==1)
denyAck2=false;tick(d,.01);assert(vars.ack_state==2 and speeds==n and writes==0)
''')
 run('completion changed native user does not fabricateACK2',r'''
d=door(7);accept(d,1);tick(d,.01);freshTick(d,3);finishForeignUser=true
freshTick(d,3,1);assert(finishes==1 and vars.ack_state==1)
freshTick(d,4);assert(vars.ack_state==0 and writes==0)
''')
 run('same-door native replacement cleans old lease before new claim','d=door(7);accept(d,1);tick(d,.01);accept(d,2);tick(d,.01);assert(d.rate==0 and plays==2 and vars.ack_request==2)')
 run('unacknowledged old cleanup cannot be replaced by a fresh session',r'''
d=door(7);accept(d,1);tick(d,.01);denyAck0=true;accept(d,2)
assert(plays==2 and d.rate==1);local n=speeds
tick(d,.01);assert(d.rate==1 and speeds==n and vars.ack_request==1)
denyAck0=false;tick(d,.01);assert(vars.ack_state==0 and d.rate==1)
cmd(2,d.raw,1,0);tick(d,.01);assert(d.rate==1 and plays==2 and writes==0)
''')
 run('inactive doors do no getter/setter work','d=door(7);local n=getterCalls;for i=1,200 do tick(d,.01)end;assert(getterCalls==n and updates==200 and speeds==0)')
 run('missing ASI creates no session','allNil=true;d=door(7);accept(d,1);local n=getterCalls;tick(d,.01);tick(d,.01);assert(d.rate==1 and not claimed and getterCalls==n)')
 run('transport disappears while paused: restore once then receipt only',r'''
d=door(7);accept(d,1);tick(d,.01);allNil=true;local n=speeds
tick(d,.01);assert(d.rate==1 and speeds==n+1 and vars.ack_state==1)
tick(d,.01);assert(speeds==n+1);allNil=false;tick(d,.01);assert(vars.ack_state==0)
''')
 result={'checks_passed':len(cases),'cases':cases,'protocol':1965,'revision':65,
  'lua_sha256':hashlib.sha256((ROOT/'mods/KCD2VRDoors/door_runtime.lua').read_bytes()).hexdigest(),
  'packaged':packaged,'native_execution':False,'headset_acceptance':False}
 if a.out:a.out.write_text(json.dumps(result,indent=2)+'\n',encoding='utf-8')
 print(f'actual C65 door companion Lua5.1: {len(cases)} scenario groups PASS (native leaves replaced)')
if __name__=='__main__':main()
