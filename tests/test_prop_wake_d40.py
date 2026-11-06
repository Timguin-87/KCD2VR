#!/usr/bin/env python3
"""Execute the actual companion Wake Lua against explicit native service spies.
Prefer Lupa Lua5.1; Linux may use installed Lua5.4 C API (reported honestly).
No game engine, installed PAK, save or native entity is executed or changed.
"""
from pathlib import Path
import argparse
import ctypes
import ctypes.util
import json

ROOT=Path(__file__).resolve().parents[1]
HARNESS=r'''
logs={}
entity={id=7,class='BasicEntity',Properties={Physics={Mass=0,bRigidBody=false}},mass=0,awakes=0,impulses=0,physicalizations=0}
System={LogAlways=function(s) logs[#logs+1]=s end,GetCurrTime=function() return 100 end,
 GetCVar=function(key) return key=='kcd2vr_door_capture_f5' and 1 or 0 end,
 GetEntity=function(id) if id==7 then return entity end end}
PE_RIGID=2
function entity:GetWorldPos() return {x=0,y=0,z=0} end
function entity:GetMass() return self.mass end
function entity:IsRigidBody() return true end -- reproduces the misleading retail BasicEntity script observation
function entity:Physicalize(slot,kind,p) self.physicalizations=self.physicalizations+1;self.mass=p.mass end
function entity:AwakePhysics(on) self.awakes=self.awakes+1 end
function entity:AddImpulse(...) self.impulses=self.impulses+1 end
request={id=7,d={x=1,y=0,z=0},p={x=0,y=0,z=0},speed=1,hand='RIGHT'}
'''
CASES={
 'static_mass_beats_script_true':"assert(not KCD2VR_Lab.Wake(request));assert(entity.awakes==0 and entity.impulses==0 and entity.physicalizations==0)",
 'unknown_mass_refuses':"entity.GetMass=nil;entity.class='PickableItem';assert(not KCD2VR_Lab.Wake(request));assert(entity.awakes==0 and entity.impulses==0 and entity.physicalizations==0)",
 'nan_mass_refuses':"entity.mass=0/0;assert(not KCD2VR_Lab.Wake(request));assert(entity.impulses==0)",
 'rigid_void_submission':"entity.mass=.3;assert(KCD2VR_Lab.Wake(request));assert(entity.awakes==1 and entity.impulses==1 and KCD2VR_Lab.Stats().wakes==1);assert(logs[#logs]:find('motion=UNVERIFIED',1,true))",
 'explicit_impulse_failure':"entity.mass=.3;entity.AddImpulse=function(self) self.impulses=self.impulses+1;return false end;assert(not KCD2VR_Lab.Wake(request));assert(KCD2VR_Lab.Stats().wakes==0)",
 'awake_exception_stops_impulse':"entity.mass=.3;entity.AwakePhysics=function() error('native refusal') end;assert(not KCD2VR_Lab.Wake(request));assert(entity.impulses==0)",
 'physicalize_readback':"entity.class='PickableItem';assert(KCD2VR_Lab.Wake(request));assert(entity.physicalizations==1 and entity.mass==1 and entity.impulses==1 and KCD2VR_Lab.Stats().physicalized==1)",
 'physicalize_no_readback':"entity.class='PickableItem';entity.Physicalize=function(self) self.physicalizations=self.physicalizations+1 end;assert(not KCD2VR_Lab.Wake(request));assert(entity.physicalizations==1 and entity.impulses==0 and KCD2VR_Lab.Stats().physicalized==0)",
 'physicalize_explicit_failure':"entity.class='PickableItem';entity.Physicalize=function(self) self.physicalizations=self.physicalizations+1;return false end;assert(not KCD2VR_Lab.Wake(request));assert(entity.impulses==0)",
 'owner_changed_during_wake':"entity.mass=.3;entity.AwakePhysics=function(self) System.GetEntity=function() return nil end end;assert(not KCD2VR_Lab.Wake(request));assert(entity.impulses==0)",
 'quest_refused':"entity.mass=.3;entity.Properties.bQuestItem=1;assert(not KCD2VR_Lab.Wake(request));assert(entity.impulses==0)",
 'actor_refused':"entity.mass=.3;entity.actor={};assert(not KCD2VR_Lab.Wake(request));assert(entity.impulses==0)",
}

def executor():
 try:
  from lupa.lua51 import LuaRuntime
  def execute(code): LuaRuntime().execute(code)
  return 'Lua5.1 (Lupa)',execute
 except ImportError:
  name=ctypes.util.find_library('lua5.4')
  if not name: raise RuntimeError('Lua5.1 Lupa or system Lua5.4 is required')
  lua=ctypes.CDLL(name);P=ctypes.c_void_p
  lua.luaL_newstate.restype=P;lua.luaL_openlibs.argtypes=[P];lua.lua_close.argtypes=[P]
  lua.luaL_loadbufferx.argtypes=[P,ctypes.c_char_p,ctypes.c_size_t,ctypes.c_char_p,ctypes.c_char_p];lua.luaL_loadbufferx.restype=ctypes.c_int
  lua.lua_pcallk.argtypes=[P,ctypes.c_int,ctypes.c_int,ctypes.c_int,ctypes.c_ssize_t,P];lua.lua_pcallk.restype=ctypes.c_int
  lua.lua_tolstring.argtypes=[P,ctypes.c_int,ctypes.POINTER(ctypes.c_size_t)];lua.lua_tolstring.restype=P
  def execute(code):
   state=lua.luaL_newstate();lua.luaL_openlibs(state)
   try:
    data=code.encode();status=lua.luaL_loadbufferx(state,data,len(data),b'D40 actual companion Wake',None)
    if not status: status=lua.lua_pcallk(state,0,0,0,0,None)
    if status:
     size=ctypes.c_size_t();at=lua.lua_tolstring(state,-1,ctypes.byref(size))
     raise RuntimeError(ctypes.string_at(at,size.value).decode() if at else 'Lua failure')
   finally: lua.lua_close(state)
  return 'Lua5.4 (system C API; retail uses5.1, native services substituted)',execute

def main():
 ap=argparse.ArgumentParser(description=__doc__)
 ap.add_argument('--source',type=Path,default=ROOT/'companion/Mods/kcd2vr_lab/Scripts/Startup/zzkcd2vr_lab.lua')
 ap.add_argument('--out',type=Path)
 args=ap.parse_args();runtime,execute=executor();source=args.source.read_text();results=[]
 for name,test in CASES.items():
  try: execute(HARNESS+'\n'+source+'\n'+test);r={'name':name,'passed':True}
  except Exception as error:r={'name':name,'passed':False,'error':str(error)}
  results.append(r);print(('PASS ' if r['passed'] else 'FAIL ')+name)
 report={'source':str(args.source.resolve()),'runtime':runtime,'results':results,'native_game_executed':False}
 if args.out:args.out.parent.mkdir(parents=True,exist_ok=True);args.out.write_text(json.dumps(report,indent=2)+'\n')
 print(f"{sum(r['passed'] for r in results)}/{len(results)} actual companion Wake cases; {runtime}")
 return 0 if all(r['passed'] for r in results) else 1
if __name__=='__main__':raise SystemExit(main())
