#!/usr/bin/env python3
"""Compile actual D42 service and extract existing production camera lease with explicit native IO leaves."""
import argparse,hashlib,json,os,subprocess
from pathlib import Path
p=argparse.ArgumentParser(description=__doc__);p.add_argument('--out',type=Path,required=True);a=p.parse_args()
root=Path(__file__).resolve().parents[1];out=a.out.resolve()
if out==root or root in out.parents:p.error('output must be outside source')
out.mkdir(parents=True,exist_ok=True)
mod=(root/'src/mod.cpp').read_text();start=mod.index('struct CombatCameraLease50 {');end=mod.index('static CombatCameraLease50 g_combat_camera50;',start)+len('static CombatCameraLease50 g_combat_camera50;')
(out/'camera_lease50_production.inc').write_text(mod[start:end]+'\n')
assert mod.count('#include "subsystems/experimental/free_targeting_service_d42.inl"')==1,'service not integrated exactly once'
cxx=os.environ.get('CXX','cl' if os.name=='nt' else 'g++');msvc=Path(cxx).name.lower() in ('cl','cl.exe')
exe=out/('service.exe' if os.name=='nt' else 'service')
args=([cxx,'/nologo','/std:c++17','/EHsc','/Od','/I'+str(root/'src'),'/I'+str(out),str(root/'tests/free_targeting_service_d42_test.cpp'),'/Fe'+str(exe),'/Fo'+str(out/'service.obj')] if msvc else [cxx,'-std=c++17','-O2','-Wall','-Wextra','-I'+str(root/'src'),'-I'+str(out),str(root/'tests/free_targeting_service_d42_test.cpp'),'-o',str(exe)])
b=subprocess.run(args,capture_output=True,text=True);(out/'build.log').write_text(b.stdout+b.stderr)
files=['src/mod.cpp','src/subsystems/experimental/free_targeting_service_d42.inl','src/subsystems/experimental/free_targeting_d42.h','src/subsystems/masterstrike_combos/finisher_diagnostics_d42.h','src/diagnostics/capture168.h','tests/free_targeting_service_d42_test.cpp']
receipt={'native_execution':False,'headset':False,'build_exit':b.returncode,'command':args,'inputs':{n:hashlib.sha256((root/n).read_bytes()).hexdigest() for n in files}}
if b.returncode==0:
 r=subprocess.run([str(exe)],capture_output=True,text=True);(out/'run.log').write_text(r.stdout+r.stderr);receipt.update(run_exit=r.returncode,summary=r.stdout);print(r.stdout)
else:print(b.stdout+b.stderr)
(out/'RESULT.json').write_text(json.dumps(receipt,indent=2)+'\n');raise SystemExit(0 if receipt.get('run_exit')==0 else 1)
