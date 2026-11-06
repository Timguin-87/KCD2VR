#!/usr/bin/env python3
"""Actual native weapon client reset recovery, with explicit native leaves.

Optionally execute identical cases on a git revision of the production client.
That control changes only the included production translation unit; the test
and provider code are identical. No Windows/native-game execution is claimed.
"""
from pathlib import Path
import argparse
import hashlib
import json
import subprocess

CASES = ['recovery', 'remove-failure', 'release-failure', 'unregister-failure',
         'constraint-debt', 'uncertain-pair', 'old-generation', 'unknown-factory', 'world-replaced']
INSTALL_CASES = ['register-refusal', 'phase-refusal', 'fault-refusal',
                 'phase-preflight', 'clearance-preflight']
PRODUCTION = 'src/subsystems/physics/runtime/native_weapon_body_t4.cpp'


def sha(p):
    return hashlib.sha256(p.read_bytes()).hexdigest()


def run(root, out, compiler, fixture, cases):
    out.mkdir(parents=True, exist_ok=True)
    exe = out / 'test'
    cmd = [compiler, '-std=c++23', '-O2', '-pthread', '-I', str(root / 'src'),
           '-I', str(root / 'src/subsystems/physics/runtime'),
           '-I', str(root / 'tests/syntax_shims'), str(fixture), '-o', str(exe)]
    built = subprocess.run(cmd, capture_output=True, text=True)
    (out / 'build.log').write_text(built.stdout + built.stderr)
    record = {'command': cmd, 'compile': built.returncode, 'fixture_sha256': sha(fixture), 'cases': []}
    if built.returncode == 0:
        for name, args in cases:
            ran = subprocess.run([str(exe), *args], capture_output=True, text=True)
            (out / (name + '.run.log')).write_text(ran.stdout + ran.stderr)
            record['cases'].append({'case': name, 'command': [str(exe), *args],
                                    'exit': ran.returncode, 'stdout': ran.stdout, 'stderr': ran.stderr})
    return record


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--source', type=Path, default=Path(__file__).resolve().parents[1])
    p.add_argument('--output', type=Path, required=True)
    p.add_argument('--compiler', default='g++')
    p.add_argument('--baseline-git', help='Optional immutable production revision, e.g. HEAD')
    args = p.parse_args()
    root, out = args.source.resolve(), args.output.resolve()
    out.mkdir(parents=True, exist_ok=True)
    fixture = root / 'tests/native_weapon_reset_recovery_r96_test.cpp'
    canonical = root / 'tests/native_weapon_body_t4_test.cpp'
    records = {
        'current_recovery': run(root, out / 'current', args.compiler, fixture, [(n, [n]) for n in CASES]),
        'existing_client': run(root, out / 'existing', args.compiler, canonical,
                               [('lifecycle', []), *[(n, [n]) for n in INSTALL_CASES]])}
    result = {'runs': records, 'source_sha256': {p: sha(root / p) for p in
              [PRODUCTION, 'tests/native_weapon_reset_recovery_r96_test.cpp',
               'tests/native_weapon_body_t4_test.cpp', 'src/native_physics_phase_t4.h']},
              'scope': 'Actual full native weapon client translation unit. Native phase/API, clock and memory leaves are controlled. Windows DLL/game/HMD NOT_RUN.'}
    if args.baseline_git:
        original = out / 'original';original.mkdir(parents=True, exist_ok=True)
        body = original / 'native_weapon_body_t4.cpp'
        body.write_bytes(subprocess.check_output(['git', 'show', args.baseline_git + ':' + PRODUCTION], cwd=root))
        provider = canonical.read_text()
        provider = provider.replace('#include "../src/native_weapon_body_t4.cpp"', '#include "' + str(body) + '"')
        provider = provider.replace('#include "../src/native_weapon_clearance_t4.cpp"',
                                    '#include "' + str(root / 'src/native_weapon_clearance_t4.cpp') + '"')
        (original / canonical.name).write_text(provider)
        (original / fixture.name).write_bytes(fixture.read_bytes())
        records['original_recovery'] = run(root, original, args.compiler, original / fixture.name, [(n, [n]) for n in CASES])
        result['original'] = {'revision': subprocess.check_output(['git', 'rev-parse', args.baseline_git], cwd=root, text=True).strip(),
                              'production_sha256': sha(body),
                              'provider_adjustment': 'Only include paths select the literal original production body and unchanged current clearance implementation.',
                              'provider_sha256': sha(original / canonical.name)}
    passed = all(r['compile'] == 0 and r['cases'] and all(c['exit'] == 0 for c in r['cases'])
                 for name, r in records.items() if name != 'original_recovery')
    if args.baseline_git:
        original = records['original_recovery']
        failed = {c['case'] for c in original['cases'] if c['exit'] != 0}
        passed = passed and original['compile'] == 0 and {'recovery', 'unknown-factory'}.issubset(failed)
    result['passed'] = passed
    (out / 'RESULT.json').write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps({'passed': passed, 'runs': {n: {'compile': r['compile'],
          'passed': sum(c['exit'] == 0 for c in r['cases']), 'cases': len(r['cases'])}
          for n, r in records.items()}, 'result': str(out / 'RESULT.json')}, indent=2))
    return 0 if passed else 1


if __name__ == '__main__':
    raise SystemExit(main())
