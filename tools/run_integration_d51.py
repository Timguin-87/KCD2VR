#!/usr/bin/env python3
"""Check D51's shared body owner and recorder integration on the host.

Executes existing production fixtures. Native calls are mocked; no Windows
DLL, SEH, game or headset result is implied by a passing host test.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import subprocess


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--out', type=Path, required=True)
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[1]
    out = args.out.resolve()
    if out == root or root in out.parents:
        parser.error('--out must be outside the source tree')
    out.mkdir(parents=True, exist_ok=True)
    rows = []
    for name in ('riding_ownership_d50', 'diagnostic_routes_d49'):
        exe, dep = out / name, out / (name + '.d')
        command = [os.environ.get('CXX', 'g++'), '-std=c++23', '-O0', '-g0', '-pthread',
                   '-fms-extensions', '-ffunction-sections', '-fdata-sections', '-Wno-subobject-linkage',
                   '-D_ReturnAddress=HostReturnAddress', '-D__fastcall=', '-MMD', '-MF', str(dep),
                   '-I' + str(root / 'tests/syntax_shims'), '-I' + str(root / 'tests/openxr_sdk'),
                   '-I' + str(root / 'src'), str(root / 'tests' / (name + '_test.cpp')),
                   '-Wl,--gc-sections', '-o', str(exe)]
        build = subprocess.run(command, capture_output=True, text=True)
        (out / (name + '.build.log')).write_text(build.stdout + build.stderr)
        row = {'test': name, 'command': command, 'compile_exit': build.returncode}
        if build.returncode == 0:
            run = subprocess.run([str(exe)], capture_output=True, text=True)
            row.update(run_exit=run.returncode, output=run.stdout + run.stderr)
            (out / (name + '.run.log')).write_text(row['output'])
        dependencies = {}
        if dep.is_file():
            for token in dep.read_text().replace('\\\n', ' ').split()[1:]:
                path = Path(token).resolve()
                if path.is_file() and root in path.parents:
                    dependencies[path.relative_to(root).as_posix()] = hashlib.sha256(path.read_bytes()).hexdigest()
        row['dependencies'] = dependencies
        row['passed'] = row.get('run_exit') == 0
        rows.append(row)
        print(name, 'PASS' if row['passed'] else 'FAIL', row.get('output', build.stderr)[-1200:], flush=True)
    receipt = {'tests': rows, 'passed': all(r['passed'] for r in rows),
               'windows_build': 'NOT_RUN', 'windows_seh': 'NOT_EXECUTED', 'headset': 'NOT_RUN'}
    (out / 'RESULTS.json').write_text(json.dumps(receipt, indent=2) + '\n')
    return 0 if receipt['passed'] else 1


if __name__ == '__main__':
    raise SystemExit(main())
