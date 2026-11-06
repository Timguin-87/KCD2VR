#!/usr/bin/env python3
"""Run the actual H40 per-VM readiness regression and retained door coverage.

The optional R96 Git baseline is a genuine previous production header, not a
reimplementation. Native script replies and pose permission are controlled
leaves; these host tests do not execute retail Lua or a Windows game process.
"""
from pathlib import Path
import argparse
import hashlib
import json
import shutil
import subprocess


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--source', type=Path, default=Path(__file__).resolve().parents[1])
    parser.add_argument('--out', type=Path, required=True)
    parser.add_argument('--cxx', default='g++')
    parser.add_argument('--baseline-git', help='Optional genuine R96 source revision in this checkout')
    args = parser.parse_args()
    root, out = args.source.resolve(), args.out.resolve()
    out.mkdir(parents=True, exist_ok=True)
    header = Path('src/subsystems/doors/hand_only_h40.h')
    test = Path('tests/door_vm_probe_r96r1_test.cpp')
    result = {
        'scope': 'Actual CandidatePress and ServiceState; native script and current-pose leaves controlled',
        'runtime_exclusions': ['retail Lua', 'Windows DLL', 'HMD/game'],
        'compiler': subprocess.check_output([args.cxx, '--version'], text=True).splitlines()[0],
        'source_sha256': {str(p): sha(root / p) for p in (
            header, test, Path('tests/door_recovery_r96_test.cpp'),
            Path('tests/door_press_why_r9_test.cpp'),
            Path('src/subsystems/doors/hand_only_h40_current.inl'))},
        'entries': [],
    }

    def run(name, source, flags, include_root, negative=False):
        folder = out / name
        folder.mkdir(exist_ok=True)
        command = [args.cxx, '-std=c++20', '-UNDEBUG', '-pthread', *flags,
                   '-I', str(include_root), str(source), '-o', str(folder / 'test')]
        built = subprocess.run(command, text=True, capture_output=True)
        (folder / 'build.log').write_text(built.stdout + built.stderr)
        row = {'name': name, 'command': command, 'compile': built.returncode,
               'expected': 'compiled assertion failure' if negative else 'pass'}
        if built.returncode == 0:
            tested = subprocess.run([str(folder / 'test')], text=True, capture_output=True)
            output = tested.stdout + tested.stderr
            (folder / 'run.log').write_text(output)
            row.update(run=tested.returncode, output=output)
            row['pass'] = (tested.returncode == 1 and
                           'FAIL_DOOR_VM_R96R1 replacement VM must probe original token before any feed' in output
                           if negative else tested.returncode == 0)
        else:
            row['pass'] = False
        result['entries'].append(row)
        print(name, 'PASS' if row['pass'] else 'FAIL', flush=True)

    for opt in ('-O0', '-O2'):
        run('current_' + opt[1:], root / test, [opt], root / 'src')
    run('retained_R96_admission_current_service', root / 'tests/door_recovery_r96_test.cpp', ['-O2'], root / 'src')

    if args.baseline_git:
        baseline = subprocess.check_output(['git', '-C', str(root), 'show', args.baseline_git + ':' + header.as_posix()])
        result['baseline_git'] = args.baseline_git
        result['baseline_header_sha256'] = hashlib.sha256(baseline).hexdigest()
        overlay = out / 'baseline_source'
        if overlay.exists():
            raise RuntimeError('Use a fresh --out directory for the optional baseline comparison')
        shutil.copytree(root / 'src', overlay / 'src')
        (overlay / header).write_bytes(baseline)
        run('genuine_R96_negative', root / test, ['-O2'], overlay / 'src', negative=True)
    result['pass'] = all(row['pass'] for row in result['entries'])
    (out / 'RESULT.json').write_text(json.dumps(result, indent=2) + '\n')
    return 0 if result['pass'] else 1


if __name__ == '__main__':
    raise SystemExit(main())
