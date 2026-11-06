#!/usr/bin/env python3
"""Compile current public hand-mode routing, with native hook leaves mocked."""
import argparse
import hashlib
import json
from pathlib import Path
import subprocess
import host_compiler
from test_fresh_pose_uncapped_menu import block

p=argparse.ArgumentParser()
p.add_argument('output',type=Path)
p.add_argument('--compiler',default=None)
a=p.parse_args()
r=Path(__file__).resolve().parents[1]
out=a.output.resolve();out.mkdir(parents=True,exist_ok=True)
inputs=['src/frik_pose_probe.cpp','src/subsystems/interactions/lifecycle_controls.inl',
        'src/vrik/user_modes175.h','src/vrik/hands_mode59c7f.h','src/vrik/calibration_restore43.h',
        'tests/hand_mode_ownership_r95_test.cpp']
functions=block((r/inputs[0]).read_text(),'bool ApplyUserMode175(')+'\n'+block((r/inputs[1]).read_text(),'bool SetHandsMode59c7f(')
(out/'hand_mode_functions_r95.inc').write_text(functions)
exe=host_compiler.executable_path(out/'hand_mode_ownership_r95_test')
cmd=host_compiler.command(r/inputs[-1],exe,[r/'src',out],a.compiler)
b=subprocess.run(cmd,capture_output=True,text=True)
result={'build_exit':b.returncode,'build_stdout':b.stdout,'build_stderr':b.stderr,
        'headset_tested':False,'source_sha256':{s:hashlib.sha256((r/s).read_bytes()).hexdigest() for s in inputs}}
if b.returncode==0:
    run=subprocess.run([str(exe)],capture_output=True,text=True,timeout=15)
    result.update(exit=run.returncode,stdout=run.stdout,stderr=run.stderr)
(out/'RESULT.json').write_text(json.dumps(result,indent=2)+'\n')
print(result.get('stdout',b.stdout));print(result.get('stderr',b.stderr))
raise SystemExit(0 if result.get('exit')==0 else 1)
