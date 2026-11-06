"""Focused MSVC production-header/caller checks; no game installation or native execution."""
from pathlib import Path
import argparse,hashlib,json,subprocess
root=Path(__file__).resolve().parents[1]
def function(text,signature):
 start=text.index(signature);brace=text.index('{',start);depth=1;end=brace+1
 while depth:
  depth+=(text[end]=='{')-(text[end]=='}');end+=1
 return text[start:end]+'\n'
def main():
 p=argparse.ArgumentParser();p.add_argument('--out',required=True,type=Path);p.add_argument('--asan',action='store_true');a=p.parse_args();out=a.out.resolve();out.mkdir(parents=True,exist_ok=True)
 camera=(root/'src/camera_hook.cpp').read_text()
 actual=function(camera,'static void RoomscaleXrSample47(const XrPose& pose')+function(camera,'kcd2vr::roomscale58::Drive CameraHook::RoomscaleDrive(')
 (out/'actual_roomscale_xr_d53.inc').write_text(actual)
 base='063c7455b42e111c6ed3c365d7eb6206cc968d6f'
 old=subprocess.check_output(['git','-C',str(root),'show',base+':src/vrik/fist_motion.h']).decode().replace('\r\n','\n')
 (out/'fist_baseline_d49.h').write_text(old.replace('namespace kcd2vr::fists {','namespace kcd2vr::fistsBaselineD49 {'))
 rows=[]
 names=['fist_rearm_d53_test','roomscale_heading_d53_test','hand_reference_dedup_d53_test','fist_stroke_d49_test','fist_baseline_d49_test','fist_motion_test','roomscale58_test','vrik_trace_h29_test']
 for name in names:
  fixture=root/'tests'/(name+'.cpp');binary=out/(name+'.exe');cmd=out/(name+'.cmd')
  cmd.write_text('@echo off\ncall "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvars64.bat" >nul\nif errorlevel 1 exit /b 1\ncl /nologo /std:c++20 /EHsc /UNDEBUG /Od /Zi /MD '+('/fsanitize=address ' if a.asan else '')+'/I"'+str(root/'src')+'" /I"'+str(root/'src/vrik')+'" /I"'+str(out)+'" "'+str(fixture)+'" /Fe:"'+str(binary)+'" /Fo:"'+str(out/(name+'.obj'))+'" /Fd:"'+str(out/(name+'.pdb'))+'" /link /INCREMENTAL:NO\nexit /b %errorlevel%\n')
  compile=subprocess.run(['cmd','/d','/c',str(cmd)],capture_output=True,text=True);(out/(name+'.compile.log')).write_text(compile.stdout+compile.stderr)
  r={'test':name,'compile':compile.returncode,'fixture_sha256':hashlib.sha256(fixture.read_bytes()).hexdigest()}
  if not compile.returncode:
   run_cmd=out/(name+'.run.cmd');run_cmd.write_text('@echo off\ncall "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvars64.bat" >nul\nif errorlevel 1 exit /b 1\n"'+str(binary)+'"\nexit /b %errorlevel%\n')
   run=subprocess.run(['cmd','/d','/c',str(run_cmd)],capture_output=True,text=True);(out/(name+'.run.log')).write_text(run.stdout+run.stderr);r['exit']=run.returncode;r['summary']=run.stdout.strip().splitlines()[-1] if run.stdout.strip() else ''
  rows.append(r);print(json.dumps(r),flush=True)
 inputs=['src/camera_hook.cpp','src/vrik/roomscale58.h','src/vrik/fist_motion.h','src/diagnostics/punch_path_c54r3.h','src/diagnostics/vrik_trace_h29.h']
 result={'passed':all(r.get('exit')==0 for r in rows),'asan':a.asan,'results':rows,'inputs':{s:hashlib.sha256((root/s).read_bytes()).hexdigest() for s in inputs},'baseline_commit':base,'caller_fixture_sha256':hashlib.sha256(actual.encode()).hexdigest(),'native_game_execution':False,'headset_tested':False}
 (out/'RESULTS.json').write_text(json.dumps(result,indent=2));return not result['passed']
if __name__=='__main__':raise SystemExit(main())
