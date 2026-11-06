"""Ordinary-pickup first callback checks. Run in a VS developer shell.
Only host executables; never builds the DLL. Mutations restore each old gate
separately and must fail the new production-adapter fixture.
"""
from pathlib import Path
import argparse,hashlib,json,subprocess
from host_compiler import command,executable_path
ROOT=Path(__file__).resolve().parents[1]
p=argparse.ArgumentParser(description=__doc__);p.add_argument('--out',required=True,type=Path);p.add_argument('--compiler',default='cl');a=p.parse_args()
out=a.out.resolve();out.mkdir(parents=True,exist_ok=True)
owner=ROOT/'src/vrik/tool_attachment59c10.inl';producer=ROOT/'src/vrik/coherent_pose.inl'
s=producer.read_text();begin=s.index('    // Carry original fingers even when the new socket has not reached the mailbox.')
end=s.index('    BuildToolPoses59c10(packet,',begin);carry=s[begin:end]
(out/'pickup_original_finger_carry_r94.inc').write_text(carry)
extract=subprocess.run(['python',str(ROOT/'tools/extract_pose_reader_d50.py'),'--source',str(ROOT),'--out',str(out)],capture_output=True,text=True)
(out/'pose_reader_generation.log').write_text(extract.stdout+extract.stderr)
if extract.returncode:raise SystemExit(extract.returncode)
rows=[]
def run(source,name,includes,expect_failure=False):
 exe=executable_path(out/name);cmd=command(source,exe,includes,a.compiler)
 if a.compiler.lower() in ('cl','cl.exe'):cmd.insert(1,'/FI'+str(ROOT/'src/vrik/static_grab_r89.h'))
 b=subprocess.run(cmd,cwd=out,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
 (out/(name+'.compile.log')).write_text(b.stdout)
 row={'test':name,'compile_exit':b.returncode,'expected_failure':expect_failure}
 if not b.returncode:
  r=subprocess.run([str(exe)],cwd=out,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
  (out/(name+'.run.log')).write_text(r.stdout);row.update(exit=r.returncode,output=r.stdout.strip())
  row['pass']=(r.returncode!=0 and ('FAIL R94' in r.stdout or 'FAIL FINISH R94' in r.stdout)) if expect_failure else r.returncode==0
  print(name+': '+r.stdout.strip(),flush=True)
 else:row['pass']=False;print(b.stdout,flush=True)
 rows.append(row)
inc=[out,ROOT/'tests/syntax_shims',ROOT/'tests/openxr_sdk',ROOT/'src',ROOT/'tests']
for name in ['item_handover_d71','pickup_pending_d72','pickup_first_callback_r94','pickup_finishing_visual_r94','pose_reader_d50','pickup_finish_owner_r92r4','pickup_pending_owner_d72','pickup_handover_r92']:
 run(ROOT/'tests'/(name+'_test.cpp'),name,inc)
original=owner.read_text()
mutations={
 'old_pending_gate':('if(!out.acceptedAttachTickD72){','if(!out.acceptedAttachTickD72||!kcd2vr::contact_handover_d71::ItemHandover()){'),
 'old_acquisition_gate':('haveAcquisition=action_time_owner59c11::ReadPendingAttachmentPresentationD72(pending)&&','haveAcquisition=kcd2vr::contact_handover_d71::ItemHandover()&&action_time_owner59c11::ReadPendingAttachmentPresentationD72(pending)&&'),
 'old_original_grip_gate':('if((!socket.nativeGrasp13&&!trackedMinigameD72&&','if((!trackedMinigameD72&&')}
# Copy only three fixture files; every production include resolves to current source
# except the one explicitly mutated adapter. No workspace source is overwritten.
for name,pair in list(mutations.items())+[('old_finger_carry',None),('old_cleanup_fallthrough',('const bool finishingGraspR94=(!ordinaryGrasp||!g_gameplay_allowed.load(std::memory_order_acquire))&&','const bool finishingGraspR94=false&&(!ordinaryGrasp||!g_gameplay_allowed.load(std::memory_order_acquire))&&'))]:
 d=out/name;d.mkdir(exist_ok=True)
 for file in ['pickup_first_callback_r94_test.cpp','item_handover_d71_test.cpp','tool_attachment59c10_test.cpp','pickup_finishing_visual_r94_test.cpp']:
  text=(ROOT/'tests'/file).read_text().replace('../src/',(ROOT/'src').as_posix()+'/')
  if file=='tool_attachment59c10_test.cpp':
   wanted=(ROOT/'src/vrik/tool_attachment59c10.inl').as_posix();assert wanted in text
   text=text.replace(wanted,(d/'tool_attachment_mutated.inl').as_posix())
  (d/file).write_text(text)
 mutation=original
 if pair:assert original.count(pair[0])==1;mutation=original.replace(*pair)
 (d/'tool_attachment_mutated.inl').write_text(mutation)
 changed_carry=carry.replace('    for(int side=0;side<2;++side)','    if(packet.itemHandoverRevisionD71)for(int side=0;side<2;++side)') if not pair else carry
 (d/'pickup_original_finger_carry_r94.inc').write_text(changed_carry)
 run(d/('pickup_finishing_visual_r94_test.cpp' if name=='old_cleanup_fallthrough' else 'pickup_first_callback_r94_test.cpp'),name,[d,ROOT/'src/vrik',*inc],True)
paths=[owner,producer,ROOT/'src/frik_pose_probe.cpp',ROOT/'src/action_time_owner59c11.cpp',ROOT/'src/action_time_owner59c11.h',ROOT/'src/native_grasp59c13.inl',ROOT/'src/core/native_grasp13.h',ROOT/'tests/pose_reader_d50_test.cpp',ROOT/'tests/pickup_finishing_visual_r94_test.cpp',ROOT/'tests/pickup_finish_owner_r92r4_test.cpp',ROOT/'tests/tool_attachment59c10_test.cpp',ROOT/'tests/pickup_first_callback_r94_test.cpp',ROOT/'tests/item_handover_d71_test.cpp',ROOT/'tests/pickup_pending_d72_test.cpp',Path(__file__)]
result={'passed':all(x['pass'] for x in rows),'tests':rows,'sha256':{str(x.relative_to(ROOT)):hashlib.sha256(x.read_bytes()).hexdigest() for x in paths},
 'producer_snippet':{'file':str(producer.relative_to(ROOT)),'start_line':s[:begin].count('\n')+1,'end_line':s[:end].count('\n'),'sha256':hashlib.sha256(carry.encode()).hexdigest()},
 'limitations':['Host source/explicit native leaf fixtures, not game or displayed-frame proof.','Fix closes optional-toggle OFF first-callback delay; five older logged first placements already used the ON path.','Picking finish body/world shift and callbacks before native acknowledgement remain separate investigations; this adds no global camera/body correction.','No DLL or game install.']}
(out/'RESULT.json').write_text(json.dumps(result,indent=2)+'\n');raise SystemExit(0 if result['passed'] else 1)
