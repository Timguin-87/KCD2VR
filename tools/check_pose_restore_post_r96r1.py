#!/usr/bin/env python3
"""Exact-buffer post-callback restoration observation, with a strict R96 control.

Compiles literal production Restore/WriteQuat/reader/recovery/qualifier and
publication/callback boundaries. The complete solver and native Windows engine
are not supplied by this host fixture. No fallback recovery is synthesized for
the original source, where the post-callback observer/call are simply absent.
"""
from pathlib import Path
import argparse
import json
import re
import subprocess

from pose_restore_extract_r96 import block, extract, sha

BASELINE = 'b2264262e5bbdbcaae362b69ae0cd4a84796f606'
SOURCE_FILES = [
    'src/frik_pose_probe.cpp', 'src/vrik/coherent_pose.inl',
    'src/subsystems/vrik/pose_anatomy.inl', 'src/subsystems/vrik/rig_resolver.inl',
    'src/vrik/pose_restore_debt_r96.h', 'src/core/observed_load_recovery_r96.h',
    'src/subsystems/throw_drop/inventory_lifecycle_t8.h',
]
EXPECTED_BEFORE_FAILURES = {
    'post_effect_failure_reopens_future_rotating_buffer': 'POST_READBACK_REOPENS:',
    'multiple_failed_joints_all_repaid': 'POST_READBACK_ALL_JOINTS:',
    'native_animation_in_unfailed_joint_is_allowed': 'POST_READBACK_UNFAILED_JOINT:',
}


def slices(source, out):
    provenance = extract(source, out)
    cpp = (source / SOURCE_FILES[0]).read_text()
    coh = (source / SOURCE_FILES[1]).read_text()
    rig = (source / 'src/subsystems/vrik/rig_resolver.inl').read_text()
    prefix = 'static bool ObservePoseRestoreAfterCallbackR96R1('
    helper = block(coh, prefix) if prefix in coh else ''
    (out / 'pose_restore_post_helper_r96r1.inc').write_text(helper + '\n')
    (out / 'pose_restore_write_r96r1.inc').write_text(block(rig, 'bool WriteQuat(') + '\n')
    (out / 'pose_restore_publish_gate_r96r1.inc').write_text(
        block(coh, '    if(!tx.committed||!tx.restored||g_pose_restore_fault6689.load()') + '\n')

    restore = '    transaction.Restore();'
    restore_at = cpp.index(restore)
    publish = block(cpp[restore_at:], '    if(committed){PublishCommitted6689(transaction);')
    palette = '    if(paletteD67R1)FinishNativePaletteD67R1(*paletteD67R1,transaction);'
    finger = '    if(fingerOutput28)FinishFingerOutput28(*fingerOutput28,transaction);'
    post_pattern = (r'    if\(g_pose_restore_fault6689\.load\(std::memory_order_acquire\)\)\n'
                    r'        \(void\)ObservePoseRestoreAfterCallbackR96R1\([^;]+;')
    match = re.search(post_pattern, cpp)
    post = match.group() if match else ''
    order = [restore_at, cpp.index(publish, restore_at), cpp.index(palette, restore_at), cpp.index(finger, restore_at)]
    assert order == sorted(order), 'Existing callback consumer order changed'
    assert bool(helper) == bool(post), 'Post-callback helper and real caller disagree'
    if post:
        order.append(match.start())
        assert order == sorted(order), 'Post recovery precedes publication/final consumers'
        callback = block(cpp, 'void __fastcall SkinningTransformCoreDetour(')
        assert callback[callback.index(post) + len(post):].strip() == '}', 'A later executable callback consumer was omitted'
    (out / 'pose_restore_tail_r96r1.inc').write_text('\n'.join([restore, publish, palette, finger, post]) + '\n')
    provenance.update(
        source_sha256={p: sha(source / p) for p in SOURCE_FILES},
        slices={p.name: sha(p) for p in sorted(out.glob('pose_restore_*.inc'))},
        post_callback_present=bool(post),
        callback_order_verified=True,
        scope='Literal Restore, WriteQuat, buffer read/validation, R96 entry recovery, new complete post-callback qualifier, first real publication gate and ordered callback tail statements. Only the committed branch is entered; unrelated !committed diagnostics and packet construction are not compiled.')
    return provenance


def run(root, source, out, compiler, fixture):
    out.mkdir(parents=True, exist_ok=True)
    provenance = slices(source, out / 'slices')
    exe = out / 'test'
    depfile = out / 'dependencies.d'
    cmd = [compiler, '-std=c++20', '-O2', '-pthread', '-MMD', '-MF', str(depfile),
           '-I', str(out / 'slices'), '-I', str(source / 'src'), '-I', str(root / 'src'),
           str(fixture), '-o', str(exe)]
    built = subprocess.run(cmd, capture_output=True, text=True)
    (out / 'build.log').write_text(built.stdout + built.stderr)
    result = {'command': cmd, 'compile': built.returncode, 'fixture_sha256': sha(fixture), 'extraction': provenance}
    if built.returncode == 0:
        ran = subprocess.run([str(exe)], capture_output=True, text=True)
        (out / 'run.log').write_text(ran.stdout + ran.stderr)
        result.update(exit=ran.returncode, stdout=ran.stdout, stderr=ran.stderr,
                      passed_cases=re.findall(r'^PASS (\S+)', ran.stdout, re.M),
                      failed_cases=dict(re.findall(r'^FAIL (\S+) : (.*)', ran.stdout, re.M)))
        deps = depfile.read_text().replace('\\\n', ' ').partition(':')[2].split()
        result['compiled_dependency_sha256'] = {p: sha(Path(p)) for p in deps if Path(p).is_file()}
    return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--source', type=Path, default=Path(__file__).resolve().parents[1])
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--compiler', default='g++')
    parser.add_argument('--baseline-git', default=BASELINE,
                        help='Immutable original revision for the negative control; requires local Git history.')
    parser.add_argument('--skip-baseline', action='store_true',
                        help='Run current and retained positive suites from an exported source tree without Git history.')
    args = parser.parse_args()
    root, out = args.source.resolve(), args.output.resolve()
    out.mkdir(parents=True, exist_ok=True)
    fixture = root / 'tests/pose_restore_post_r96r1_test.cpp'
    current = run(root, root, out / 'current', args.compiler, fixture)
    before, baseline_revision, strict_before = None, None, None
    expected = EXPECTED_BEFORE_FAILURES
    if not args.skip_baseline:
        original = out / 'baseline_source'
        for name in SOURCE_FILES:
            path = original / name;path.parent.mkdir(parents=True, exist_ok=True)
            path.write_bytes(subprocess.check_output(['git', 'show', args.baseline_git + ':' + name], cwd=root))
        before = run(root, original, out / 'before', args.compiler, fixture)
        baseline_revision = subprocess.check_output(['git', 'rev-parse', args.baseline_git], cwd=root, text=True).strip()
        strict_before = (before['compile'] == 0 and before.get('exit') == 1
                         and set(before.get('failed_cases', {})) == set(expected)
                         and all(marker in before['failed_cases'][name] for name, marker in expected.items())
                         and len(before.get('passed_cases', [])) == 25)
    passed = (current['compile'] == 0 and current.get('exit') == 0
              and len(current.get('passed_cases', [])) == 28 and not current.get('failed_cases')
              and (args.skip_baseline or strict_before))
    # The existing R96 entry-recovery fixtures remain useful for load restitution.
    old_fixture = root / 'tests/pose_restore_recovery_r96_test.cpp'
    retained = run(root, root, out / 'retained_r96', args.compiler, old_fixture)
    passed = passed and retained['compile'] == 0 and retained.get('exit') == 0 and len(retained.get('passed_cases', [])) == 28  # R97: +load_rearm_retires_stale_debt_for_next_failure
    result = {
        'passed': passed,
        'baseline': baseline_revision,
        'baseline_status': ('SKIPPED' if args.skip_baseline else
                            'STRICT_EXPECTED_FAILURES_VERIFIED' if strict_before else 'FAILED'),
        'current': current, 'before': before, 'retained_r96': retained,
        'strict_original_control': strict_before, 'expected_original_failures': expected,
        'runner_sha256': sha(Path(__file__)),
        'limits': ['Windows DLL, Windows SEH, retail game, engine scheduling and HMD NOT_RUN.',
                   'Full coherent solver/translation unit not compiled. Native identity/getter/setter and descriptor memory are declared host leaves.',
                   'The exact old native buffer must remain freshly qualified through both sides of readback. A rotated or unresolved indebted buffer is not forgiven.',
                   'The new path does not use load restitution, a setter, or replacement-owner retirement.']}
    (out / 'RESULT.json').write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps({'passed': passed, 'current_passed': len(current.get('passed_cases', [])),
                      'baseline_status': result['baseline_status'],
                      'before_passed': len(before.get('passed_cases', [])) if before else None,
                      'before_expected_failures': before.get('failed_cases') if before else None,
                      'retained_r96_passed': len(retained.get('passed_cases', [])),
                      'result': str(out / 'RESULT.json')}, indent=2))
    return 0 if passed else 1


if __name__ == '__main__':
    raise SystemExit(main())
