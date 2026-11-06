#!/usr/bin/env python3
"""Run production diagnostic headers and actual Windows queue/writer with MSVC."""
import argparse,json,subprocess,hashlib
from pathlib import Path
from prepare_dmk_dependency import prepare
ROOT=Path(__file__).resolve().parents[1]
def main():
 p=argparse.ArgumentParser();p.add_argument('--out',required=True,type=Path);a=p.parse_args();out=a.out.resolve()
 if out==ROOT or ROOT in out.parents:p.error('out must be outside source')
 out.mkdir(parents=True,exist_ok=True);results=[]
 dependency=prepare(ROOT/"deps/DetourModKit",out/"dependency")
 for name in ['interaction_snapshot_d42','diagnostic_health_d42','diagnostic_health_c54r3']:
  folder=out/name;folder.mkdir(exist_ok=True);exe=folder/(name+'.exe')
  cmd=['cl','/nologo','/std:c++latest','/EHsc','/Od','/UNDEBUG','/DNOMINMAX','/I'+str(ROOT/'src'),str(ROOT/'tests'/(name+'_test.cpp'))]
  if name.startswith('diagnostic_health'):cmd+=['/I'+str(dependency/'include'),str(ROOT/'src/diagnostic_trace.cpp'),str(dependency/'lib/libDetourModKit.lib'),'user32.lib','xinput.lib']
  cmd+=['/Fe:'+str(exe),'/Fo:'+str(folder)+'\\']
  r={'name':name,'command':cmd};b=subprocess.run(cmd,cwd=folder,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True);(folder/'build.log').write_text(b.stdout);r['build_exit']=b.returncode
  if b.returncode==0:
   b=subprocess.run([str(exe)],cwd=folder,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True);(folder/'run.log').write_text(b.stdout);r['run_exit']=b.returncode
  r['passed']=r.get('run_exit')==0;print(name+': '+('PASS' if r['passed'] else 'FAIL'),flush=True);results.append(r)
 files=['src/diagnostics/interaction_snapshot_d42.h','src/diagnostics/capture168.h','src/diagnostics/capture_control168.h','src/diagnostics/coverage_d42.h','src/diagnostic_trace.cpp']
 report={'passed':all(x['passed'] for x in results),'tests':results,'source_sha256':{x:hashlib.sha256((ROOT/x).read_bytes()).hexdigest() for x in files},'native_game':'NOT_RUN','headset':'NOT_RUN'}
 (out/'RESULTS.json').write_text(json.dumps(report,indent=2)+'\n');return 0 if report['passed'] else 1
if __name__=='__main__':raise SystemExit(main())
