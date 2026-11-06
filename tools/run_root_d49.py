"""Focused Windows host checks. Native IO is doubled; no headset claim."""
import argparse,json,os,subprocess,hashlib
from pathlib import Path
root=Path(__file__).resolve().parents[1]
p=argparse.ArgumentParser();p.add_argument('--out',type=Path,required=True);a=p.parse_args();out=a.out.resolve();out.mkdir(parents=True,exist_ok=True)
vc=Path('C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvars64.bat')
cases=['pickup_near_d40','prompt_family_d49','stowed_hide_hw','diagnostic_routes_d49']
rows=[]
for name in cases:
 exe=out/(name+'.exe');src=root/'tests'/(name+'_test.cpp');cmd=out/(name+'.cmd')
 cmd.write_text(f'@echo off\ncall "{vc}" >nul\nif errorlevel 1 exit /b 1\ncl /nologo /std:c++20 /EHsc /O1 /I"{root / "src"}" "{src}" /Fe:"{exe}" /Fo:"{out / (name+".obj")}"\nexit /b %errorlevel%\n')
 c=subprocess.run(['cmd','/d','/c',str(cmd)],capture_output=True,text=True);(out/(name+'.compile.log')).write_text(c.stdout+c.stderr)
 row={'name':name,'compile_exit':c.returncode,'fixture_sha256':hashlib.sha256(src.read_bytes()).hexdigest()}
 if c.returncode==0:
  r=subprocess.run([str(exe)],capture_output=True,text=True);row.update(run_exit=r.returncode,output=r.stdout+r.stderr);(out/(name+'.run.log')).write_text(r.stdout+r.stderr)
 row['passed']=row.get('run_exit')==0;rows.append(row);print(name,row['passed'],row.get('output',c.stdout+c.stderr))
(out/'RESULTS.json').write_text(json.dumps({'tests':rows,'passed':all(r['passed'] for r in rows),'native_game':False,'headset':False},indent=2)+'\n')
raise SystemExit(0 if all(r['passed'] for r in rows) else 1)
