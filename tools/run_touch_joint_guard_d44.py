#!/usr/bin/env python3
"""Replay existing mapped/native capsule suite and actual two legacy producer blocks."""
import argparse,json,subprocess
from pathlib import Path
HERE=Path(__file__).resolve().parents[1]
p=argparse.ArgumentParser();p.add_argument('--source',type=Path,default=HERE);p.add_argument('--out',type=Path,required=True);a=p.parse_args();r=a.source.resolve();o=a.out.resolve();o.mkdir(parents=True,exist_ok=True)
f=(HERE/'tests/native_touch_h9_test.cpp').read_text().replace('#include "../src/vrik/touch_react43.inl"','#include "vrik/touch_react43.inl"');(o/'native_touch_existing.inc').write_text(f)
c=(r/'src/frik_pose_probe.cpp').read_text();start=c.index('    {static kcd2vr::impact59c6::Seen lastTouch43');end=c.index('    // F9 owns this mapped FREE-HAND',start);(o/'touch_legacy_actual.inc').write_text(c[start:end])
l=(r/'src/vrik/lab58.inl').read_text();start=l.index('namespace lab58n {');end=l.index('// ------------------------------------------------------------ C. corpse limb drag',start);(o/'lab_living_actual.inc').write_text(l[start:end]+'\n}\n')
cmd=['g++','-std=c++20','-O0','-g0','-Wall','-Wextra','-Wno-misleading-indentation','-Wno-unused-function','-Wno-unused-variable','-Wno-unused-parameter','-I'+str(r/'src'),'-I'+str(o),str(HERE/'tests/touch_joint_guard_d44_test.cpp'),'-o',str(o/'touch_joint_guard_d44')]
x=subprocess.run(cmd,capture_output=True,text=True);(o/'compile.log').write_text(x.stdout+x.stderr);v={'source':str(r),'command':cmd,'compile_returncode':x.returncode}
if x.returncode==0:
 b=o/'touch_joint_guard_d44';b.chmod(b.stat().st_mode|0o700);q=subprocess.run([str(b),str(o/'producer_trace.log')],capture_output=True,text=True);(o/'run.log').write_text(q.stdout+q.stderr);v['run_returncode']=q.returncode;print(q.stdout,end='')
else:print(x.stderr)
(o/'results.json').write_text(json.dumps(v,indent=2)+'\n');raise SystemExit(v.get('run_returncode',x.returncode))
