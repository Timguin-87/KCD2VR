#!/usr/bin/env python3
"""Run four D39 regression fixtures against an immutable supplied D38 directory.

Only locally compiled host fixtures execute. The native DLL/game is never run.
A temporary copy receives fixture files plus one explicit-construction spelling
workaround for GCC; no baseline collision or action logic is changed.
"""
from pathlib import Path
import argparse
import hashlib
import json
import shutil
import subprocess
import tempfile

p=argparse.ArgumentParser(description=__doc__)
p.add_argument('--baseline-source',required=True,type=Path)
p.add_argument('--out',required=True,type=Path)
p.add_argument('--compiler',default=shutil.which('clang++') or shutil.which('g++'))
a=p.parse_args()
if not a.compiler:p.error('Host C++ compiler required')
root=Path(__file__).resolve().parents[1];base=a.baseline_source.resolve();out=a.out.resolve();out.mkdir(parents=True,exist_ok=True)
targets={
 'anvil_escape_d39':'tests/anvil_escape_d39_test.cpp',
 'retained_mortar_grip':'tests/subsystems/alchemy/mortar_geometry_repair_test.cpp',
 'alchemy_tool_presentation':'tests/alchemy_tool_presentation_d39_test.cpp',
 'opposite_hand_acquisition':'tests/subsystems/alchemy/acquisition_owner_h5_test.cpp',
}
results=[]
with tempfile.TemporaryDirectory(prefix='d39_crafting_red_') as temp:
 copy=Path(temp);shutil.copytree(base/'src',copy/'src')
 # GCC rejects the baseline's bare-brace assignment to this nested aggregate.
 # Explicit default construction is the same operation, with no logic change.
 anvil=copy/'src/subsystems/smithing/anvil_a2.h';original=anvil.read_text()
 anvil.write_text(original.replace('a={};','a=Anvil{};'))
 files=list(targets.values())+['tests/tool_attachment59c10_test.cpp']
 for relative in files:
  target=copy/relative;target.parent.mkdir(parents=True,exist_ok=True);shutil.copyfile(root/relative,target)
 target=copy/'tests/subsystems/alchemy/input67_test.cpp'
 shutil.copyfile(base/'tests/subsystems/alchemy/input67_test.cpp',target)
 target=copy/'tests/subsystems/alchemy/generated_acquisition_public_h5.inc'
 shutil.copyfile(base/'tests/subsystems/alchemy/generated_acquisition_public_h5.inc',target)
 for name,relative in targets.items():
  exe=copy/name;cmd=[a.compiler,'-std=c++17','-O1','-UNDEBUG','-I',str(copy/'src'),'-I',str(copy/'tests'),str(copy/relative),'-o',str(exe)]
  build=subprocess.run(cmd,text=True,capture_output=True,timeout=120)
  (out/(name+'.build.log')).write_text(build.stdout+build.stderr)
  run=subprocess.run([str(exe)],text=True,capture_output=True,timeout=30) if build.returncode==0 else None
  text=(run.stdout+run.stderr) if run else ''
  (out/(name+'.run.log')).write_text(text)
  result={'name':name,'compile_exit':build.returncode,'run_exit':run.returncode if run else None,'output':text,
          'expected_behavioral_red':bool(run and run.returncode==1 and 'FAIL' in text)}
  results.append(result);print(json.dumps(result),flush=True)
summary={'baseline':str(base),'baseline_anvil_sha256':hashlib.sha256((base/'src/subsystems/smithing/anvil_a2.h').read_bytes()).hexdigest(),
 'host_portability_only_transform':'bare a={} -> explicit a=Anvil{} in the temporary baseline copy; collision logic unchanged',
 'tests':results,'native_game_execution':False,'status':'PASS' if all(r['expected_behavioral_red'] for r in results) else 'FAIL'}
(out/'SUMMARY.json').write_text(json.dumps(summary,indent=2)+'\n')
raise SystemExit(0 if summary['status']=='PASS' else 1)
