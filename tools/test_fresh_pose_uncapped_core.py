#!/usr/bin/env python3
"""Execute production mode4 dispatch and manager pose acquisition with host XR doubles.

The mailbox, pose composition, mode selection, early dispatch, and extracted
manager functions are production code. The XR runtime, Win32 clock, and render
ownership callbacks are host doubles; this is not headset validation.
"""
import argparse
import hashlib
import json
from pathlib import Path
import subprocess

from test_fresh_pose_uncapped_menu import block
import host_compiler


def condition(text, marker):
    start = text.index(marker)
    opening = text.index('(', start)
    depth = 0
    for pos in range(opening, len(text)):
        if text[pos] == '(':
            depth += 1
        elif text[pos] == ')':
            depth -= 1
            if depth == 0:
                return text[opening + 1:pos]
    raise ValueError('Unterminated condition: ' + marker)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('output', type=Path)
    parser.add_argument('--compiler', default=None)
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[1]
    output = args.output.resolve()
    output.mkdir(parents=True, exist_ok=True)
    paths = ['src/xr_manager.cpp', 'src/xr_manager.h', 'src/frame_sync.cpp',
             'src/camera_hook.cpp', 'src/vrik/motion_trials_d67.h',
             'src/vrik/early_frame_pacing_d45.inl', 'src/vrik/smooth_lab_f2.h',
             'src/tracking_sample59c6.h', 'src/vrik/fresh_hand_target_r94.h',
             'src/core/snapshot_mailbox.h', 'tests/fresh_pose_uncapped_core_test.cpp']
    source = {p: (root / p).read_text() for p in paths}
    receipt = block(source[paths[1]], '    struct FreshPoseUncappedReceipt') + ';\n'
    (output / 'fresh_uncapped_receipt.inc').write_text(receipt)
    functions = '\n\n'.join(block(source[paths[0]], marker) for marker in (
        'XrManager::FreshPoseUncappedReceipt XrManager::RefreshUncappedPoseFamily()',
        'bool XrManager::SampleHeadPoseAtTime(',
        'bool XrManager::SampleHeadPoseNow('))
    (output / 'fresh_uncapped_manager.inc').write_text(functions + '\n')
    claim = block(source[paths[6]], 'inline bool ClaimPacingFrame(')
    (output / 'fresh_uncapped_claim.inc').write_text(claim + '\n')
    predicates = (
        'bool LegacyCameraWaitSelectedFixture(int xr_gate_mode) {return ' +
        condition(source[paths[2]], '    if (xr_gate_mode>0') + ';}\n' +
        'bool CameraFailureReceiptSelectedFixture(int pose_source,bool sync_ok) {return ' +
        condition(source[paths[3]], '    if (pose_source!=0 && !sync_ok &&') + ';}\n')
    (output / 'fresh_uncapped_predicates.inc').write_text(predicates)

    # Integration boundary remains before the native physical/update calls.
    nexus = (root / 'src/native_stereo_outer_lab.cpp').read_text()
    entry = nexus.index('FrameSync::PaceGameplayBeforeUpdateD45();')
    physical = nexus.index('PhysicalNativeFrameTick', entry)
    assert entry < physical
    assert 'old_depth==0' in nexus[entry - 200:entry]
    assert '0x20' in nexus[entry - 200:entry]
    # An uncapped selection does not remove the independent Present identity.
    assert 'if (xr_gate_mode>=2 && publish_pose_applied && publish_pose_frame)' in source[paths[2]]
    assert 'SetExpectedPresentCameraFrame(publish_pose_frame);' in source[paths[2]]
    assert '||motion_trials_d67::UncappedSelectedAt(0))&&qpc_time_extension_enabled_' in source[paths[0]]

    executable = host_compiler.executable_path(output / 'fresh_pose_uncapped_core_test')
    command = host_compiler.command(root / paths[-1], executable, [root / 'src', output], args.compiler)
    built = subprocess.run(command, capture_output=True, text=True)
    report = {'build_exit': built.returncode, 'build_stdout': built.stdout,
              'build_stderr': built.stderr, 'headset_tested': False,
              'native_dll_built': False, 'source_sha256': {
                  p: hashlib.sha256((root / p).read_bytes()).hexdigest() for p in paths}}
    if built.returncode == 0:
        result = subprocess.run([str(executable)], capture_output=True, text=True, timeout=15)
        report.update(exit=result.returncode, stdout=result.stdout, stderr=result.stderr)
    (output / 'RESULT.json').write_text(json.dumps(report, indent=2) + '\n')
    print(report.get('stdout', ''), report.get('stderr', ''), built.stderr)
    return 0 if report.get('exit') == 0 else 1


if __name__ == '__main__':
    raise SystemExit(main())
