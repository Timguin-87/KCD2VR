#!/usr/bin/env python3
"""Focused D55 station host fixtures; native memory/rendering are explicit leaves.
Run from a VS x64 developer environment. No game calls or DLL/headset claim.
"""
from pathlib import Path
import argparse, concurrent.futures, hashlib, json, os, re, subprocess, sys
ROOT=Path(__file__).resolve().parents[1]
TESTS=[
 'alchemy_book_grip_d53','alchemy_mortar_handoff_d53','alchemy_pestle_presentation_d53',
 'alchemy_tool_contact_d53','crafting_input_d53','station_reposition_d53',
 'alchemy_tool_presentation_d39','alchemy_pestle_park_d47','tool_attachment59c10',
 'subsystems/alchemy/input67','subsystems/alchemy/mortar_pour70','subsystems/alchemy/mortar_retry72',
 'subsystems/alchemy/native_return67','subsystems/alchemy/physical_release_repair',
 'subsystems/alchemy/bridge67','subsystems/alchemy/near_selection67',
 'tracked_frame51','station_ws_j27','station_fallback_r9c']
def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--out',type=Path,required=True)
 p.add_argument('--asan',action='store_true');p.add_argument('--tests',nargs='+',choices=TESTS);p.add_argument('--jobs',type=int,default=2)
 a=p.parse_args();a.out.mkdir(parents=True,exist_ok=True)
 def run(name):
  label=name.replace('/','_');source=ROOT/'tests'/(name+'_test.cpp');dest=a.out/label;dest.mkdir(exist_ok=True)
  cmd=['cl','/nologo','/EHsc','/std:c++20','/O1','/Gy','/I'+str(ROOT/'src'),'/I'+str(ROOT/'tests'),str(source),'/Fe:'+str(dest/'test.exe'),'/Fo:'+str(dest/'test.obj')]
  if a.asan:cmd+=['/fsanitize=address','/Zi','/Fd:'+str(dest/'test.pdb')]
  cmd+=['/link','/OPT:REF']
  if a.asan:cmd+=['/DEBUG']
  r={'name':name,'command':cmd,'native_game_execution':False,'asan':a.asan}
  try:
   r['source_sha256']=hashlib.sha256(source.read_bytes()).hexdigest()
   c=subprocess.run(cmd,cwd=ROOT,text=True,capture_output=True,timeout=180);r['compile_exit']=c.returncode
   (dest/'compile.log').write_text(c.stdout+c.stderr,encoding='utf-8')
   if c.returncode==0:
    e=subprocess.run([str(dest/'test.exe')],cwd=ROOT,text=True,capture_output=True,timeout=60);r.update(run_exit=e.returncode,output=e.stdout+e.stderr)
    (dest/'run.log').write_text(e.stdout+e.stderr,encoding='utf-8')
   r['status']='PASS' if r.get('run_exit')==0 else 'FAIL'
  except (OSError,subprocess.TimeoutExpired) as e:r.update(status='ERROR',error=str(e))
  print(name,r['status'],flush=True);return r
 with concurrent.futures.ThreadPoolExecutor(max_workers=a.jobs) as pool:results=list(pool.map(run,a.tests or TESTS))
 summary={'scope':'Actual production headers/adapters with explicit native memory/render leaves; no DLL/headset claim','results':results,'passed':sum(r['status']=='PASS' for r in results),'total':len(results)}
 (a.out/'RESULTS.json').write_text(json.dumps(summary,indent=2)+'\n',encoding='utf-8')
 return 0 if summary['passed']==summary['total'] else 1
if __name__=='__main__':raise SystemExit(main())
