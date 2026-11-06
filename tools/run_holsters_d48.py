#!/usr/bin/env python3
"""Run focused D48 production holster/release adapters and record evidence.

The optional baseline runs the NEW acquisition assertions against unchanged
older production headers. A successful build followed by the expected failing
assertions is the counterfactual, not a native game or headset execution.
"""
import argparse
import hashlib
import json
import subprocess
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path

HERE = Path(__file__).resolve().parents[1]


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--out', type=Path, required=True)
    p.add_argument('--baseline', type=Path)
    a = p.parse_args()
    out = a.out.resolve()
    out.mkdir(parents=True, exist_ok=True)
    cases = [
        ('holster_producer', 'holster_instant_d44_test.cpp', []),
        ('initial_acquisition', 'initial_acquire_sweep_d44_test.cpp', ['-DD44_INITIAL_SWEEP']),
        ('physical_regression', 'physical_weapon_test.cpp', []),
        ('front_holster', 'front_holster_d48_test.cpp', []),
        ('stock_gesture', 'native_supply_t2_test.cpp', []),
        ('stowed_policy', 'stowed_hide_hw_test.cpp', []),
        ('bow_reveal', 'draw_visibility_d45_test.cpp', []),
    ]

    def compile_run(case, source=HERE, negative=False):
        name, file, flags = case
        directory = out / name
        directory.mkdir(exist_ok=True)
        binary = directory / name
        fixture = HERE / 'tests' / file
        cmd = ['g++', '-std=c++20', '-O1', '-pthread', '-I' + str(source / 'src'), *flags, str(fixture), '-o', str(binary)]
        c = subprocess.run(cmd, capture_output=True, text=True)
        (directory / 'compile.log').write_text(c.stdout + c.stderr)
        row = {'name': name, 'command': cmd, 'compile_returncode': c.returncode,
               'fixture_sha256': hashlib.sha256(fixture.read_bytes()).hexdigest(), 'native_execution': False}
        if c.returncode == 0:
            r = subprocess.run([str(binary)], capture_output=True, text=True)
            (directory / 'run.log').write_text(r.stdout + r.stderr)
            row['run_returncode'] = r.returncode
            row['passed'] = r.returncode == 0 if not negative else r.returncode != 0 and \
                'FAIL budget or unknown acquisition never drags the visible wrist toward native animation' in r.stdout
            row['summary'] = r.stdout.strip().splitlines()[-1] if r.stdout.strip() else ''
        else:
            row['passed'] = False
        (directory / 'result.json').write_text(json.dumps(row, indent=2) + '\n')
        return row

    with ThreadPoolExecutor(max_workers=2) as pool:
        rows = list(pool.map(compile_run, cases))
    for name, script in [('stowed_actual', 'run_stowed_attachments_d44.py'), ('lifecycle_actual', 'run_holster_lifecycle_d48.py'), ('selection_actual', 'run_holster_selection_d48.py')]:
        command = ['python3', str(HERE / 'tools' / script), '--out', str(out / name)]
        if name == 'selection_actual' and a.baseline:
            command += ['--baseline', str(a.baseline.resolve())]
        result = subprocess.run(command, capture_output=True, text=True)
        (out / (name + '.log')).write_text(result.stdout + result.stderr)
        rows.append({'name': name, 'command': command, 'run_returncode': result.returncode, 'passed': result.returncode == 0})
    if a.baseline:
        baseline = a.baseline.resolve()
        rows.append(compile_run(('baseline_counterfactual', 'initial_acquire_sweep_d44_test.cpp', ['-DD44_INITIAL_SWEEP']), baseline, True))
    report = {'passed': all(row['passed'] for row in rows), 'tests': rows,
              'native_game_execution': False, 'headset_tested': False,
              'scope': 'production FSM/kernel/adapter tests; native leaves explicitly substituted where named'}
    (out / 'RESULTS.json').write_text(json.dumps(report, indent=2) + '\n')
    for row in rows:
        print(('PASS' if row['passed'] else 'FAIL'), row['name'], row.get('summary', ''))
    return 0 if report['passed'] else 1


if __name__ == '__main__':
    raise SystemExit(main())
