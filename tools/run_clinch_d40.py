#!/usr/bin/env python3
"""Compile actual contact producer and full native clinch adapter with bounded host IO."""
from pathlib import Path
import argparse, hashlib, json, subprocess, os
p=argparse.ArgumentParser(description=__doc__)
p.add_argument('--source',type=Path,default=Path(__file__).resolve().parents[1])
p.add_argument('--adapter-source',type=Path,help='Optional immutable older source for a behavioral negative control')
p.add_argument('--out',type=Path,required=True)
a=p.parse_args();root=a.source.resolve();adapter=(a.adapter_source or root).resolve();out=a.out.resolve();out.mkdir(parents=True,exist_ok=True)
text=(adapter/'src/vrik/native_bridges168.inl').read_text()
begin=text.index('static void CloseCombatAutoGate175(');end=text.index('static bool ArmCloseCombat175(',begin)
native=(adapter/'src/vrik/clinch_distance32.inl').read_text()+'\n'+text[begin:end]
(out/'clinch_adapter_d40_production.inc').write_text(native)
compiler=os.environ.get('CXX','cl' if os.name=='nt' else 'g++');msvc=Path(compiler).name.lower() in ('cl','cl.exe')
exe=out/('clinch_d40.exe' if os.name=='nt' else 'clinch_d40')
cmd=[compiler,'-std=c++17','-O2','-Wall','-Wextra','-Wno-misleading-indentation','-Wno-unused-variable','-Wno-unused-function','-I',str(root/'src'),'-I',str(out),str(root/'tests/clinch_intent_d40_test.cpp'),'-o',str(exe)]
if msvc:cmd=[compiler,'/nologo','/std:c++17','/EHsc','/Od','/I'+str(root/'src'),'/I'+str(out),str(root/'tests/clinch_intent_d40_test.cpp'),'/Fe'+str(exe),'/Fo'+str(out/'clinch_d40.obj')]
b=subprocess.run(cmd,capture_output=True,text=True)
receipt={'source':str(root),'adapter_source':str(adapter),'native_executed':False,'adapter_sha256':hashlib.sha256(native.encode()).hexdigest(),'build_exit':b.returncode,'build_stdout':b.stdout,'build_stderr':b.stderr}
if b.returncode==0:
 r=subprocess.run([str(exe)],capture_output=True,text=True);receipt.update(exit=r.returncode,stdout=r.stdout,stderr=r.stderr)
(out/'RESULT.json').write_text(json.dumps(receipt,indent=2)+'\n')
print(b.stderr);print(receipt.get('stdout',''));print(receipt.get('stderr',''))
raise SystemExit(0 if receipt.get('exit')==0 else 1)
