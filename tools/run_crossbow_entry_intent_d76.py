#!/usr/bin/env python3
"""Compile actual adapter/client; optional old-header causal control, never game code."""
import argparse,hashlib,json,os,subprocess,re
from pathlib import Path
p=argparse.ArgumentParser();p.add_argument('--out',type=Path,required=True);p.add_argument('--negative',type=Path);p.add_argument('--asan',action='store_true');a=p.parse_args()
r=Path(__file__).resolve().parents[1];o=a.out.resolve();o.mkdir(parents=True,exist_ok=True)
if a.negative:
 old=a.negative.read_text();base=r/'src/subsystems/ranged_reload'
 old=re.sub(r'#include "([^"]+)"',lambda m:'#include "'+(base/m.group(1)).resolve().as_posix()+'"',old)
 (o/'negative_header.h').write_text(old)
for name in ['crossbow_native_xr34_test.cpp','crossbow_entry_intent_d76_test.cpp']:
 s=(r/'tests'/name).read_text().replace('"../src/','"'+(r/'src').as_posix()+'/')
 if a.negative:s=s.replace('"'+(r/'src/subsystems/ranged_reload/crossbow_native_xr34.h').as_posix()+'"','"'+(o/'negative_header.h').as_posix()+'"')
 (o/name).write_text(s)
exe=o/'crossbow_entry_intent_d76.exe';cmd=['cl','/nologo','/std:c++20','/EHsc','/Od','/UNDEBUG','/Gy','/Gw','/bigobj','/I'+str(r/'src'),'/I'+str(r/'tests/syntax_shims'),str(o/'crossbow_entry_intent_d76_test.cpp'),'/Fe:'+str(exe),'/Fo:'+str(o)+os.sep]
if a.asan:cmd+=['/fsanitize=address','/Zi']
cmd+=['/link','/OPT:REF']
c=subprocess.run(cmd,capture_output=True,text=True);(o/'compile.log').write_text(c.stdout+c.stderr);v={'command':cmd,'compile':c.returncode,'asan':a.asan,'negative':str(a.negative) if a.negative else None}
if c.returncode==0:
 x=subprocess.run([str(exe)],capture_output=True,text=True);(o/'run.log').write_text(x.stdout+x.stderr);v.update(run=x.returncode,output=x.stdout+x.stderr)
else:v['output']=c.stdout+c.stderr
inputs=['src/subsystems/ranged_reload/crossbow_native_xr34.h','src/subsystems/ranged_reload/crossbow_native_xr34.inl','tests/crossbow_native_xr34_test.cpp','tests/crossbow_entry_intent_d76_test.cpp']
v['hashes']={n:hashlib.sha256((r/n).read_bytes()).hexdigest() for n in inputs};(o/'RESULTS.json').write_text(json.dumps(v,indent=2));print(json.dumps(v,indent=2))
raise SystemExit(0 if c.returncode==0 and (v.get('run')==1 if a.negative else v.get('run')==0) else 1)
