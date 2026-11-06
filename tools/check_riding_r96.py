#!/usr/bin/env python3
"""Focused R96 riding lifecycle, seat foundation and pose restoration tests.

The native animation adapter and finisher implementation are compiled directly.
The small restoration fixture compiles literal production boundaries extracted
by pose_restore_extract_r96.py. Native memory/clock/identity are host leaves;
this is not a Windows DLL build, retail game test or HMD observation.
"""
from pathlib import Path
import argparse
from concurrent.futures import ThreadPoolExecutor
import json
import subprocess
import sys

from host_compiler import command, executable_path
from pose_restore_extract_r96 import extract, sha
from check_finisher_upper_d83 import run_one as run_finisher


def run_case(root, out, compiler, name, extra=()):
    exe = executable_path(out / name)
    fixture = root / 'tests' / (name + '_test.cpp')
    includes = [*extra, root / 'src', root / 'tests/syntax_shims', root / 'tests/openxr_sdk']
    cmd = command(fixture, exe, includes, compiler)
    built = subprocess.run(cmd, capture_output=True, text=True)
    (out / (name + '.build.log')).write_text(built.stdout + built.stderr)
    record = {'test': name, 'command': cmd, 'fixture_sha256': sha(fixture),
              'compile': built.returncode, 'passed': False}
    if built.returncode == 0:
        ran = subprocess.run([str(exe)], capture_output=True, text=True)
        (out / (name + '.run.log')).write_text(ran.stdout + ran.stderr)
        record.update(run=ran.returncode, passed=ran.returncode == 0, stdout=ran.stdout)
    return record


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--source', type=Path, default=Path(__file__).resolve().parents[1])
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--compiler', default='g++', help='C++20 GCC/Clang host compiler')
    args = parser.parse_args()
    root, out = args.source.resolve(), args.output.resolve()
    out.mkdir(parents=True, exist_ok=True)
    provenance = extract(root, out / 'restore_slices')
    names = ['riding_lifecycle_r96', 'riding_foundation_r96', 'riding_handoff_d81',
             'riding_pose_d80', 'riding_stationary_r92', 'riding_seated_r95',
             'tracked_reference_d86', 'tracked_frame51', 'body_height_r98',
             'pose_restore_recovery_r96']
    def run(name):
        extra = [out / 'restore_slices'] if name == 'pose_restore_recovery_r96' else []
        return run_case(root, out, args.compiler, name, extra)
    with ThreadPoolExecutor(max_workers=2) as pool:
        records = list(pool.map(run, names))
    finisher = run_finisher(root, root / 'tests/finisher_upper_d83_test.cpp',
                           out / 'finisher', args.compiler, ['-O2'])
    inputs = ['src/animation_filter.cpp', 'src/vrik/riding_vrik_d76.h',
              'src/vrik/tracked_frame51.h', 'src/vrik/coherent_pose.inl',
              'src/vrik/finisher_upper_d83.inl', 'src/vrik/pose_restore_debt_r96.h',
              'src/core/observed_load_recovery_r96.h',
              'src/subsystems/throw_drop/inventory_lifecycle_t8.h']
    result = {'passed': all(r['passed'] for r in records) and finisher['exit'] == 0,
              'results': records, 'finisher': finisher, 'restore_extraction': provenance,
              'source_sha256': {p: sha(root / p) for p in inputs},
              'limits': ['Windows DLL/game/HMD NOT_RUN',
                         'Complete coherent solver fixture is stale and not compiled here; restoration uses literal focused source slices.',
                         'Some older riding tests invoke the same handoff fixture, so their displayed assertion counts overlap.']}
    (out / 'RESULT.json').write_text(json.dumps(result, indent=2) + '\n')
    for r in records:
        print(r['test'], 'PASS' if r['passed'] else 'FAIL')
    print('finisher_upper_d83', 'PASS' if finisher['exit'] == 0 else 'FAIL',
          len(finisher['passed']), 'passed', len(finisher['failed']), 'failed')
    print('RESULT', out / 'RESULT.json')
    return 0 if result['passed'] else 1


if __name__ == '__main__':
    raise SystemExit(main())
