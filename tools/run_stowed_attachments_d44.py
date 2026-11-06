#!/usr/bin/env python3
"""Replay actual character resolver and full attachment tick against explicit native leaves."""
import argparse,json,subprocess
from pathlib import Path
from run_input_regressions_r38 import function
HERE=Path(__file__).resolve().parents[1]
p=argparse.ArgumentParser();p.add_argument('--source',type=Path,default=HERE);p.add_argument('--out',type=Path,required=True);a=p.parse_args();r=a.source.resolve();o=a.out.resolve();o.mkdir(parents=True,exist_ok=True)
new=r/'src/subsystems/ranged_reload/stowed_character_d44.inl'
s=new.read_text() if new.exists() else function((r/'src/vrik/physical_native.inl').read_text(),'HenryCharacter59c15')
(o/'stowed_character_actual.inc').write_text(s)
identity=(r/'src/frik_pose_probe.cpp').read_text()
(o/'stowed_identity_actual.inc').write_text(function(identity,'ObservePlayerIdentity37')+function(identity,'ResetPlayerIdentity37'))
cmd=['g++','-std=c++20','-O0','-g0','-Wall','-Wextra','-Wno-misleading-indentation','-Wno-unused-function','-Wno-unused-variable','-I'+str(r/'src'),'-I'+str(o),str(HERE/'tests/stowed_attachments_d44_test.cpp'),'-o',str(o/'stowed_attachments_d44')]
c=subprocess.run(cmd,capture_output=True,text=True);(o/'compile.log').write_text(c.stdout+c.stderr);v={'source':str(r),'command':cmd,'compile_returncode':c.returncode}
if c.returncode==0:
 x=o/'stowed_attachments_d44';x.chmod(x.stat().st_mode|0o700);q=subprocess.run([str(x),str(o/'producer_trace.log')],capture_output=True,text=True);(o/'run.log').write_text(q.stdout+q.stderr);v['run_returncode']=q.returncode;print(q.stdout,end='')
else:print(c.stderr)
(o/'results.json').write_text(json.dumps(v,indent=2)+'\n');raise SystemExit(v.get('run_returncode',c.returncode))
