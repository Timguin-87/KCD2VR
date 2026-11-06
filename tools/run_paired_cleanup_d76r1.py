#!/usr/bin/env python3
"""Exercise the actual paired adapter's cleanup ownership and its D76 control."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import shlex
import subprocess


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--source', type=Path, default=Path(__file__).resolve().parents[1])
    parser.add_argument('--baseline', type=Path, required=True,
                        help='Exact unmodified D76 source; runs the same cleanup fixture')
    parser.add_argument('--out', type=Path, required=True)
    args = parser.parse_args()
    source, baseline, out = args.source.resolve(), args.baseline.resolve(), args.out.resolve()
    if out == source or source in out.parents or out == baseline or baseline in out.parents:
        parser.error('output must be outside both production source trees')
    out.mkdir(parents=True, exist_ok=True)
    fixture = source / 'tests/subsystems/masterstrike_combos/lease_cleanup_d76r1_test.cpp'
    adapter = Path('src/subsystems/masterstrike_combos/ms_reaction_owner_d62c.inl')
    records = []
    cases = [
        ('cleanup_current', source, fixture, ['-O0'], False),
        ('cleanup_sanitized', source, fixture,
         ['-O1', '-fsanitize=address,undefined', '-fno-sanitize-recover=all',
          '-fno-omit-frame-pointer', '-fno-pie', '-no-pie'], False),
        ('cleanup_D76_control', baseline, fixture, ['-O0'], True),
        ('native_pair_d72', source, source / 'tests/native_pair_d72_test.cpp', ['-O0'], False),
        ('ms_paired_timing_d62c', source, source / 'tests/ms_paired_timing_d62c_test.cpp', ['-O0'], False),
        ('final_combo_contact_d72', source, source / 'tests/final_combo_contact_d72_test.cpp', ['-O0'], False),
    ]
    for name, production, test, flags, control in cases:
        executable, dependency = out / name, out / (name + '.d')
        command = [os.environ.get('CXX', 'g++'), '-std=c++20', '-g0', '-pthread',
                   *flags, '-I' + str(production / 'src'), '-MMD', '-MF', str(dependency)]
        if test == fixture:
            command += ['-DKCD2VR_MS_ADAPTER_D76R1="' + str(production / adapter) + '"']
        command += [str(test), '-o', str(executable)]
        build = subprocess.run(command, text=True, capture_output=True)
        build_log = out / (name + '.build.log')
        build_log.write_text(build.stdout + build.stderr)
        record = {'name': name, 'source': str(production), 'fixture': str(test),
                  'fixture_sha256': digest(test), 'control_expected_failure': control,
                  'compile_command': command, 'compile_exit': build.returncode,
                  'build_log': str(build_log), 'build_log_sha256': digest(build_log)}
        output = ''
        if build.returncode == 0:
            env = dict(os.environ, ASAN_OPTIONS='detect_leaks=0',
                       UBSAN_OPTIONS='print_stacktrace=1:halt_on_error=1')
            run = subprocess.run([str(executable)], text=True, capture_output=True, env=env)
            output = run.stdout + run.stderr
            log = out / (name + '.log');log.write_text(output)
            record.update(run_exit=run.returncode, log=str(log), log_sha256=digest(log), output=output)
        dependencies = {}
        if dependency.is_file():
            for token in shlex.split(dependency.read_text().replace('\\\n', ' '))[1:]:
                path = Path(token)
                if path.is_file():
                    dependencies[str(path.resolve())] = digest(path)
        record['dependencies'] = dependencies
        match = re.search(r'D76R1 paired cleanup: (\d+)/(\d+)', output)
        if match:
            passed, total = map(int, match.groups())
            record.update(checks=total, passed_checks=passed, failed_checks=total-passed)
        record['failure_messages'] = [line[5:] for line in output.splitlines() if line.startswith('FAIL ')]
        record['passed'] = build.returncode == 0 and record.get('run_exit') == (1 if control else 0)
        if control:
            record['passed'] &= bool(record['failure_messages']) and record.get('failed_checks', 0) > 0
        record['passed'] &= 'AddressSanitizer' not in output and 'runtime error:' not in output
        records.append(record)
        print(name, 'PASS' if record['passed'] else 'FAIL', output.strip()[-1200:], flush=True)
    # The change intentionally leaves selection/admission, pair policy, final
    # contact recipe semantics and the preference gate byte-identical to D76.
    retained = {}
    for path in [
        'src/subsystems/masterstrike_combos/ms_reaction_owner_d62c.h',
        'src/subsystems/masterstrike_combos/native_pair_d72.h',
        'src/subsystems/masterstrike_combos/final_combo_contact_d72.h',
        'src/vrik/instant_activities_d72.h',
    ]:
        before, after = digest(baseline / path), digest(source / path)
        retained[path] = {'D76_sha256': before, 'current_sha256': after, 'unchanged': before == after}
    payload = {
        'passed': all(row['passed'] for row in records) and all(row['unchanged'] for row in retained.values()),
        'scope': 'Complete production paired adapter and production membership walk; cleanup cases seed an already admitted row. Native memory, timing and unrelated engine leaves are controlled.',
        'tests': records, 'retained_policies': retained,
        'adapter_sha256': digest(source / adapter), 'D76_adapter_sha256': digest(baseline / adapter),
        'native_game': 'NOT_RUN', 'native_SEH': 'NOT_RUN', 'headset': 'NOT_RUN',
        'DLL_build': 'NOT_RUN', 'native_pair_admission_integration': 'NOT_RUN',
    }
    (out / 'RESULTS.json').write_text(json.dumps(payload, indent=2) + '\n')
    return 0 if payload['passed'] else 1


if __name__ == '__main__':
    raise SystemExit(main())
