"""Compile literal production pickup producer/admission/final-owner boundaries."""
from pathlib import Path
import argparse,json,hashlib,subprocess
from host_compiler import command,executable_path
R=Path(__file__).resolve().parents[1]
p=argparse.ArgumentParser();p.add_argument('--out',type=Path,required=True);a=p.parse_args();out=a.out.resolve();out.mkdir(parents=True,exist_ok=True)
s=(R/'src/subsystems/vrik/tracking_producer.inl').read_text();b=s.index('    // A procedural kick');chunk=s[:b];a0=s.index('    const bool poseAllowed=');a1=s.index('    const bool headUsable=',a0)
chunk+=s[a0:a1]+'    lastAllowed=poseAllowed;\n    if(!poseAllowed||!hv||!(rv||lv))g_committed_pose_valid6689=false;\n}\n'
(out/'pickup_producer_r95_actual.inc').write_text(chunk)
s=(R/'src/subsystems/vrik/pose_frame_foundation.inl').read_text();a0=s.index('    const bool ordinaryFrame51=');a1=s.index('    // Saved FREE',a0);(out/'pickup_foundation_r95_actual.inc').write_text(s[a0:a1])
s=(R/'src/vrik/coherent_pose.inl').read_text();a0=s.index('    if(tx.adoptedVisualRequiredR95)');a1=s.index('    const auto policy=',a0);(out/'pickup_final_r95_actual.inc').write_text(s[a0:a1])
rows=[]
for mode in ['debug','optimized','old_producer']:
 d=out/mode;d.mkdir(exist_ok=True);inc=[d,out,R/'src'];exe=executable_path(d/'test')
 if mode=='old_producer':(d/'pickup_producer_r95_actual.inc').write_text(chunk.replace('||acquisitionPose55||adoptedPoseR95','||acquisitionPose55'))
 cmd=command(R/'tests/pickup_adopted_producer_r95_test.cpp',exe,inc,'cl')
 if mode=='debug':cmd=[('/Od' if x=='/O2' else x)for x in cmd]
 b=subprocess.run(cmd,cwd=d,text=True,capture_output=True);(d/'build.log').write_text(b.stdout+b.stderr);row={'test':mode,'compile_exit':b.returncode,'passed':False}
 if b.returncode==0:
  q=subprocess.run([str(exe)],text=True,capture_output=True);(d/'run.log').write_text(q.stdout+q.stderr);row.update(output=q.stdout,exit=q.returncode,passed=(q.returncode!=0 and 'FAIL producer' in q.stdout)if mode=='old_producer'else q.returncode==0)
 print(row,flush=True);rows.append(row)
(out/'RESULT.json').write_text(json.dumps({'passed':all(r['passed']for r in rows),'tests':rows,'limits':['Literal producer/admission/ordinary frame gates/final owner; native owner input reader is a leaf, separately checked by real owner fixture.','No DLL or headset result.']},indent=2))
raise SystemExit(0 if all(r['passed']for r in rows)else 1)
