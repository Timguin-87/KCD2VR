"""Focused source-only mounted classification, lifecycle and shared-frame tests."""
from pathlib import Path
import argparse,json,subprocess,sys
from host_compiler import command,executable_path
root=Path(__file__).resolve().parents[1]
p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);p.add_argument('--compiler',default='cl');a=p.parse_args()
a.output.mkdir(parents=True,exist_ok=True);results=[]
for name in ['riding_pose_d80','riding_stationary_r92','riding_seated_r95','tracked_reference_d86','tracked_frame51','body_height_r98']:
 exe=executable_path(a.output/name)
 cmd=command(root/'tests'/(name+'_test.cpp'),exe,[root/'src',root/'tests/syntax_shims',root/'tests/openxr_sdk'],a.compiler)
 if Path(a.compiler).name.lower() in ('cl','cl.exe'):
  cmd+=['/Gy','/bigobj','/D_ReturnAddress=HostReturnAddress','/link','/OPT:REF']
 run=subprocess.run(cmd,capture_output=True,text=True);(a.output/(name+'.build.log')).write_text(run.stdout+run.stderr)
 row={'test':name,'compile':run.returncode,'passed':False}
 if run.returncode==0:
  run=subprocess.run([str(exe)],capture_output=True,text=True);(a.output/(name+'.run.log')).write_text(run.stdout+run.stderr)
  row.update(run=run.returncode,passed=run.returncode==0,output=run.stdout)
 results.append(row);print(name,'PASS' if row['passed'] else 'FAIL',flush=True)
(a.output/'results.json').write_text(json.dumps({'headset':'NOT_RUN','game_build':'NOT_RUN','results':results},indent=2)+'\n')
sys.exit(0 if all(r['passed'] for r in results) else 1)
