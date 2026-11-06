#!/usr/bin/env python3
"""Execute production reload clock, pose consumer and multi-frame gesture path."""
from pathlib import Path
import argparse,hashlib,json,subprocess,os
from check_crossbow_d82 import function
from host_compiler import command as host_command, executable_path
ROOT=Path(__file__).resolve().parents[1]

def main():
 ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('--out',type=Path,required=True);ap.add_argument('--debug-only',action='store_true');ap.add_argument('--body-source-only',action='store_true');ap.add_argument('--placement-only',action='store_true');ap.add_argument('--source-gap-only',action='store_true');ap.add_argument('--handover-only',action='store_true');ap.add_argument('--compiler',default=None);a=ap.parse_args()
 msvc=Path(a.compiler or os.environ.get('CXX','g++')).name.lower() in ('cl','cl.exe')
 out=a.out.resolve();assert ROOT not in out.parents;out.mkdir(parents=True,exist_ok=True)
 adapterpath=ROOT/'src/subsystems/ranged_reload/crossbow_native_xr34.inl';passpath=adapterpath.with_name('reload_passenger_xr34.inl');rangepath=adapterpath.with_name('crossbow_rifle_request_h21.inl')
 adapter=adapterpath.read_text();start=adapter.index('    __try{started=g_crossbow_xr34.startNewAfterRestore(');endtxt='__finally{(void)ma::Finish(token,&XR34FinishAdmissionT25,&started);}';end=adapter.index(endtxt,start)+len(endtxt)
 body=adapter[start:end].replace('    __try{','    {struct HostFinishD82 {ma::Token& token;bool& started;~HostFinishD82(){(void)ma::Finish(token,&XR34FinishAdmissionT25,&started);}} finish{token,started};',1).replace('}\n    '+endtxt,'}')
 adapter=adapter[:start]+body+adapter[end:]
 passenger=''.join(function(passpath.read_text(),s) for s in ['static bool ReloadNativeClockCurrentD86(','static bool ReloadPassengerCurrentXR34(','static bool ReadReloadPassengerXR34(','static bool ReloadPoseWriterCurrentXR34('])
 source_route=function(rangepath.read_text(),'static bool RangedSourceXR34(')
 # GCC 13 ICEs when this large aggregate temporary is assigned in the
 # combined host fixture. Name the value-initialized temporary; same copy.
 source_route=source_route.replace('out=xcXR34::Source{};', 'const xcXR34::Source emptySourceD87{};out=emptySourceD87;',1).replace('out=xcXR34::Source{};', 'out=emptySourceD87;')
 completed=function(rangepath.read_text(),'static bool RangedRawIntentD78(')+function(passpath.read_text(),'static xcXR34::Source CompletedReloadPoseSourceXR34(')
 retained=function(passpath.read_text(),'static bool RangedReloadSourceXR34(').replace('out=xcXR34::Source{};', 'const xcXR34::Source emptySourceD87{};out=emptySourceD87;',1)
 exclusion=function((ROOT/'src/animation_filter.cpp').read_text(),'bool ReloadPassengerOnlyXR34(').replace('__try {','try {').replace('__except(EXCEPTION_EXECUTE_HANDLER)','catch(...)')
 fixtures={n:(ROOT/'tests'/n).read_text().replace('"../src/','"'+(ROOT/'src').as_posix()+'/') for n in ['crossbow_native_xr34_test.cpp','crossbow_scope_d82_test.cpp','crossbow_pipeline_d87_test.cpp','crossbow_source_route_d87_fixture.h','crossbow_filter_d87_fixture.h','crossbow_diagnostic_d87_fixture.h','crossbow_clock_cycle_r92_test.cpp','crossbow_transition_d80_test.cpp','crossbow_body_source_r93r3_test.cpp','crossbow_placement_wait_r94_test.cpp','crossbow_source_gap_r95_test.cpp','crossbow_handover_r95_test.cpp']}
 fixtures['crossbow_native_xr34_test.cpp']=fixtures['crossbow_native_xr34_test.cpp'].replace('"'+adapterpath.as_posix()+'"','"native_adapter_host.inc"')
 publisher=function(rangepath.read_text(),'static bool RangedQuiverInputR95(')+function(rangepath.read_text(),'static bool RangedGapReleaseR95(')+function(rangepath.read_text(),'static void RangedPublishXR34(')
 runs=[]
 def run(name,ad=adapter,pa=passenger,sr=source_route,ex=exclusion,pu=publisher,flags=('-O0','-g'),negative=False,test='crossbow_pipeline_d87_test.cpp'):
  d=out/name;d.mkdir(exist_ok=True)
  for n,s in fixtures.items():(d/n).write_text(s)
  for n,s in [('publish_source_r95_actual.inc',pu),('native_adapter_host.inc',ad),('passenger_consumer_d82_actual.inc',pa),('completed_source_d82_actual.inc',completed),('retained_source_d87_actual.inc',retained),('ranged_source_d87_actual.inc',sr),('passenger_filter_d87_actual.inc',ex),('prepare_reload_body_r93r3_actual.inc',''.join(function(passpath.read_text(), sig) for sig in ['static bool PrepareReloadPoseExactR93R3(', 'static bool SameReloadPoseRetryR93R3(', 'static bool PrepareReloadPoseXR34(']))]:(d/n).write_text(s)
  exe=executable_path(d/name)
  cmd=host_command(d/test,exe,[ROOT/'src',ROOT/'tests/syntax_shims',d],a.compiler)
  if msvc:
   cmd=[('/Od' if '-O0' in flags else '/O2') if x=='/O2' else x for x in cmd]
  else:
   cmd=[x for x in cmd if x!='-O2']+list(flags)+['-fmax-errors=6']
  b=subprocess.run(cmd,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True);(d/'build.log').write_text(b.stdout)
  if b.returncode:raise RuntimeError(name+' build failed\n'+b.stdout[-5000:])
  env=os.environ.copy();env['ASAN_OPTIONS']='detect_leaks=0';p=subprocess.run([str(exe)],stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True,env=env);(d/'run.log').write_text(p.stdout);exe.unlink()
  ok=(p.returncode==1 and ('FAIL_R95' if test=='crossbow_source_gap_r95_test.cpp' else 'FAIL_HANDOVER_R95' if test=='crossbow_handover_r95_test.cpp' else 'FAIL_D87') in p.stdout) if negative else p.returncode==0
  runs.append({'name':name,'passed':ok,'expected_failure':negative,'exit_code':p.returncode,'output':p.stdout,'command':cmd});print(name+': '+p.stdout,end='',flush=True)
  if not ok:raise RuntimeError(name+' failed')
 if a.handover_only:
  run('handover_debug',test='crossbow_handover_r95_test.cpp')
  run('handover_optimized',flags=('-O2',),test='crossbow_handover_r95_test.cpp')
  run('old_exact_transition_clock',pa=passenger.replace('ReloadNativeClockCurrentD86(p,character,true)','ReloadNativeClockCurrentD86(p,character,false)'),negative=True,test='crossbow_handover_r95_test.cpp')
  run('old_noop_publication_revoke',ad=adapter.replace('const auto publication=changed?', 'const auto publication=true?'),negative=True,test='crossbow_handover_r95_test.cpp')
  run('native_button_reacquired',ad=adapter.replace('if(!g_crossbow_xr34.acceptsNewActionR95(s,now)){','if(false){'),negative=True,test='crossbow_handover_r95_test.cpp')
 elif a.source_gap_only:
  run('source_gap_debug',test='crossbow_source_gap_r95_test.cpp')
  run('source_gap_optimized',flags=('-O2',),test='crossbow_source_gap_r95_test.cpp')
  assert publisher.count('if((publicationPending||auxiliaryMissing)&&')==1
  run('old_stable_empty_source_cancels',pu=publisher.replace('if((publicationPending||auxiliaryMissing)&&','if(publicationPending&&'),negative=True,test='crossbow_source_gap_r95_test.cpp')
  assert publisher.count('kcd2vr::reload_throw_claim_t25::workingHandUnclaimed(previous)&&')==1
  run('missing_working_hand_revocation',pu=publisher.replace('kcd2vr::reload_throw_claim_t25::workingHandUnclaimed(previous)&&',''),negative=True,test='crossbow_source_gap_r95_test.cpp')
  assert publisher.count('previous.auxiliaryReleasedR95|=released;')==1
  run('lost_release_replays_nock',pu=publisher.replace('previous.auxiliaryReleasedR95|=released;','previous.auxiliaryReleasedR95|=false;'),negative=True,test='crossbow_source_gap_r95_test.cpp')
 elif a.placement_only:
  run('placement_debug',test='crossbow_placement_wait_r94_test.cpp')
  run('placement_optimized',flags=('-O2',),test='crossbow_placement_wait_r94_test.cpp')
 elif a.body_source_only:
  run('body_source_debug',test='crossbow_body_source_r93r3_test.cpp')
  run('body_source_optimized',flags=('-O2',),test='crossbow_body_source_r93r3_test.cpp')
 else:
  run('debug')
  run('clock_cycle_debug',test='crossbow_clock_cycle_r92_test.cpp')
  run('transition_debug',test='crossbow_transition_d80_test.cpp')
  if not a.debug_only:
   run('optimized',flags=('-O2',))
   run('clock_cycle_optimized',flags=('-O2',),test='crossbow_clock_cycle_r92_test.cpp')
   if not msvc:run('asan_ubsan',flags=('-O1','-g','-fsanitize=address,undefined','-fno-omit-frame-pointer','-no-pie'))
   run('old_duplicate_rejection',ad=adapter.replace('c.id,entries,c.nativeTransitionD80,observed)','c.id,entries,false,observed)'),negative=True)
   run('old_exact_publication_reader',pa=passenger.replace('ReadPoseLeaseD87','ReadPassenger'),negative=True)
   run('old_full_body_source_switch',sr=source_route.replace('currentReloadXR34.clockOwned&&RangedReloadSourceXR34(out)','animation_filter::nativeBodyOwned112.load()&&RangedReloadSourceXR34(out)'),negative=True)
   run('effectful_transition_not_qualified',ad=adapter.replace('||c.despawn>=0||effectsD87','||c.despawn>=0'),negative=True)
   run('broad_same_id_filter_bypass',ex=exclusion.replace('kcd2vr::crossbow_clock_entry_d87::inertTerminal(readEntryD87,reinterpret_cast<std::uintptr_t>(at))','true'),negative=True)
 sources=[adapterpath,passpath,rangepath,adapterpath.with_suffix('.h'),adapterpath.with_name('crossbow_clock_entry_d87.h'),adapterpath.with_name('reload_diagnostics_d48.h'),ROOT/'src/animation_filter.cpp',Path(__file__),*[ROOT/'tests'/n for n in fixtures]]
 (out/'RESULT.json').write_text(json.dumps({'passed':all(r['passed'] for r in runs),'compiler':a.compiler or os.environ.get('CXX','g++'),'sanitizers':'ASan/UBSan executed' if any('asan' in r['name'] for r in runs) else 'not run for selected suite','runs':runs,'source_sha256':{str(p.relative_to(ROOT)):hashlib.sha256(p.read_bytes()).hexdigest() for p in sources},'limits':['Production clock, admission, current-owner memory checks, passenger consumer, completed-source math, hand projection and native-event interval logic execute.','The fixture supplies native memory, alias/protected-action exclusion, identity lookup, mechanism pose evaluation, binding receipts and effects. Separate socket tests execute actual attachment placement/readback.','No Windows build, game, headset, actual animation job scheduling or finite-stock inventory outcome was run.']},indent=2)+'\n')
if __name__=='__main__':main()
