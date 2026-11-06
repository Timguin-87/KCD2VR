#!/usr/bin/env python3
"""Execute D51 backup production callbacks/TU and optional D50 counterexamples.

Records source/dependency hashes, compiler commands and output. Native IO is
mocked. This is host regression evidence, never a Windows DLL/headset verdict.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import subprocess
import sys


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--source', type=Path, default=Path(__file__).resolve().parents[1])
    parser.add_argument('--out', type=Path, required=True)
    parser.add_argument('--baseline', type=Path)
    parser.add_argument('--sanitize', action='store_true')
    args = parser.parse_args()
    fixtures = Path(__file__).resolve().parents[1]
    source, out = args.source.resolve(), args.out.resolve()
    if out == source or source in out.parents:
        parser.error('--out must be outside the source tree')
    out.mkdir(parents=True, exist_ok=True)
    generated = out / 'generated'
    subprocess.run([sys.executable, str(fixtures / 'tools/extract_combat_backups_d51.py'),
                    '--source', str(source), '--out', str(generated)], check=True)
    subprocess.run([sys.executable, str(fixtures / 'tools/generate_native_paired_aliases_d51.py'), '--check'], check=True)
    results = []

    def run(name, production, folder, extracted, counterexample=False):
        folder.mkdir(parents=True, exist_ok=True)
        exe, dep = folder / name, folder / (name + '.d')
        cmd = [os.environ.get('CXX', 'g++'), '-std=c++23', '-O0', '-g0', '-pthread', '-fms-extensions',
               '-ffunction-sections', '-fdata-sections', '-Wno-subobject-linkage', '-D_ReturnAddress=HostReturnAddress',
               '-D__fastcall=', '-MMD', '-MF', str(dep), '-I' + str(production / 'src'), '-I' + str(extracted),
               '-I' + str(fixtures / 'src'), '-I' + str(fixtures / 'tests/syntax_shims'), '-I' + str(fixtures / 'tests/openxr_sdk')]
        if name == 'native_paired_playback_d51':
            cmd += ['-DPRODUCTION_ANIMATION_SOURCE_D51="' + str(production / 'src/animation_filter.cpp') + '"']
        if args.sanitize:
            cmd += ['-fsanitize=address,undefined', '-fno-sanitize-recover=all', '-fno-omit-frame-pointer', '-fno-pie', '-no-pie']
        cmd += [str(fixtures / 'tests' / (name + '_test.cpp')), '-Wl,--gc-sections', '-o', str(exe)]
        build = subprocess.run(cmd, capture_output=True, text=True)
        (folder / (name + '.build.log')).write_text(build.stdout + build.stderr)
        row = {'test': name, 'source': str(production), 'counterexample': counterexample,
               'compile_command': cmd, 'compile_exit': build.returncode}
        if build.returncode == 0:
            env = dict(os.environ, ASAN_OPTIONS='detect_leaks=0', UBSAN_OPTIONS='print_stacktrace=1:halt_on_error=1')
            result = subprocess.run([str(exe)], capture_output=True, text=True, env=env)
            row.update(run_exit=result.returncode, output=result.stdout + result.stderr)
            (folder / (name + '.run.log')).write_text(row['output'])
        dependencies = {}
        if dep.is_file():
            for token in dep.read_text().replace('\\\n', ' ').split()[1:]:
                path = Path(token)
                if path.is_file():
                    dependencies[str(path.resolve())] = hashlib.sha256(path.read_bytes()).hexdigest()
        row['dependencies'] = dependencies
        # A counterexample must compile and fail assertions as expected. A
        # compiler error, crash or sanitizer diagnostic is never counted as proof.
        row['passed'] = row.get('run_exit') == (1 if counterexample else 0)
        if counterexample:
            row['passed'] = row['passed'] and 'FAIL ' in row.get('output', '')
        results.append(row)
        print(('D50 expected failure: ' if counterexample else '') + name,
              'PASS' if row['passed'] else 'FAIL', row.get('output', build.stderr)[-1600:], flush=True)

    for name in ('native_kill_anim_nf', 'live_settings_d39', 'finisher_backups_d51',
                 'native_master_backups_d51', 'native_paired_playback_d51'):
        run(name, source, out / 'current', generated)
    if args.baseline:
        baseline = args.baseline.resolve()
        old_generated = out / 'baseline_generated'
        subprocess.run([sys.executable, str(fixtures / 'tools/extract_combat_backups_d51.py'),
                        '--source', str(baseline), '--out', str(old_generated)], check=True)
        for name in ('finisher_backups_d51', 'native_master_backups_d51', 'native_paired_playback_d51'):
            run(name, baseline, out / 'baseline', old_generated, True)
    receipt = {'tests': results, 'passed': all(r['passed'] for r in results), 'sanitizers': args.sanitize,
               'native_game': 'NOT_RUN', 'Windows_SEH': 'NOT_EXECUTED', 'DLL': 'NOT_BUILT', 'headset': 'NOT_RUN'}
    (out / 'RESULTS.json').write_text(json.dumps(receipt, indent=2) + '\n')
    return 0 if receipt['passed'] else 1


if __name__ == '__main__':
    raise SystemExit(main())
