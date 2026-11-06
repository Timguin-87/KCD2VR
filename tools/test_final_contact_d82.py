#!/usr/bin/env python3
"""Compile actual final-intent/contact/queue code; optional actual old-source negative."""
from pathlib import Path
import argparse,hashlib,json,subprocess
p=argparse.ArgumentParser();p.add_argument('--out',type=Path,required=True);p.add_argument('--baseline',type=Path);p.add_argument('--cxx',default='g++');a=p.parse_args()
root=Path(__file__).resolve().parents[1];a.out.mkdir(parents=True,exist_ok=True)
def extract(text,name):
 start=text.index(name+'(');start=text.rfind('static ',0,start);at=text.index('{',start);depth=1;end=at+1
 while depth:
  depth+=(text[end]=='{')-(text[end]=='}');end+=1
 return text[start:end]
def run(source,out,new):
 out.mkdir(parents=True,exist_ok=True);text=(source/'src/vrik/native_input117.inl').read_text();queue=(source/'src/vrik/physical_native.inl').read_text()
 start=text.index('struct NativeOwnedComboInputH14 {');end=text.index('\n};',start)+3
 prefix=text[start:end]+'\n'+extract(text,'NativeSameOwnedComboInputH14')+'\n'+extract(text,'NativeMeleeCommandCurrentH28')+'\n'
 start=text.index('namespace finalD72=');end=text.index('\nstatic bool NativePendingBlockCommitR18(',start)
 functions=prefix+text[start:end];(out/'final_adapter_extracted.inc').write_text(functions)
 startq=queue.index('struct PhysCommands6698 {');endq=queue.index('static PhysCommands6698 g_phys_commands6698;',startq)+len('static PhysCommands6698 g_phys_commands6698;')
 ring=queue[startq:endq]
 if new:ring+='\n'+extract(queue,'PhysFinalQueuedConflictD82')
 (out/'final_ring_extracted.inc').write_text(ring)
 start=queue.index('        if(g_phys_fault6698.load())break;',queue.index('int budget=8;'))
 end=queue.index('        // D48_DRAW_INTENT_CONSUMER_BEGIN',start)
 (out/'final_queue_extracted.inc').write_text(queue[start:end])
 if new:
  start=text.index('    if(!NativeFinalEnterD82(finalPlanD72,prepare))');end=text.index('\n',text.index('const bool returned=PhysCallAction6698',start))
  (out/'final_dispatch_extracted.inc').write_text(text[start:end])
  start=queue.index('        if(!prepareQueueD72){ph::Command originalD82{};');end=queue.index('\n',start)
  (out/'final_pop_extracted.inc').write_text(queue[start:end])
 exe=out/'final_contact_d82';cmd=[a.cxx,'-std=c++20','-O1','-g','-Wall','-Wextra','-I'+str(source/'src'),'-I'+str(out),str(root/'tests/final_contact_d82_host.cpp'),'-o',str(exe),'-pthread']
 if new:cmd.insert(1,'-DD82_NEW_SOURCE')
 built=subprocess.run(cmd,capture_output=True,text=True);(out/'build.log').write_text(built.stdout+built.stderr)
 if built.returncode:print(built.stderr);raise SystemExit('production extraction compile failed')
 tested=subprocess.run([str(exe)],capture_output=True,text=True);(out/'run.log').write_text(tested.stdout+tested.stderr);print(tested.stdout)
 result={'source_sha256':hashlib.sha256((source/'src/vrik/native_input117.inl').read_bytes()).hexdigest(),'queue_sha256':hashlib.sha256((source/'src/vrik/physical_native.inl').read_bytes()).hexdigest(),'extracted_sha256':hashlib.sha256(functions.encode()).hexdigest(),'compile':cmd,'test_exit':tested.returncode,'native_abi_headset':'NOT_RUN'}
 (out/'result.json').write_text(json.dumps(result,indent=2)+'\n')
 if new and tested.returncode:raise SystemExit('new production regression failed')
 if not new and (tested.returncode!=1 or 'FAIL 109 ms:' not in tested.stdout or 'FAIL 140 ms:' not in tested.stdout):raise SystemExit('baseline did not fail the expected timing controls')
 return result
results={'new':run(root,a.out/'new',True)}
if a.baseline:results['old_negative']=run(a.baseline,a.out/'old_negative',False)
(a.out/'results.json').write_text(json.dumps(results,indent=2)+'\n')
