"""Run the actual action-owner and attachment adapter with explicit native leaves."""
from pathlib import Path
import argparse, hashlib, json, subprocess
from host_compiler import command, executable_path
root=Path(__file__).resolve().parents[1]
p=argparse.ArgumentParser();p.add_argument('--out',required=True,type=Path);p.add_argument('--compiler',default='cl');p.add_argument('--expect-old-failure',action='store_true');a=p.parse_args()
out=a.out.resolve();out.mkdir(parents=True,exist_ok=True)
results=[]
names=['pickup_handover_r92'] if a.expect_old_failure else ['pickup_handover_r92','pickup_pending_owner_d72','pickup_pending_d72']
for name in names:
 for optimization in (['/Od'] if a.expect_old_failure else ['/Od','/O2']):
  exe=executable_path(out/(name+optimization[1:]))
  cmd=command(root/'tests'/f'{name}_test.cpp',exe,[root/'tests/syntax_shims',root/'tests/openxr_sdk',root/'src',root/'tests'],a.compiler)
  # The inherited owner fixture also compiles the pickup input producer; its
  # unrelated scenery-grab exclusion uses the current header's inline state.
  if a.compiler.lower() in ('cl','cl.exe'):cmd.insert(1,'/FI'+str(root/'src/vrik/static_grab_r89.h'))
  if '/O2' in cmd:cmd[cmd.index('/O2')]=optimization
  b=subprocess.run(cmd,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True);exe.with_suffix('.build.log').write_text(b.stdout)
  if b.returncode:raise RuntimeError(b.stdout)
  r=subprocess.run([str(exe)],stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True);exe.with_suffix('.run.log').write_text(r.stdout)
  results.append({'test':name,'optimization':optimization,'exit':r.returncode,'output':r.stdout})
  print(r.stdout,flush=True)
  expected=r.returncode!=0 and 'ReadAcquisitionFoundationD56(basis)' in r.stdout if a.expect_old_failure else r.returncode==0
  if not expected:raise RuntimeError(f'{name} returned unexpected result')
paths=[root/'src/action_time_owner59c11.cpp',root/'src/native_grasp59c13.inl',root/'src/vrik/tool_attachment59c10.inl',root/'tests/pickup_handover_r92_test.cpp',Path(__file__)]
(out/'RESULT.json').write_text(json.dumps({'passed':True,'expected_old_failure':a.expect_old_failure,'results':results,'sha256':{str(x.relative_to(root)):hashlib.sha256(x.read_bytes()).hexdigest() for x in paths},'limits':'Production action owner and tool callback with native memory/call doubles. No game scheduling or pixel proof, no DLL.'},indent=2))
