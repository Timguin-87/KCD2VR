"""Extract actual body/leg callsites and replay native stance on a small rigid skeleton."""
from pathlib import Path
import argparse,hashlib,json,subprocess
from host_compiler import command,executable_path
from test_crossbow_mechanism_d78 import function
ROOT=Path(__file__).resolve().parents[1]
def main():
 ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('--out',type=Path,required=True);ap.add_argument('--compiler',default='cl');args=ap.parse_args()
 out=args.out.resolve();assert out!=ROOT and ROOT not in out.parents;out.mkdir(parents=True,exist_ok=True)
 paths=['src/frik_pose_probe.cpp','src/vrik/coherent_pose.inl','src/subsystems/vrik/pose_anatomy.inl','src/subsystems/vrik/pose_fingers.inl','src/subsystems/vrik/pose_leg_solver.inl','src/subsystems/vrik/pose_follow_input.inl','src/subsystems/vrik/pose_torso_stage.inl','src/vrik/tracked_frame51.h','src/vrik/body_follow_r38.h','tests/native_crouch_body_r93_test.cpp']
 texts={p:(ROOT/p).read_text() for p in paths};frik=texts[paths[0]];coherent=texts[paths[1]];follow=texts[paths[5]];torso=texts[paths[6]]
 inc={}
 inc['native_crouch_math_r93.inc']=frik[frik.index('static V3 Add(V3'):frik.index('std::optional<DetourModKit::hook::Hook> g_setik_hook;')]+coherent[coherent.index('static pc::Vec3 PV('):coherent.index('constexpr std::size_t kPoseCapacity6689')]
 inc['native_crouch_solve_r93.inc']=function(texts[paths[2]],'static bool Compose6689(')+function(texts[paths[3]],'static void SetWorldRotation6689(')+function(texts[paths[4]],'static void SetWorldPosition128(')+function(texts[paths[4]],'static bool SolveLeg128(')
 inc['native_crouch_follow_gate_r93.inc']=follow[follow.index('const bool follow128='):follow.index('// J27: Free/6DOF')]
 inc['native_crouch_drop_r93.inc']=torso[torso.index('const double legMin='):torso.index('            if(stationTorsoJ27){',torso.index('const double legMin='))]
 a=torso.index('            const int pel=rig.pelvis;');b=torso.index('        if(tx.kickFrame176.active){',a)
 inc['native_crouch_torso_r93.inc']=torso[a:b].rsplit('        }',1)[0]
 runs=[]
 def run(name,changed=None,asan=False,negative=False):
  d=out/name;d.mkdir(exist_ok=True)
  for file,data in inc.items():(d/file).write_text(changed.get(file,data) if changed else data)
  source=d/'test.cpp';source.write_text(texts[paths[-1]])
  exe=executable_path(d/'test');cmd=command(source,exe,[d,ROOT/'src'],args.compiler)
  cmd=[('/Od' if x=='/O2' else x) for x in cmd]
  if asan:cmd.insert(1,'/fsanitize=address')
  build=subprocess.run(cmd,capture_output=True,text=True,cwd=d);(d/'build.log').write_text(build.stdout+build.stderr)
  row=dict(name=name,command=cmd,compile_rc=build.returncode,passed=False)
  if not build.returncode:
   r=subprocess.run([str(exe)],capture_output=True,text=True,cwd=d);(d/'run.log').write_text(r.stdout+r.stderr);row.update(exit=r.returncode,output=r.stdout+r.stderr,expected_failure=negative,passed=(r.returncode==1 and 'FAIL ' in r.stdout) if negative else r.returncode==0)
  runs.append(row);print(name+': '+('PASS' if row['passed'] else 'FAIL'),flush=True)
 run('current');run('current_asan',asan=True)
 torso=inc['native_crouch_torso_r93.inc'];gate=inc['native_crouch_follow_gate_r93.inc']
 old='tx.frame51.valid?VP(tx.frame51.bodyOffset):V3{}';assert torso.count(old)==1
 run('double_crouch_negative',{'native_crouch_torso_r93.inc':torso.replace(old,'tx.frame51.valid?Mul(VP(tx.frame51.bodyOffset),2.f):V3{}')},negative=True)
 run('follow_off_omits_crouch_negative',{'native_crouch_torso_r93.inc':torso.replace(old,'tx.frame51.valid&&follow128?VP(tx.frame51.bodyOffset):V3{}')},negative=True)
 old='Add(Add(An,VP(foot.current)),VP(tx.frame51.stationOffsetD53))';assert torso.count(old)==1
 run('feet_follow_crouch_negative',{'native_crouch_torso_r93.inc':torso.replace(old,'Add(Add(Add(An,VP(foot.current)),VP(tx.frame51.stationOffsetD53)),shared51)')},negative=True)
 report=dict(passed=all(r['passed'] for r in runs),runs=runs,sha256={p:hashlib.sha256((ROOT/p).read_bytes()).hexdigest() for p in paths},slice_sha256={p:hashlib.sha256(t.encode()).hexdigest() for p,t in inc.items()},limits=['Synthetic ten-joint skeleton and native stance receipt; current production math, frame, follow gate, pelvis/feet callsite, composition and leg solve execute.','No physical HMD displacement, horizontal locomotion, station/rider/kick branch, engine pose job or headset validation.'])
 (out/'RESULT.json').write_text(json.dumps(report,indent=2)+'\n');return 0 if report['passed'] else 1
if __name__=='__main__':raise SystemExit(main())
