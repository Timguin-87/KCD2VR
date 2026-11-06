"""Execute unmodified retail AnimDoor plus the authored VR door companion.

Only external native/script leaves are doubles. OnUsed, lock branch selection,
DoPlayAnimation, SetPhysicalFlags and Server.OnUpdate are the retail functions.
This is an integration/ordering fixture, not native execution or headset proof.
"""
from pathlib import Path
import argparse
import hashlib
import json
import zipfile

try:
    from lupa.lua51 import LuaRuntime
except ImportError as error:
    raise SystemExit("This fixture requires lupa.lua51; expose its installed directory with PYTHONPATH.") from error

SOURCE = Path(__file__).resolve().parents[1]
PAK = Path("C:/Steam/steamapps/common/KingdomComeDeliverance2/Data/Scripts.pak")
MEMBER = "Scripts/Entities/Doors/AnimDoor.lua"
EXPECTED_SCRIPT_SHA = "62aad1a999e37babf447b39583ad251e4a7facbdd297f92029915da4759068e4"

LEAVES = r'''
Script={ReloadScript=function()end}
function Pick(cond,a,b) if cond then return a else return b end end
ENTITY_FLAG_EXTENDED_FORCE_UPDATE=8;PHYSICPARAM_FLAGS=4;pef_always_shoot_ray_down=256
enum_sound={door=4};RPG={CrimeDoorSoundMod=.5}
Calendar={GetGameTime=function()return 100 end}
events={};logs={};operations={};nativeActions=0;objectStarts=0;writes=0;speedWrites=0
heldReceipts=0;unlockReports=0;manipulationStarts=0;physicalCalls=0;doorReports=0
crimeSounds=0;crimeVolumes=0;getters=0;claimCount=0;nativeActorFlags=0
System={LogAlways=function(s)table.insert(logs,s)end,
 ActivatePortal=function(pos,on,id)table.insert(events,'portal:'..on)end}
EntityCommon={BroadcastEvent=function(d,event)table.insert(events,event)end}
Crime={ProduceAiSoundOnDudePosition=function()crimeSounds=crimeSounds+1 end}
XGenAIModule={FindLinks=function(id,kind)
  if kind=='crime_doNotSpawnSuspiciousDoorVolume' then return skipCrime and {true} or {} end
  return {}
 end,
 SpawnPerceptibleVolume=function()crimeVolumes=crimeVolumes+1;return 99 end,
 AddLink=function()end,SendMessageToEntity=function()table.insert(events,'aiMessage')end}
vars={seq=2,version=1944,companion_46=46,request=0,target_hi=0,target_lo=0,phase=0,fraction=0}
function cmd(request,target,phase,fraction)
 vars.seq=vars.seq+2;vars.request=request;vars.target_hi=math.floor(target/65536)
 vars.target_lo=target%65536;vars.phase=phase;vars.fraction=math.floor((fraction or 0)*1000000+.5)
end
System.GetCVar=function(name)
 getters=getters+1
 local k=string.sub(name,13)
 local cs,cr=string.match(k,'^claim_(%d+)_(%d+)$')
 if cs then
  assert(tonumber(cs)==vars.seq and tonumber(cr)==vars.request and vars.phase==1)
  claimCount=claimCount+1
  return refuseClaim and 0 or 1
 end
 local seq,request,state,fraction=string.match(k,'^ack_(%d+)_(%d+)_(%d+)_(%d+)$')
 if seq then
  if refuseAck0 and tonumber(state)==0 then return 0 end
  vars.ack_source=tonumber(seq);vars.ack_request=tonumber(request)
  vars.ack_state=tonumber(state);vars.ack_fraction=tonumber(fraction)/1000000
  if tonumber(state)==1 then heldReceipts=heldReceipts+1 end
  return 1
 end
 return vars[k]
end
player={id={nativeHandle=1},actor={}}
g_localActor=player
function player:GetWorldPos() return {x=0,y=-1,z=0} end
function player:SetPhysicParams(kind,flags)
 assert(kind==PHYSICPARAM_FLAGS)
 nativeActorFlags=flags.flags==pef_always_shoot_ray_down and 1 or 0
 physicalCalls=physicalCalls+1
end
System.GetEntity=function(id)if id==player.id then return player end return nil end
function player.actor:StartInteractiveActionByName(name,id,force,speed)
 nativeActions=nativeActions+1
 assert(force==true and speed==1 and id==doorInstance.id)
 assert(string.sub(name,1,5)=='door_')
 table.insert(operations,'actor-before-object')
 -- Exact C++ accepted-action hook runs inside this native return, before the
 -- retail DoPlayAnimation starts its object clip. The pending worker command
 -- can arrive either synchronously or before the next door update.
 if acceptedAction~=false and admitRequest~=false then
  if deferredCommand then pendingRequest=(vars.request or 0)+1
  else cmd((vars.request or 0)+1,doorInstance.raw,1,0) end
 end
 if acceptedAction==false then return nil end
 return 17
end
'''

DOUBLES = r'''
function door(raw)
 local d={id={opaque=raw},raw=raw,this={id={opaque=raw}},time=0,rate=1,running=false,
  curAnim='',inUse=0,bNeedUpdate=0,bLocked=false,nDirection=-1,nUpdateAfterLoad=0,
  Properties={Lock={esLockTypes='none/none',bLockReversed=false,bSendMessage=false,guidItemClassId='key'},
   Animation={anim_Open='Open',anim_Close='Close'},Sounds={snd_Open='open',snd_Close='close'}}}
 -- Same boolean values produced by retail Reset, rather than the pre-reset
 -- class's numeric defaults (Lua treats numeric 0 as truthy).
 d.bUseSameAnim=d.Properties.Animation.anim_Close==''
 d.bNoAnims=d.Properties.Animation.anim_Open=='' and d.Properties.Animation.anim_Close==''
 setmetatable(d,{__index=AnimDoor});doorInstance=d
 d.animDoor={IsInteractive=function()return interactive~=false end,
  UpdateBeforeAnimation=function(self,id)table.insert(operations,'update-before')end,
  ReportManipulationStart=function()manipulationStarts=manipulationStarts+1 end,
  GetObjectAnimName=function(self,id,opened,wanted,front)return opened and 'door_l_b_c_object' or 'door_l_b_o_object' end,
  GetUserAnimName=function(self,id,opened,wanted,front)return opened and 'door_l_b_c' or 'door_l_b_o' end,
  ReportOpened=function(self,id)assert(id==player.id);doorReports=doorReports+1;table.insert(events,'report:open')end,
  ReportClosed=function(self,id)assert(id==player.id);doorReports=doorReports+1;table.insert(events,'report:close')end}
 d.lockBase={PlayerHoldsKey=function()return hasKey==true end,
  ReportUnlocked=function()unlockReports=unlockReports+1 end}
 function d:GetRawId()return self.raw end
 function d:GetWorldPos()return {x=0,y=0,z=0}end
 function d:GetPos()return self:GetWorldPos()end
 function d:GetWorldDir()return {x=0,y=1,z=0}end
 function d:GetAnimationTime(slot,layer)assert(slot==0 and layer==0);return self.time end
 function d:GetAnimationSpeed(slot,layer)assert(slot==0 and layer==0);if badRate then error('unreadable rate')end;return self.rate end
 function d:IsAnimationRunning(slot,layer)assert(slot==0 and layer==0);return self.running end
 function d:GetAnimationLength(slot,name)assert(slot==0);return 1 end
 function d:StopAnimation(slot,layer)assert(slot==0 and layer==0);self.running=false end
 function d:StartAnimation(slot,name)
  assert(slot==0 and nativeActions>0 or allowEvent==true)
  objectStarts=objectStarts+1;self.running=true;self.time=0
  table.insert(operations,'object-start')
 end
 function d:SetAnimationSpeed(slot,layer,rate)
  assert(slot==0 and layer==0)
  if not (failRestore and rate~=0) then self.rate=rate end
  speedWrites=speedWrites+1
 end
 function d:SetAnimationTime(slot,layer,time)
  assert(slot==0 and layer==0);self.time=time;writes=writes+1
 end
 function d:SetFlagsExtended(flag,mode)assert(flag==ENTITY_FLAG_EXTENDED_FORCE_UPDATE)end
 function d:Activate(on)assert(on==1)end
 return d
end
function advance(d,dt)
 -- The native scheduler advances/evaluates the clip outside Lua; only this
 -- external leaf is modeled. Native IsAnimationRunning is FIFO-count-based,
 -- not rate-based (retail +13C4978 -> CSkeletonAnim vf+78 -> +83A8B0).
 if d.running then d.time=math.max(0,math.min(1,d.time+dt*d.rate)) end
 AnimDoor.Server.OnUpdate(d,dt)
end
function isLogged(s)
 for _,line in ipairs(logs)do if string.find(line,s,1,true)then return true end end
 return false
end
'''


def main():
    """Keep the earlier CLI name while exercising the current C66 contract."""
    from test_door_retail66 import main as current_main
    current_main()


if __name__ == "__main__":
    main()
