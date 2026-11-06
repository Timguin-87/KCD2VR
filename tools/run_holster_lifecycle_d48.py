#!/usr/bin/env python3
"""Compile exact production cleanup and native-entry retirement adapters.

Native leaves are substituted in the fixture. The negative control removes
only the cleanup scheduling term that was missing in D46; it must fail.
"""
import argparse
import hashlib
import json
import re
import subprocess
from pathlib import Path
from run_input_regressions_r38 import function

HERE = Path(__file__).resolve().parents[1]


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--source', type=Path, default=HERE)
    p.add_argument('--out', type=Path, required=True)
    a = p.parse_args()
    root, out = a.source.resolve(), a.out.resolve()
    out.mkdir(parents=True, exist_ok=True)
    service_path = root / 'src/subsystems/throw_drop/runtime/native_ammo_service_t22.inl'
    supply_path = root / 'src/subsystems/throw_drop/runtime/native_supply_t2.inl'
    h36_path = root / 'src/subsystems/throw_drop/holster_retirement_h36.inl'
    service, supply, h36 = [x.read_text() for x in (service_path, supply_path, h36_path)]
    block = re.search(r'struct AmmoServiceRecordT22 \{.*?\n\};', service, re.S).group()
    chunks = [block]
    for name in ('AmmoServiceSetCurrentD48', 'AmmoServiceSelectedT22', 'AmmoServiceOwnsT22', 'AmmoServiceReleaseClaimT22'):
        chunks.append(function(service, name))
    chunks.append(function(supply, 'SupplyWantedT2'))
    chunks.append(function(h36, 'PhysCancelUnenteredEquipmentH36'))
    production = '\n'.join(chunks)
    # Assert that the tested scheduler is live in BOTH production adapters.
    assert 'if(AmmoServiceOwnsT22()){AmmoServiceScriptT22(ctx,in);return;}' in supply
    assignments = re.findall(r'g_ammo_current22\[[^\]]+\]\s*=(?!=)', service)
    assert len(assignments) == 1, assignments  # all production mutations use SetCurrent
    native = (root / 'src/vrik/physical_native.inl').read_text()
    assert 'if(!PhysCurrentDrawIntentD48(frame.input,c))' in native
    assert 'PhysCancelUnenteredEquipmentH36(e,c)' in native
    assert 'PhysApplyHolsterRejectionsD48(input,equipment,holsters)' in native
    (out / 'holster_lifecycle_actual_d48.inc').write_text(production)
    command = ['g++', '-std=c++20', '-O1', '-pthread', '-Wno-misleading-indentation',
               '-I' + str(root / 'src'), '-I' + str(out), str(HERE / 'tests/holster_lifecycle_d48_test.cpp'),
               '-o', str(out / 'holster_lifecycle_d48')]
    result = {'native_execution': False, 'source': str(root), 'command': command,
              'source_sha256': {str(x.relative_to(root)): hashlib.sha256(x.read_bytes()).hexdigest()
                                for x in (service_path, supply_path, h36_path)}}
    compile_result = subprocess.run(command, capture_output=True, text=True)
    (out / 'compile.log').write_text(compile_result.stdout + compile_result.stderr)
    result['compile_returncode'] = compile_result.returncode
    if compile_result.returncode:
        print(compile_result.stderr)
    else:
        run = subprocess.run([str(out / 'holster_lifecycle_d48')], capture_output=True, text=True)
        (out / 'run.log').write_text(run.stdout + run.stderr)
        result['run_returncode'] = run.returncode
        print(run.stdout, end='')
        missing_term = 'g_ammo_cleanup_pending_d48.load(std::memory_order_acquire)||'
        assert production.count(missing_term) == 1
        (out / 'holster_lifecycle_actual_d48.inc').write_text(production.replace(missing_term, ''))
        red_build = subprocess.run(command[:-1] + [str(out / 'holster_lifecycle_negative')], capture_output=True, text=True)
        (out / 'negative_compile.log').write_text(red_build.stdout + red_build.stderr)
        if red_build.returncode == 0:
            red = subprocess.run([str(out / 'holster_lifecycle_negative')], capture_output=True, text=True)
            (out / 'negative_run.log').write_text(red.stdout + red.stderr)
            result['negative_control'] = {'removed_only': missing_term, 'returncode': red.returncode,
                'expected_failure_observed': red.returncode != 0 and 'OFF cleanup still schedules exact token work' in red.stdout}
        else:
            result['negative_control'] = {'compile_returncode': red_build.returncode, 'expected_failure_observed': False}
        (out / 'holster_lifecycle_actual_d48.inc').write_text(production)
    passed = result.get('run_returncode') == 0 and result.get('negative_control', {}).get('expected_failure_observed', False)
    result['passed'] = passed
    (out / 'results.json').write_text(json.dumps(result, indent=2) + '\n')
    print('D48 missing cleanup scheduling negative control:', 'CAUGHT' if passed else 'FAILED')
    return 0 if passed else 1


if __name__ == '__main__':
    raise SystemExit(main())
