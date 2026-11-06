#!/usr/bin/env python3
"""Extract and execute actual living-contact/CVar service paths on the host.

Native memory, callback and CVar IO are explicit fixture leaves. --baseline
executes identical two adapter fixtures against the given unmodified source;
those are expected to fail the D51 suspension contract. This is not a game run.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import subprocess

ROOT=Path(__file__).resolve().parents[1]
p=argparse.ArgumentParser(description=__doc__)
p.add_argument('--source',type=Path,default=ROOT)
p.add_argument('--out',type=Path,required=True)
p.add_argument('--baseline',type=Path)
p.add_argument('--sanitize',action='store_true')
p.add_argument('--extract-only',action='store_true')
a=p.parse_args()

def extract(source,out):
    out.mkdir(parents=True,exist_ok=True)
    f=(ROOT/'tests/native_touch_h9_test.cpp').read_text()
    f=f[:f.index('int main(){')].replace('#include "../src/vrik/touch_react43.inl"','#include "vrik/touch_react43.inl"')
    (out/'native_touch_fixture_d51.inc').write_text(f)
    host=(source/'src/subsystems/physics/host.inl').read_text()
    begin=host.index('static std::atomic<bool> g_touch43')
    end=host.index('static std::atomic<bool> g_ragdoll_wanted59',begin)
    (out/'living_reaction_declarations_actual_d51.inc').write_text(host[begin:end])
    mod=(source/'src/mod.cpp').read_text()
    begin=mod.index('{RagdollConsole59 io43;')
    depth=0;end=None
    for i in range(begin,len(mod)):
        if mod[i]=='{':depth+=1
        elif mod[i]=='}':
            depth-=1
            if depth==0:end=i+1;break
    if end is None:raise RuntimeError('Missing complete native reaction service block')
    (out/'living_reaction_service_actual_d51.inc').write_text(mod[begin:end]+'\n')
    lab=(source/'src/vrik/lab58.inl').read_text()
    begin=lab.index('static void PhysicsDiagnosticsW5(')
    end=lab.index('static void Tick(',begin)
    (out/'physics_diagnostics_actual_d51.inc').write_text(lab[begin:end])
    inputs=['src/ragdoll_lease59.h','src/vrik/lab58.h','src/vrik/touch_react43.inl',
            'src/vrik/lab58.inl','src/diagnostics/capture168.h','src/subsystems/physics/host.inl',
            'src/subsystems/physics/touch_react_service60.h','src/mod.cpp']
    receipt={str(path):hashlib.sha256((source/path).read_bytes()).hexdigest() for path in inputs}
    (out/'INPUTS.json').write_text(json.dumps(receipt,indent=2)+'\n')
    return receipt

source=a.source.resolve();out=a.out.resolve()
if a.extract_only:
    extract(source,out)
    print('D51 living-reaction actual production fixtures extracted')
    raise SystemExit(0)

results=[]
variants=[('current',source,0)]
if a.baseline:variants.append(('baseline_expected_failure',a.baseline.resolve(),1))
for label,current,expected in variants:
    directory=out/label;generated=directory/'generated';inputs=extract(current,generated)
    names=['living_touch_suspension_d51','living_reaction_service_d51']
    if label=='current':names.insert(0,'living_reaction_retirement_d51')
    for name in names:
        binary=directory/name
        command=['g++','-std=c++20','-O0','-g0','-pthread','-I'+str(current/'src'),'-I'+str(generated),
                 str(ROOT/'tests'/f'{name}_test.cpp'),'-o',str(binary)]
        if a.sanitize and label=='current':command[1:1]=['-fsanitize=address,undefined','-fno-sanitize-recover=all','-fno-omit-frame-pointer','-fno-pie','-no-pie']
        compile_result=subprocess.run(command,capture_output=True,text=True)
        (directory/f'{name}.build.log').write_text(compile_result.stdout+compile_result.stderr)
        result={'source':str(current),'variant':label,'test':name,'command':command,'compile_exit':compile_result.returncode,
                'expected_run_exit':expected,'source_inputs':inputs,'native_execution':False,'headset':'NOT_RUN'}
        if compile_result.returncode==0:
            binary.chmod(binary.stat().st_mode|0o700)
            env=os.environ.copy()
            if a.sanitize and label=='current':
                # This managed host does not expose /proc/<pid>/task to LSAN.
                # Keep address/undefined checks; do not claim a leak check.
                env['ASAN_OPTIONS']='detect_leaks=0'
                result['leak_check']='NOT_RUN: managed host cannot expose /proc/<pid>/task to LeakSanitizer'
                result['sanitizer_environment']={'ASAN_OPTIONS':env['ASAN_OPTIONS']}
            run=subprocess.run([str(binary)],capture_output=True,text=True,env=env)
            (directory/f'{name}.run.log').write_text(run.stdout+run.stderr)
            result.update(run_exit=run.returncode,output=run.stdout+run.stderr,passed=run.returncode==expected)
        else:result['passed']=False
        results.append(result)
        print(f'{label}: {name}: '+('PASS' if result['passed'] else 'FAIL'))
summary={'results':results,'passed':all(x['passed'] for x in results),'sanitizers':a.sanitize,
         'scope':'Actual production contact consumers, extracted CVar service and F3-gated physics observer; native IO mocked. Expected failures are D50 negative controls, not passing product tests.'}
(out/'RESULTS.json').write_text(json.dumps(summary,indent=2)+'\n')
raise SystemExit(0 if summary['passed'] else 1)
