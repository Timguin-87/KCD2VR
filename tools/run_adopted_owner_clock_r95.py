"""Actual owner read with a controlled raw-input publication between its fixed copies."""
from pathlib import Path
import argparse,hashlib,json,re,subprocess
from host_compiler import command,executable_path
from check_interaction_review_r95 import absolute_includes
R=Path(__file__).resolve().parents[1]
p=argparse.ArgumentParser();p.add_argument('--out',type=Path,required=True);a=p.parse_args();out=a.out.resolve();out.mkdir(parents=True,exist_ok=True)
owner=(R/'src/action_time_owner59c11.cpp').resolve();text=owner.read_text();start=text.index('static bool ReadFinishingVisualImplR94(');end=text.index('bool ReadFinishingAttachmentPresentationR94',start);part=text[start:end]
old='!g_acquisition_input55.read(input))return false;';assert part.count(old)==1
part=part.replace(old,'!(::TestFinishingInputReadR95(),g_acquisition_input55.read(input)))return false;')
current=text[:start]+part+text[end:];rows=[]
for mode in ['debug','optimized','old_clock']:
 d=out/mode;d.mkdir(exist_ok=True);change=current
 if mode=='old_clock':
  oldpart=part.replace('    const auto now=std::uint64_t(GetTickCount64());\n','')
  oldpart=oldpart.replace('    if(!live()||!g_finishing_visual_r94.read(p)','    const auto now=std::uint64_t(GetTickCount64());\n    if(!live()||!g_finishing_visual_r94.read(p)')
  change=text[:start]+oldpart+text[end:]
 test=(R/'tests/pickup_adopted_owner_clock_r95_test.cpp').resolve();graph={};todo=[test,(R/'src/vrik/static_grab_r89.h').resolve()]
 todo.extend(p.resolve()for base in [R/'tests/syntax_shims',R/'tests/openxr_sdk']for p in base.rglob('*')if p.is_file()and p.suffix in('.h','.hpp'))
 while todo:
  f=todo.pop()
  if f in graph:continue
  s=change if f==owner else f.read_text()
  if f.name=='pickup_finish_owner_r92r4_test.cpp':s=s.replace('int main(){','int ExistingFinishingOwnerMainR95(){')
  graph[f]=s
  for name in re.findall(r'#include "([^\"]+)"',s):
   for q in [f.parent/name,R/'src'/name,R/'tests'/name,R/'src/vrik'/name]:
    if q.is_file():
     q=q.resolve()
     if q.is_relative_to(R):todo.append(q)
     break
 overrides={f:d/'tree'/f.relative_to(R)for f in graph}
 for f,s in graph.items():target=overrides[f];target.parent.mkdir(parents=True,exist_ok=True);target.write_text(absolute_includes(s,f.parent,overrides))
 exe=executable_path(d/'test');cmd=command(overrides[test],exe,[d/'tree/tests/syntax_shims',d/'tree/tests/openxr_sdk',R/'tests/syntax_shims',R/'tests/openxr_sdk',R/'src',R/'tests'],'cl');cmd.insert(1,'/FI'+str(d/'tree/src/vrik/static_grab_r89.h'))
 if mode=='debug':cmd=[('/Od'if x=='/O2'else x)for x in cmd]
 b=subprocess.run(cmd,cwd=d,text=True,capture_output=True);(d/'build.log').write_text(b.stdout+b.stderr);row={'test':mode,'compile_exit':b.returncode,'passed':False}
 if b.returncode==0:
  r=subprocess.run([str(exe)],text=True,capture_output=True);(d/'run.log').write_text(r.stdout+r.stderr);row.update(exit=r.returncode,output=r.stdout,passed=(r.returncode!=0 and 'FAIL R94'in r.stdout)if mode=='old_clock'else r.returncode==0)
 rows.append(row);print(row,flush=True)
(out/'RESULT.json').write_text(json.dumps({'passed':all(x['passed']for x in rows),'tests':rows,'source_sha256':hashlib.sha256(owner.read_bytes()).hexdigest(),'limits':['Production owner; original native callbacks are controlled leaves; only input-copy scheduling hook is inserted into source copy. No DLL/headset execution.']},indent=2))
raise SystemExit(0 if all(x['passed']for x in rows)else 1)
