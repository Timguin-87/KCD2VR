#!/usr/bin/env python3
"""Exercise actual mortar input against immutable D40R1 and current source.

The fresh fixture is added to a temporary copy of the supplied baseline's src.
No baseline files are changed. Native facts are test
inputs; this does not run the game, a DLL, recipe events or render effects.
"""
from pathlib import Path
import argparse
import hashlib
import json
import os
import shutil
import subprocess
import tempfile

def run(compiler, source, output, sanitized=False):
    output.mkdir(parents=True,exist_ok=True)
    exe=output/('mortar_motion_d41.exe' if os.name=='nt' else 'mortar_motion_d41')
    command=[compiler,'-std=c++17','-O1','-g','-UNDEBUG','-fno-omit-frame-pointer']
    if sanitized: command+=['-fsanitize=address,undefined','-fno-sanitize-recover=all']
    command += [str(source/'tests/subsystems/alchemy/mortar_motion_d41_test.cpp'),'-o',str(exe)]
    build=subprocess.run(command,capture_output=True,text=True,timeout=180)
    (output/'build.log').write_text(build.stdout+build.stderr)
    result={'command':command,'compile_exit':build.returncode,'native_game':False,'sanitizers':sanitized}
    if build.returncode==0:
        env=dict(os.environ,ASAN_OPTIONS='detect_leaks=0',UBSAN_OPTIONS='print_stacktrace=1')
        executed=subprocess.run([str(exe)],capture_output=True,text=True,env=env,timeout=30)
        result.update(run_exit=executed.returncode,output=executed.stdout+executed.stderr)
        (output/'run.log').write_text(result['output'])
    return result

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--baseline-source',type=Path,required=True)
    parser.add_argument('--out',type=Path,required=True)
    parser.add_argument('--compiler',default=shutil.which('g++') or shutil.which('clang++'))
    args=parser.parse_args()
    if not args.compiler: parser.error('A host C++ compiler is required')
    root=Path(__file__).resolve().parents[1];base=args.baseline_source.resolve();out=args.out.resolve()
    if out==root or root in out.parents: parser.error('--out must be outside the source tree')
    files=['src/subsystems/alchemy/gesture_work67.h','src/subsystems/alchemy/input67.h','src/subsystems/alchemy/mortar_contact_repair.h']
    baseline_hashes={p:hashlib.sha256((base/p).read_bytes()).hexdigest() for p in files}
    results={}
    with tempfile.TemporaryDirectory(prefix='mortar_d41_baseline_') as tmp:
        stage=Path(tmp)
        # Copy source rather than hardlink it, so the baseline remains immutable
        # on Windows too. Only the two fixture files are added to this copy.
        shutil.copytree(base/'src',stage/'src')
        destination=stage/'tests/subsystems/alchemy';destination.mkdir(parents=True)
        shutil.copyfile(base/'tests/subsystems/alchemy/input67_test.cpp',destination/'input67_test.cpp')
        shutil.copyfile(root/'tests/subsystems/alchemy/mortar_motion_d41_test.cpp',destination/'mortar_motion_d41_test.cpp')
        results['baseline']=run(args.compiler,stage,out/'baseline')
    results['current']=run(args.compiler,root,out/'current')
    results['sanitized']=run(args.compiler,root,out/'sanitized',True)
    old=results['baseline']
    expected_red=old.get('compile_exit')==0 and old.get('run_exit')==1 and 'FAIL qualified pestle twist' in old.get('output','') and 'FAIL observed open pestle hand' in old.get('output','')
    ok=expected_red and all(results[k].get('run_exit')==0 for k in ['current','sanitized'])
    report={'status':'PASS' if ok else 'FAIL','baseline_sha256':baseline_hashes,'expected_behavioral_red':expected_red,
            'tests':results,'windows_dll':'NOT_RUN','headset':'NOT_RUN'}
    (out/'SUMMARY.json').write_text(json.dumps(report,indent=2)+'\n')
    for name,result in results.items(): print(name,result.get('compile_exit'),result.get('run_exit'),result.get('output','').strip(),flush=True)
    return 0 if ok else 1

if __name__=='__main__': raise SystemExit(main())
