#!/usr/bin/env python3
"""D41 feature/outcome routing and literal producer checks. No headset claims."""
from pathlib import Path
import argparse
import hashlib
import json
import os
import subprocess

parser=argparse.ArgumentParser()
parser.add_argument('--source',type=Path,default=Path(__file__).resolve().parents[1])
parser.add_argument('--baseline',type=Path)
parser.add_argument('--output',type=Path,required=True)
parser.add_argument('--compiler',default=os.environ.get('CXX','cl' if os.name=='nt' else 'c++'))
parser.add_argument('--sanitize',action='store_true')
args=parser.parse_args()
fixture_root=Path(__file__).resolve().parents[1]/'tests'
results=[]
for label,root in [('current',args.source)]+([('baseline',args.baseline)] if args.baseline else []):
    out=args.output/label
    out.mkdir(parents=True,exist_ok=True)
    frik=(root/'src/frik_pose_probe.cpp').read_text()
    sources=[]
    for name,start,end in [
        ('corpse_cut_d41_production.inc','    {static kcd2vr::impact59c6::Seen lastCorpse43','    // N2 (Miki: impalements)'),
        ('corpse_stab_d41_production.inc','    {static kcd2vr::impact59c6::Seen lastStabR15','    {static kcd2vr::impact59c6::Seen lastTouch43')]:
        first=frik.index(start)
        chunk=frik[first:frik.index(end,first)]
        (out/name).write_text(chunk)
        sources.append({'file':'src/frik_pose_probe.cpp','fragment':name,'sha256':hashlib.sha256(chunk.encode()).hexdigest()})
    rel='src/subsystems/masterstrike_combos/physical_finisher_gate_cb1.inl'
    text=(root/rel).read_text()
    first=text.index('static bool OpponentHeightCB1(')
    chunk=text[first:text.index('// The outer hook',first)]
    (out/'finisher_adapter_d41_production.inc').write_text(chunk)
    sources.append({'file':rel,'fragment':'finisher_adapter_d41_production.inc','sha256':hashlib.sha256(chunk.encode()).hexdigest()})
    (out/'extraction.json').write_text(json.dumps(sources,indent=2))
    for name in ['combat_finish_d41','combat_corpse_d41','combat_finisher_adapter_d41']:
        msvc=Path(args.compiler).name.lower() in ("cl","cl.exe")
        if msvc and args.sanitize:raise ValueError("The address/undefined sanitizer lane requires a GNU-compatible compiler")
        exe=out/(name+(".exe" if os.name=="nt" else ""))
        command=[args.compiler,'-std=c++17','-O1' if args.sanitize else '-O2','-I',str(root/'src'),'-I',str(out),str(fixture_root/(name+'_test.cpp')),'-o',str(exe)]
        if msvc:command=[args.compiler,"/nologo","/std:c++17","/EHsc","/Od","/I"+str(root/"src"),"/I"+str(out),str(fixture_root/(name+"_test.cpp")),"/Fe"+str(exe),"/Fo"+str(out/(name+".obj"))]
        if args.sanitize:command+=['-fsanitize=address,undefined','-fno-omit-frame-pointer','-fno-pie','-no-pie']
        build=subprocess.run(command,text=True,capture_output=True)
        (out/(name+'_build.txt')).write_text(build.stdout+build.stderr)
        env=os.environ.copy()
        # LeakSanitizer cannot inspect /proc/*/task in this managed runtime.
        # Address/undefined checks remain enabled; do not report a leak check.
        if args.sanitize:env['ASAN_OPTIONS']=env.get('ASAN_OPTIONS','')+':detect_leaks=0'
        run=subprocess.run([str(exe)],text=True,capture_output=True,env=env) if build.returncode==0 else None
        if run:(out/(name+'_run.txt')).write_text(run.stdout+run.stderr)
        result={'tree':label,'test':name,'build_exit':build.returncode,'run_exit':run.returncode if run else None,'summary':run.stdout.splitlines()[-1] if run and run.stdout else '', 'native_game':False,'sanitizers':'address,undefined; leak check disabled (managed /proc)' if args.sanitize else 'none'}
        results.append(result)
        print(json.dumps(result),flush=True)
(args.output/'RESULTS.json').write_text(json.dumps(results,indent=2))
raise SystemExit(1 if any(r['tree']=='current' and (r['build_exit'] or r['run_exit']!=0) for r in results) else 0)
