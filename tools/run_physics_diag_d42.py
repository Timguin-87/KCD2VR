#!/usr/bin/env python3
"""MSVC production-header diagnostics/retained physical-owner checks.
Run in a VS x64 developer environment. This never loads the game DLL.
"""
import argparse, hashlib, importlib, json, os, subprocess, sys
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
def run(command,log):
    proc=subprocess.run(command,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True,timeout=180)
    log.write_text(proc.stdout,encoding='utf-8');return {'command':command,'exit':proc.returncode}
def main():
    parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('--out',type=Path,required=True)
    args=parser.parse_args();out=args.out.resolve()
    if out==ROOT or ROOT in out.parents:parser.error('--out must be outside source')
    out.mkdir(parents=True,exist_ok=True)
    sys.path.insert(0,str(ROOT/'tools'));probe=importlib.import_module('run_grab_context_d41')
    shims=out/'extracted';shims.mkdir(exist_ok=True);wiring=probe.integration(shims)
    names=['physics_stage_d42','native_held_reason_d42','native_held_pose_t4','grab_context_d41','grab_context_wiring_d41','grab_rearm_d41']
    results=[]
    for name in names:
        exe=out/(name+'.exe');obj=out/(name+'.obj')
        cmd=['cl','/nologo','/std:c++20','/EHsc','/Od','/UNDEBUG','/DNOMINMAX','/I'+str(ROOT/'src'),'/I'+str(ROOT/'src/vrik'),
             '/I'+str(ROOT/'src/core'),'/I'+str(ROOT/'tests'),'/I'+str(ROOT/'tests/openxr_sdk'),'/I'+str(shims),
             str(ROOT/'tests'/(name+'_test.cpp')),'/Fe:'+str(exe),'/Fo:'+str(obj)]
        result={'name':name,'build':run(cmd,out/(name+'.build.log'))}
        if result['build']['exit']==0:result['run']=run([str(exe)],out/(name+'.run.log'))
        result['passed']=result.get('run',{}).get('exit')==0;results.append(result)
        print(name+': '+('PASS' if result['passed'] else 'FAIL'),flush=True)
    paths=['src/diagnostics/physics_stage_d42.h','src/vrik/native_held_pose_t4.h','src/subsystems/physics/pose_held_solver_stage.inl',
           'src/subsystems/physics/runtime/native_grab_producer60.h','src/subsystems/physics/runtime/native_grab_producer60.inl',
           'src/subsystems/physics/runtime/native_grab_backend60.inl']
    report={'passed':wiring['passed'] and all(x['passed'] for x in results),'tests':results,'grab_integration':wiring,
            'source_sha256':{p:hashlib.sha256((ROOT/p).read_bytes()).hexdigest() for p in paths},'headset':'NOT_RUN','native_game':'NOT_RUN'}
    (out/'RESULTS.json').write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8')
    return 0 if report['passed'] else 1
if __name__=='__main__':raise SystemExit(main())
