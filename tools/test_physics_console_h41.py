from pathlib import Path
import sys,json,hashlib,argparse
sys.path.insert(0,r'C:/Users/turcs/.codex/worktrees/8fd6/KCD2VR/held-map-c7fm1/map-test-deps')
from lupa.lua51 import LuaRuntime
P=argparse.ArgumentParser();P.add_argument('--output',type=Path,required=True);a=P.parse_args();S=Path(__file__).resolve().parents[1]/'mods/kcd2vr_tests/zzkcd2vr_phys.lua'
L=LuaRuntime(unpack_returned_tuples=True)
L.execute(r'''
checks=0;impulses=0;timers=0;mutations=0;world={};logs={}
function ck(x)checks=checks+1;assert(x,'check '..checks)end
player={id={opaque=true},GetWorldPos=function()return{x=0,y=0,z=0}end};world[player.id]=player
local function dangerous()mutations=mutations+1 end
npc={id=20,actor={IsDead=function()return false end,Fall=dangerous,RagDollize=dangerous,StandUp=dangerous},GetWorldPos=function()return{x=0,y=2,z=0}end}
function npc:AddImpulse(part,p,d,amount,scale)ck(part==-1 and amount==4 and scale==1);impulses=impulses+1;lastDirection=d;if failImpulse then error('native partial unknown')end end
function npc:GetVelocity()return{x=0,y=0,z=0}end
function npc:GetPhysicalStats()return{mass=80}end
npc.AddConstraint=dangerous;npc.RemoveConstraint=dangerous;npc.SetVelocity=dangerous;world[20]=npc
System={LogAlways=function(s)logs[#logs+1]=s end,GetEntity=function(id)if lookupFail then error('unknown')end;return world[id]end,GetEntitiesInSphere=function()return{player,npc}end}
Script={SetTimer=function()timers=timers+1 end}
''');L.execute(S.read_text());L.execute(r'''
ck(impulses==0 and mutations==0 and timers==0)
ck(KCD2VR_Phys.Push(4));ck(impulses==1 and lastDirection.y==1)
ck(KCD2VR_Phys.Pull(4));ck(impulses==2 and lastDirection.y==-1)
ck(KCD2VR_Phys.Stats());ck(impulses==2)
for _,f in ipairs({'Drag','Grab','Constraint','Fall','Ragdoll','StandUp','Stop'})do ck(not KCD2VR_Phys[f](3))end
ck(impulses==2 and mutations==0 and timers==0)
for _,v in ipairs({'nan','inf','junk',0,-4,21})do ck(not KCD2VR_Phys.Push(v))end
ck(impulses==2)
lookupFail=true;ck(not KCD2VR_Phys.Push(4));lookupFail=false;ck(impulses==2)
local e=npc.GetWorldPos;npc.GetWorldPos=function()world[20]={id=20};return e()end;ck(not KCD2VR_Phys.Push(4));ck(impulses==2);npc.GetWorldPos=e;world[20]=npc
failImpulse=true;ck(not KCD2VR_Phys.Push(4));ck(impulses==3);failImpulse=false;ck(timers==0 and mutations==0)
''');a.output.mkdir(parents=True,exist_ok=True);r={'source':str(S),'sha256':hashlib.sha256(S.read_bytes()).hexdigest(),'checks':L.globals().checks,'PASS':True,'limits':'Actual Lua module, native methods mocked. Console-only one-shot command; no controller provider/native movement/headset acceptance.'};(a.output/'RECEIPT.json').write_text(json.dumps(r,indent=2)+'\n');print(json.dumps(r))
