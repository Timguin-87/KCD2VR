#!/usr/bin/env python3
"""Run the D47/D48 live arm fixture and retained portable geometry regressions.
Outputs exact commands, statuses, source extraction hashes and stdout per target.
Optional negative source selects D46 arm headers/caller only (the current host
acceptance harness stays unchanged); its numerical failure is expected.
"""
from pathlib import Path
import argparse, json, os, subprocess, sys
p=argparse.ArgumentParser();p.add_argument('--out',type=Path,required=True);p.add_argument('--sanitizers',action='store_true');p.add_argument('--negative-source',type=Path);p.add_argument('--keep-binaries',action='store_true');a=p.parse_args()
root=Path(__file__).resolve().parents[1];out=a.out.resolve();out.mkdir(parents=True,exist_ok=True)
cxx=os.environ.get('CXX','g++');generated=out/'generated'
command=[sys.executable,str(root/'tools/extract_vrik_arm_d47.py'),'--out',str(generated)]
r=subprocess.run(command,cwd=root,capture_output=True,text=True);(out/'extraction.log').write_text(r.stdout+r.stderr)
if r.returncode:sys.exit(r.returncode)
results=[]
for name in ('vrik_arm_adapter_d47_test','vrik_native_observer_d47_test','d48_fist_mask_test','elbow_continuity_test','frik_elbow_lab_test','elbow_shoulder_d45_test'):
 flags=['-std=c++23','-O0','-g0','-I'+str(generated),'-I'+str(root/'tests'),'-I'+str(root/'src'),'-pthread']
 if a.sanitizers:flags+=['-fsanitize=address,undefined','-fno-omit-frame-pointer','-fno-pie','-no-pie']
 cmd=[cxx,*flags,str(root/'tests'/f'{name}.cpp'),'-o',str(out/name)]
 compiled=subprocess.run(cmd,cwd=root,capture_output=True,text=True);(out/f'{name}_compile.log').write_text(compiled.stdout+compiled.stderr)
 result={'name':name,'command':cmd,'compile_exit':compiled.returncode,'run_exit':None}
 if compiled.returncode==0:
  binary=out/name;binary.chmod(binary.stat().st_mode|0o100)
  env=dict(os.environ,ASAN_OPTIONS='detect_leaks=0',UBSAN_OPTIONS='print_stacktrace=1:halt_on_error=1')
  ran=subprocess.run([str(binary)],cwd=root,capture_output=True,text=True,env=env)
  (out/f'{name}.log').write_text(ran.stdout+ran.stderr);result['run_exit']=ran.returncode
 result['passed']=result['compile_exit']==0 and result['run_exit']==0;results.append(result)
 print(('PASS ' if result['passed'] else 'FAIL ')+name,flush=True)
negative=None
if a.negative_source:
 base=a.negative_source.resolve();name='d46_arm_numerical_negative'
 cmd=[cxx,'-std=c++23','-O0','-g0','-I'+str(generated),'-I'+str(root/'tests'),'-I'+str(base/'src'),str(root/'tests/vrik_arm_adapter_d47_test.cpp'),'-pthread','-o',str(out/name)]
 compiled=subprocess.run(cmd,cwd=root,capture_output=True,text=True);(out/f'{name}_compile.log').write_text(compiled.stdout+compiled.stderr)
 negative={'command':cmd,'compile_exit':compiled.returncode,'run_exit':None,'scope':'D46 arm headers and actual arm caller; current acceptance fixture, no full D46 build'}
 if not compiled.returncode:
  binary=out/name;binary.chmod(binary.stat().st_mode|0o100)
  ran=subprocess.run([str(binary)],cwd=root,capture_output=True,text=True);negative['run_exit']=ran.returncode
  (out/f'{name}.log').write_text(ran.stdout+ran.stderr)
  negative['expected_failure_observed']=ran.returncode!=0 and 'FAIL sub-0.081-degree arm rotations' in ran.stdout
 print('EXPECTED_NEGATIVE '+str(negative.get('expected_failure_observed',False)),flush=True)
report={'compiler':subprocess.run([cxx,'--version'],capture_output=True,text=True).stdout.splitlines()[0],
 'sanitizers':a.sanitizers,'leak_detection':False,'native_game_or_render_execution':False,'results':results,'negative_control':negative,
 'passed':sum(r['passed'] for r in results),'total':len(results)}
(out/'results.json').write_text(json.dumps(report,indent=2)+'\n')
if not a.keep_binaries:
 for result in results:
  binary=out/result['name']
  if binary.exists():binary.unlink()
 if negative is not None and (out/'d46_arm_numerical_negative').exists():(out/'d46_arm_numerical_negative').unlink()
print(f"{report['passed']}/{report['total']} PASS; native ABI/render/headset not executed.")
sys.exit(0 if all(r['passed'] for r in results) and (negative is None or negative.get('expected_failure_observed',False)) else 1)
