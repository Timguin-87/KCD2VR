#!/usr/bin/env python3
"""Build and execute targeted D48 integration fixtures, retaining exact-source receipts.

Host tests only. Windows SEH, native game and headset behavior are not executed.
Pass --lua to additionally verify the exact C++/Lua door-target wire with lupa.lua51.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import subprocess
import sys

p=argparse.ArgumentParser(description=__doc__)
p.add_argument('--out',type=Path,required=True)
p.add_argument('--lua',action='store_true')
a=p.parse_args();root=Path(__file__).resolve().parents[1];out=a.out.resolve()
if out==root or root in out.parents:p.error('--out must be outside the source tree')
out.mkdir(parents=True,exist_ok=True);compiler=os.environ.get('CXX','g++')
targets=[('diagnostic_policy','d48_diagnostic_policy_test.cpp',[]),
         ('interaction_intent','d48_interaction_intent_test.cpp',[]),
         ('manual_arrow','manual_arrow64_test.cpp',['-pthread','-D__try=try','-D__except(x)=catch(...)']),
         ('frame_summary','frame_summary_d30_test.cpp',[]),
         ('door_packet','d48_door_packet_test.cpp',[])]
results=[]
for name,test,extra in targets:
    exe=out/name;dep=out/(name+'.d')
    cmd=[compiler,'-std=c++20','-O2','-MMD','-MF',str(dep),'-I',str(root/'src'),*extra,str(root/'tests'/test),'-o',str(exe)]
    build=subprocess.run(cmd,capture_output=True,text=True)
    (out/(name+'.build.log')).write_text(build.stdout+build.stderr)
    row={'target':name,'command':cmd,'build_exit':build.returncode}
    if build.returncode==0:
        run=subprocess.run([str(exe)],capture_output=True,text=True)
        row.update(exit=run.returncode,stdout=run.stdout,stderr=run.stderr)
        (out/(name+'.stdout')).write_text(run.stdout)
    dependencies={}
    if dep.is_file():
        for token in dep.read_text().replace('\\\n',' ').split()[1:]:
            f=Path(token)
            if f.is_file() and root in f.resolve().parents:
                dependencies[f.resolve().relative_to(root).as_posix()]=hashlib.sha256(f.read_bytes()).hexdigest()
    row['actual_dependency_sha256']=dependencies;results.append(row)
    print(name, 'PASS' if row.get('exit')==0 else 'FAIL',row.get('stdout',build.stderr)[-1400:])

for name,cmd in [('live_menu',[sys.executable,str(root/'tools/run_live_runtime_d40.py'),'--out',str(out/'live_menu')])]+(
    [('door_lua',[sys.executable,str(root/'tools/test_door_target_d48.py'),'--packet-output',str(out/'door_packet.stdout'),'--out',str(out/'door_lua')])] if a.lua else []):
    run=subprocess.run(cmd,capture_output=True,text=True);results.append({'target':name,'command':cmd,'exit':run.returncode,'stdout':run.stdout,'stderr':run.stderr})
    (out/(name+'.log')).write_text(run.stdout+run.stderr)
    print(name,'PASS' if run.returncode==0 else 'FAIL',run.stdout[-1400:],run.stderr[-1400:])
receipt={'build_marker':'KCD2VR-R28-PLUS-D48-20261029','native_game':'NOT_RUN','Windows_SEH':'NOT_EXECUTED','headset':'NOT_RUN','targets':results}
receipt['status']='PASS' if all(r.get('exit')==0 for r in results) else 'FAIL'
(out/'RESULT.json').write_text(json.dumps(receipt,indent=2)+'\n')
raise SystemExit(0 if receipt['status']=='PASS' else 1)
