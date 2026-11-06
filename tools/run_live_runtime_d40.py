#!/usr/bin/env python3
"""Execute actual D40 Mod contributions and owners with explicit host IO leaves."""
from pathlib import Path
import argparse, hashlib, json, subprocess, os
p=argparse.ArgumentParser(description=__doc__);p.add_argument('--out',type=Path,required=True)
a=p.parse_args();root=Path(__file__).resolve().parents[1];out=a.out.resolve()
if out==root or root in out.parents:p.error('--out must be outside source')
out.mkdir(parents=True,exist_ok=True)
# Reuse the existing balanced production-function extraction implementation.
extractor=(root/'tools/test_live_experiments_d39.py').read_text()
namespace={};exec(extractor[extractor.index('def function('):extractor.index('\nshared_names =')],namespace)
function=namespace['function']
frik=(root/'src/frik_pose_probe.cpp').read_text();lab=(root/'src/vrik/lab58.inl').read_text();mod=(root/'src/mod.cpp').read_text()
names=['void SetCombatNoStep55(','bool CombatNoStep55(','void SetCombatNoTurnD40(','bool CombatNoTurnD40(',
       'bool CombatNoTurnReadyD40(','void SetClinchModeD40(','int ClinchModeD40(','bool ClinchGateReadyD40(']
owners='\n'.join(function(frik,n) for n in names)+'\n'+'\n'.join(function(lab,n) for n in ['void SetLab58LivingTouch(','void SetLab58('])+'\n'
(out/'live_owners_d40_production.inc').write_text(owners)
for mode in ('READ','APPLY','SAVE','LOAD'):
    inclusion='#define KCD2VR_D40_LIVE_'+mode+'\n#include "subsystems/experimental/live_runtime_d40.inl"\n#undef KCD2VR_D40_LIVE_'+mode
    assert mod.count(inclusion)==1,('missing/duplicate Mod contribution',mode)
oldload=[line for line in mod.splitlines() if 'SetCombatNoStep55(ReadIniBoolExplicit' in line or ('"VrFrikElbows"' in line and 'ReadIniBoolExplicit' in line)]
assert len(oldload)==2
(out/'live_legacy_load_d40_production.inc').write_text('\n'.join(oldload)+'\n')
combat_host=(root/'src/subsystems/masterstrike_combos/host.inl').read_text()
combat_keys=('VrNpcReactsToBlade','VrNpcReactsStaminaHits','VrPhysicalComboFinisher')
combat_load=[line for line in combat_host.splitlines() if any('"'+key+'"' in line for key in combat_keys) and 'ReadIniBoolExplicit' in line]
assert len(combat_load)==3
(out/'live_combat_load_d41_production.inc').write_text('\n'.join(combat_load)+'\n')
oldsave=[line for line in mod.splitlines() if any('wb("'+key+'"' in line for key in ('VrLabLivingHitImpact58','VrLabCorpseDrag58','VrLabPropWake58'))]
assert len(oldsave)==3
(out/'live_legacy_save_d40_production.inc').write_text('\n'.join(oldsave)+'\n')
compiler=os.environ.get('CXX','cl' if os.name=='nt' else 'g++');msvc=Path(compiler).name.lower() in ('cl','cl.exe')
exe=out/('live_runtime_d40.exe' if os.name=='nt' else 'live_runtime_d40')
cmd=([compiler,'/nologo','/std:c++17','/EHsc','/Od','/I'+str(root/'src'),'/I'+str(out),str(root/'tests/live_runtime_d40_test.cpp'),'/Fe'+str(exe),'/Fo'+str(out/'live_runtime_d40.obj')] if msvc else [compiler,'-std=c++17','-O2','-Wall','-Wextra','-Werror','-I',str(root/'src'),'-I',str(out),str(root/'tests/live_runtime_d40_test.cpp'),'-o',str(exe)])
b=subprocess.run(cmd,capture_output=True,text=True)
files=['src/mod.cpp','src/frik_pose_probe.cpp','src/vrik/lab58.inl','src/vrik/frik_elbow_lab.h','src/subsystems/masterstrike_combos/host.inl','src/subsystems/experimental/live_runtime_d40.inl','src/subsystems/experimental/live_settings_d39.h','src/subsystems/ranged_reload/arrow_hold_d48.h','src/npc_look_policy63.h','src/vrik/camera_hold_e1.h','src/vrik/alchemy_tool_contact_d55.h','src/dialogue_state59c10r2.h','src/dialogue_membership19c2.h','tests/live_runtime_d40_test.cpp']
receipt={'native_game':False,'GDI':'NOT_RUN','command':cmd,'build_exit':b.returncode,'build_stdout':b.stdout,'build_stderr':b.stderr,'inputs':{f:hashlib.sha256((root/f).read_bytes()).hexdigest() for f in files}}
if b.returncode==0:
    r=subprocess.run([str(exe)],capture_output=True,text=True);receipt.update(exit=r.returncode,stdout=r.stdout,stderr=r.stderr)
(out/'RESULT.json').write_text(json.dumps(receipt,indent=2)+'\n')
print(b.stderr);print(receipt.get('stdout',''));print(receipt.get('stderr',''))
raise SystemExit(0 if receipt.get('exit')==0 else 1)
