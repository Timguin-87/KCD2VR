#!/usr/bin/env python3
"""Focused GCC host review of D50 root changes and retained D49 integrations.

Uses actual production headers/callbacks and the full animation owner TU.
Native engine IO and Windows SEH are not executed. No DLL/headset claim.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import subprocess
import sys

p=argparse.ArgumentParser(description=__doc__)
p.add_argument('--out',type=Path,required=True)
p.add_argument('--sanitize',action='store_true')
a=p.parse_args();root=Path(__file__).resolve().parents[1];out=a.out.resolve()
if out==root or root in out.parents:p.error('--out must be outside the source tree')
out.mkdir(parents=True,exist_ok=True);generated=out/'generated'
subprocess.run([sys.executable,str(root/'tools/extract_prompt_callbacks_d50.py'),'--source',str(root),'--out',str(generated)],check=True)
targets=['pickup_near_d40','prompt_family_d49','stowed_hide_hw','diagnostic_routes_d49','prompt_callbacks_d50','riding_ownership_d50']
results=[]
for name in targets:
    exe=out/name;dep=out/(name+'.d')
    cmd=[os.environ.get('CXX','g++'),'-std=c++23','-O0','-g0','-pthread','-fms-extensions','-ffunction-sections','-fdata-sections',
         '-Wno-subobject-linkage','-D_ReturnAddress=HostReturnAddress','-D__fastcall=',
         '-MMD','-MF',str(dep),'-I'+str(root/'tests/syntax_shims'),'-I'+str(root/'tests/openxr_sdk'),'-I'+str(root/'src'),'-I'+str(generated)]
    if a.sanitize:cmd+=['-fsanitize=address,undefined','-fno-sanitize-recover=all','-fno-omit-frame-pointer','-fno-pie','-no-pie']
    cmd+=[str(root/'tests'/(name+'_test.cpp')),'-Wl,--gc-sections','-o',str(exe)]
    build=subprocess.run(cmd,capture_output=True,text=True)
    (out/(name+'.build.log')).write_text(build.stdout+build.stderr)
    row={'test':name,'compile_command':cmd,'compile_exit':build.returncode}
    if build.returncode==0:
        env=dict(os.environ,ASAN_OPTIONS='detect_leaks=0',UBSAN_OPTIONS='print_stacktrace=1:halt_on_error=1')
        run=subprocess.run([str(exe)],capture_output=True,text=True,env=env)
        row.update(run_exit=run.returncode,output=run.stdout+run.stderr)
        (out/(name+'.run.log')).write_text(run.stdout+run.stderr)
    dependencies={}
    if dep.is_file():
        for token in dep.read_text().replace('\\\n',' ').split()[1:]:
            f=Path(token)
            if f.is_file() and root in f.resolve().parents:dependencies[f.resolve().relative_to(root).as_posix()]=hashlib.sha256(f.read_bytes()).hexdigest()
    row['dependencies']=dependencies;row['passed']=row.get('run_exit')==0;results.append(row)
    print(name,'PASS' if row['passed'] else 'FAIL',row.get('output',build.stderr)[-1600:],flush=True)
marker=re.search(r'#define\s+KCD2VR_BUILD_MARKER\s+"([^"]+)"',(root/'src/build_identity.h').read_text()).group(1)
receipt={'marker':marker,'tests':results,'passed':all(r['passed'] for r in results),'sanitizers':a.sanitize,
         'native_game':'NOT_RUN','Windows_SEH':'NOT_EXECUTED','headset':'NOT_RUN'}
(out/'RESULTS.json').write_text(json.dumps(receipt,indent=2)+'\n')
raise SystemExit(0 if receipt['passed'] else 1)
