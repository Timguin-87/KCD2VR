"""Run current raw/visual boundary regressions; native/headset leaves are modeled.

Linux: python3 tools/run_hand_present_d62.py --out /absolute/external/checks
Windows: use a VS x64 Developer Prompt; --cxx cl selects MSVC ASAN.
Historical D61 test receipts remain in repair_evidence/d61.
"""
from pathlib import Path
import argparse
import hashlib
import json
import os
import subprocess


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--out', type=Path, required=True)
    ap.add_argument('--cxx', default='cl' if os.name == 'nt' else 'g++')
    args = ap.parse_args()
    root = Path(__file__).resolve().parents[1]
    out = args.out.resolve()
    if out == root or root in out.parents:
        ap.error('keep compiler outputs outside source')
    out.mkdir(parents=True, exist_ok=True)
    coherent = (root / 'src/vrik/coherent_pose.inl').read_text()
    extractions = {}

    def extract(start, end, name):
        begin = coherent.index(start)
        block = coherent[begin:coherent.index(end, begin)]
        (out / name).write_text(block)
        extractions[name] = {'source': 'src/vrik/coherent_pose.inl',
                             'sha256': hashlib.sha256(block.encode()).hexdigest()}

    extract('static bool HandPresentContextD62()', '\nstruct CoherentTransaction6689', 'display_context_d62.inc')
    extract('    // Only a staged optional display experiment', '    if(tx.reloadWorkingHandXR34', 'display_owner_d62.inc')
    extract('    for(int side=0;side<2;++side){packet.native[side]=',
            '    for(int side=0;side<2;++side){\n        QToQuatT(packet.propRequested', 'display_packet_d62.inc')
    # Execute current admission/profile assignment verbatim, with modeled
    # equipment mailbox/fill leaves. The unrelated bow/attribute tail is not
    # part of this test. Its output enters the actual ph::step kernel.
    profile = (root / 'src/subsystems/physics/native_handling_profile.inl').read_text()
    begin = profile.index('static bool PhysPrepare6698(')
    block = profile[begin:profile.index('    BowGeometry166 bowGeometry{};', begin)]
    (out / 'display_profile_d62.inc').write_text(block + '    return true;\n}\n')
    extractions['display_profile_d62.inc'] = {'source': 'src/subsystems/physics/native_handling_profile.inl',
        'sha256': hashlib.sha256(block.encode()).hexdigest(),
        'scope': 'Actual admission/profile assignment prefix; bow and attribute tail excluded explicitly.'}
    # Integration placement, supplementing the executable current-adapter test.
    assert coherent.index('BuildPassiveHandEvidence169R1(tx,collisionMode') < coherent.index('#include "../subsystems/vrik/pose_hand_display_d62.inl"')
    assert 'hand_present_d61::Select' not in (root / 'src/subsystems/vrik/pose_tracking_goals.inl').read_text()
    assert 'const V3 semanticHandD61=FromT(packet.allowed[side]);' in coherent
    assert '!tx.handPresentOnlyD61[sample.side]' not in coherent
    assert 'if(!tx.handPresentPolicyD61){tx.consumersD49' not in coherent
    xr = (root / 'src/xr_manager.cpp').read_text()
    begin = xr.index('// D62: locate optional display data')
    producer = xr[begin:xr.index('// NOW mode', begin)]
    assert producer.index('TryLocateGripPair(') < producer.index('tracking59c6::PublishPrepared(')
    assert producer.count('hand_present_d61::Publish(') == 1
    assert 'WantsSample(source59c6.epoch,source59c6.tick)' in producer
    tests = [('retained_helper', 'hand_present_d61_test.cpp'),
             ('display_bridge', 'hand_present_d62_bridge_test.cpp'),
             ('retained_tracking', 'tracking59c6_test.cpp'),
             ('retained_xr_input', 'xr_pose_family59c6_test.cpp')]
    results = {}
    compiler = Path(args.cxx).stem.lower()
    msvc = compiler in ('cl', 'clang-cl')
    for name, source in tests:
        exe = out / (name + ('.exe' if msvc else ''))
        includes = [root / 'src', out, root / 'tests/syntax_shims', root / 'tests/openxr_sdk']
        if msvc:
            command = [args.cxx, '/nologo', '/std:c++latest', '/EHsc', '/Od', '/Zi', '/bigobj',
                       '/UNDEBUG', '/fsanitize=address', *('/I' + str(p) for p in includes),
                       str(root / 'tests' / source), '/Fe:' + str(exe), '/link', '/INCREMENTAL:NO', '/STACK:33554432']
        else:
            command = [args.cxx, '-std=c++20', '-g', '-O1', '-UNDEBUG', '-fsanitize=address,undefined',
                       '-fno-omit-frame-pointer', '-fno-pie', '-no-pie',
                       *('-I' + str(p) for p in includes), str(root / 'tests' / source), '-o', str(exe)]
        built = subprocess.run(command, cwd=out, capture_output=True, text=True)
        (out / (name + '.build.log')).write_text(built.stdout + built.stderr)
        environment = os.environ.copy()
        environment['ASAN_OPTIONS'] = 'detect_leaks=0'
        ran = subprocess.run([str(exe)], cwd=out, env=environment, capture_output=True, text=True) if built.returncode == 0 else None
        run_text = ran.stdout + ran.stderr if ran else 'NOT_RUN: compilation failed\n'
        (out / (name + '.run.log')).write_text(run_text)
        results[name] = {'source': 'tests/' + source, 'command': command, 'build_exit': built.returncode,
                         'run_exit': ran.returncode if ran else None, 'passed': built.returncode == 0 and ran.returncode == 0,
                         'run_log': run_text}
    paths = ['src/vrik/hand_present_d61.h', 'src/vrik/hand_present_d62.h', 'src/tracking_sample59c6.h',
             'src/vrik/coherent_pose.inl', 'src/vrik/visual_selection59c7f.h', 'src/subsystems/vrik/pose_tracking_goals.inl',
             'src/subsystems/vrik/pose_hand_display_d62.inl', 'src/subsystems/interactions/hand_pose_service.inl',
             'src/subsystems/physics/native_handling_profile.inl', 'src/vrik/physical_weapon.h',
             'src/xr_manager.cpp', 'src/frik_pose_probe.cpp', 'tools/run_hand_present_d62.py',
             *('tests/' + src for _, src in tests)]
    report = {'passed': all(r['passed'] for r in results.values()), 'results': results, 'extractions': extractions,
              'input_sha256': {p: hashlib.sha256((root / p).read_bytes()).hexdigest() for p in paths},
              'limits': ['Late adapter, packet construction and semantic readers execute current production source.',
                         'Public-mode test executes actual profile admission/assignment prefix and ph::step with modeled equipment leaves.',
                         'Native identity, scene queries and skeleton FK platform leaves are in-memory fixtures.',
                         'Actual input source uses mocked OpenXR calls; no runtime or headset was executed.',
                         'No production Windows DLL/link or game acceptance is implied by these tests.']}
    (out / 'RESULTS.json').write_text(json.dumps(report, indent=2) + '\n')
    print(json.dumps({k: {'passed': v['passed'], 'build_exit': v['build_exit'], 'run_exit': v['run_exit']} for k, v in results.items()}, indent=2))
    return 0 if report['passed'] else 1


if __name__ == '__main__':
    raise SystemExit(main())
