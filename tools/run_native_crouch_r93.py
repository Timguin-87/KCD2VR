"""Focused native crouch policy + actual foundation and gesture stateflow checks; no DLL."""
from pathlib import Path
import argparse,subprocess,json,hashlib
from host_compiler import command,executable_path
ROOT=Path(__file__).resolve().parents[1]
def main():
 ap=argparse.ArgumentParser();ap.add_argument('--out',type=Path,required=True);ap.add_argument('--compiler',default='cl');a=ap.parse_args();out=a.out.resolve();out.mkdir(parents=True,exist_ok=True);rows=[]
 subprocess.run(['python',str(ROOT/'tools/extract_intent_frame_h8.py'),str(out/'generated')],check=True)
 for name in ['native_crouch_r93','tracked_frame51','intent_frame_h8','release_menu_r92']:
  for opt in ['debug','optimized']:
   folder=out/(name+'_'+opt);folder.mkdir(exist_ok=True);exe=executable_path(folder/'test');cmd=command(ROOT/'tests'/(name+'_test.cpp'),exe,[ROOT/'src',ROOT/'tests',ROOT/'tests/openxr_sdk',ROOT/'deps/DetourModKit/include',out/'generated'],a.compiler)
   if name=='intent_frame_h8':cmd.insert(1,'/DKCD2VR_HAS_INTENT_FRAME_H8=1')
   cmd=[('/std:c++latest' if x=='/std:c++20' else '/Od' if x=='/O2' and opt=='debug' else x) for x in cmd]
   built=subprocess.run(cmd,capture_output=True,text=True,cwd=folder);(folder/'build.log').write_text(built.stdout+built.stderr);row=dict(name=name,mode=opt,compile_exit=built.returncode,passed=False)
   if not built.returncode:
    ran=subprocess.run([str(exe)],capture_output=True,text=True,cwd=folder);(folder/'run.log').write_text(ran.stdout+ran.stderr);row.update(exit=ran.returncode,passed=ran.returncode==0,output=ran.stdout+ran.stderr)
   rows.append(row);print(name+'/'+opt+': '+('PASS' if row['passed'] else 'FAIL'),flush=True)
 paths=['src/vrik/native_crouch_r93.h','src/subsystems/vrik/native_crouch_r93.inl','src/vrik/tracked_frame51.h','src/subsystems/vrik/pose_frame_foundation.inl','src/subsystems/vrik/body_settings.inl','src/mod.cpp','src/release_menu_r92.h','src/subsystems/vrik/menu.inl','tests/native_crouch_r93_test.cpp','tests/intent_frame_h8_test.cpp']
 report=dict(passed=all(r['passed'] for r in rows),tests=rows,sha256={p:hashlib.sha256((ROOT/p).read_bytes()).hexdigest() for p in paths},limits=['Current native function/memory are synthetic leaves; the extracted current frame foundation, fist publisher, and complete physical/fist kernels run in host fixtures; native skeleton leaves are synthetic.','Retail bytes checked separately; no headset execution or injection claim.'])
 (out/'RESULT.json').write_text(json.dumps(report,indent=2)+'\n');return 0 if report['passed'] else 1
if __name__=='__main__':raise SystemExit(main())
