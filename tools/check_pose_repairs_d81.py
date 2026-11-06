#!/usr/bin/env python3
"""Run D81 mounted/contact/dice production-code fixtures and optional D80 controls.

No source-tree generated files are overwritten. Native geometry, clocks, Windows
memory reads and room-origin leaves are synthetic; this is not a game/SDK build.
"""
from pathlib import Path
import argparse
import concurrent.futures
import hashlib
import json
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]
EXTRACT = r'''
from pathlib import Path
import os,runpy,sys
source=Path(sys.argv[1]);output=Path(sys.argv[2]);output.mkdir(parents=True,exist_ok=True)
original_write=Path.write_text;original_replace=os.replace
def redirect(path):
 path=Path(path)
 return output/path.name if path.parent==source/'tests' else path
try:
 Path.write_text=lambda path,*a,**kw:original_write(redirect(path),*a,**kw)
 os.replace=lambda a,b:original_replace(redirect(a),redirect(b))
 for name in ('extract_adapter_types.py','extract_world_adapter_test.py','extract_activity_eye_camera43.py'):
  runpy.run_path(str(source/'tools'/name),run_name='__main__')
finally:
 Path.write_text=original_write;os.replace=original_replace
'''
ROOM_LEAF = '''#include "subsystems/roomscale/native_roomscale_d76.h"
namespace kcd2vr::native_roomscale_d76 {
 bool ReadCharacterOrigin(std::uintptr_t p,Origin& o) noexcept {o={};o.serial=2;return p!=0;}
 bool SameSerial(Surface,std::uintptr_t p,std::uint64_t serial) noexcept {return p!=0&&serial==2;}
}
'''
CONTACT = ('contact_recovery_d81', 'contact_unavailable_d80',
           'contact_continuity_d71', 'contact_locality_d64')


def prepare(source, output):
    output.mkdir(parents=True, exist_ok=True)
    p = subprocess.run([sys.executable, '-c', EXTRACT, str(source), str(output)],
                       capture_output=True, text=True)
    (output / 'extraction.log').write_text(p.stdout + p.stderr)
    if p.returncode:
        raise RuntimeError('extraction failed: ' + str(output / 'extraction.log'))
    path = output / 'generated_world_adapter.inc'
    path.write_text(path.read_text().replace('#include "subsystems/roomscale/pose_producer_d76.inl"',
        '// HOST LEAF: unrelated roomscale pose publication omitted; origin reads remain.'))
    path = output / 'generated_adapter_types.h'
    path.write_text('#include "vrik/dialogue_fingers_d78.h"\n' + path.read_text())
    fixture = (ROOT / 'tests/world_adapter_test.cpp').read_text().replace('"../src/', '"' + str(source / 'src') + '/')
    (output / 'world_fixture.inc').write_text(ROOM_LEAF + fixture)


def compile_run(source, output, name, compiler, negative=False):
    test = (ROOT / 'tests' / (name + '_test.cpp')).read_text()
    test = test.replace('"../src/', '"' + str(source / 'src') + '/')
    test = test.replace('"world_adapter_test.cpp"', '"world_fixture.inc"')
    path = output / (name + '_test.cpp')
    path.write_text(test)
    exe = output / name
    command = [compiler, '-std=c++23', '-O0', '-UNDEBUG', '-pthread', '-fms-extensions',
        '-ffunction-sections', '-fdata-sections', '-D_ReturnAddress=HostReturnAddress', '-D__fastcall=',
        '-I' + str(source / 'src'), '-I' + str(source / 'tests/syntax_shims'),
        '-I' + str(source / 'tests/openxr_sdk'), '-I' + str(output), '-I' + str(ROOT / 'tests'),
        str(path), '-Wl,--gc-sections', '-o', str(exe)]
    p = subprocess.run(command, capture_output=True, text=True)
    (output / (name + '.build.log')).write_text(p.stdout + p.stderr)
    row = dict(test=name, negative_control=negative, compile=p.returncode, command=command, passed=False)
    if p.returncode == 0:
        p = subprocess.run([str(exe)], capture_output=True, text=True)
        (output / (name + '.run.log')).write_text(p.stdout + p.stderr)
        row.update(run=p.returncode, stdout=p.stdout, stderr=p.stderr)
        row['passed'] = (p.returncode == 1 and 'FAIL ' in p.stdout and not p.stderr) if negative else p.returncode == 0
    print(f'{output.name}/{name}: {"PASS" if row["passed"] else "FAIL"}', flush=True)
    return row


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output', required=True, type=Path)
    parser.add_argument('--baseline', type=Path, help='Untouched D80 source root for compiled negative controls')
    parser.add_argument('--compiler', default='g++')
    args = parser.parse_args()
    out = args.output.resolve()
    baseline = args.baseline.resolve() if args.baseline else None
    for source in (ROOT, baseline):
        if source and (out == source or source in out.parents):
            parser.error('Output must be outside both source trees')
    prepare(ROOT, out / 'current')
    if baseline:
        prepare(baseline, out / 'baseline')
    jobs = [(ROOT, out / 'current', n, args.compiler, False) for n in
            (*CONTACT, 'riding_handoff_d81', 'dice_body_d81', 'dice_camera_d81', 'activity_eye59c19')]
    if baseline:
        jobs += [(baseline, out / 'baseline', n, args.compiler, True)
                 for n in ('contact_recovery_d81', 'riding_handoff_d81')]
    with concurrent.futures.ThreadPoolExecutor(max_workers=2) as pool:
        results = list(pool.map(lambda job: compile_run(*job), jobs))
    inputs = ('src/animation_filter.cpp', 'src/vrik/riding_vrik_d76.h', 'src/vrik/coherent_pose.inl',
        'src/subsystems/vrik/pose_frame_foundation.inl', 'src/subsystems/vrik/pose_follow_input.inl',
        'src/subsystems/vrik/pose_tracking_goals.inl', 'src/frik_pose_probe.cpp',
        'src/subsystems/physics/pose_free_hand_contact.inl', 'src/vrik/station_hand_contact_d75.h',
        'src/vrik/station_hand_admission_d75.inl', 'src/vrik/hand_contact_types.inl',
        'src/vrik/dice_body_d81.h', 'src/vrik/reading_pose32.h', 'src/camera_hook.cpp',
        'src/activity_eye59c19.h')
    metadata = {'scope': __doc__, 'headset': 'NOT_RUN', 'windows_sdk_build': 'NOT_RUN',
        'passed': all(r['passed'] for r in results), 'results': results,
        'source_sha256': {p: hashlib.sha256((ROOT / p).read_bytes()).hexdigest() for p in inputs}}
    (out / 'results.json').write_text(json.dumps(metadata, indent=2) + '\n')
    return 0 if metadata['passed'] else 1


if __name__ == '__main__':
    raise SystemExit(main())
