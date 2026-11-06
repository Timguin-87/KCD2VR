"""Build/run the actual read-only native slot resolver with substituted memory."""
import argparse
import hashlib
import json
from pathlib import Path
import subprocess

root=Path(__file__).resolve().parents[3]
ap=argparse.ArgumentParser(description=__doc__)
ap.add_argument('--build-dir',type=Path,required=True)
ap.add_argument('--report',type=Path,required=True)
ap.add_argument('--asan',action='store_true')
a=ap.parse_args()
build=a.build_dir.resolve();build.mkdir(parents=True,exist_ok=True)
test=Path(__file__).with_name('native_attachment_slots_f2_test.cpp')
policy=root/'src/subsystems/physics/native_attachment_slots_f2.inl'
inputs={str(p.relative_to(root)):hashlib.sha256(p.read_bytes()).hexdigest() for p in [test,policy,Path(__file__)]}
vcvars=Path('C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvars64.bat')
cmd=build/'run_attachment_slots_f2.cmd'
cmd.write_text('@echo off\n'+f'call "{vcvars}" >nul\nif errorlevel 1 exit /b 1\n'+
    'cl /nologo /std:c++20 /EHsc /UNDEBUG /Od /Zi /MD '+('/fsanitize=address ' if a.asan else '')+
    f'/I "{root/"src"}" /Fo"{build/"slots.obj"}" /Fd"{build/"slots.pdb"}" /Fe"{build/"slots.exe"}" "{test}"\n'+
    f'if errorlevel 1 exit /b 1\n"{build/"slots.exe"}"\n')
r=subprocess.run(['cmd.exe','/d','/c',str(cmd)],cwd=build,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
after={str(p.relative_to(root)):hashlib.sha256(p.read_bytes()).hexdigest() for p in [test,policy,Path(__file__)]}
record={'result':'PASS' if r.returncode==0 and inputs==after else 'FAIL','returncode':r.returncode,
        'asan':a.asan,'actual_production_resolver':True,'fixture_substitutions':['native memory Read leaves'],
        'input_sha256':inputs,'inputs_unchanged':inputs==after,'stdout':r.stdout,
        'native_game_executed':False,'headset_accepted':False}
a.report.parent.mkdir(parents=True,exist_ok=True)
a.report.write_text(json.dumps(record,indent=2)+'\n')
print(r.stdout,end='')
raise SystemExit(r.returncode if r.returncode else int(inputs!=after))
