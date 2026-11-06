"""Run bounded production/exact-retail kick composition without touching game/source.

Native instructions are extracted from the fingerprinted local retail DLL into
the requested evidence directory. They are never embedded in source delivery.
Original context/allocator/RPG and score/output leaves are explicit host doubles;
this does not load the game or assert headset kick success.
"""
from pathlib import Path
import subprocess,json,hashlib,argparse
A=argparse.ArgumentParser();A.add_argument('--source',type=Path,default=Path(__file__).resolve().parents[1]);A.add_argument('--output',type=Path,required=True);A.add_argument('--pak',type=Path,required=True);A.add_argument('--fixture-source',type=Path);A.add_argument('--baseline',default='7c0316be437ab6d92ab7818065787dcb09e74cf3');a=A.parse_args()
S=a.source.resolve();T=(a.fixture_source or S).resolve();R=a.output.resolve();R.mkdir(parents=True,exist_ok=True)
sha=lambda b:hashlib.sha256(b).hexdigest()
subprocess.run(['C:/Users/turcs/miniconda3/python.exe',str(T/'tools/prepare_native_kick_selector_h41.py'),'--source',str(S),'--output',str(R),'--pak',str(a.pak.resolve())],check=True)
source=(S/'src/vrik/native_bridges168.inl').read_text()
def function(text):
 start=text.index('static void NativeSpecialTick168() noexcept {');end=text.index('\n}\n',start)+3;return text[start:end]
body=function(source)
parent=subprocess.run(['git','show',a.baseline+':src/vrik/native_bridges168.inl'],cwd=S,capture_output=True,check=True).stdout.decode()
parent_body=function(parent)
assert 'const bool direct17=!unarmedCLH&&' in parent_body
(R/'candidate_consumer_fixture.inl').write_text(parent_body.replace('NativeSpecialTick168()', 'NativeSpecialParentFixture()',1))
# Current host Windows shims map __try to C++ try. Cache only this unrelated
# thread-marker wrapper adaptation; no R3 body or its native calls are changed.
fist=(S/'src/vrik/fist_native.inl').read_text();old='__try {FistNativeTickBodyT22();}\n    __finally {g_fist_native_tick_active_t22=previous;}'
assert fist.count(old)==1
(R/'host_fist_native.inl').write_text(fist.replace(old,'FistNativeTickBodyT22();\n    g_fist_native_tick_active_t22=previous;'))
frik=(S/'src/frik_pose_probe.cpp').read_text();assert frik.count('#include "vrik/fist_native.inl"')==1
(R/'host_frik_pose_probe.cpp').write_text(frik.replace('#include "vrik/fist_native.inl"','#include "host_fist_native.inl"'))
retained=(S/'tests/bridges168_bridge_test.cpp').read_text();at=retained.rfind('}');retained=retained[:at]+'return 0;\n'+retained[at:]
(R/'host_retained_bridges168.cpp').write_text(retained.replace('#include "../src/frik_pose_probe.cpp"','#include "host_frik_pose_probe.cpp"'))
results=[]
for unit in ['native','production']:
 for mode in ['Debug','ASAN']:
  O=R/(unit+'_'+mode.lower());O.mkdir(exist_ok=True)
  flags='/fsanitize=address /Zi' if mode=='ASAN' else '/Od /Zi'
  if unit=='production':
   extra='/Gy /Gw /bigobj /D_ReturnAddress=HostReturnAddress /DBRIDGE_HAS_MODE_STUBS /FI"'+str(S/'tests/host_test_runtime.h')+'" /I "'+str(S/'tests/syntax_shims')+'" /I "'+str(S/'tests/openxr_sdk')+'" /I "'+str(S/'tests')+'" /I "'+str(S/'src')+'" '
   inputs='"'+str(T/'tests/native_kick_selector_production_test.cpp')+'" "'+str(S/'tests/bridge_external_stubs112.cpp')+'" "'+str(S/'src/subsystems/doors/module.cpp')+'"'
   linker='/OPT:REF /STACK:33554432 '
  else:extra='';inputs='"'+str(T/'tests/native_kick_selector_native_test.cpp')+'"';linker=''
  cmd='@echo off\ncall "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvars64.bat" >nul\ncd /d "'+str(O)+'"\ncl /nologo /std:c++latest /EHsc '+flags+' /I "'+str(R)+'" '+extra+inputs+' /Fetest.exe /link '+linker+'/INCREMENTAL:NO\nif errorlevel 1 exit /b %errorlevel%\ntest.exe "'+str(R)+'"\nexit /b %errorlevel%\n'
  (O/'build.cmd').write_text(cmd);p=subprocess.run(['cmd','/c',str(O/'build.cmd')],capture_output=True,text=True);(O/'RUN.log').write_text(p.stdout+p.stderr)
  results.append({'unit':unit,'mode':mode,'exit':p.returncode,'log':str(O/'RUN.log'),'tail':(p.stdout+p.stderr)[-6000:]});print(json.dumps(results[-1]))
  if p.returncode and ('error C' in p.stdout or 'error LNK' in p.stdout):break
 # Parent reproduction intentionally may be RED, but still run the native and
 # production fixture independently to expose the boundary rather than hide it.
inputs={str(p):sha(p.read_bytes()) for p in [T/'tests/native_kick_selector_production_test.cpp',T/'tests/native_kick_selector_native_test.cpp',T/'tools/run_native_kick_selector_h41.py',T/'tools/prepare_native_kick_selector_h41.py',R/'candidate_consumer_fixture.inl',R/'native_selection_inputs.h',R/'native_selection_ranges.bin',R/'actual_rows.bin',R/'actual_zone_mapping.bin',S/'src/vrik/native_bridges168.inl',S/'src/vrik/native_input117.inl',S/'src/frik_pose_probe.cpp']}
record={'source':str(S),'fixture_source':str(T),'source_head':subprocess.run(['git','rev-parse','HEAD'],cwd=S,capture_output=True,text=True).stdout.strip(),'tests':results,'inputs':inputs,'production_body_sha256':sha(body.encode()),'parent_body_sha256':sha(parent_body.encode()),'native_bytes_are_asan_instrumented':False,'limits':['Outer C_PlayerInput::OnAction8E5324/player4A0/AC0queue are not executed; exact name→handler registration pins are in SELECTION_INPUTS.json.','Native primary handler is covered separately by native unit; production mapped action leaf calls current NativeInputBegin/Release117 with pinned native return PCs.','Native context, allocation, RPG, scored-vector output and low-level wrapper constructor are explicit leaves; full native filter/zone/lifecycle/action-manager bodies execute.','Default collector retains all eligible rows in PAK order. It has no kick preference; actual native zone match excludes punch rows.','Private cached fist thread-marker wrapper and retained renamed-main adaptation are outside the R3 tested path.','Host tests do not prove a live animation played, damage, stamina, or headset acceptance.']}
(R/'TEST_RESULTS.json').write_text(json.dumps(record,indent=2)+'\n')
raise SystemExit(any(x['exit'] for x in results))
