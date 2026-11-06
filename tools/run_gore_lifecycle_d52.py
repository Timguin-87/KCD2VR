#!/usr/bin/env python3
"""Compile/run actual visibility policy and production cut-watch with native doubles."""
import argparse, hashlib, json, os, subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
p=argparse.ArgumentParser(description=__doc__);p.add_argument('--out',type=Path,required=True);out=p.parse_args().out.resolve();out.mkdir(parents=True,exist_ok=True)
reports={}
for name in ['gore_lifecycle_d52_test','gore_visibility59_test']:
 exe=out/(name+'.exe');vc=Path(os.environ.get('ProgramFiles','C:/Program Files'))/'Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvars64.bat'
 script=out/(name+'.cmd');script.write_text('@echo off\ncall "'+str(vc)+'" >nul\nif errorlevel 1 exit /b 1\ncl /nologo /std:c++20 /EHsc /O1 /I"'+str(ROOT/'src')+'" /I"'+str(out)+'" "'+str(ROOT/'tests'/(name+'.cpp'))+'" /Fe:"'+str(exe)+'" /Fo:"'+str(out/(name+'.obj'))+'"\nexit /b %errorlevel%\n')
 build=subprocess.run(['cmd','/d','/c',str(script)],capture_output=True,text=True);(out/(name+'.compile.log')).write_text(build.stdout+build.stderr)
 result={'compile_exit':build.returncode,'passed':False,'headset_tested':False,'native_game_execution':False}
 if build.returncode==0:
  run=subprocess.run([str(exe)],capture_output=True,text=True);(out/(name+'.run.log')).write_text(run.stdout+run.stderr);result.update(run_exit=run.returncode,summary=run.stdout.strip(),passed=run.returncode==0)
 reports[name]=result
reports['inputs']={x:hashlib.sha256((ROOT/x).read_bytes()).hexdigest() for x in ['src/subsystems/gore/visibility_policy.h','src/subsystems/gore/cut_lifecycle_d52.inl','src/subsystems/gore/limb_cut_d43.inl','src/subsystems/gore/native_gore.inl','tests/gore_lifecycle_d52_test.cpp','tests/gore_visibility59_test.cpp']}
(out/'RESULTS.json').write_text(json.dumps(reports,indent=2)+'\n');print(json.dumps(reports,indent=2))
raise SystemExit(0 if all(x['passed'] for k,x in reports.items() if k!='inputs') else 1)
