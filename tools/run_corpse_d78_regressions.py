"""Focused D78R1 corpse regressions; actual source, explicit native leaves.

Writes binaries, literal mod.cpp extraction and receipts outside the source tree.
An optional immutable baseline must compile and fail an intended assertion; a
compiler failure or crash is not accepted as a regression reproduction.
"""
import argparse
import concurrent.futures
import hashlib
import json
import os
from pathlib import Path
import subprocess
import sys


def extract(source, destination):
    destination.mkdir(parents=True, exist_ok=True)
    text = (source / 'src/mod.cpp').read_text(encoding='utf-8-sig')
    first = text.index('// CG1_CORPSE_INPUT_BEGIN\n') + len('// CG1_CORPSE_INPUT_BEGIN\n')
    last = text.index('// W5_CG1_RIGHT_END', first)
    pieces = {'corpse_input_both_d78.inc': text[first:last]}
    start = 'for(int side=0;side<2;++side)if(!fallbackPressH40[side]&&corpseReturnedPressD78[side])'
    at = text.find(start)
    end = 'fallbackPressH40[side]=corpseReturnedPressD78[side];'
    pieces['corpse_returned_press_d78.inc'] = text[at:text.index(end, at)+len(end)] if at >= 0 else ''
    start = 'if(held114.owner>=0&&fallbackPressH40[held114.owner])heldUse114.pressed='
    at = text.index(start)
    pieces['corpse_held_press_d78.inc'] = text[at:text.index(';', at)+1]
    for name, contents in pieces.items():
        (destination / name).write_text(contents + '\n', encoding='utf-8')
    (destination / 'source.json').write_text(json.dumps({
        'source': str(source / 'src/mod.cpp'),
        'source_sha256': hashlib.sha256((source / 'src/mod.cpp').read_bytes()).hexdigest(),
        'pieces': {name: hashlib.sha256(value.encode()).hexdigest() for name, value in pieces.items()},
        'scope': 'Literal LEFT/RIGHT CG1 blocks and original-press handoff; native/XR leaves controlled.'
    }, indent=2) + '\n')


def verify_checked_in_extractions(root):
    source = (root / 'src/mod.cpp').read_text(encoding='utf-8-sig')
    start = source.index('// CG1_CORPSE_INPUT_BEGIN\n') + len('// CG1_CORPSE_INPUT_BEGIN\n')
    expected = source[start:source.index('// CG1_CORPSE_INPUT_END', start)]
    visual = (root / 'src/subsystems/interactions/held_visual_policy.inl').read_text(encoding='utf-8-sig')
    expected_visual = '// Actual production visual-only permission; regenerate via tools/extract_held_visual43.py.\n' + visual.rstrip() + '\n'
    for name, actual in [('generated_corpse_grip_worker_cg1.inc', expected), ('generated_held_visual43.inc', expected_visual)]:
        if (root / 'tests' / name).read_text(encoding='utf-8-sig') != actual:
            raise RuntimeError(f'{name} is stale; regenerate its actual-source fixture before testing')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--baseline', type=Path)
    parser.add_argument('--lua-path', type=Path)
    parser.add_argument('--compiler', default='g++')
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[1]
    output = args.output.resolve()
    baseline = args.baseline.resolve() if args.baseline else None
    if any(source == output or source in output.parents for source in (root, baseline) if source):
        parser.error('--output must be outside source and baseline')
    verify_checked_in_extractions(root)
    output.mkdir(parents=True, exist_ok=True)
    sources = [('current', root)] + ([('baseline', baseline)] if baseline else [])
    for label, source in sources:
        extract(source, output / label)
        subprocess.run([sys.executable, str(root / 'tools/extract_corpse_drag_d40.py'),
                        str(output / label), '--source-root', str(source)], check=True)
    names = ['corpse_grip_pending_d78', 'corpse_input_worker_d78', 'corpse_drag_native_d40',
             'saved_corpse_native_d72', 'saved_corpse_drag_d72', 'corpse_grip_cg1',
             'corpse_body_owner_d40', 'lab58', 'physics_lab_w5', 'held_visual43']
    negative = ['corpse_grip_pending_d78', 'corpse_input_worker_d78', 'corpse_drag_native_d40',
                'saved_corpse_native_d72']
    jobs = [(label, source, name) for label, source in sources for name in (names if label == 'current' else negative)]

    def run(job):
        label, source, name = job
        binary = output / label / name
        command = [args.compiler, '-std=c++20', '-O0', '-UNDEBUG', '-pthread', '-I', str(source / 'src'),
                   '-I', str(output / label), '-I', str(root / 'tests'), str(root / 'tests' / (name + '_test.cpp')), '-o', str(binary)]
        build = subprocess.run(command, capture_output=True, text=True)
        (binary.with_suffix('.build.log')).write_text(build.stdout + build.stderr)
        row = {'name': name, 'label': label, 'source': str(source), 'command': command,
               'build_rc': build.returncode, 'passed': False}
        if build.returncode == 0:
            test = subprocess.run([str(binary)], capture_output=True, text=True)
            row.update(run_rc=test.returncode, stdout=test.stdout, stderr=test.stderr)
            row['passed'] = test.returncode == 0 if label == 'current' else test.returncode == 1 and 'FAIL ' in test.stdout and not test.stderr
            binary.with_suffix('.run.log').write_text(test.stdout + test.stderr)
        print(f'{label} {name}: {"PASS" if row["passed"] else "FAIL"}', flush=True)
        return row

    with concurrent.futures.ThreadPoolExecutor(max_workers=2) as pool:
        rows = list(pool.map(run, jobs))
    env = dict(os.environ)
    if args.lua_path:
        env['PYTHONPATH'] = str(args.lua_path.resolve()) + os.pathsep + env.get('PYTHONPATH', '')
    for label, source in sources:
        env['KCD2VR_CORPSE_LUA_SOURCE'] = str(source / 'companion/source/zzkcd2vr_corpse_d72.lua')
        cmd = [sys.executable, str(root / 'tests/test_saved_corpse_drag_d72.py')]
        if label == 'baseline':
            cmd += ['Corpse.test_documented_kcd2_limb_names', 'Corpse.test_clutter_does_not_spend_corpse_budget',
                    'Corpse.test_sorted_tie_break_is_independent_of_enumeration_order',
                    'Corpse.test_eligible_pages_rotate_without_unbounded_bone_queries',
                    'Corpse.test_near_nomination_promotes_same_token_without_motion']
        test = subprocess.run(cmd, env=env, capture_output=True, text=True)
        passed = test.returncode == 0 if label == 'current' else test.returncode == 1 and 'FAIL:' in test.stderr and 'ERROR:' not in test.stderr
        (output / label / 'lua51.run.log').write_text(test.stdout + test.stderr)
        rows.append({'name': 'actual_lua51', 'label': label, 'source': env['KCD2VR_CORPSE_LUA_SOURCE'],
                     'command': cmd, 'run_rc': test.returncode, 'passed': passed,
                     'stdout': test.stdout, 'stderr': test.stderr})
        print(f'{label} actual_lua51: {"PASS" if passed else "FAIL"}', flush=True)
    report = {'scope': 'Host execution of actual source with mocked native/XR/physics leaves; no headset or native game execution.',
              'passed': all(row['passed'] for row in rows), 'results': rows}
    (output / 'results.json').write_text(json.dumps(report, indent=2) + '\n')
    return 0 if report['passed'] else 1


if __name__ == '__main__':
    raise SystemExit(main())
