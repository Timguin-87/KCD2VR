#!/usr/bin/env python3
"""MSVC execution of exact production finisher callbacks; no game or DLL run."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import subprocess
import sys


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--out', type=Path, required=True)
    p.add_argument('--compiler', type=Path, default=Path('C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.44.35207/bin/Hostx64/x64/cl.exe'))
    p.add_argument('--baseline', default='7dca976ad9022750b9eb2967b4b7d3cdc877a4e6')
    a = p.parse_args()
    source = Path(__file__).resolve().parents[1]
    out = a.out.resolve()
    if out == source or source in out.parents:
        p.error('Output must be outside the source tree')
    out.mkdir(parents=True, exist_ok=True)
    generated = out / 'generated'
    subprocess.run([sys.executable, str(source / 'tools/extract_combat_backups_d51.py'), '--source', str(source), '--out', str(generated)], check=True)
    old = out / 'baseline_generated'
    old.mkdir(exist_ok=True)
    callback = 'src/subsystems/masterstrike_combos/physical_finisher_gate_cb1.inl'
    before = subprocess.check_output(['git', 'show', a.baseline + ':' + callback], cwd=source).decode()
    before = before[:before.index('static void UninstallFinisherCB1(')]
    (old / 'finisher_callbacks_d51_production.inc').write_text(before)
    vc = a.compiler.resolve().parents[3]
    kits = Path('C:/Program Files (x86)/Windows Kits/10')
    version = '10.0.26100.0'
    env = dict(os.environ)
    env['INCLUDE'] = ';'.join(str(v) for v in [vc / 'include', kits / 'Include' / version / 'ucrt', kits / 'Include' / version / 'um', kits / 'Include' / version / 'shared'])
    env['LIB'] = ';'.join(str(v) for v in [vc / 'lib/x64', kits / 'Lib' / version / 'ucrt/x64', kits / 'Lib' / version / 'um/x64'])
    env['PATH'] = str(a.compiler.parent) + ';' + env.get('PATH', '')
    results = []
    for name, extracted, sanitize, expected in [('current', generated, False, 0), ('current_asan', generated, True, 0), ('baseline_callback_counterexample', old, False, 1)]:
        cmd = [str(a.compiler), '/nologo', '/std:c++20', '/EHsc', '/W4', '/I' + str(source / 'src'), '/I' + str(extracted),
               '/Fe' + str(out / (name + '.exe')), '/Fo' + str(out / (name + '.obj'))]
        if sanitize:
            cmd += ['/fsanitize=address', '/Zi', '/Fd' + str(out / (name + '.pdb'))]
        cmd += [str(source / 'tests/finisher_backups_d51_test.cpp')]
        build = subprocess.run(cmd, cwd=out, env=env, capture_output=True, text=True)
        (out / (name + '.build.log')).write_text(build.stdout + build.stderr)
        row = {'case': name, 'command': cmd, 'compile_exit': build.returncode, 'expected_run_exit': expected}
        if not build.returncode:
            run = subprocess.run([str(out / (name + '.exe'))], cwd=out, env=env, capture_output=True, text=True)
            row.update(run_exit=run.returncode, output=run.stdout + run.stderr)
            (out / (name + '.run.log')).write_text(row['output'])
        row['passed'] = row.get('run_exit') == expected and (expected != 1 or 'FAIL ' in row.get('output', ''))
        results.append(row)
        print(name, 'PASS' if row['passed'] else 'FAIL', row.get('output', build.stderr))
    files = [callback, 'src/subsystems/masterstrike_combos/physical_finisher_cb1.h', 'src/vrik/physical_native.inl',
             'src/diagnostics/combo_snapshot.h', 'src/diagnostics/capture168.h', 'tests/finisher_backups_d51_test.cpp',
             'tests/combat_backups_d51_host.h', 'tools/extract_combat_backups_d51.py', 'tools/run_finisher_query_d60.py']
    receipt = {'results': results, 'passed': all(r['passed'] for r in results),
               'baseline_callback_commit': a.baseline, 'baseline_callback_sha256': hashlib.sha256(before.encode()).hexdigest(),
               'counterexample_scope': 'Old complete callback body with current fixture and compatible current policy header; not a baseline DLL build',
               'source_sha256': {f: hashlib.sha256((source / f).read_bytes()).hexdigest() for f in files},
               'game': 'NOT_RUN', 'headset': 'NOT_RUN', 'DLL': 'ROOT_BUILDS_SEPARATELY'}
    (out / 'RESULTS.json').write_text(json.dumps(receipt, indent=2) + '\n')
    return 0 if receipt['passed'] else 1


if __name__ == '__main__':
    raise SystemExit(main())
