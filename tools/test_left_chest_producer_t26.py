from pathlib import Path
import sys,subprocess,json,hashlib
import argparse
a=argparse.ArgumentParser();a.add_argument('--build-dir',type=Path,required=True);a.add_argument('--out',type=Path,required=True);args=a.parse_args()
S=Path(__file__).resolve().parents[1];B=args.build_dir.resolve();B.mkdir(parents=True,exist_ok=True);sys.path.insert(0,str(S/'tools'))
from test_pending_reach_t23 import extract_function
subprocess.run([sys.executable,str(S/'tools/test_pending_reach_t23.py'),'--build-dir',str(B/'retained'),'--out',str(B/'RETAINED.json')],check=True)
for p in (B/'retained').glob('*.inc'):(B/p.name).write_bytes(p.read_bytes())
(B/'frozen_native_supply_publication.inl').write_bytes((S/'tests/subsystems/throw_drop/fixtures/pending_reach_t23/frozen_native_supply_publication.inl').read_bytes())
h=S/'src/subsystems/throw_drop/native_ammo_service_t22.h';raw=h.read_text();assert raw.count('integrated=false')==1
O=B/'activation/subsystems/throw_drop';O.mkdir(parents=True,exist_ok=True);(O/h.name).write_text(raw.replace('integrated=false','integrated=true'))
cmd=B/'run.cmd';cmd.write_text('@echo off\ncall "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvars64.bat" >nul\ncd /d "'+str(B)+'"\ncl /nologo /std:c++20 /EHsc /Od /Zi /UNDEBUG /fsanitize=address /I "'+str(B/'activation')+'" /I "'+str(S/'src')+'" /I "'+str(S/'src/subsystems/throw_drop')+'" /I "'+str(B)+'" "'+str(S/'tests/subsystems/throw_drop/left_chest_producer_t26_test.cpp')+'" /Fe:test.exe\nif errorlevel 1 exit /b %errorlevel%\ntest.exe\nexit /b %errorlevel%\n')
r=subprocess.run(['cmd','/c',str(cmd)],capture_output=True,text=True);(B/'RUN.log').write_text(r.stdout+r.stderr);print(r.stdout+r.stderr);args.out.parent.mkdir(parents=True,exist_ok=True);args.out.write_text(json.dumps({'exit':r.returncode,'activation_variant':'ONLY copied public integrated=false -> true; actual producer unchanged','native_game':False,'source_sha256':{n:hashlib.sha256((S/n).read_bytes()).hexdigest() for n in ['src/subsystems/throw_drop/native_supply_publication.inl','src/subsystems/throw_drop/pending_reach_t23.h','src/vrik/native_supply_t2.h']}},indent=2)+'\n');assert r.returncode==0
