#!/usr/bin/env python3
"""Execute real stowed holder + entity adapters against explicit native memory leaves."""
import argparse,hashlib,json,subprocess,os
from pathlib import Path
from run_input_regressions_r38 import function
HERE=Path(__file__).resolve().parents[1]
p=argparse.ArgumentParser();p.add_argument('--source',type=Path,default=HERE);p.add_argument('--out',type=Path,required=True);a=p.parse_args();r=a.source.resolve();o=a.out.resolve();o.mkdir(parents=True,exist_ok=True)
(o/'stowed_character_actual.inc').write_text((r/'src/subsystems/ranged_reload/stowed_character_d44.inl').read_text())
s=(r/'src/frik_pose_probe.cpp').read_text();(o/'stowed_identity_actual.inc').write_text(function(s,'ObservePlayerIdentity37')+function(s,'ResetPlayerIdentity37'))
cmd=['g++','-std=c++20','-O1','-g','-fsanitize=address,undefined','-fno-omit-frame-pointer','-fno-pie','-no-pie','-Wno-misleading-indentation','-I'+str(r/'src'),'-I'+str(o),str(HERE/'tests/stowed_back_entity_d63_test.cpp'),'-o',str(o/'stowed_entities_d63')]
c=subprocess.run(cmd,capture_output=True,text=True);(o/'compile.log').write_text(c.stdout+c.stderr)
v={'source':str(r),'command':cmd,'compile_returncode':c.returncode,'native_game':'NOT_EXECUTED'}
if c.returncode==0:
 q=subprocess.run([str(o/'stowed_entities_d63')],capture_output=True,text=True,env=dict(os.environ,ASAN_OPTIONS='detect_leaks=0',UBSAN_OPTIONS='halt_on_error=1'));(o/'run.log').write_text(q.stdout+q.stderr);v['run_returncode']=q.returncode;print(q.stdout,end='')
else:print(c.stderr)
paths=['src/subsystems/ranged_reload/stowed_bow_attachment.inl','src/subsystems/ranged_reload/station_weapon_entities_d56.inl','src/subsystems/ranged_reload/stowed_hide_hw.h','src/subsystems/ranged_reload/stowed_character_d44.inl','src/frik_pose_probe.cpp']
v['input_sha256']={x:hashlib.sha256((r/x).read_bytes()).hexdigest() for x in paths}
(o/'RESULTS.json').write_text(json.dumps(v,indent=2)+'\n');raise SystemExit(v.get('run_returncode',c.returncode))
