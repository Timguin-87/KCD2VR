#!/usr/bin/env python3
"""R100: execute the actual companion SwapSpawnR100/SwapRemoveR100/SwapClearR100 and the
R94 scenery prepare id fix with explicit engine service spies (Lua 5.4 host, not retail 5.1)."""
from pathlib import Path
from lua_host_r94 import LuaHost
ROOT=Path(__file__).resolve().parents[1]
SOURCE=ROOT/'companion/Mods/kcd2vr_lab/Scripts/Startup/zzkcd2vr_lab.lua'
HARNESS=r'''
PE_RIGID=2
ENTITY_FLAG_NO_SAVE=32768
local nextId=100
entities={}
spawned={}
notes={}
noteAnswer=1
spawnFails=false
System={GetCurrTime=function()return 100 end,GetEntity=function(id)return entities[id]end,
 RemoveEntity=function(id)entities[id]=nil end,
 GetCVar=function(name)
  if name:find('kcd2vr_swap_note_r100:',1,true)==1 then notes[#notes+1]=name return noteAnswer end
  return 0 end,
 SpawnEntity=function(p)
  if spawnFails then return nil end
  nextId=nextId+1
  local e={id=nextId,class=p.class,params=p,Properties=p.properties,mass=p.properties.Physics.Mass,awakes=0}
  function e:GetMass() return self.mass end
  function e:GetSlotCount() return 1 end
  function e:IsSlotValid(s) return s==0 end
  function e:AwakePhysics(on) self.awakes=self.awakes+1 end
  function e:GetName() return p.name end
  entities[nextId]=e spawned[#spawned+1]=e
  return e end}
function spawn() return KCD2VR_Lab.SwapSpawnR100(7,'objects/props/kitchen/tankard_01.cgf',1,2,3,1,1,1,.42) end
'''
CASES={
 'spawn_not_saved_rigid_same_model':"""assert(spawn()==true);local e=spawned[1]
  assert(e.params.class=='BasicEntity' and e.params.flags==ENTITY_FLAG_NO_SAVE and e.params.name=='kcd2vr_swap_r100_7')
  assert(e.Properties.object_Model=='objects/props/kitchen/tankard_01.cgf' and e.Properties.Physics.bRigidBody==true)
  assert(e.params.position.x==1 and e.params.position.z==3 and e.params.scale.y==1 and e.Properties.Physics.Mass==.42)
  assert(#notes==1 and notes[1]=='kcd2vr_swap_note_r100:7:101' and KCD2VR_Lab.swapR100[101]==e)""",
 'no_note_answer_removes_copy':"noteAnswer=0;assert(spawn()==false);assert(entities[101]==nil and KCD2VR_Lab.swapR100[101]==nil)",
 'spawn_failure_refused':"spawnFails=true;assert(spawn()==false and #spawned==0)",
 'no_save_flag_required':"ENTITY_FLAG_NO_SAVE=nil;assert(spawn()==false and #spawned==0)",
 'bad_path_refused':"""assert(KCD2VR_Lab.SwapSpawnR100(7,"objects/a'.cgf",1,2,3,1,1,1,.4)==false)
  assert(KCD2VR_Lab.SwapSpawnR100(7,'objects/a.cga',1,2,3,1,1,1,.4)==false)
  assert(KCD2VR_Lab.SwapSpawnR100(7,'../a.cgf',1,2,3,1,1,1,.4)==false and #spawned==0)""",
 'bad_numbers_refused':"""assert(KCD2VR_Lab.SwapSpawnR100(7,'a/b.cgf',0/0,2,3,1,1,1,.4)==false)
  assert(KCD2VR_Lab.SwapSpawnR100(7,'a/b.cgf',1,2,3,1,1,1,9)==false)
  assert(KCD2VR_Lab.SwapSpawnR100(0,'a/b.cgf',1,2,3,1,1,1,.4)==false and #spawned==0)""",
 'remove_only_our_copy':"""assert(spawn());assert(KCD2VR_Lab.SwapRemoveR100(101)==true and entities[101]==nil)
  entities[55]={id=55,GetName=function()return 'guard' end};assert(KCD2VR_Lab.SwapRemoveR100(55)==false and entities[55]~=nil)""",
 'clear_after_load':"""assert(spawn());assert(KCD2VR_Lab.SwapSpawnR100(8,'a/b.cgf',1,2,3,1,1,1,.4))
  entities[102]={id=102} -- the level reloaded: id now belongs to someone else
  assert(KCD2VR_Lab.SwapClearR100());assert(entities[101]==nil and entities[102]~=nil and next(KCD2VR_Lab.swapR100)==nil)""",
 'prepare_accepts_loose_copy':"""assert(spawn());local e=spawned[1]
  assert(KCD2VR_Lab.PrepareSceneryR94(101,'BasicEntity',.42,false)==true and e.awakes==1)""",
}
def main():
 source=SOURCE.read_text();passed=0
 for name,test in CASES.items():
  try:
   with LuaHost() as lua:lua.run(HARNESS+'\n'+source+'\n'+test,name)
   passed+=1;print('PASS '+name)
  except Exception as error:print('FAIL '+name+': '+str(error))
 print(f'{passed}/{len(CASES)} R100 companion swap cases (Lua host; engine services substituted)')
 return 0 if passed==len(CASES) else 1
if __name__=='__main__':raise SystemExit(main())
