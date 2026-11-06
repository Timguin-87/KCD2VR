"""Verify production reload diagnostic queues with independent F5/F7/F11 leases."""
import argparse,json,hashlib,os,subprocess
from pathlib import Path

def main():
 ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('--output',type=Path,required=True);ap.add_argument('--baseline',type=Path);args=ap.parse_args()
 root=Path(__file__).resolve().parents[1];out=args.output.resolve();out.mkdir(parents=True,exist_ok=True)
 names=('reload_capture_d84','activity_links_d77','d57_reload_request_diagnostics')
 rows=[]
 for name in names:
  for mode,opt in [('debug',['-O0']),('release',['-O2'])]+([('sanitized',['-O1','-g','-fsanitize=address,undefined','-fno-omit-frame-pointer','-no-pie'])] if name=='reload_capture_d84' else []):
   binary=out/(name+'_'+mode);cmd=['g++','-std=c++20','-UNDEBUG','-pthread',*opt,'-I',str(root/'src'),str(root/'tests'/(name+'_test.cpp')),'-o',str(binary)]
   build=subprocess.run(cmd,capture_output=True,text=True);binary.with_suffix('.build.log').write_text(build.stdout+build.stderr)
   r={'name':name,'mode':mode,'command':cmd,'build_rc':build.returncode,'passed':False}
   if build.returncode==0:
    env=dict(os.environ);env['ASAN_OPTIONS']='detect_leaks=0'
    run=subprocess.run([str(binary)],capture_output=True,text=True,env=env);binary.with_suffix('.run.log').write_text(run.stdout+run.stderr)
    r.update(run_rc=run.returncode,stdout=run.stdout,stderr=run.stderr,passed=run.returncode==0)
   rows.append(r);print(name,mode,'PASS' if r['passed'] else 'FAIL',flush=True)
 # The regression counterexample uses the actual original headers in a small
 # preimage-only fixture, with their original sink signatures preserved.
 if args.baseline:
  base=args.baseline.resolve();p=out/'baseline_f5_f7.cpp'
  p.write_text('''#include "subsystems/ranged_reload/reload_diagnostics_d48.h"
#include <cstdio>
namespace c=kcd2vr::diagnostics::capture168;namespace r=kcd2vr::reload_diagnostics_d48;
int main(){c::gate.select(c::Domain::Interaction,true,1000);r::recordStart("TAKEN",1001,42);r::Event e{};
if(!r::pop(e)){std::puts("FAIL D84 F5-alone native reload event absent in preimage");return 1;}return 0;}
''')
  binary=out/'baseline_f5_f7';cmd=['g++','-std=c++20','-O0','-I',str(base/'src'),str(p),'-o',str(binary)]
  build=subprocess.run(cmd,capture_output=True,text=True);binary.with_suffix('.build.log').write_text(build.stdout+build.stderr)
  r={'name':'baseline_f5_f7','command':cmd,'build_rc':build.returncode,'passed':False}
  if build.returncode==0:
   run=subprocess.run([str(binary)],capture_output=True,text=True);binary.with_suffix('.run.log').write_text(run.stdout+run.stderr);r.update(run_rc=run.returncode,stdout=run.stdout,stderr=run.stderr,passed=run.returncode==1 and 'FAIL D84 F5-alone' in run.stdout and not run.stderr)
  rows.append(r);print('baseline F5-only','PASS' if r['passed'] else 'FAIL',flush=True)
 (out/'results.json').write_text(json.dumps(rows,indent=2)+'\n')
 files=['src/diagnostics/reload_capture_d84.h','src/diagnostics/activity_links_d77.h','src/subsystems/ranged_reload/reload_diagnostics_d48.h','src/subsystems/ranged_reload/crossbow_native_xr34.inl','src/subsystems/ranged_reload/crossbow_rifle_request_h21.inl']
 (out/'source_hashes.json').write_text(json.dumps({f:hashlib.sha256((root/f).read_bytes()).hexdigest() for f in files},indent=2)+'\n')
 return 0 if all(r['passed'] for r in rows) else 1
if __name__=='__main__':raise SystemExit(main())
