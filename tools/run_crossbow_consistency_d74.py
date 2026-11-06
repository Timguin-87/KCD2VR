#!/usr/bin/env python3
"""Run actual crossbow adapter and spatial-policy fixtures without invoking game code."""
import argparse,hashlib,json,os,subprocess
from pathlib import Path
p=argparse.ArgumentParser();p.add_argument('--out',type=Path,required=True);p.add_argument('--negative',action='store_true');a=p.parse_args()
r=Path(__file__).resolve().parents[1];o=a.out.resolve();o.mkdir(parents=True,exist_ok=True)
s=(r/'src/subsystems/ranged_reload/crossbow_rifle_request_h21.inl').read_text();start=s.index('    if(source.classId==15){');end=s.index('    result.qualified=RangedOwnerXR34(o)',start);block=s[start:end]
if a.negative:block=block.replace('if(source.classId==15){','if(source.classId==15&&!o.source.instantD72){')
(o/'generated_crossbow_spatial_d74.inc').write_text(block)
for name in ['crossbow_native_xr34','crossbow_instant_d72','crossbow_crank_d66c','crossbow_consistency_d74']:
 s=(r/'tests'/(name+'_test.cpp')).read_text().replace('"../src/','"'+(r/'src').as_posix()+'/');(o/(name+'_test.cpp')).write_text(s)
cases=['crossbow_consistency_d74'] if a.negative else ['crossbow_consistency_d74','crossbow_instant_d72','crossbow_crank_d66c']
records=[]
for name in cases:
 exe=o/(name+'.exe');cmd=['cl','/nologo','/std:c++20','/EHsc','/Od','/UNDEBUG','/Gy','/Gw','/bigobj','/I'+str(r/'src'),'/I'+str(r/'tests/syntax_shims'),str(o/(name+'_test.cpp')),'/Fe:'+str(exe),'/Fo:'+str(o)+os.sep,'/link','/OPT:REF']
 c=subprocess.run(cmd,capture_output=True,text=True);(o/(name+'.compile.log')).write_text(c.stdout+c.stderr);v={'case':name,'command':cmd,'compile':c.returncode}
 if c.returncode==0:
  x=subprocess.run([str(exe)],capture_output=True,text=True);(o/(name+'.run.log')).write_text(x.stdout+x.stderr);v.update(run=x.returncode,output=(x.stdout+x.stderr)[-2500:])
 else:v['output']=(c.stdout+c.stderr)[-2500:]
 print(name,v,flush=True);records.append(v)
inputs=['src/subsystems/ranged_reload/crossbow_rifle_request_h21.inl','src/subsystems/ranged_reload/crossbow_native_xr34.h','src/subsystems/ranged_reload/crossbow_native_xr34.inl','tests/crossbow_consistency_d74_test.cpp','tests/crossbow_instant_d72_test.cpp']
(o/'RESULTS.json').write_text(json.dumps({'negative':a.negative,'cases':records,'hashes':{n:hashlib.sha256((r/n).read_bytes()).hexdigest() for n in inputs}},indent=2))
raise SystemExit(0 if all((x.get('run',0)!=0 and x['compile']==0) if a.negative else x.get('run')==0 for x in records) else 1)
