"""MSVC actual GP/stow and recipe-observer bodies; engine/native leaves modeled.
No game execution, hook installation or inventory delivery is claimed.
"""
from pathlib import Path
import argparse,subprocess,hashlib,json
ROOT=Path(__file__).resolve().parents[1]
VC=Path(r'C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvars64.bat')
DMK=Path(r'E:/backup/DetourModKit')
def main():
 p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);p.add_argument('--asan',action='store_true');a=p.parse_args();a.output.mkdir(parents=True,exist_ok=True)
 receipts=[]
 for target in ('grip_stow_h41','alchemy_finish_h41','native_item_context_h41','grab_producer60'):
  work=a.output/target;work.mkdir(exist_ok=True);test=ROOT/'tests'/(target+'_test.cpp')
  flags='/fsanitize=address' if a.asan else ''
  libs=''
  if target in ('alchemy_finish_h41','native_item_context_h41'):
   # Prebuilt retail DMK is not STL-container-annotated. Match its ABI while
   # retaining address/stack/heap instrumentation in this actual source body.
   flags+=' /MD /Gy /D_DISABLE_STRING_ANNOTATION /D_DISABLE_VECTOR_ANNOTATION /I "'+str(DMK/'include')+'"'
   libs=' /OPT:REF '+' '.join('"'+str(DMK/'lib'/x)+'"' for x in ('libDetourModKit.lib','safetyhook.lib','Zydis.lib','Zycore.lib'))+' bcrypt.lib User32.lib Shell32.lib Ole32.lib Advapi32.lib Version.lib Dbghelp.lib'
  command='cl /nologo /std:c++latest /EHsc /Od /Zi '+flags+' /I "'+str(ROOT/'src')+'" "'+str(test)+'" /Fe:test.exe /link /INCREMENTAL:NO'+libs+' >compile.log 2>&1'
  script='@echo off\ncall "'+str(VC)+'" >nul\n'+command+'\nif errorlevel 1 exit /b 1\ntest.exe >run.log 2>&1\nexit /b %errorlevel%\n'
  (work/'build.cmd').write_text(script);run=subprocess.run(['cmd.exe','/c','build.cmd'],cwd=work)
  if run.returncode:raise SystemExit('FAILED '+target+' see '+str(work))
  output=(work/'run.log').read_text();print(output.strip());receipts.append({'target':target,'exit_code':run.returncode,'stdout':output,'test_sha256':hashlib.sha256(test.read_bytes()).hexdigest()})
 files=['src/subsystems/physics/runtime/native_grab_producer60.h','src/subsystems/physics/runtime/native_grab_producer60.inl','src/subsystems/throw_drop/grip_stow_cb1.h','src/subsystems/throw_drop/runtime/grip_stow_cb1.cpp','src/subsystems/alchemy/finish_observer_cb1.h','src/subsystems/alchemy/finish_observer_cb1.cpp','src/subsystems/interactions/native_item_context_h41.inl','src/frik_pose_probe.cpp','src/mod.cpp']
 frik=(ROOT/'src/frik_pose_probe.cpp').read_text();mod=(ROOT/'src/mod.cpp').read_text()
 assert frik.count('SetCurrentReader(&AlchemyFinishCurrentH41)')==1 and frik.count('SetCurrentReader(&GripStowCurrentH41)')==1
 assert mod.count('alchemy_finish_cb1::SetEnabled(g_alchemy_finish_observer_cb1.load')==1
 result={'registration_static_check':'two actual production readers + INI capture wiring present once','asan':a.asan,'source_root':str(ROOT),'sources':{r:hashlib.sha256((ROOT/r).read_bytes()).hexdigest() for r in files},'tests':receipts,'scope':'actual bodies; native leaves modeled, no adapter registration/native execution/headset proof'}
 (a.output/'RESULT.json').write_text(json.dumps(result,indent=2))
if __name__=='__main__':main()
