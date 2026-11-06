"""Execute D72 production menu bindings and complete startup migration with IO leaves."""
from pathlib import Path
import argparse,hashlib,json,re,subprocess
p=argparse.ArgumentParser();p.add_argument('output',type=Path);a=p.parse_args()
r=Path(__file__).resolve().parents[1];a.output.mkdir(parents=True,exist_ok=True)
s=(r/'src/subsystems/experimental/live_runtime_d40.inl').read_text()
for label in ('READ','APPLY','SAVE','LOAD'):
 block=s.split('defined(KCD2VR_D40_LIVE_'+label+')',1)[1].split('#elif',1)[0].split('#endif',1)[0]
 lines=[l for l in block.splitlines() if 'instant_d72::' in l or ('auto& d40=' in l)]
 assert len(lines)>=5,(label,lines)
 (a.output/('instant_'+label.lower()+'_production.inc')).write_text('\n'.join(lines)+'\n')
s=(r/'src/mod.cpp').read_text();start=s.index('    if(ReadIniIntExplicit(explicit_ini,"Settings","VrCandidateDefaultsD72",0)==0){');end=s.index('    thrust_tuning_d67::enabled.store',start)
body=s[start:end];assert body.rstrip().endswith('}')
(a.output/'instant_defaults_production.inc').write_text(body)
cmd=['g++','-std=c++23','-O2','-Wall','-Wextra','-Werror','-I',str(r/'src'),'-I',str(a.output),str(r/'tests/instant_settings_d72_test.cpp'),'-o',str(a.output/'instant_settings_d72')]
b=subprocess.run(cmd,text=True,capture_output=True)
report={'build_exit':b.returncode,'build_stderr':b.stderr,'native_windows_ini_execution':False,'scope':'actual D72 live bindings plus complete one-time migration; Win32 file/INI leaves explicit','inputs':{str(q.relative_to(r)):hashlib.sha256(q.read_bytes()).hexdigest() for q in [r/'src/subsystems/experimental/live_runtime_d40.inl',r/'src/subsystems/experimental/live_settings_d39.h',r/'src/vrik/instant_activities_d72.h',r/'src/vrik/candidate_defaults_d72.h',r/'src/mod.cpp']}}
if not b.returncode:
 t=subprocess.run([str(a.output/'instant_settings_d72')],text=True,capture_output=True,timeout=10);report.update(exit=t.returncode,stdout=t.stdout,stderr=t.stderr)
(a.output/'report.json').write_text(json.dumps(report,indent=2)+'\n');print(report.get('stdout',''),report.get('stderr',''),b.stderr);raise SystemExit(0 if report.get('exit')==0 else 1)
