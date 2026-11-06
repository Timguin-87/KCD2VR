#!/usr/bin/env python3
"""PU1/AL1/DI1 exact source adapters and strict negative controls.

Tests use native/OS/placement leaves, not a running game or HMD. The action-owner
fixture's unrelated crossbow/bow/ammo SEH scopes require portable syntax shims on
Linux; those scopes are not executed and no Windows exception claim is made.
"""
from pathlib import Path
import argparse, hashlib, json, re, subprocess, os
from host_compiler import command, executable_path
ROOT=Path(__file__).resolve().parents[1]

def absolute_includes(text,parent,overrides):
 def resolve(m):
  name=m[1]
  for p in [parent/name, ROOT/'src'/name, ROOT/'tests'/name, ROOT/'src/vrik'/name]:
   if p.exists():
    path=p.resolve();return '#include '+json.dumps(overrides.get(path,path).as_posix())
  return m[0]
 return re.sub(r'#include "([^"]+)"',resolve,text)

def main():
 a=argparse.ArgumentParser(description=__doc__);a.add_argument('--out',type=Path,required=True);a.add_argument('--only',default='',help='Rerun selected labels and retain prior results only if production hashes match');args=a.parse_args()
 out=args.out.resolve();out.mkdir(parents=True,exist_ok=True)
 source=ROOT/'src/activity_lens59c10r1.cpp';view=source.read_text()
 begin=view.index('    {   // Retain only this qualified dice visit');end=view.index('    // Diagnose the formerly silent rejection paths',begin)
 exitblock=view[begin:end];(out/'dice_exit_adapter_r95.inc').write_text(exitblock)
 selected=set(args.only.split(','))if args.only else set()
 prior=json.loads((out/'RESULT.json').read_text())if selected and (out/'RESULT.json').exists()else None
 rows=[]
 includes=[out,ROOT/'src',ROOT/'src/vrik',ROOT/'tests',ROOT/'tests/syntax_shims',ROOT/'tests/openxr_sdk']
 def run(path,label,flags,inc=includes,negative=False,owner=False):
  if selected and label not in selected:return
  exe=executable_path(out/label)
  if os.name=='nt':
   cmd=command(path,exe,inc,'cl')
   forced=next((x/'src/vrik/static_grab_r89.h' for x in path.parents if (x/'src/vrik/static_grab_r89.h').exists()),ROOT/'src/vrik/static_grab_r89.h')
   cmd.insert(1,'/FI'+str(forced))
   if label.startswith('dice_snapshot_d84'):cmd.insert(1,'/D_ReturnAddress=HostReturnAddress')
   if '-O0' in flags:cmd=[('/Od' if v=='/O2' else v) for v in cmd]
  else:
   cmd=['g++','-std=c++20','-UNDEBUG','-pthread',*flags]
   if owner:cmd+=['-D__fastcall=','-D__finally=catch(...) { throw; } if(true)','-D__declspec(x)=','-include',str(path.parents[1]/'src/vrik/static_grab_r89.h')]
   for p in inc:cmd+=['-I',str(p)]
   cmd += [str(path),'-o',str(exe)]
  c=subprocess.run(cmd,cwd=out,text=True,capture_output=True);(out/(label+'.compile.txt')).write_text(c.stdout+c.stderr)
  row={'test':label,'command':cmd,'compile_exit':c.returncode,'negative_control':negative,'passed':False}
  if c.returncode==0:
   r=subprocess.run([str(exe)],text=True,capture_output=True);(out/(label+'.run.txt')).write_text(r.stdout+r.stderr)
   row.update(run_exit=r.returncode,output=r.stdout+r.stderr)
   row['passed']=bool(r.returncode and ('FAIL' in r.stdout+r.stderr)) if negative else r.returncode==0
  rows.append(row);print(label+': '+('PASS' if row['passed'] else 'FAIL'),flush=True)
 tests=['pickup_adopted_policy_r95','alchemy_clock_order_r95','dice_exit_adapter_r95','xr_haptics']
 variants=[('debug',['-O0','-g']),('optimized',['-O2'])]
 if os.name!='nt':variants.append(('ubsan',['-O1','-g','-fsanitize=undefined','-fno-sanitize-recover=all']))
 for variant,flags in variants:
  for name in tests:run(ROOT/'tests'/(name+'_test.cpp'),name+'_'+variant,flags)
  run(ROOT/'tests/pickup_finish_owner_r92r4_test.cpp','pickup_finish_owner_'+variant,flags,owner=True)
 # Existing real attachment and plate contracts guard all changed consumers.
 for name in ['alchemy_tool_presentation_d39','plate_hand_mapping_r93','item_handover_d71','pickup_finishing_visual_r94','activity_lens59c10r1','dice_snapshot_d84','haptics136r1','impact_haptics133']:
  run(ROOT/'tests'/(name+'_test.cpp'),name+'_regression',['-O2'])
 run(ROOT/'tests/subsystems/alchemy/presentation71_test.cpp','presentation71_regression',['-O2'])
 adapter=ROOT/'src/vrik/tool_attachment59c10.inl';bridge=ROOT/'src/subsystems/alchemy/bridge67.h';lens=ROOT/'src/activity_lens59c10r1.h';owner=ROOT/'src/action_time_owner59c11.cpp'
 def swap(text,old,new):
  assert text.count(old)==1,(old,text.count(old));return text.replace(old,new)
 adapter_text=adapter.read_text();bridge_text=bridge.read_text();lens_text=lens.read_text()
 mutants=[]
 pickup_old=swap(adapter_text,'const bool finishingOwned=(!owned||!g_gameplay_allowed.load(std::memory_order_acquire))&&','const bool finishingOwned=!owned&&')
 pickup_old=swap(pickup_old,'const bool finishingGraspR94=(!ordinaryGrasp||!g_gameplay_allowed.load(std::memory_order_acquire))&&','const bool finishingGraspR94=!ordinaryGrasp&&')
 mutants.append(('old_pickup_adopted_policy','pickup_adopted_policy_r95',{adapter:pickup_old},None,False))
 old_al=adapter_text.replace('presentationOwnsCurrentR95(alchemy18,body.stamp.epoch,[]() noexcept {return std::uint64_t(GetTickCount64());},','presentationOwns71(alchemy18,now,body.stamp.epoch,')
 assert old_al!=adapter_text
 mutants.append(('old_alchemy_callback_clock','alchemy_clock_order_r95',{adapter:old_al},None,False))
 mutants.append(('old_alchemy_lookup_clock','alchemy_clock_order_r95',{adapter:swap(adapter_text,'    now=std::uint64_t(GetTickCount64());\n    // A live scenery','    // A live scenery')},None,False))
 before_clock=swap(bridge_text,'    if(!readPresentationR95(b,snapshot))return false;\n    const std::uint64_t now=clock(); // after ALL snapshots, including the other-hand plate witness',
   '    const std::uint64_t now=clock();\n    if(!readPresentationR95(b,snapshot))return false; // deliberate wrong ordering')
 mutants.append(('alchemy_clock_before_copies','alchemy_clock_order_r95',{bridge:before_clock},None,False))
 mutants.append(('old_dice_stale_means_off','dice_exit_adapter_r95',{},swap(exitblock,'view.known,havePolicy&&p.eye,dice,dialog.player,','view.known,eyePolicy&&!dialog.active,dice,dialog.player,'),False))
 mutants.append(('dice_unknown_erases_identity','dice_exit_adapter_r95',{lens:swap(lens_text,'if(!f.known)return false;','if(!f.known){h={};return false;}')},None,False))
 mutants.append(('dice_final_lifetime_unchecked','dice_exit_adapter_r95',{},swap(exitblock,'w.lifetime175.load(std::memory_order_acquire)!=view.lifetime','false'),False))
 owntext=owner.read_text();start=owntext.index('static bool ReadFinishingVisualImplR94(');finish=owntext.index('bool ReadFinishingAttachmentPresentationR94',start)
 chunk=owntext[start:finish];unsafe=swap(chunk,'!FreshAcquisitionInput55(input,now)','false');owntext=owntext[:start]+unsafe+owntext[finish:]
 mutants.append(('pickup_adopted_stale_input','pickup_finish_owner_r92r4',{owner:owntext},None,True))
 no_protected=exitblock[:exitblock.index('            // Reobserve the existing native dialogue/player and camera chains:')]+exitblock[exitblock.index('            Policy finalPolicyR95{};'):]
 mutants.append(('dice_final_protected_unchecked','dice_exit_adapter_r95',{},no_protected,False))
 haptic_source=ROOT/'src/subsystems/hud_ui_controls/runtime/xr_input.cpp';haptic_core=ROOT/'src/core/haptics.h'
 mutants.append(('haptic_xr_failure_claimed_success','xr_haptics',{haptic_source:swap(haptic_source.read_text(),'hr==XR_SUCCESS?"XR_SUBMITTED":"XR_REJECTED"','"XR_SUBMITTED"')},None,False))
 mutants.append(('haptic_queue_rejected','xr_haptics',{haptic_core:swap(haptic_core.read_text(),'const bool accepted=queue[side].publish(pulse);','const bool accepted=false; // deliberate busy mailbox leaf')},None,False))
 files=['pickup_adopted_policy_r95_test.cpp','item_handover_d71_test.cpp','tool_attachment59c10_test.cpp','alchemy_clock_order_r95_test.cpp','alchemy_tool_presentation_d39_test.cpp','dice_exit_adapter_r95_test.cpp','pickup_finish_owner_r92r4_test.cpp','alchemy_resource_receipt_d60_test.cpp','alchemy_semi46_native_test.cpp','native_grasp_owner13_test.cpp']
 for label,test,changes,changed_block,needs_owner in mutants:
  d=out/('mutant_'+label);d.mkdir(exist_ok=True)
  # Copy the complete reachable quoted-include graph. A mutated leaf reached
  # through an unchanged intermediate header must not silently load production.
  test_path=(ROOT/'tests'/(test+'_test.cpp')).resolve();graph={};todo=[test_path]
  todo.append((ROOT/'src/vrik/static_grab_r89.h').resolve())
  todo.extend(p.resolve() for base in [ROOT/'tests/syntax_shims',ROOT/'tests/openxr_sdk'] for p in base.rglob('*') if p.is_file() and p.suffix in ('.h','.hpp'))
  while todo:
   p=todo.pop()
   if p in graph:continue
   contents=changes.get(p,p.read_text());graph[p]=contents
   for name in re.findall(r'#include "([^"]+)"',contents):
    for q in [p.parent/name,ROOT/'src'/name,ROOT/'tests'/name,ROOT/'src/vrik'/name]:
     if q.exists() and q.is_file():
      q=q.resolve()
      if q.is_relative_to(ROOT):todo.append(q)
      break
  assert all(p.resolve() in graph for p in changes),'mutant must be in compiled include graph'
  overrides={p:d/'tree'/p.relative_to(ROOT) for p in graph}
  for p,contents in graph.items():
   target=overrides[p];target.parent.mkdir(parents=True,exist_ok=True)
   target.write_text(absolute_includes(contents,p.parent,overrides))
  (d/'dice_exit_adapter_r95.inc').write_text(changed_block if changed_block is not None else exitblock)
  run(overrides[test_path],label,['-O2'],[d,d/'tree/tests/syntax_shims',d/'tree/tests/openxr_sdk',*includes],negative=True,owner=needs_owner)
 result={'passed':all(r['passed'] for r in rows),'tests':rows,
  'production_sha256':{str(p.relative_to(ROOT)):hashlib.sha256(p.read_bytes()).hexdigest() for p in [adapter,bridge,lens,source,owner,ROOT/'src/vrik/alchemy_tool_contact_d53.inl',haptic_source,haptic_core]},
  'extraction':{'source':str(source.relative_to(ROOT)),'sha256':hashlib.sha256(exitblock.encode()).hexdigest(),'source_start_line':view[:begin].count('\n')+1},
  'limits':['Host native memory, OS, placement and event leaves are controlled. No game, real OpenXR runtime, Windows DLL build or HMD validation.',
   'Full action owner uses portable parsing for three unrelated SEH wrappers; those wrappers are not exercised and Windows exception behavior is not verified.',
   'Physical dice-cup acquisition/gesture/native throw accounting is unimplemented because no verified native cup contract is available.']}
 if selected:
  assert prior and prior['production_sha256']==result['production_sha256'],'partial rerun requires identical production source'
  assert {r['test']for r in rows}==selected,'all selected checks must run'
  replacement={r['test']:r for r in rows}
  result['tests']=[replacement.get(r['test'],r)for r in prior['tests']]
  result['passed']=all(r['passed']for r in result['tests']);result['selected_rerun']=sorted(selected)
 (out/'RESULT.json').write_text(json.dumps(result,indent=2)+'\n')
 raise SystemExit(0 if result['passed'] else 1)
if __name__=='__main__':main()
