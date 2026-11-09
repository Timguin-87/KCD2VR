#!/usr/bin/env python3
"""R143: compile the real CheatCVarWrite143/CheatCVarSet*143 (from src/mod.cpp) and the real CombatWindowsConsoleH3
(from src/subsystems/direct/host.inl) with tests/cheat_cvar_write_r143_test.cpp against a fake retail console."""
import argparse,hashlib,json,os,subprocess
from pathlib import Path
p=argparse.ArgumentParser(description=__doc__);p.add_argument('--out',type=Path,required=True);a=p.parse_args()
root=Path(__file__).resolve().parents[1];out=a.out.resolve()
if out==root or root in out.parents:p.error('output must be outside source')
out.mkdir(parents=True,exist_ok=True)
mod=(root/'src/mod.cpp').read_text(encoding='utf-8')
start=mod.index('static bool CheatCVarWrite143(');end=mod.index('struct CombatCameraLease50 {',start)
(out/'cheat_cvar_r143_production.inc').write_text(mod[start:end]+'\n',encoding='utf-8')
host=(root/'src/subsystems/direct/host.inl').read_text(encoding='utf-8')
hs=host.index('struct CombatWindowsConsoleH3 : RagdollConsole59 {');he=host.index('};',hs)+2
(out/'combat_windows_console_h3_production.inc').write_text(host[hs:he]+'\n',encoding='utf-8')
for user in ('src/subsystems/masterstrike_combos/block_cooldown_lease_h24.inl','src/subsystems/interactions/host.inl','src/subsystems/dialogues/host.inl','src/subsystems/direct/host.inl'):
    assert 'CheatCVarSet' in (root/user).read_text(encoding='utf-8'),user+' does not use the R143 route'
cxx=os.environ.get('CXX','cl' if os.name=='nt' else 'g++');msvc=Path(cxx).name.lower() in ('cl','cl.exe')
exe=out/('cheat_cvar_r143.exe' if os.name=='nt' else 'cheat_cvar_r143')
src=str(root/'tests/cheat_cvar_write_r143_test.cpp')
args=([cxx,'/nologo','/std:c++17','/EHsc','/Od','/I'+str(root/'src'),'/I'+str(out),src,'/Fe'+str(exe),'/Fo'+str(out/'cheat_cvar_r143.obj')] if msvc else
      [cxx,'-std=c++17','-O2','-Wall','-Wextra','-Wno-unused-function','-Wno-unused-parameter','-I'+str(root/'src'),'-I'+str(out),src,'-o',str(exe)])
b=subprocess.run(args,capture_output=True,text=True);(out/'build.log').write_text(b.stdout+b.stderr)
files=['src/mod.cpp','src/subsystems/direct/host.inl','src/subsystems/interactions/host.inl','src/subsystems/dialogues/host.inl',
       'src/subsystems/masterstrike_combos/block_cooldown_lease_h24.inl','src/focus_zoom58c2.h','src/vrik/dodge_window_d70.h','tests/cheat_cvar_write_r143_test.cpp']
receipt={'native_execution':False,'headset':False,'build_exit':b.returncode,'command':args,'inputs':{n:hashlib.sha256((root/n).read_bytes()).hexdigest() for n in files}}
if b.returncode==0:
    r=subprocess.run([str(exe)],capture_output=True,text=True);(out/'run.log').write_text(r.stdout+r.stderr);receipt.update(run_exit=r.returncode,summary=r.stdout);print(r.stdout)
else:print(b.stdout+b.stderr)
(out/'RESULT.json').write_text(json.dumps(receipt,indent=2)+'\n');raise SystemExit(0 if receipt.get('run_exit')==0 else 1)
