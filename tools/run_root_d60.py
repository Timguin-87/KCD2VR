from pathlib import Path
import subprocess,os,json,argparse,hashlib
root=Path(__file__).resolve().parents[1]
ap=argparse.ArgumentParser();ap.add_argument('--out',type=Path,required=True);ap.add_argument('--only');a=ap.parse_args();out=a.out.resolve();out.mkdir(parents=True,exist_ok=True)
names=['pet_gesture_d59','pet_service_d59','misc_input_d59','pet_producer_d59r1','pet_withdrawal_d60','pose_cost_d60','hud_notifications_d59r1','hud_writer175_bridge','hud_policy59c7']
selected=a.only.split(',') if a.only else names
results=[]
for name in selected:
 source=root/'tests'/(name+'_test.cpp');exe=out/(name+'.exe')
 inc=[root/'src',root/'tests/openxr_sdk']
 if name=='hud_writer175_bridge': inc=[root/'tests/ui175/mocks',root/'tests/syntax_shims',*inc]
 cmd=out/(name+'.cmd');cmd.write_text('@echo off\ncall "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvars64.bat" >nul\ncl /nologo /std:c++20 /EHsc /Od /Zi /UNDEBUG /fsanitize=address /Gy /Gw /bigobj /D_CRT_SECURE_NO_WARNINGS '+ ' '.join('/I"'+str(i)+'"' for i in inc)+' "'+str(source)+'" /Fe:"'+str(exe)+'" /Fo:"'+str(out/(name+'.obj'))+'" /Fd:"'+str(out/(name+'.pdb'))+'" /link /OPT:REF\nif errorlevel 1 exit /b 1\n"'+str(exe)+'"\nexit /b %errorlevel%\n')
 run=subprocess.run(['cmd','/d','/c',str(cmd)],cwd=out,capture_output=True,text=True)
 (out/(name+'.log')).write_text(run.stdout+run.stderr)
 row={'name':name,'passed':run.returncode==0,'exit':run.returncode,'source_sha256':hashlib.sha256(source.read_bytes()).hexdigest()};results.append(row)
 print(name,'PASS' if row['passed'] else 'FAIL',(run.stdout+run.stderr)[-1600:],flush=True)
prior=out/'RESULTS.json'
if a.only and prior.exists(): results=[r for r in json.loads(prior.read_text())['results'] if r['name'] not in selected]+results
receipt={'passed':len(results)==len(names) and all(r['passed'] for r in results),'results':results,'kind':'MSVC_ASAN_COMPILE_LINK_EXECUTE','headset':'NOT_RUN'}
prior.write_text(json.dumps(receipt,indent=2)+'\n')
raise SystemExit(0 if receipt['passed'] else 1)
