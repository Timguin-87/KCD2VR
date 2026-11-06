"""Actual retained C++ admission + original retail Lua composition, no source copy."""
from pathlib import Path
import hashlib,json,subprocess,sys,zipfile
BASE=Path(r"D:/KCD2VR-review-builds/H41_STEP30_F9_INDEPENDENT_GRIP_20261024/delivery_8a8c547_CANONICAL_HOST_QA2/KCD2VR_SOURCE")
LANE=Path(__file__).resolve().parents[1]
LUA=Path(r"D:/KCD2VR-review-builds/H41_GORE_PART_PHYSICS_OFF_20261024/review/lua51_private")
sys.path.insert(0,str(BASE/'tools'));import test_hand_only_doors_h40 as h
H40=LANE/'mods/KCD2VRDoors/hand_only_h40.lua'
CPP=LANE/'tests/hand_only_h40_full_bridge.cpp'
def quote(s):
 e='='
 while ']'+e+']'in s:e+='='
 return '['+e+'['+s+']'+e+']'
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def main():
 with zipfile.ZipFile(h.PAK)as z:retail=z.read(h.MEMBER)
 assert hashlib.sha256(retail).hexdigest()==h.EXPECTED_SCRIPT_SHA
 for opt in (False,True):
  out=LANE/'verification'/('actual_cpp_O2'if opt else'actual_cpp_Debug');out.mkdir(parents=True,exist_ok=True)
  for order in ('r4_first','h40_first'):
   units=[('test/native_leaves.lua',h.LEAVES),(h.MEMBER,retail.decode('utf-8-sig')),('test/native_object_leaves.lua',h.DOUBLES),('test/h40_external.lua',h.EXTERNAL)]
   for p in ([h.R4,H40]if order=='r4_first'else[H40,h.R4]):units.append(('mods/KCD2VRDoors/hand_only_h40.lua'if p==H40 else'mods/KCD2VRDoors/door_runtime.lua',p.read_text()))
   (out/(order+'.lua')).write_text('\n'.join('assert(loadstring('+quote(s)+','+quote('@'+n)+'))()'for n,s in units),encoding='utf-8')
  objects=[p for p in sorted((LUA/'build_asan').glob('*.obj'))if p.name!='lua.obj'];assert len(objects)==29
  cmd='@echo off\ncall "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvars64.bat" >nul\ncd /d "'+str(out)+'"\ncl /nologo /std:c++latest /EHsc /Zi /fsanitize=address /DKCD2VR_H40_PARTIAL_REST_LUA51=1 '+('/O2'if opt else'/Od')+' /MD /I"'+str(LUA/'lua-5.1.5/src')+'" /I"'+str(BASE/'tests')+'" "'+str(CPP)+'" '+' '.join('"'+str(p)+'"'for p in objects)+' /Fe:door_h44.exe /sourceDependencies door_h44.deps.json /link /INCREMENTAL:NO /STACK:33554432\nif errorlevel 1 exit /b %errorlevel%\ndoor_h44.exe r4_first.lua >r4_first.log 2>&1\nset r1=%errorlevel%\ndoor_h44.exe h40_first.lua >h40_first.log 2>&1\nset r2=%errorlevel%\nif not %r1%==0 exit /b %r1%\nexit /b %r2%\n'
  (out/'build.cmd').write_text(cmd,encoding='utf-8',newline='\r\n')
  run=subprocess.run(['cmd','/d','/c',str(out/'build.cmd')],capture_output=True,text=True)
  (out/'COMPILE.log').write_text(run.stdout+run.stderr)
  result=dict(exit=run.returncode,optimized=opt,asan=True,inputs={str(p):sha(p)for p in(CPP,H40,BASE/'src/subsystems/doors/hand_only_h40.h',h.R4)},retail_sha256=h.EXPECTED_SCRIPT_SHA,results={n:(out/(n+'.log')).read_text()if(out/(n+'.log')).exists()else'NOT_RUN'for n in('r4_first','h40_first')},limitations=['Actual C++ admission/transport and Lua5.1 retail operation; native leaves modeled, no engine/headset acceptance.'])
  (out/'RESULT.json').write_text(json.dumps(result,indent=2)+'\n')
  print(json.dumps(result,indent=2))
  if run.returncode:raise SystemExit(run.returncode)
if __name__=='__main__':main()
