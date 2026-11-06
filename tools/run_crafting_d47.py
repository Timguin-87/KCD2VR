#!/usr/bin/env python3
"""Focused actual-production crafting regressions. No retail/Windows SEH claim."""
import argparse
import concurrent.futures
import hashlib
import json
from pathlib import Path
import subprocess
import tempfile

TESTS = [
    'bellows_pull_d51',
    'alchemy_input_supersession_d50',
    'subsystems/alchemy/work_cadence_d49', 'workstation_routing_d47', 'alchemy_semi46', 'alchemy_semi46_native',
    'alchemy_tool_presentation_d39', 'alchemy_pestle_grip_d44', 'alchemy_pestle_park_d47', 'tool_attachment59c10',
    'subsystems/alchemy/ownership_d47', 'subsystems/alchemy/input67', 'subsystems/alchemy/gesture_work67',
    'subsystems/alchemy/mortar_motion_d41', 'subsystems/alchemy/mortar_input69', 'subsystems/alchemy/mortar_pour70',
    'subsystems/alchemy/mortar_retry72', 'subsystems/alchemy/physical_release_repair', 'subsystems/alchemy/bellows_type_r8b',
    'subsystems/alchemy/proximity_craft1', 'subsystems/alchemy/selected_pickup_d44', 'subsystems/alchemy/bridge67',
    'subsystems/alchemy/native_return67', 'subsystems/alchemy/native_return67_connected',
    'subsystems/alchemy/native_mortar69', 'subsystems/alchemy/station_capture_d42',
    'smith_tool_contact_repair', 'smith_contact_dispatch_repair', 'station_ws_j27', 'anvil_a3',
    'subsystems/smithing/bound_tools67', 'subsystems/smithing/foreaft_craft1', 'subsystems/smithing/native_contact68',
    'subsystems/smithing/native_mode_clock67', 'subsystems/smithing/mode_clock67',
]
NATIVE = {'alchemy_input_supersession_d50', 'alchemy_semi46_native', 'subsystems/alchemy/native_return67_connected','subsystems/smithing/native_mode_clock67'}
BASELINE_FAILURES = {
    'subsystems/alchemy/proximity_craft1': 'baseline_subsystems_alchemy_proximity_craft1',
    'subsystems/smithing/bound_tools67': 'baseline_subsystems_smithing_bound_tools67',
    'subsystems/smithing/mode_clock67': 'baseline_subsystems_smithing_mode_clock67',
}

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--out',type=Path,required=True)
    parser.add_argument('--compiler',default='g++')
    parser.add_argument('--jobs',type=int,default=2)
    parser.add_argument('--tests',nargs='+',choices=TESTS)
    parser.add_argument('--sanitize',action='store_true')
    args=parser.parse_args();root=Path(__file__).resolve().parents[1];out=args.out.resolve();out.mkdir(parents=True,exist_ok=True)
    def run(name,executables):
        label=name.replace('/','_');source=root/'tests'/(name+'_test.cpp');exe=executables/label
        cmd=[args.compiler,'-std=c++20','-O0','-g0','-UNDEBUG','-pthread','-fms-extensions','-ffunction-sections','-fdata-sections',
             '-I'+str(root/'tests/syntax_shims'),'-I'+str(root/'src'),'-I'+str(root/'tests')]
        if args.sanitize:cmd+=['-fsanitize=address,undefined','-fno-sanitize-recover=all','-fno-omit-frame-pointer','-no-pie']
        if name in NATIVE:
            # The retained full owner TU includes unrelated Windows-only
            # finally wrappers. Only normal flow is exercised on this host;
            # this translation is NOT a validation of exception unwinding.
            cmd+=['-include',str(root/'tests/syntax_shims/windows.h'),'-D__finally=catch(...) { throw; } if(true)']
        cmd+=[str(source)]
        if name in {'alchemy_input_supersession_d50','alchemy_semi46_native','subsystems/smithing/native_mode_clock67'}:cmd+=[str(root/'tests/crafting_native_stubs_d47.cpp')]
        cmd+=['-Wl,--gc-sections','-o',str(exe)]
        result={'test':name,'source_sha256':hashlib.sha256(source.read_bytes()).hexdigest(),'compile_command':cmd,
                'native_game_execution':False,'windows_seh':'NOT_EXECUTED','sanitizers':args.sanitize}
        run_log=out/(label+'.run.log');run_log.unlink(missing_ok=True)
        try:
            build=subprocess.run(cmd,cwd=root,text=True,capture_output=True,timeout=180)
            (out/(label+'.build.log')).write_text(build.stdout+build.stderr);result['compile_exit']=build.returncode
            if build.returncode==0:
                execution=subprocess.run([str(exe)],cwd=root,text=True,capture_output=True,timeout=60)
                run_log.write_text(execution.stdout+execution.stderr);result['run_exit']=execution.returncode
            result['status']='PASS' if result.get('run_exit')==0 else 'FAIL'
            # These are retained failures, not an assertion waiver. Only mark
            # the comparison when the freshly executed output matches the
            # saved untouched-D46 output exactly; the overall exit stays nonzero.
            baseline=BASELINE_FAILURES.get(name)
            if baseline and result.get('compile_exit')==0 and result.get('run_exit')!=0:
                prior=root/'repair_evidence/d47/crafting'/(baseline+'.run.log')
                if prior.exists() and run_log.read_bytes()==prior.read_bytes():
                    result['comparison']='BASELINE_FAILURE_UNCHANGED'
                    result['baseline_receipt']=baseline+'.json'
        except (OSError,subprocess.TimeoutExpired) as error:result.update(status='ERROR',error=str(error))
        (out/(label+'.json')).write_text(json.dumps(result,indent=2)+'\n')
        print(name,result['status'],flush=True);return result
    with tempfile.TemporaryDirectory(prefix='kcd2vr_crafting_d47_') as tmp:
        with concurrent.futures.ThreadPoolExecutor(max_workers=max(1,args.jobs)) as pool:
            results=list(pool.map(lambda name:run(name,Path(tmp)),args.tests or TESTS))
    summary={'scope':'Focused production headers and actual adapters with explicit native leaves; no DLL/headset or Windows SEH execution',
             'results':results,'passed':sum(r['status']=='PASS' for r in results),'total':len(results),
             'baseline_failures_unchanged':sum(r.get('comparison')=='BASELINE_FAILURE_UNCHANGED' for r in results),
             'uncompared_failures':sum(r['status']!='PASS' and 'comparison' not in r for r in results)}
    (out/'SUMMARY.json').write_text(json.dumps(summary,indent=2)+'\n')
    return 0 if summary['passed']==summary['total'] else 1
if __name__=='__main__':raise SystemExit(main())
