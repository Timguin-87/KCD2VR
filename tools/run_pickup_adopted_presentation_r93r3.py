"""Current ordinary attachment callback and exact pre-fix negative control; no DLL."""
from pathlib import Path
import argparse,hashlib,json,subprocess
from host_compiler import command,executable_path
ROOT=Path(__file__).resolve().parents[1]
BASE='5b47eb121f55c7d2c3b60c12a6c73416befe293c'
def main():
 ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('--out',type=Path,required=True);ap.add_argument('--baseline-source',type=Path,help='Optional complete R93R2 source for the negative control');a=ap.parse_args()
 out=a.out.resolve();out.mkdir(parents=True,exist_ok=True);rows=[]
 owner=ROOT/'src/vrik/tool_attachment59c10.inl'
 baseline=None
 if a.baseline_source:baseline=(a.baseline_source/'src/vrik/tool_attachment59c10.inl').read_text()
 else:
  try:baseline=subprocess.check_output(['git','show',BASE+':src/vrik/tool_attachment59c10.inl'],cwd=ROOT,text=True,stderr=subprocess.DEVNULL)
  except (OSError,subprocess.CalledProcessError):pass
 cases=[('debug',ROOT/'tests/pickup_adopted_presentation_r93r3_test.cpp',False),('optimized',ROOT/'tests/pickup_adopted_presentation_r93r3_test.cpp',False)]
 if baseline is not None:
  d=out/'baseline';d.mkdir(exist_ok=True)
  (d/'tool_attachment_baseline.inl').write_text(baseline)
  for name in ['pickup_adopted_presentation_r93r3_test.cpp','item_handover_d71_test.cpp','tool_attachment59c10_test.cpp']:
   t=(ROOT/'tests'/name).read_text().replace('../src/',(ROOT/'src').as_posix()+'/')
   if name=='tool_attachment59c10_test.cpp':t=t.replace(owner.as_posix(),(d/'tool_attachment_baseline.inl').as_posix())
   (d/name).write_text(t)
  cases.append(('baseline',d/'pickup_adopted_presentation_r93r3_test.cpp',True))
 else:print('Baseline negative control NOT_RUN: provide --baseline-source or pinned Git history',flush=True)
 for mode,source,old in cases:
  dest=out/mode;dest.mkdir(exist_ok=True);exe=executable_path(dest/'test')
  cmd=command(source,exe,[ROOT/'src/vrik',ROOT/'tests/syntax_shims',ROOT/'tests/openxr_sdk',ROOT/'src',ROOT/'tests'],'cl')
  cmd.insert(1,'/FI'+str(ROOT/'src/vrik/static_grab_r89.h'))
  if mode!='optimized':cmd=[('/Od' if x=='/O2' else x) for x in cmd]
  b=subprocess.run(cmd,cwd=dest,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
  (dest/'build.log').write_text(b.stdout);row=dict(name=mode,compile_exit=b.returncode,negative_control=old,passed=False)
  if not b.returncode:
   r=subprocess.run([str(exe)],cwd=dest,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
   (dest/'run.log').write_text(r.stdout);row.update(exit=r.returncode,output=r.stdout.strip(),passed=(r.returncode!=0 and 'FAIL R93R3 line=17' in r.stdout) if old else r.returncode==0)
  rows.append(row);print(mode+': '+str(row),flush=True)
 paths=[owner,ROOT/'tests/pickup_adopted_presentation_r93r3_test.cpp',ROOT/'tests/tool_attachment59c10_test.cpp',ROOT/'tests/item_handover_d71_test.cpp',Path(__file__)]
 result=dict(passed=all(r['passed'] for r in rows),baseline=BASE,baseline_adapter_sha256=hashlib.sha256(baseline.encode()).hexdigest() if baseline is not None else None,baseline_negative_control='RUN' if baseline is not None else 'NOT_RUN: R93R2 source unavailable',tests=rows,sha256={p.relative_to(ROOT).as_posix():hashlib.sha256(p.read_bytes()).hexdigest() for p in paths},limits=['Actual production adapter; native leaves and exact cleanup-adoption interleaving are controlled.','No observed display-frame measurement or headset result. No DLL or game install.','Active cleanup, pre-Attach callbacks, other rejection causes and body/camera movement remain outside this correction.'])
 (out/'RESULT.json').write_text(json.dumps(result,indent=2)+'\n');return 0 if result['passed'] else 1
if __name__=='__main__':raise SystemExit(main())
