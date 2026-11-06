from pathlib import Path
import argparse,subprocess,json,hashlib
ap=argparse.ArgumentParser();ap.add_argument('--source',type=Path,default=Path(__file__).resolve().parents[1]);ap.add_argument('--output',type=Path,required=True);ap.add_argument('--baseline',default='');ap.add_argument('--asan',action='store_true');ap.add_argument('--optimized',action='store_true');a=ap.parse_args();S=a.source.resolve();O=a.output.resolve();O.mkdir(parents=True,exist_ok=True);actual=S
files=['src/subsystems/ranged_reload/crossbow_native_xr34.inl','src/action_time_owner59c11.cpp']
if a.baseline:
 actual=O/'baseline';actual.mkdir(exist_ok=True)
 for f in files:
  p=actual/f;p.parent.mkdir(parents=True,exist_ok=True);p.write_bytes(subprocess.check_output(['git','-C',str(S),'show',a.baseline+':'+f]))
subprocess.run(['C:/Users/turcs/miniconda3/python.exe',str(S/'tools/extract_reload_callback_counter_xr34.py'),'--source',str(actual),'--output',str(O)],check=True)
(O/'test.cpp').write_bytes((S/'tests/reload_callback_counter_xr34_test.cpp').read_bytes())
flags=('/fsanitize=address /Zi ' if a.asan else '/Zi ')+('/O2 ' if a.optimized else '/Od ')
cmd='@echo off\ncall "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvars64.bat" >nul\ncd /d "'+str(O)+'"\ncl /nologo /std:c++latest /EHsc '+flags+'test.cpp /Fetest.exe\nif errorlevel 1 exit /b %errorlevel%\ntest.exe\nexit /b %errorlevel%\n'
(O/'build.cmd').write_text(cmd);r=subprocess.run(['cmd','/c',str(O/'build.cmd')],capture_output=True,text=True);(O/'RUN.log').write_text(r.stdout+r.stderr,newline='\n')
result={'source':str(S),'baseline':a.baseline or None,'sourceSHA256':{f:hashlib.sha256((actual/f).read_bytes()).hexdigest() for f in files},'fixtureSHA256':hashlib.sha256((O/'test.cpp').read_bytes()).hexdigest(),'generatedSHA256':hashlib.sha256((O/'reload_callback_counter_xr34_actual.inc').read_bytes()).hexdigest(),'exit':r.returncode,'stdout':r.stdout,'stderr':r.stderr,'actualWindowsSDK':True,'EH':'/EHsc','asan':a.asan,'optimized':a.optimized,'nativeEngineExecution':False,'scope':'Actual3nativecallback wrappers and actual OwnerTransaction extracted; original and post calls explicit host leaves. Real Windows RaiseException caught outside entries.'}
(O/'RESULT.json').write_text(json.dumps(result,indent=2),newline='\n');print(json.dumps(result));raise SystemExit(r.returncode)
