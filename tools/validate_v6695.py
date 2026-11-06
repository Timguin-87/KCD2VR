#!/usr/bin/env python3
"""Reproducible current-source tests. Host shims do not validate Windows injection or native physics scheduling."""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
import argparse,hashlib,json,re,shutil,subprocess,sys,tempfile
from validate_v6694 import native_contract,run
R=Path(__file__).resolve().parents[1]

def mutation(out,name,file,old,new,test):
    with tempfile.TemporaryDirectory() as td:
        t=Path(td)
        shutil.copytree(R/'tests',t/'tests',ignore=shutil.ignore_patterns('__pycache__'))
        for d in ['vrik','diagnostics','core']:shutil.copytree(R/'src'/d,t/'src'/d)
        p=t/file;s=p.read_text();assert old in s,(name,old);p.write_text(s.replace(old,new,1))
        run(['g++','-std=c++17','-O0','-pthread',t/'tests'/test,'-o',t/'test'],out/(name+'_compile.log'))
        x=subprocess.run([str(t/'test')],cwd=t,capture_output=True,text=True)
        (out/(name+'_run.log')).write_text(x.stdout+x.stderr)
        assert x.returncode==1 and 'FAIL' in x.stdout,f'{name}: not a clean behavioral rejection ({x.returncode})'

def main():
    ap=argparse.ArgumentParser();ap.add_argument('--whgame',required=True,type=Path);ap.add_argument('--build-dir',type=Path,default=R/'build-contracts-v6695');ap.add_argument('--only',choices=['all','native','build','python','host','sanitizers','mutations','report'],default='all');a=ap.parse_args()
    out=R/'delivery/v6695';out.mkdir(parents=True,exist_ok=True)
    if a.only in ('all','native'):
        nc=native_contract(a.whgame);(out/'NATIVE_CONTRACTS.json').write_text(json.dumps(nc,indent=2)+'\n')
    if a.only=='native':print('PASS native contracts');return
    frik=read_source(R/'src/frik_pose_probe.cpp');mod=read_source(R/'src/mod.cpp');body=read_source(R/'src/vrik/coherent_pose.inl');finger=read_source(R/'src/vrik/finger_reference.inl')
    assert 'KCD2VR-V66.95-CONTINUITY-CONTACT' in mod and 'constexpr int f4_immersive_ui_rung=0;' in mod
    assert 'FindCEntityForItemObjectV6679' not in frik
    writer=frik[frik.index('static bool ControllerWeaponTransform6694'):frik.index('static void WeaponControlTickImpl')]
    for f in ['g_f12_visible_owner_proven','proofUntil','fingersReady','ReadPoseReferenceState','FindCEntityForItemObject']:assert f not in writer
    assert 'const bool full=rung>=3;' in body and 'const bool collisionMode=rung==4;' in body
    assert 'relaxed_idle_both_player' in finger and 'V66.95/player-neutral-v2' in finger
    assert 'snapshot_mailbox.h' in frik and 'SESSION_END' in frik and 'rung==4' in body
    assert 'validationTick>=poseSnapshot.tick&&validationTick-poseSnapshot.tick<=100' in frik
    if a.only in ('all','build'):
        run(['cmake','-S',R/'tests','-B',a.build_dir],out/'configure.log')
        run(['cmake','--build',a.build_dir,'--parallel','4'],out/'build.log')
        text=run(['ctest','--test-dir',a.build_dir,'-V'],out/'ctest.log')
    if a.only=='build':print('PASS eight CTest groups');return
    text=(out/'ctest.log').read_text()
    names='active_math|active_adapter|controller_adapter|state_queue|playback_and_lab|combo_snapshot|world_adapter|snapshot_mailbox'
    counts={k:[int(v),int(n)] for k,v,n in re.findall(r'\b('+names+r')=(\d+)/(\d+)',text)}
    assert len(counts)==8 and all(a==b for a,b in counts.values()),counts
    py={}
    for test in ['test_diagnostic_report.py','test_combo_report.py','test_controller_report.py','test_continuity_report.py']:
        if a.only in ('all','python'):s=run([sys.executable,R/'tests'/test,'-v'],out/(test+'.log'))
        else:s=(out/(test+'.log')).read_text()
        m=re.search(r'Ran (\d+) tests',s);assert m and 'OK' in s;py[test]=int(m.group(1))
    if a.only=='python':print('PASS',sum(py.values()),'Python tests');return
    with tempfile.TemporaryDirectory() as td:
        t=Path(td);objects=[]
        if a.only in ('all','host'):
            for name in ['frik_pose_probe','animation_filter','combat_diag_probe','diagnostic_trace']:
                o=t/(name+'.o');run(['g++','-std=c++17','-O0','-Wformat=2','-Werror=format','-I'+str(R/'tests/syntax_shims'),'-I'+str(R/'src'),'-c',R/'src'/(name+'.cpp'),'-o',o],out/(name+'_host.log'));objects.append(o)
            (t/'main.cpp').write_text('int main(){return 0;}\n');run(['g++',*objects,t/'main.cpp','-o',t/'hostlink'],out/'host_link.log')
        if a.only in ('all','sanitizers'):
            for name in ['pose_math','pose_adapter','controller_adapter','world_adapter','snapshot_mailbox']:
                run(['g++','-std=c++17','-O1','-pthread','-fsanitize=address,undefined','-fno-omit-frame-pointer','-no-pie',R/'tests'/(name+'_test.cpp'),'-o',t/name],out/(name+'_sanitizer_compile.log'))
                run([t/name],out/(name+'_sanitizer_run.log'),t)
    if a.only in ('host','sanitizers'):print('PASS',a.only);return
    mutants=[
      ('bad_item_link','tests/generated_controller_adapter.inc','+0x88,link','+0x80,link','controller_adapter_test.cpp'),
      ('body_required_for_weapon_only','tests/generated_controller_adapter.inc','if(!ReadWeaponTracking6694(raw))return false;','if(!ReadWeaponTracking6694(raw)||g_cal_state.load()!=2)return false;','controller_adapter_test.cpp'),
      ('old_extension_reserve','src/vrik/pose_math.h','double reserve=-1.0','double reserve=0.008','pose_math_test.cpp'),
      ('finger_inputs_swapped','src/vrik/pose_math.h','finger==Finger::Index?trigger:grip','finger==Finger::Index?grip:trigger','pose_adapter_test.cpp'),
      ('native_snap_on_contention','tests/generated_controller_adapter.inc','if(!guard.owned){g_weapon_contended6695.fetch_add(1,std::memory_order_relaxed);return;}','if(!guard.owned){g_weapon_contended6695.fetch_add(1,std::memory_order_relaxed);native();return;}','controller_adapter_test.cpp'),
      ('replay_refreshes_age','tests/generated_controller_adapter.inc','std::copy_n(state.lastWanted,8,wanted);','state.lastSource.tick=now;std::copy_n(state.lastWanted,8,wanted);','controller_adapter_test.cpp'),
      ('fault_loop_returns','src/vrik/world_query_contract.h','if(result==Result::Fault) {','if(false && result==Result::Fault) {','world_adapter_test.cpp'),
      ('unavailable_contact_freezes_hand','tests/generated_world_adapter.inc','||g_collision_fan_result6695!=wc::Result::Busy','','world_adapter_test.cpp')]
    if a.only in ('all','mutations'):
        for m in mutants:
            print('Checking mutant',m[0],flush=True);mutation(out,*m)
    if a.only=='mutations':print('PASS',len(mutants),'behavioral rejections');return
    for m in mutants:
        assert 'FAIL' in (out/(m[0]+'_run.log')).read_text()
    for name in ['pose_math','pose_adapter','controller_adapter','world_adapter','snapshot_mailbox']:
        assert 'FAIL' not in (out/(name+'_sanitizer_run.log')).read_text()
    current={p.relative_to(R).as_posix():hashlib.sha256(p.read_bytes()).hexdigest() for p in (R/'src').rglob('*') if p.is_file()}
    assert current==json.loads((out/'NATIVE_CONTRACTS.json').read_text())['current_source_sha256'], 'Source changed after native manifest; rerun tests'
    for name in ['frik_pose_probe','animation_filter','combat_diag_probe','diagnostic_trace']:
        assert (out/(name+'_host.log')).exists() and 'error:' not in (out/(name+'_host.log')).read_text()
    assert (out/'host_link.log').exists()
    result={'revision':'V66.95','marker':'KCD2VR-V66.95-CONTINUITY-CONTACT','ctest_groups':8,'assertions':counts,'total_cpp_assertions':sum(x[0] for x in counts.values()),
      'python_tests':py,'total_python_tests':sum(py.values()),'behavioral_mutants_rejected':[m[0] for m in mutants],
      'sanitizers':'ASan+UBSan: portable math, current body/controller/world adapters and threaded mailbox fixture; native APIs replaced',
      'concurrency':'4 reader threads/79000 post-initial publications exercised; no formal whole-engine scheduling proof or ThreadSanitizer result claimed',
      'host_compile_link':'4 complete native-facing TUs using explicit Windows/DetourModKit replacements',
      'retail_static':'same SHA/timestamp and native ID, entity, sampler, alias, world pointer/caller/vtable contracts as V66.94/R1',
      'full_windows_mod_cpp':'NOT_COMPILED','msvc':'NOT_RUN','windows_seh':'NOT_RUN','game_injection':'NOT_RUN','headset':'NOT_RUN','fps':'NOT_MEASURED',
      'body':'V66.94 position/orientation and solver math retained. Snapshot continuity repaired; every extreme-reach visibility event remains unproved.',
      'fingers':'Player-neutral selector/cache version repaired. Max-fist compactness/additive recipe unchanged and unverified; saved user cache was not supplied.',
      'contact':'Explicit F10=4 static+terrain hand ray fan/arm centerline experiment; same allowed hand feeds combined F12. No rigid-body impulses, blade, NPC, moving-object or initial-overlap guarantee.',
      'contact_budget':'18 rays/pass, >=16ms between passes, 1000us checked before additional calls, >8000us single-call stop on return. Cannot interrupt a call already in flight; native scheduling not tested.',
      'interaction':'Native action/target observer retained with precise playback-gap fields. Physical grab, chest storage, drop and door phase control NOT_IMPLEMENTED.',
      'f3':'Same identity predicate plus bounded first failed-stage record. Prior 155 rejected callbacks are not relabeled as player callbacks.'}
    (out/'VERIFICATION.json').write_text(json.dumps(result,indent=2)+'\n');print(json.dumps(result,indent=2))
if __name__=='__main__':main()
