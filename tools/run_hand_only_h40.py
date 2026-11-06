from pathlib import Path
import subprocess,argparse,json,hashlib,sys,zipfile

def partial_rest_lua51(source,output,lua_root,optimized):
 # Explicit optional real-Lua test mode of the existing fixture; no native ABI
 # or extra runtime fragment. The ordinary whole-frik target stays unchanged.
 sys.path.insert(0,str(source/'tools'))
 import test_hand_only_doors_h40 as h
 def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
 def quote(text):
  eq='='
  while ']'+eq+']'in text:eq+='='
  return '['+eq+'['+text+']'+eq+']'
 inputs=[source/'tests/hand_only_h40_full_bridge.cpp',source/'src/subsystems/doors/hand_only_h40.h',h.H40,h.R4,Path(h.__file__),Path(__file__)]
 before={str(p):sha(p)for p in inputs}
 with zipfile.ZipFile(h.PAK)as z:retail=z.read(h.MEMBER)
 assert hashlib.sha256(retail).hexdigest()==h.EXPECTED_SCRIPT_SHA
 for order in ('r4_first','h40_first'):
  units=[('test/native_leaves.lua',h.LEAVES),(h.MEMBER,retail.decode('utf-8-sig')),('test/native_object_leaves.lua',h.DOUBLES),('test/h40_external.lua',h.EXTERNAL)]
  for module in([h.R4,h.H40]if order=='r4_first'else[h.H40,h.R4]):units.append((str(module),module.read_text(encoding='utf-8')))
  (output/(order+'.lua')).write_text('\n'.join('assert(loadstring('+quote(text)+', '+quote('@'+name)+'))()'for name,text in units),encoding='utf-8',newline='\n')
 objects=[p for p in sorted((lua_root/'build_asan').glob('*.obj'))if p.name!='lua.obj']
 assert len(objects)==29 and(lua_root/'lua-5.1.5/src/lua.h').is_file(),'official Lua5.1.5 ASAN object closure required'
 extra='/O2 /MD'if optimized else'/Od /MD'
 command='@echo off\ncall "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvars64.bat" >nul\ncd /d "'+str(output)+'"\ncl /nologo /std:c++latest /EHsc /Zi /fsanitize=address /DKCD2VR_H40_PARTIAL_REST_LUA51=1 '+extra+' /I"'+str(lua_root/'lua-5.1.5/src')+'" "'+str(source/'tests/hand_only_h40_full_bridge.cpp')+'" '+' '.join('"'+str(p)+'"'for p in objects)+' /Fe:partial_rest.exe /sourceDependencies partial_rest.deps.json /link /INCREMENTAL:NO /STACK:33554432\nif errorlevel 1 exit /b %errorlevel%\npartial_rest.exe r4_first.lua >r4_first.log 2>&1\nset r1=%errorlevel%\npartial_rest.exe h40_first.lua >h40_first.log 2>&1\nset r2=%errorlevel%\nif not %r1%==0 exit /b %r1%\nexit /b %r2%\n'
 (output/'build.cmd').write_text(command,encoding='utf-8',newline='\r\n')
 run=subprocess.run(['cmd','/d','/c',str(output/'build.cmd')],capture_output=True,text=True)
 (output/'COMPILE.log').write_text(run.stdout+run.stderr,encoding='utf-8')
 dependencies={}
 if(output/'partial_rest.deps.json').is_file():
  d=json.loads((output/'partial_rest.deps.json').read_text(encoding='utf-8-sig'))['Data'];dependencies={p:sha(Path(p))for p in[d['Source']]+d['Includes']}
 result=dict(exit=run.returncode,source=str(source),sourceStable=before=={str(p):sha(p)for p in inputs},inputs=before,actualIncludes=dependencies,luaObjects={str(p):sha(p)for p in objects},optimized=optimized,
  results={n:(output/(n+'.log')).read_text(encoding='utf-8')if(output/(n+'.log')).exists()else'NOT_RUN'for n in('r4_first','h40_first')},retailSHA256=h.EXPECTED_SCRIPT_SHA,
  limits=['Actual C++ hand admission/service and real Lua5.1 retail AnimDoor/R4/H40. Native animation, named bone, scheduler, clock and component leaves controlled. No engine/headset acceptance.','Slow-release/rest/reacquire only; no fast-flick completion, guessed partial native terminal or new actor flags.'])
 (output/'RESULT.json').write_text(json.dumps(result,indent=2)+'\n',encoding='utf-8')
 print(json.dumps(dict(exit=result['exit'],results=result['results'],result=str(output/'RESULT.json')),indent=2));return run.returncode

p=argparse.ArgumentParser();p.add_argument('--source',type=Path,default=Path(__file__).resolve().parents[1]);p.add_argument('--output',type=Path,required=True);p.add_argument('--asan',action='store_true');p.add_argument('--target',default='hand_only_h40_full_bridge');p.add_argument('--partial-rest-lua51',type=Path,help='explicit private official Lua5.1.5 source/build_asan root');p.add_argument('--optimized',action='store_true');a=p.parse_args();S=a.source.resolve();O=a.output.resolve();O.mkdir(parents=True,exist_ok=True)
if a.partial_rest_lua51:raise SystemExit(partial_rest_lua51(S,O,a.partial_rest_lua51.resolve(),a.optimized))
for tool in ['extract_holster_completion_h36.py','extract_holster_side_retirement_h36.py']:
 subprocess.run(['C:/Users/turcs/miniconda3/python.exe',str(S/'tools'/tool)],check=True)
extra='/fsanitize=address /Zi ' if a.asan else ''
cmd='@echo off\ncall "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvars64.bat" >nul\ncd /d "'+str(O)+'"\ncl /nologo /std:c++latest /EHsc /Gy /Gw /bigobj /D_ReturnAddress=HostReturnAddress /DH36R1_HAS_COMPLETION=1 /DH36R2_HAS_INITIAL_OWNER=1 /FI"'+str(S/'tests/host_test_runtime.h')+'" '+extra+' /I "'+str(S/'tests/syntax_shims')+'" /I "'+str(S/'tests/openxr_sdk')+'" /I "'+str(S/'src')+'" "'+str(S/('tests/'+a.target+'.cpp'))+'" "'+str(S/'tests/bridge_external_stubs112.cpp')+'" "'+str(S/'src/subsystems/doors/module.cpp')+'" /Fetest.exe /link /OPT:REF /INCREMENTAL:NO /STACK:33554432\nif errorlevel 1 exit /b %errorlevel%\ntest.exe\nexit /b %errorlevel%\n'
(O/'build.cmd').write_text(cmd);r=subprocess.run(['cmd','/c',str(O/'build.cmd')],capture_output=True,text=True)
files=['src/frik_pose_probe.cpp','src/mod.cpp','src/vrik/coherent_pose.inl','src/vrik/physical_native.inl','src/vrik/pickup_use_producer19c3.inl','src/subsystems/base/pose_physical_executor.inl','src/subsystems/throw_drop/holster_gestures.inl','src/subsystems/hud_ui_controls/host.inl','tests/hand_only_h40_full_bridge.cpp','tests/holster_completion_h36_bridge.cpp','tests/holster_side_retirement_h36_bridge.cpp','tests/generated_holster_completion_h36.inc','tests/generated_holster_side_retirement_h36.inc']
files += [str(f.relative_to(S)) for f in sorted((S/'src/subsystems/doors').glob('hand_only_h40*'))]
(O/'COMPILE.log').write_text(r.stdout+r.stderr,newline='\n')
result={'source':str(S),'asan':a.asan,'target':a.target,'exit':r.returncode,'stdout':r.stdout,'stderr':r.stderr,'sourceSHA256':{f:hashlib.sha256((S/f).read_bytes()).hexdigest() for f in files},'nativePrimitiveLimit':'Explicit host original native callbacks and actual fake-memory attachment effects. No retail native engine executed.'}
(O/'RESULT.json').write_text(json.dumps(result,indent=2),newline='\n');print(json.dumps({'exit':r.returncode,'stdout':r.stdout,'stderr':r.stderr,'result':str(O/'RESULT.json')}));raise SystemExit(r.returncode)
