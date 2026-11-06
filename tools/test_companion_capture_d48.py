#!/usr/bin/env python3
"""Execute complete current companion Lua scripts against explicit engine leaves.

Requires lupa.lua51. The diagnostic locals are returned by a test-only trailing
return statement after the unmodified full script; no gate is reimplemented.
This is Lua 5.1/host validation, not retail ScriptBind or native-game execution.
"""
import argparse
import hashlib
import json
from pathlib import Path
from lupa.lua51 import LuaRuntime

ROOT = Path(__file__).resolve().parents[1]
SCRIPTS = [
    ('mods/KCD2VRMap/map_native_read_h25.lua', False),
    ('mods/KCD2VRMap/map_navigation_t2.lua', False),
    ('mods/KCD2VRMap/map_presentation_m31.lua', False),
    ('mods/KCD2VRMap/map_runtime.lua', False),
    ('mods/KCD2VRMap/map_letter_d44.lua', False),
    ('mods/KCD2VRDoors/door_runtime.lua', False),
    ('mods/KCD2VRDoors/hand_only_h40.lua', False),
    ('src/subsystems/throw_drop/resources/startup_reservation_t22.lua', True),
    ('src/subsystems/throw_drop/resources/startup_supply_t22.lua', True),
    ('src/subsystems/throw_drop/resources/inventory_delivery_t26.lua', True),
    ('companion/Mods/kcd2vr_lab/Scripts/Startup/zzkcd2vr_lab.lua', False),
    ('mods/kcd2vr_tests/zzkcd2vr_phys.lua', False),
    ('companion/source/Scripts/Startup/zzkcd2vr_chest_sheath_d33.lua', False),
]
PREAMBLE = r'''
__logs={};__toasts={};__queries={};__flags={};__reloads={};__timers=0;__clock=20
__throwGet=false;__throwLog=false
System={
 GetCVar=function(key)
  __queries[#__queries+1]=key
  if __throwGet then error('native getter unavailable') end
  return __flags[key]
 end,
 LogAlways=function(s) if __throwLog then error('sink unavailable') end __logs[#__logs+1]=s end,
 Log=function(s) __logs[#__logs+1]=s end,
 GetCurrTime=function() return __clock end,
 GetEntity=function() return nil end,
 GetEntityByName=function() return nil end,
 GetEntitiesInSphere=function() return {} end
}
Script={
 SetTimer=function() __timers=__timers+1;return __timers end,
 KillTimer=function() end,
 ReloadScript=function(name)
  __reloads[#__reloads+1]=name
  if name=='Scripts/KCD2VR/native_supply_t2.lua' or name=='Scripts/KCD2VR/native_throw_reservation_t20.lua' then
   KCD2VRThrowBootstrapVersion22=22;KCD2VRThrowReservationVersion20=20
   kcd2vr_throw_reservation_t20=function() end
  end
 end
}
UIAction={RegisterEventSystemListener=function() return true end,
 RegisterElementListener=function() return true end,CallFunction=function() return false end}
Game={SendInfoText=function(s) __toasts[#__toasts+1]=s end}
KCD2VRMap={}
'''
EXPOSE = '\nreturn {log=log,capture=captureD48,say=sayWhy,test=type(D)=="table" and D.Test}\n'
CASES = [
    ('off', None, None, False, False),
    ('f2', 'f2', 1, False, False),
    ('f3', 'f3', 1, False, False),
    ('f7', 'f7', 1, False, False),
    ('f5', 'f5', 1, False, False),
    ('f11', 'f11', 1, False, False),
    ('string_one', 'f5', '1', False, False),
    ('boolean_true', 'f5', True, False, False),
    ('getter_throws', 'f5', 1, True, False),
    ('getter_missing', 'f5', 1, False, True),
]

def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--out',type=Path,required=True)
    a=p.parse_args();a.out.mkdir(parents=True,exist_ok=True);checks=0;rows=[]
    def check(ok,label):
        nonlocal checks
        checks+=1
        if not ok:raise AssertionError(label)
    for path,reload_ok in SCRIPTS:
        source=(ROOT/path).read_text(encoding='utf-8-sig');base_effects=None;startup_seen=False
        for name,key,value,throws,missing in CASES:
            lua=LuaRuntime(unpack_returned_tuples=True);lua.execute(PREAMBLE)
            if key:lua.globals().__flags['kcd2vr_door_capture_'+key]=value
            lua.globals().__throwGet=throws
            if missing:lua.execute('System.GetCVar=nil')
            api=lua.execute(source+EXPOSE,name='@'+path)
            expected=name=='f5' or (reload_ok and name=='f11')
            n=lua.eval('#__logs');startup_seen=startup_seen or n>0
            check(expected or n==0,path+' '+name+' startup remains quiet')
            # No native operation/event registration/timer behavior changes when
            # recording changes. The engine services are explicit host leaves.
            effects=(lua.eval('#__reloads'),lua.eval('__timers'))
            if base_effects is None:base_effects=effects
            check(effects==base_effects,path+' '+name+' preserves boot behavior')
            check(api is not None and api['capture'] is not None,path+' actual complete script reached gate')
            if api['log'] is None and api['say'] is None:
                check(n==(1 if expected else 0),path+' '+name+' actual startup-only log gate')
            lua.execute('__logs={};__toasts={};__clock=__clock+2')
            if api['log'] is not None:
                api['log']('host diagnostic witness')
                check(lua.eval('#__logs')==(1 if expected else 0),path+' '+name+' actual log function')
            if api['say'] is not None:
                st=lua.table_from({'failure':'door locked'});api['say'](st)
                check(lua.eval('#__logs')==(1 if expected else 0),path+' '+name+' refusal log')
                check(lua.eval('#__toasts')==(1 if expected else 0),path+' '+name+' refusal toast')
                check(bool(st['said'])==expected,path+' '+name+' diagnostic state only when enabled')
                lua.execute('__logs={};__toasts={}')
                result=api['test'](.5)
                check(result[0] is False,path+' console test still returns native refusal')
                check(lua.eval('#__logs')==(1 if expected else 0),path+' '+name+' H41_TEST log')
            # Unavailable logger cannot turn diagnostics into a Lua exception.
            if api['log'] is not None and expected:
                lua.execute('__throwLog=true');api['log']('sink may fail')
                lua.execute('System.LogAlways=nil;System.Log=nil');api['log']('sink absent')
                lua.execute('System=nil');api['log']('engine services absent')
                check(True,path+' missing/error sink stays harmless')
        rows.append({'source':path,'sha256':hashlib.sha256((ROOT/path).read_bytes()).hexdigest(),
                     'allowed_recorders':['F5','F11'] if reload_ok else ['F5'],
                     'whole_script_executed':True,'startup_log_path_observed':startup_seen})
    result={'status':'PASS','checks':checks,'scripts':rows,'lua':'Lua 5.1',
            'native_game':'NOT_EXECUTED','Windows_SEH':'NOT_EXECUTED',
            'scope':'Full production Lua scripts; returned actual diagnostic closures; explicit timer, reload, entity, UI, log and GetCVar leaves',
            'test_sha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest()}
    (a.out/'LUA_RESULT.json').write_text(json.dumps(result,indent=2)+'\n')
    print(f'PASS companion diagnostic gates: {checks} checks, {len(rows)} whole Lua 5.1 scripts; native game NOT_EXECUTED')

if __name__=='__main__':main()
