from pathlib import Path
import subprocess,argparse,json,hashlib
p=argparse.ArgumentParser();p.add_argument('--source',type=Path,default=Path(__file__).resolve().parents[1]);p.add_argument('--output',type=Path,required=True);p.add_argument('--asan',action='store_true');p.add_argument('--baseline-native-no-hook',action='store_true');p.add_argument('--target',default='holster_initial_draw_h36r3_bridge');a=p.parse_args();S=a.source.resolve();O=a.output.resolve();O.mkdir(parents=True,exist_ok=True)
for tool in ['extract_holster_completion_h36.py','extract_holster_side_retirement_h36.py']:
 subprocess.run(['C:/Users/turcs/miniconda3/python.exe',str(S/'tools'/tool)],check=True)
extra='/fsanitize=address /Zi ' if a.asan else ''
if a.baseline_native_no_hook:extra+=' /DH36R3_BASELINE_HOST=1 '
cmd='@echo off\ncall "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvars64.bat" >nul\ncd /d "'+str(O)+'"\ncl /nologo /std:c++latest /EHsc /Gy /Gw /bigobj /D_ReturnAddress=HostReturnAddress /DH36R1_HAS_COMPLETION=1 /DH36R2_HAS_INITIAL_OWNER=1 /FI"'+str(S/'tests/host_test_runtime.h')+'" '+extra+' /I "'+str(S/'tests/syntax_shims')+'" /I "'+str(S/'tests/openxr_sdk')+'" /I "'+str(S/'src')+'" "'+str(S/('tests/'+a.target+'.cpp'))+'" "'+str(S/'tests/bridge_external_stubs112.cpp')+'" "'+str(S/'src/subsystems/doors/module.cpp')+'" /Fetest.exe /link /OPT:REF /INCREMENTAL:NO /STACK:33554432\nif errorlevel 1 exit /b %errorlevel%\ntest.exe\nexit /b %errorlevel%\n'
(O/'build.cmd').write_text(cmd);r=subprocess.run(['cmd','/c',str(O/'build.cmd')],capture_output=True,text=True)
files=['src/frik_pose_probe.cpp','src/subsystems/throw_drop/holster_completion_h36.inl','src/subsystems/throw_drop/holster_retirement_h36.inl','src/core/initial_draw_completion_h36r3.h','tests/holster_initial_draw_h36r3_bridge.cpp','tests/holster_completion_h36_bridge.cpp','tools/extract_holster_completion_h36.py','tests/generated_holster_completion_h36.inc']
new=S/'src/subsystems/throw_drop/initial_draw_callback_h36r3.inl'
if new.exists():files.append(str(new.relative_to(S)))
result={'source':str(S),'asan':a.asan,'target':a.target,'baselineNativeNoHook':a.baseline_native_no_hook,'exit':r.returncode,'stdout':r.stdout,'stderr':r.stderr,'sourceSHA256':{f:hashlib.sha256((S/f).read_bytes()).hexdigest() for f in files},'nativePrimitiveLimit':'Explicit host original native callbacks and actual fake-memory attachment effects. No retail native engine executed.'}
(O/'RESULT.json').write_text(json.dumps(result,indent=2),newline='\n');print(json.dumps({'exit':r.returncode,'stdout':r.stdout,'stderr':r.stderr}));raise SystemExit(r.returncode)
