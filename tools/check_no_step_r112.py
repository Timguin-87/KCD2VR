#!/usr/bin/env python3
"""Actual current-action/early-position veto regression for R112.

Compiles the literal production predicates/dispatch and integrated selection
controls, with the existing bounded-memory D40 fixture leaves. It does not
execute a native mover or establish why an actor moved in a headset capture.
"""
from pathlib import Path
import argparse
import hashlib
import json
import os
import subprocess
from check_body_ownership_r112 import block

ROOT = Path(__file__).resolve().parents[1]
BASELINE = 'c2a696b'


def sha(data):
    return hashlib.sha256(data).hexdigest()


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--source', type=Path, default=ROOT)
    ap.add_argument('--out', type=Path, required=True)
    ap.add_argument('--skip-baseline', action='store_true')
    ap.add_argument('--baseline-source', type=Path)
    ap.add_argument('--sanitizers', action='store_true')
    args = ap.parse_args()
    root, out = args.source.resolve(), args.out.resolve()
    out.mkdir(parents=True, exist_ok=True)
    results = []
    for baseline in ([False] if args.skip_baseline else [False, True]):
        label = 'baseline' if baseline else 'current'
        destination = out / label
        destination.mkdir(parents=True, exist_ok=True)
        files = {}
        def read(path):
            if baseline and args.baseline_source is None:
                data = subprocess.check_output(['git', 'show', BASELINE + ':' + path], cwd=root)
            else:
                data = ((args.baseline_source if baseline else root) / path).read_bytes()
            files[path] = sha(data)
            return data.decode().replace('\r\n', '\n')
        owner = read('src/subsystems/base/native_motion_owner.inl')
        controls = read('src/subsystems/base/combat_mode_controls.inl')
        frik = read('src/frik_pose_probe.cpp')
        fixture_prefix = (root / 'tests/combat_no_turn_d40_test.cpp').read_text().split('#include "generated_combat_no_turn_d40.inc"')[0]
        old_sync = 'static bool CombatSynchronized59c10r9(){return synchronizedD40;}\n'
        assert fixture_prefix.count(old_sync) == 1
        fixture_prefix = fixture_prefix.replace(old_sync, '')
        # R114: combat_no_turn_d40_test.cpp also stubs the integrated gate (R113); this fixture brings its own production one.
        fixture_prefix = '\n'.join(l for l in fixture_prefix.split('\n') if not l.startswith('static bool CombatIntegrated178r22c2Enabled()'))
        (destination / 'motion_fixture_prefix_r112.inc').write_text(fixture_prefix)
        globals = owner[owner.index('using NativeAlignmentQuery112='):owner.index('// D42 selection')]
        literal = globals + '\n'
        literal += block(controls, 'bool CombatExperiment175Enabled(') + '\n'
        literal += block(controls, 'bool CombatIntegrated178r22c2Enabled(') + '\n'
        literal += block(frik, 'bool CombatSynchronized59c10r9(') + '\n'
        for name in ('NativeNoTurnSelectedD42', 'CurrentOrdinaryMotion59c3', 'NativeFistMotionD37',
                     'NativeMotionContext112', 'NativePositionCommandOwned36'):
            literal += block(owner, 'static bool ' + name + '(') + '\n'
        literal += block(owner, 'static void NativePositionDispatch36(') + '\n'
        (destination / 'motion_position_r112.inc').write_text(literal)
        fixture = root / 'tests/no_step_r112_test.cpp'
        for opt in ['-O0', '-O2']:
            name = label + opt
            exe = destination / ('no_step_r112' + opt)
            command = [os.environ.get('CXX', 'g++'), '-std=c++20', opt, '-pthread',
                       '-I' + str(destination), '-I' + str(root / 'src'), str(fixture), '-o', str(exe)]
            if args.sanitizers:
                command[1:1] = ['-fsanitize=address,undefined', '-fno-omit-frame-pointer', '-fno-pie', '-no-pie']
            built = subprocess.run(command, capture_output=True, text=True)
            (destination / (name + '.build.log')).write_text(built.stdout + built.stderr)
            record = {'name': name, 'command': command, 'compile': built.returncode,
                      'source_sha256': files, 'fixture_sha256': sha(fixture.read_bytes()),
                      'existing_native_leaves_sha256': sha(fixture_prefix.encode()),
                      'literal_slice_sha256': sha(literal.encode())}
            if not built.returncode:
                env = dict(os.environ, ASAN_OPTIONS='detect_leaks=0', UBSAN_OPTIONS='halt_on_error=1:print_stacktrace=1')
                ran = subprocess.run([str(exe)], capture_output=True, text=True, env=env)
                output = ran.stdout + ran.stderr
                (destination / (name + '.run.log')).write_text(output)
                failures = [line for line in output.splitlines() if line.startswith('FAIL ')]
                expected = (ran.returncode != 0 and failures and
                            all('R112_FIXED:' in line for line in failures)) if baseline else ran.returncode == 0
                record.update(run=ran.returncode, failures=failures, passed=bool(expected), output=output)
            else:
                record['passed'] = False
            results.append(record)
            print(name, 'PASS' if record['passed'] else 'FAIL', flush=True)
    report = {'passed': all(r['passed'] for r in results), 'results': results,
              'baseline': 'SKIPPED' if args.skip_baseline else str(args.baseline_source or BASELINE),
              'sanitizers': args.sanitizers, 'leak_detection': False,
              'limits': ['No new native ABI or pin; actual R111 contract gates retained.',
                         'Bounded host memory, owner/clock/input/native-call leaves; retail native mover NOT_RUN.',
                         'This mode repair does not cancel the synchronized capture\'s unexplained entity travel.',
                         'Walking/roomscale and arbitrary native root animation are not intercepted.']}
    (out / 'RESULT.json').write_text(json.dumps(report, indent=2) + '\n')
    print(out / 'RESULT.json')
    return 0 if report['passed'] else 1


if __name__ == '__main__':
    raise SystemExit(main())
