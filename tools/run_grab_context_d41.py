#!/usr/bin/env python3
"""Actual grab lifecycle, backend, proxy and extracted frik context regression.
The extracted context includes its actual physics/general reservation helpers;
native equipment and late-XR leaf counters prove the physics path excludes both.
--baseline-source runs the identical new behavior fixture against frozen D40R1.
This is a portable source fixture, not a Windows DLL or headset execution.
"""
from pathlib import Path
import argparse, concurrent.futures, hashlib, json, os
from run_pickup_d40 import run_one

ROOT=Path(__file__).resolve().parents[1]
TESTS=['grab_context_d41','grab_context_wiring_d41','grab_rearm_d41','grab_producer60','grab_backend60',
       'grab_connected_fixture','grab_input_d40','grab_release_input_h21','grip_stow_h41',
       'native_grasp13','pickup_near_d40','grab_use_producer_d40']

def function(source,signature):
    start=source.index(signature);brace=source.index('{',start);depth=0
    for i in range(brace,len(source)):
        if source[i]=='{':depth+=1
        elif source[i]=='}':
            depth-=1
            if depth==0:return source[start:i+1]+'\n'
    raise ValueError('unterminated production function: '+signature)

def integration(shims):
    text=(ROOT/'src/frik_pose_probe.cpp').read_text()
    extracted=function(text,'static bool GrabSourceCurrentD41(')
    helpers={signature:function(text,signature) for signature in (
        'static bool FistHandsClosedPhysicsD41(',
        'static unsigned PhysicalHandReservationsBaseH40(',
        'unsigned PhysicalHandReservations114(',
        'unsigned PhysicalHandReservationsPhysicsD41(')}
    (shims/'grab_source_d41_generated.inl').write_text(
        'namespace frik_pose_probe {\n'+''.join(helpers.values())+'}\n'+extracted)
    tick=function(text,'static void NativeRhythmTick178()')
    context=function(text,'void PublishGameStateContext(')
    policy=context[context.index('if(policyChanged57){'):context.index('g_ctx_menu.store(')]
    runtime=(ROOT/'src/subsystems/physics/runtime/native_weapon_body_t4.cpp').read_text()
    refresh=runtime.index('grab_producer60::PhysicsRefresh(')
    motor=runtime.index('hand_proxy60::ServiceStep(',refresh)
    consume=runtime.index('grab_producer60::PhysicsStep(',motor)
    native=text[text.index('if(nativeBody){',text.index('static bool GrabSourceCurrentD41(')):]
    native=native[:native.index('combatPoseTick')]
    checks={
      'registered_before_game_thread_evidence':tick.index('SetSourcePredicateD41(&GrabSourceCurrentD41)')<tick.index('grab_input60::GameThreadTick('),
      'context_transition_cancels_both_sides':all('grab_producer60::Cancel('+str(side)+')' in policy for side in (0,1)),
      'native_body_takeover_cancels_both_sides':all('grab_producer60::Cancel('+str(side)+')' in native for side in (0,1)),
      'refresh_before_proxy_before_consume':refresh<motor<consume,
      'no_grab_status_recursion':'ReadStatus' not in extracted,
      'physics_reservation_entry_used':'PhysicalHandReservationsPhysicsD41()' in extracted and 'PhysicalHandReservations114()' not in extracted,
      'physics_entry_disables_native_fallback':'PhysicalHandReservationsBaseH40(false)' in helpers['unsigned PhysicalHandReservationsPhysicsD41('],
      'general_entry_keeps_native_fallback':'PhysicalHandReservationsBaseH40()' in helpers['unsigned PhysicalHandReservations114('],
    }
    return {'checks':checks,'passed':all(checks.values()),
            'extracted_predicate_sha256':hashlib.sha256(extracted.encode()).hexdigest(),
            'extracted_reservation_helpers_sha256':{k:hashlib.sha256(v.encode()).hexdigest() for k,v in helpers.items()}}

def main():
    ap=argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--out',type=Path,required=True);ap.add_argument('--baseline-source',type=Path)
    ap.add_argument('--compiler',default=os.environ.get('CXX','g++'));ap.add_argument('--jobs',type=int,default=3)
    args=ap.parse_args();out=args.out.resolve()
    if out==ROOT or ROOT in out.parents:ap.error('--out must be outside source')
    out.mkdir(parents=True,exist_ok=True);shims=out/'host_shims';shims.mkdir(exist_ok=True)
    (shims/'excpt.h').write_text('#pragma once\n#define __fastcall\n#define __try try\n#define __except(x) catch(...)\n#define EXCEPTION_EXECUTE_HANDLER 1\n')
    wiring=integration(shims)
    with concurrent.futures.ThreadPoolExecutor(max_workers=max(1,args.jobs)) as pool:
        futures=[pool.submit(run_one,ROOT,out/'current',name,args.compiler,shims) for name in TESTS]
        results=[f.result() for f in futures]
    baseline=None
    if args.baseline_source:
        base=args.baseline_source.resolve();overlay=out/'baseline_fixture_overlay';overlay.mkdir(exist_ok=True)
        dest=overlay/'src'
        if not dest.exists():dest.symlink_to(base/'src',target_is_directory=True)
        tests=overlay/'tests';tests.mkdir(exist_ok=True)
        for entry in (base/'tests').iterdir():
            target=tests/entry.name
            if not target.exists():target.symlink_to(entry,target_is_directory=entry.is_dir())
        fixture=tests/'grab_context_d41_test.cpp'
        if fixture.is_symlink():fixture.unlink()
        fixture.write_bytes((ROOT/'tests/grab_context_d41_test.cpp').read_bytes())
        baseline=run_one(overlay,out/'baseline','grab_context_d41',args.compiler,shims)
        output=baseline.get('run',{}).get('output','')
        baseline['expected_behavioral_red']=baseline['build']['exit']==0 and baseline.get('run',{}).get('exit')==1 and all(
            'FAIL '+case in output for case in ('cutscene_withdraw_before_motor','cancel_then_same_press_republish','context_retires_during_prepare','disabled_still_retires'))
    production=['src/subsystems/physics/runtime/native_grab_producer60.h',
      'src/subsystems/physics/runtime/native_grab_producer60.inl',
      'src/subsystems/physics/runtime/native_grab_backend60.inl',
      'src/subsystems/physics/runtime/native_hand_proxy_owner60.inl','src/frik_pose_probe.cpp','src/frik_pose_probe.h',
      'src/subsystems/vrik/tracking_reader.inl','src/vrik/fist_native.inl',
      'src/subsystems/interactions/native_passenger_a34.h','src/vrik/workstation178r20c1.h',
      'src/subsystems/doors/hand_only_h40.h']
    report={'passed':wiring['passed'] and all(x['passed'] for x in results) and (baseline is None or baseline['expected_behavioral_red']),
            'integration':wiring,'current':results,'baseline':baseline,
            'production_sha256':{p:hashlib.sha256((ROOT/p).read_bytes()).hexdigest() for p in production},
            'native_game_executed':False,'windows_dll_build':'NOT_RUN','headset':'NOT_RUN'}
    (out/'RESULTS.json').write_text(json.dumps(report,indent=2)+'\n')
    print(f"{sum(r['passed'] for r in results)}/{len(results)} portable adapters; integration={'PASS' if wiring['passed'] else 'FAIL'}; D40R1 behavioral red={baseline and baseline['expected_behavioral_red']}; native game/headset NOT_RUN",flush=True)
    return 0 if report['passed'] else 1
if __name__=='__main__':raise SystemExit(main())
