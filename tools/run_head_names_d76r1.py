#!/usr/bin/env python3
"""Host verification of actual D76R1 label adapters and D76 camera coordinates.

Use --baseline /path/to/untouched/D76 to run the exact D76 hand-only adapter
through the same name regression fixture. This does not emulate a headset.
"""
from pathlib import Path
import argparse
import hashlib
import json
import os
import shlex
import subprocess
import sys

ap = argparse.ArgumentParser(description=__doc__)
ap.add_argument('--out', required=True, type=Path)
ap.add_argument('--baseline', type=Path)
args = ap.parse_args()
root = Path(__file__).resolve().parents[1]
out = args.out.resolve()
out.mkdir(parents=True, exist_ok=True)
compiler = shlex.split(os.environ.get('CXX', 'c++'))
rows = []

def run(name, command, expected=0):
    r = subprocess.run([str(x) for x in command], cwd=root, capture_output=True, text=True)
    (out / (name + '.log')).write_text(r.stdout + r.stderr)
    rows.append({'name': name, 'command': [str(x) for x in command],
                 'exit_code': r.returncode, 'expected_exit_code': expected,
                 'passed': r.returncode == expected})
    print(name + ': ' + ('PASS' if r.returncode == expected else 'FAIL'))
    if r.returncode != expected:
        print(r.stdout + r.stderr)
        raise RuntimeError(name)
    return r.stdout

try:
    actual = out / 'actual_visual_ray_d76r1.inl'
    run('extract_visual', [sys.executable, 'tests/extract_visual_ray_d75.py', '--out', actual])
    run('extract_coordinates', [sys.executable, 'tools/run_roomscale_camera_coordinate_d76.py',
                                '--out', out, '--extract-only'])
    suites = [
        ('head_names', 'tests/alchemy_head_name_d75_test.cpp', ['-I', 'src', '-I', 'tests/syntax_shims']),
        ('visual_ray', 'tests/visual_ray_d75_test.cpp', ['-DKCD2VR_VISUAL_RAY_D75_EXTRACT="' + str(actual) + '"']),
        ('coordinates', 'tests/roomscale_camera_coordinate_d76_test.cpp', [])]
    for name, source, flags in suites:
        exe = out / name
        run(name + '_compile', compiler + ['-std=c++20', '-O1', '-g', '-pthread'] + flags + [source, '-o', exe])
        print(run(name, [exe]).strip())
    if args.baseline:
        source = (args.baseline.resolve() / 'src/interaction_alignment.cpp').read_text()
        start = source.index('bool AlchemyHandNameWantedD72() noexcept {')
        end = source.index('\nvoid SetDoorDiagnosticsCaptureC54R3(', start)
        adapter = out / 'baseline_name_adapter_d76.inl'
        adapter.write_text(source[start:end])
        exe = out / 'head_names_baseline'
        run('baseline_compile', compiler + ['-std=c++20', '-O1', '-g', '-pthread', '-I', 'src', '-I',
            'tests/syntax_shims', '-DKCD2VR_NAME_ADAPTER_D75="' + str(adapter) + '"',
            'tests/alchemy_head_name_d75_test.cpp', '-o', exe])
        result = run('baseline_expected_failure', [exe], expected=1)
        if '54 checks, 27 failures' not in result:
            raise RuntimeError('counterfactual failed in an unexpected way')
        print('Untouched D76 adapter: 54 checks, 27 expected failures')
finally:
    tracked = ['src/camera_hook.cpp', 'src/camera_hook.h', 'src/interaction_alignment.cpp',
               'src/interaction_alignment.h', 'src/interaction_alchemy_names_d75.inl',
               'src/vrik/visual_ray_d75.h', 'src/subsystems/hud_ui_controls/runtime/alchemy_head_name_d75.h',
               'src/subsystems/alchemy/bridge67.h', 'src/subsystems/alchemy/input67.h',
               'tests/alchemy_head_name_d75_test.cpp', 'tests/visual_ray_d75_test.cpp',
               'tests/roomscale_camera_coordinate_d76_test.cpp',
               'tests/generated/roomscale_camera_coordinate_d76.inc']
    (out / 'RESULTS.json').write_text(json.dumps({'scope': 'actual production adapter/translation blocks; controlled native/camera leaves',
        'headset': 'NOT_RUN', 'results': rows,
        'source_sha256': {p: hashlib.sha256((root / p).read_bytes()).hexdigest() for p in tracked}}, indent=2) + '\n')
