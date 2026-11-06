#!/usr/bin/env python3
"""Run focused actual D40 pickup/grasp adapters and companion Lua, never the game.
Outputs binaries/logs outside source. --baseline-source adds behavioral red
checks against an immutable D39 source using temporary fixture overlays only.
"""
from pathlib import Path
import argparse, concurrent.futures, hashlib, json, os, shutil, subprocess, sys

ROOT=Path(__file__).resolve().parents[1]
TESTS=['pickup_near_d40','tool_attachment59c10','alchemy_tool_presentation_d39','grab_input_d40',
       'grab_use_producer_d40','grab_release_input_h21','grab_backend60','door_producer46','native_grasp13','grab_producer60','grip_stow_h41']

def execute(command,log,timeout=180):
 try:
  p=subprocess.run(command,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True,timeout=timeout)
  log.write_text(p.stdout);return {'command':[str(x) for x in command],'exit':p.returncode,'output':p.stdout}
 except subprocess.TimeoutExpired:
  log.write_text('TIMEOUT\n');return {'command':[str(x) for x in command],'exit':None,'output':'TIMEOUT'}

def run_one(root,out,name,cxx,shims):
 out.mkdir(parents=True,exist_ok=True);exe=out/name
 flags=['-std=c++23','-O0','-g0','-UNDEBUG','-fms-extensions','-D__fastcall=','-D_ReturnAddress=HostReturnAddress',
        '-ffunction-sections','-fdata-sections','-pthread','-I'+str(shims),'-I'+str(root/'tests/syntax_shims'),'-I'+str(root/'tests/openxr_sdk'),'-I'+str(root/'src'),'-I'+str(root/'tests')]
 result={'name':name,'native_game_executed':False}
 result['build']=execute([cxx,*flags,str(root/'tests'/f'{name}_test.cpp'),'-Wl,--gc-sections','-o',str(exe)],out/f'{name}.build.log')
 if result['build']['exit']==0:
  exe.chmod(exe.stat().st_mode|0o100)
  result['run']=execute([str(exe)],out/f'{name}.run.log',60)
 result['passed']=result.get('run',{}).get('exit')==0
 (out/f'{name}.json').write_text(json.dumps(result,indent=2)+'\n')
 print(f"{out.name} {name}: {'PASS' if result['passed'] else 'FAIL'}",flush=True)
 return result

def main():
 ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('--out',type=Path,required=True)
 ap.add_argument('--baseline-source',type=Path);ap.add_argument('--compiler',default=os.environ.get('CXX','g++'))
 ap.add_argument('--jobs',type=int,default=2);ap.add_argument('--tests',nargs='+',choices=TESTS)
 args=ap.parse_args();out=args.out.resolve()
 if out==ROOT or ROOT in out.parents:ap.error('--out must be outside source')
 out.mkdir(parents=True,exist_ok=True);shims=out/'host_shims';shims.mkdir(exist_ok=True)
 (shims/'excpt.h').write_text('#pragma once\n#define __fastcall\n#define __try try\n#define __except(x) catch(...)\n#define EXCEPTION_EXECUTE_HANDLER 1\n')
 selected=args.tests or TESTS
 with concurrent.futures.ThreadPoolExecutor(max_workers=max(1,args.jobs)) as pool:
  futures=[pool.submit(run_one,ROOT,out/'current',n,args.compiler,shims) for n in selected]
  results=[f.result() for f in futures]
 lua=execute([sys.executable,str(ROOT/'tests/test_prop_wake_d40.py'),'--out',str(out/'prop_wake_current.json')],out/'prop_wake_current.log')
 baseline=[]
 if args.baseline_source:
  base=args.baseline_source.resolve();overlay=out/'baseline_fixture_overlay';overlay.mkdir(exist_ok=True)
  for folder in ('src','tests'):
   dest=overlay/folder
   if folder=='src':
    if not dest.exists():dest.symlink_to(base/folder,target_is_directory=True)
   else:
    dest.mkdir(exist_ok=True)
    for entry in (base/folder).iterdir():
     target=dest/entry.name
     if not target.exists():target.symlink_to(entry,target_is_directory=entry.is_dir())
  # New connected public adapter fixture calls the old public interface too.
  new=overlay/'tests/pickup_near_d40_test.cpp'
  if new.is_symlink():new.unlink()
  new.write_bytes((ROOT/'tests/pickup_near_d40_test.cpp').read_bytes())
  red=run_one(overlay,out/'baseline','pickup_near_d40',args.compiler,shims)
  red['expected_behavioral_red']=red['build']['exit']==0 and red.get('run',{}).get('exit')==1 and 'FAIL close pickup' in red.get('run',{}).get('output','')
  baseline.append(red)
  oldlua=execute([sys.executable,str(ROOT/'tests/test_prop_wake_d40.py'),'--source',str(base/'companion/Mods/kcd2vr_lab/Scripts/Startup/zzkcd2vr_lab.lua'),'--out',str(out/'prop_wake_baseline.json')],out/'prop_wake_baseline.log')
  baseline.append({'name':'prop_wake_baseline','expected_behavioral_red':oldlua['exit']==1 and 'FAIL static_mass_beats_script_true' in oldlua['output'],'run':oldlua})
 changes=['src/vrik/pickup_use19c3.h','src/vrik/pickup_use_producer19c3.inl','src/vrik/tool_attachment59c10.inl',
          'src/core/native_grasp13.h','src/subsystems/physics/native_grab_input60.h','src/subsystems/physics/throw_policy60.h',
          'src/subsystems/physics/runtime/native_grab_backend60.inl','companion/Mods/kcd2vr_lab/Scripts/Startup/zzkcd2vr_lab.lua']
 report={'passed':all(r['passed'] for r in results) and lua['exit']==0 and all(r['expected_behavioral_red'] for r in baseline),
         'current':results,'lua':lua,'baseline':baseline,'production_sha256':{p:hashlib.sha256((ROOT/p).read_bytes()).hexdigest() for p in changes},
         'native_game_executed':False,'windows_dll_build':'NOT_RUN','headset':'NOT_RUN',
         'full_native_grasp_owner':'Requires Windows SEH __finally; not ported or simulated by this runner'}
 (out/'RESULTS.json').write_text(json.dumps(report,indent=2)+'\n')
 print(f"{sum(r['passed'] for r in results)}/{len(results)} portable C++ adapters; Lua={'PASS' if lua['exit']==0 else 'FAIL'}; native game/headset NOT_RUN",flush=True)
 return 0 if report['passed'] else 1
if __name__=='__main__':raise SystemExit(main())
