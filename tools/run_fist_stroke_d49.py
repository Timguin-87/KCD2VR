#!/usr/bin/env python3
"""MSVC/portable detector comparisons; engine and headset outcomes unclaimed."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import subprocess

ROOT=Path(__file__).resolve().parents[1]
BASE='063c7455b42e111c6ed3c365d7eb6206cc968d6f'
def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--out',required=True,type=Path)
    p.add_argument('--baseline-header',type=Path,help='Original user-D48 fist_motion.h for source ZIPs without Git history')
    a=p.parse_args();out=a.out.resolve();out.mkdir(parents=True,exist_ok=True)
    baseline=a.baseline_header.read_bytes() if a.baseline_header else subprocess.check_output(['git','-C',str(ROOT),'show',BASE+':src/vrik/fist_motion.h'])
    text=baseline.decode().replace('\r\n','\n');assert text.count('namespace kcd2vr::fists {')==1
    (out/'fist_baseline_d49.h').write_text(text.replace('namespace kcd2vr::fists {','namespace kcd2vr::fistsBaselineD49 {'))
    rows=[]
    for name in ['fist_stroke_d49_test','fist_baseline_d49_test','fist_motion_test','fist_qualification_h29_test','fist_guard175_test']:
        fixture=ROOT/'tests'/(name+'.cpp');binary=out/(name+('.exe' if os.name=='nt' else ''))
        if os.name=='nt':
            vc=Path(os.environ.get('ProgramFiles','C:/Program Files'))/'Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvars64.bat'
            cmd=out/(name+'.cmd');cmd.write_text('@echo off\ncall "'+str(vc)+'" >nul\nif errorlevel 1 exit /b 1\ncl /nologo /std:c++20 /EHsc /O1 /I"'+str(ROOT/'src')+'" /I"'+str(ROOT/'src/vrik')+'" /I"'+str(out)+'" "'+str(fixture)+'" /Fe:"'+str(binary)+'" /Fo:"'+str(out/(name+'.obj'))+'"\nexit /b %errorlevel%\n')
            command=['cmd','/d','/c',str(cmd)]
        else:command=['g++','-std=c++20','-O1','-I'+str(ROOT/'src'),'-I'+str(ROOT/'src/vrik'),'-I'+str(out),str(fixture),'-o',str(binary)]
        build=subprocess.run(command,capture_output=True,text=True);(out/(name+'.compile.log')).write_text(build.stdout+build.stderr)
        row={'case':name,'compile_exit':build.returncode,'fixture_sha256':hashlib.sha256(fixture.read_bytes()).hexdigest()}
        if build.returncode==0:
            run=subprocess.run([str(binary)],capture_output=True,text=True);row['run_exit']=run.returncode;row['summary']=run.stdout.strip().splitlines()[-1] if run.stdout.strip() else ''
            (out/(name+'.log')).write_text(run.stdout+run.stderr)
        row['passed']=row.get('run_exit')==0;rows.append(row);print(('PASS ' if row['passed'] else 'FAIL ')+name+' '+row.get('summary',build.stdout+build.stderr))
    files=['src/vrik/fist_motion.h','src/vrik/fist_native.inl','src/vrik/fist_stroke_d49.h','src/diagnostics/punch_path_c54r3.h']
    result={'passed':all(r['passed'] for r in rows),'results':rows,'baseline_commit':BASE,'baseline_header_sha256':hashlib.sha256(baseline).hexdigest(),'inputs':{n:hashlib.sha256((ROOT/n).read_bytes()).hexdigest() for n in files},'native_game_execution':False,'headset_tested':False}
    (out/'RESULTS.json').write_text(json.dumps(result,indent=2)+'\n');return 0 if result['passed'] else 1
if __name__=='__main__':raise SystemExit(main())
