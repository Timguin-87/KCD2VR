#!/usr/bin/env python3
"""Test actual native query/sweep/weapon code, with bounded explicit native leaves."""
import argparse,hashlib,json,os,runpy,subprocess
from pathlib import Path
HERE=Path(__file__).resolve().parents[1]
p=argparse.ArgumentParser();p.add_argument('--source',type=Path,default=HERE);p.add_argument('--out',type=Path,required=True)
a=p.parse_args();r=a.source.resolve();o=a.out.resolve();o.mkdir(parents=True,exist_ok=True)
original=Path.write_text
def redirected(path,text,*args,**kwargs):
    if path.parent==r/'tests' and path.name.startswith('generated_'):path=o/path.name
    return original(path,text,*args,**kwargs)
Path.write_text=redirected
try:
    runpy.run_path(str(r/'tools/extract_adapter_types.py'))
    runpy.run_path(str(r/'tools/extract_world_adapter_test.py'))
finally:Path.write_text=original
fixture=(HERE/'tests/world_adapter_test.cpp').read_text().replace('"../src/','"'+(r/'src').as_posix()+'/')
needle='static int MockRay('
assert fixture.count(needle)==1
fixture=fixture.replace(needle,'static int NativeExitLeafD63(const V3&,const V3&,void*);\n'+needle,1)
needle=' if(replyH25==1)throw'
assert fixture.count(needle)==1
fixture=fixture.replace(needle,' if(mockMode==63)return NativeExitLeafD63(*origin,*sweep,hit);\n'+needle,1)
(o/'world_exit_fixture.inc').write_text(fixture)
source=(r/'src/vrik/physical_native.inl').read_text()
start=source.index('static ph::Contact103 PhysSweep6698(')
end=source.index('\n#include "../subsystems/physics/shield_primary_b4.inl"',start)
helper=r/'src/vrik/weapon_exit_d63.h'
(o/'physical_sweep_exit_actual.inc').write_text(('#include "'+helper.as_posix()+'"\n' if helper.exists() else '')+source[start:end])
cmd=['g++','-std=c++23','-O1','-g0','-fsanitize=address,undefined','-fno-omit-frame-pointer','-no-pie','-pthread','-fms-extensions','-D__fastcall=',
     '-I'+str(r/'src'),'-I'+str(o),'-I'+str(r/'tests/syntax_shims'),'-I'+str(r/'tests/openxr_sdk'),
     str(HERE/'tests/weapon_exit_d63_test.cpp'),'-o',str(o/'weapon_exit_d63')]
compiled=subprocess.run(cmd,capture_output=True,text=True);(o/'compile.log').write_text(compiled.stdout+compiled.stderr)
report={'source':str(r),'command':cmd,'compile_returncode':compiled.returncode,'native_game':'NOT_EXECUTED'}
if compiled.returncode==0:
    q=subprocess.run([str(o/'weapon_exit_d63')],capture_output=True,text=True,env=dict(os.environ,ASAN_OPTIONS='detect_leaks=0',UBSAN_OPTIONS='halt_on_error=1'))
    (o/'run.log').write_text(q.stdout+q.stderr);report.update(run_returncode=q.returncode,summary=q.stdout.splitlines()[-1:],failures=[x for x in q.stdout.splitlines() if x.startswith('FAIL')],sanitizer=q.stderr)
    print(report['summary'],report['failures'])
else:print(compiled.stderr[-8000:])
paths=['src/frik_pose_probe.cpp','src/vrik/physical_native.inl','src/vrik/physical_weapon.h','src/vrik/weapon_sweep112.h']
if helper.exists():paths.append('src/vrik/weapon_exit_d63.h')
report['source_sha256']={name:hashlib.sha256((r/name).read_bytes()).hexdigest() for name in paths}
report['fixture_sha256']={name:hashlib.sha256((o/name).read_bytes()).hexdigest() for name in ['generated_world_adapter.inc','physical_sweep_exit_actual.inc','world_exit_fixture.inc']}
report['test_sha256']=hashlib.sha256((HERE/'tests/weapon_exit_d63_test.cpp').read_bytes()).hexdigest()
(o/'RESULTS.json').write_text(json.dumps(report,indent=2)+'\n')
raise SystemExit(report.get('run_returncode',compiled.returncode))
