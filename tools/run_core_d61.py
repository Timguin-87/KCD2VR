"""Focused Windows/ASan guard and preserved hand/holster regression checks."""
from pathlib import Path
import subprocess,json,hashlib,argparse
root=Path(__file__).resolve().parents[1]
ap=argparse.ArgumentParser();ap.add_argument('--out',type=Path,required=True);a=ap.parse_args();out=a.out.resolve();out.mkdir(parents=True,exist_ok=True)
names=['guard_parry176','fist_guard175','bow_acquisition_authority_d44','door_holster_query_d49','elbow_continuity','elbow_endpoint_b3','calibration_restore43']
results=[]
for name in names:
 source=root/'tests'/(name+'_test.cpp');exe=out/(name+'.exe')
 cmd=out/(name+'.cmd')
 cmd.write_text('@echo off\ncall "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvars64.bat" >nul\ncl /nologo /std:c++20 /EHsc /Od /Zi /UNDEBUG /fsanitize=address /Gy /Gw /bigobj /D_CRT_SECURE_NO_WARNINGS /I"'+str(root/'src')+'" /I"'+str(root/'tests/openxr_sdk')+'" "'+str(source)+'" /Fe:"'+str(exe)+'" /Fo:"'+str(out/(name+'.obj'))+'" /Fd:"'+str(out/(name+'.pdb'))+'" /link /OPT:REF\nif errorlevel 1 exit /b 1\n"'+str(exe)+'"\nexit /b %errorlevel%\n')
 run=subprocess.run(['cmd','/d','/c',str(cmd)],cwd=out,capture_output=True,text=True)
 (out/(name+'.log')).write_text(run.stdout+run.stderr)
 row={'name':name,'passed':run.returncode==0,'exit':run.returncode,'source_sha256':hashlib.sha256(source.read_bytes()).hexdigest()};results.append(row)
 print(name,'PASS' if row['passed'] else 'FAIL',(run.stdout+run.stderr)[-900:],flush=True)
report={'passed':all(x['passed'] for x in results),'kind':'MSVC_ASAN_COMPILE_LINK_EXECUTE','results':results,'headset':'NOT_RUN'}
(out/'RESULTS.json').write_text(json.dumps(report,indent=2)+'\n')
raise SystemExit(0 if report['passed'] else 1)
