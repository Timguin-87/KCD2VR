#!/usr/bin/env python3
"""Replay actual hand collision code with quantized Windows timing and owned surface cases."""
import argparse,hashlib,json,os,runpy,subprocess,sys
from pathlib import Path
HERE=Path(__file__).resolve().parents[1]
p=argparse.ArgumentParser();p.add_argument('--source',type=Path,default=HERE);p.add_argument('--out',type=Path,required=True);p.add_argument('--legacy',action='store_true');p.add_argument('--public',action='store_true');p.add_argument('--omit-feature-recheck',action='store_true',help='negative control: omit ONLY the D63 lost-feature requalification in extracted test code');a=p.parse_args();r=a.source.resolve();o=a.out.resolve();o.mkdir(parents=True,exist_ok=True)
original=Path.write_text
def redirected(path,text,*args,**kwargs):
 if path.parent==r/'tests' and path.name.startswith('generated_'):path=o/path.name
 return original(path,text,*args,**kwargs)
Path.write_text=redirected
try:
 runpy.run_path(str(r/'tools/extract_adapter_types.py'));runpy.run_path(str(r/'tools/extract_world_adapter_test.py'))
finally:Path.write_text=original
if a.omit_feature_recheck:
 generated=o/'generated_world_adapter.inc';code=generated.read_text()
 start=code.index('        // D63: losing the palm ray')
 end=code.index('        if(feature<0)for(int p=0;p<queried.planeCount;',start)
 code=code[:start]+code[end:];generated.write_text(code)
fixture=(HERE/'tests/world_adapter_test.cpp').read_text().replace('"../src/','"'+(r/'src').as_posix()+'/')
(o/'world_fixture.inc').write_text(fixture)
test=(HERE/'tests/contact_continuity_d63_test.cpp').read_text().replace('"world_adapter_test.cpp"','"world_fixture.inc"')
(o/'contact_continuity_d63_test.cpp').write_text(test)
cases=['contact_continuity_d63']
if a.legacy:(o/'world_adapter_d63_test.cpp').write_text(fixture);cases.append('world_adapter_d63')
if a.public:
 public=(HERE/'tests/contact_release_public_d62_test.cpp').read_text().replace('"world_adapter_test.cpp"','"world_fixture.inc"').replace('"../src/','"'+(r/'src').as_posix()+'/')
 (o/'contact_release_public_d63_test.cpp').write_text(public);cases.append('contact_release_public_d63')
records=[]
for case in cases:
 cmd=['g++','-std=c++23','-O1','-g0','-fsanitize=address,undefined','-fno-omit-frame-pointer','-no-pie','-pthread','-fms-extensions','-D__fastcall=','-I'+str(r/'src'),'-I'+str(o),'-I'+str(r/'tests/syntax_shims'),'-I'+str(r/'tests/openxr_sdk'),str(o/(case+'_test.cpp')),'-o',str(o/case)]
 c=subprocess.run(cmd,capture_output=True,text=True);(o/(case+'.compile.log')).write_text(c.stdout+c.stderr)
 v={'case':case,'command':cmd,'compile_returncode':c.returncode,'native_game':'NOT_EXECUTED'}
 if c.returncode==0:
  q=subprocess.run([str(o/case)],capture_output=True,text=True,env=dict(os.environ,ASAN_OPTIONS='detect_leaks=0',UBSAN_OPTIONS='halt_on_error=1'));(o/(case+'.run.log')).write_text(q.stdout+q.stderr);v['run_returncode']=q.returncode;v['summary']=q.stdout.splitlines()[-1:] ;v['failures']=[x for x in q.stdout.splitlines() if x.startswith('FAIL')];v['sanitizer']=q.stderr;print(case,v['summary'],v['failures'][:10])
 else:print(c.stderr[-4000:])
 records.append(v)
paths=['src/frik_pose_probe.cpp','src/vrik/hand_contact_types.inl','src/vrik/contact_release_d45.h','src/vrik/npc_contact172.h','src/subsystems/physics/pose_free_hand_contact.inl']
report={'source':str(r),'negative_control':'omit_D63_lost_feature_recheck' if a.omit_feature_recheck else None,'generated_adapter_sha256':hashlib.sha256((o/'generated_world_adapter.inc').read_bytes()).hexdigest(),'results':records,'input_sha256':{x:hashlib.sha256((r/x).read_bytes()).hexdigest() for x in paths}}
(o/'RESULTS.json').write_text(json.dumps(report,indent=2)+'\n');raise SystemExit(records[0].get('run_returncode',records[0]['compile_returncode']))
