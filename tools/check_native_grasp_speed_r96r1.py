#!/usr/bin/env python3
"""Bounded full-owner speed-restitution tests and immutable R96 negative control.

All host/native substitutions are those of native_grasp_owner13_test.cpp. The
normal Linux __finally compatibility macro does NOT emulate Windows SEH.
"""
from pathlib import Path
import argparse
import concurrent.futures
import hashlib
import io
import json
import shutil
import subprocess
import tarfile

ROOT = Path(__file__).resolve().parents[1]
BASELINE = 'b2264262e5bbdbcaae362b69ae0cd4a84796f606'
BASELINE_GRASP_SHA256 = '28c89108d7c1ee3e7db8e3646e168ea6aeac4085a483ea4aa9d75db4cfaa30f3'
TEST = 'tests/native_grasp_speed_recovery_r96r1_test.cpp'
GRASP = 'src/native_grasp59c13.inl'


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def replace_once(text, old, new):
    if text.count(old) != 1:
        raise ValueError('Expected exactly one source mutation: '+old)
    return text.replace(old, new, 1)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--out', type=Path, required=True)
    parser.add_argument('--baseline-source', type=Path,
                        help='Optional R96 exported source tree; does not require Git history')
    parser.add_argument('--require-baseline', action='store_true',
                        help='Fail explicitly if the original R96 source is unavailable')
    parser.add_argument('--only', help='Comma-separated entry names for a focused rerun; report contains only those entries')
    args = parser.parse_args()
    out = args.out.resolve(); out.mkdir(parents=True, exist_ok=True)
    original = out/'original_b226426'
    have_baseline = False
    baseline_status = {'status': 'unavailable', 'comparison': 'NOT_RUN'}
    if args.baseline_source:
        baseline_input = args.baseline_source.resolve()
        if not (baseline_input/GRASP).is_file() or digest(baseline_input/GRASP) != BASELINE_GRASP_SHA256:
            parser.error('--baseline-source must contain the exact pinned R96 native_grasp59c13.inl')
        if baseline_input == original:
            parser.error('--baseline-source must differ from the output original_b226426 directory')
        for sub in ('src', 'tests'):
            if not (baseline_input/sub).is_dir():
                parser.error('--baseline-source is missing '+sub)
            shutil.copytree(baseline_input/sub, original/sub, dirs_exist_ok=True)
        have_baseline = True
        baseline_status = {'status': 'available', 'origin': 'explicit source export',
                           'source': str(baseline_input), 'comparison': 'RUN',
                           'whole_git_tree_authenticated': False}
    else:
        try:
            archived = subprocess.run(['git', 'archive', BASELINE, 'src', 'tests'], cwd=ROOT,
                                      capture_output=True)
        except FileNotFoundError:
            archived = None
        if archived is not None and archived.returncode == 0:
            original.mkdir(exist_ok=True)
            with tarfile.open(fileobj=io.BytesIO(archived.stdout), mode='r:') as tar:
                tar.extractall(original, filter='data')
            have_baseline = True
            baseline_status = {'status': 'available', 'origin': 'git archive',
                               'commit': BASELINE, 'comparison': 'RUN',
                               'whole_git_tree_authenticated': True}
    selected = set(args.only.split(',')) if args.only else None
    require_baseline = args.require_baseline or (selected is not None and 'original_source_negative' in selected)
    if require_baseline and not have_baseline:
        parser.error('Requested R96 baseline is unavailable; provide --baseline-source or Git history for '+BASELINE)
    if have_baseline:
        if digest(original/GRASP) != BASELINE_GRASP_SHA256:
            raise ValueError('Original-source negative differs from the pinned immutable R96 source')
        # The identical NEW regression is the sole added original-tree input.
        shutil.copyfile(ROOT/TEST, original/TEST)
    else:
        print('SKIP original_source_negative: no R96 history/export; current-source checks remain runnable', flush=True)
    source = (ROOT/GRASP).read_text()
    # Keep the old cleanup/quarantine regression verbatim, without invoking the
    # omnibus fixture's unrelated supply-readiness section. Its Context::Current
    # leaf is false and that older section expects true; neither is a pickup
    # assertion. The focused fixture and all native-grasp production stay whole.
    retained_source = ROOT/'tests/native_grasp_owner13_test.cpp'
    retained_text = retained_source.read_text()
    retained_start = retained_text.index(' // Reviewed C25 safety closure:')
    retained_end = retained_text.index(' // C62 real preference/owner graph:', retained_start)
    retained_block = retained_text[retained_start:retained_end]
    retained_driver = out/'retained_c25.cpp'
    retained_driver.write_text(
        '#define main retainedOmnibusMainR96R1\n#include "'+str(retained_source)+'"\n#undef main\n'
        '#include "'+str(ROOT/'tests/pickup_target_d40r1_stubs.cpp')+'"\n'
        'namespace kcd2vr::frik_pose_probe {int ReloadMode47C2() noexcept{return 0;}}\n'
        'namespace kcd2vr::diagnostic_trace {void WriteFor(diagnostics::capture168::Domain,const char* s) noexcept{Write(s);}}\n'
        'int main(){using namespace kcd2vr::action_time_owner59c11;kcd2vr::collab_h21::setThrowing(true);\n'
        +retained_block+'return 0;}\n')
    mutations = {
        'foreign_aba': replace_once(source,
            '        // continue. Do not later reclaim an equal-valued native write (ABA).\n'
            '        g_owner.speedWritten=false;return true;',
            '        // continue. Do not later reclaim an equal-valued native write (ABA).\n'
            '        return true;'),
        'outer_gate': replace_once(source,
            'const bool speedSettled=!nativeUncertain&&SettleSpeed19c5();',
            'const bool speedSettled=SettleSpeed19c5();'),
        'item_debt': replace_once(source,
            'const bool settled=released&&speedSettled;',
            'const bool settled=speedSettled;'),
    }
    jobs = [('current_debug', ROOT, TEST, '-O0', None),
            ('current_optimized', ROOT, TEST, '-O2', None),
            ('retained_c25_cleanup_quarantine', ROOT, str(retained_driver), '-O2', None)]
    if have_baseline:
        jobs.append(('original_source_negative', original, TEST, '-O2',
                     'restore current held speed before original native Update without waiting for hand release'))
    expected = {
        'foreign_aba': 'positive foreign speed retires only our speed lease',
        'outer_gate': 'active, unobserved or foreign outer lifetime blocks all speed restoration',
        'item_debt': 'speed recovery retains the held-item quarantine',
    }
    for name, text in mutations.items():
        tree = out/('mutation_'+name)
        for sub in ('src', 'tests'):
            shutil.copytree(ROOT/sub, tree/sub, dirs_exist_ok=True,
                            ignore=shutil.ignore_patterns('__pycache__'))
        (tree/GRASP).write_text(text)
        jobs.append(('negative_'+name, tree, TEST, '-O2', expected[name]))
    if selected is not None:
        unknown = selected - {job[0] for job in jobs}
        if unknown:
            parser.error('Unknown or unavailable entries: '+', '.join(sorted(unknown)))
        jobs = [job for job in jobs if job[0] in selected]

    def run(job):
        name, tree, test, opt, expected_failure = job
        exe = out/name; dep = out/(name+'.d')
        command = ['g++', '-std=c++20', '-UNDEBUG', '-pthread', '-D__fastcall=',
                   '-D__finally=catch(...) { throw; } if(true)', '-D__declspec(x)=',
                   '-include', str(tree/'src/vrik/static_grab_r89.h'), opt, '-g0',
                   '-MMD', '-MF', str(dep)]
        command += ['-I'+str(tree/p) for p in ('src', 'src/vrik', 'tests',
                                              'tests/syntax_shims', 'tests/openxr_sdk')]
        command += [str(tree/test), '-o', str(exe)]
        print('BUILD '+name, flush=True)
        compiled = subprocess.run(command, cwd=tree, capture_output=True, text=True, timeout=180)
        (out/(name+'.compile.log')).write_text(compiled.stdout+compiled.stderr)
        executed = subprocess.run([str(exe)], cwd=tree, capture_output=True, text=True,
                                  timeout=60) if compiled.returncode == 0 else None
        output = (executed.stdout+executed.stderr) if executed else ''
        (out/(name+'.run.log')).write_text(output)
        rejected = bool(executed and executed.returncode == 1 and
                        'FAIL R96R1 native grasp: '+str(expected_failure) in output)
        passed = compiled.returncode == 0 and executed is not None and (
            rejected if expected_failure else executed.returncode == 0)
        print(name+': '+('PASS' if passed else 'FAIL'), flush=True)
        return {'name': name, 'passed': passed, 'command': command,
                'compile_exit': compiled.returncode,
                'run_exit': executed.returncode if executed else None,
                'negative_control': bool(expected_failure),
                'expected_check_failure': expected_failure, 'output': output,
                'tree': str(tree), 'grasp_sha256': digest(tree/GRASP),
                'fixture_sha256': digest(tree/test)}

    with concurrent.futures.ThreadPoolExecutor(max_workers=2) as pool:
        rows = list(pool.map(run, jobs))
    sources = {ROOT/GRASP, ROOT/TEST, Path(__file__).resolve()}
    for dep in out.glob('*.d'):
        for word in dep.read_text().replace('\\\n', ' ').split(':', 1)[1].split():
            path = Path(word).resolve()
            if path.is_file() and path.is_relative_to(ROOT):
                sources.add(path)
    report = {'passed': all(row['passed'] for row in rows), 'results': rows,
              'baseline_commit': BASELINE,
              'baseline_comparison': baseline_status,
              'original_grasp_matches_pinned_r96_sha256': digest(original/GRASP) == BASELINE_GRASP_SHA256 if have_baseline else None,
              'original_fixture_identical': digest(original/TEST) == digest(ROOT/TEST) if have_baseline else None,
              'selected_entries': sorted(selected) if selected is not None else None,
              'source_sha256': {str(p.relative_to(ROOT)): digest(p) for p in sorted(sources)},
              'retained_regression': {'source': str(retained_source.relative_to(ROOT)),
                    'source_sha256': digest(retained_source),
                    'literal_block_sha256': hashlib.sha256(retained_block.encode()).hexdigest(),
                    'driver_sha256': digest(retained_driver),
                    'start_line': retained_text[:retained_start].count('\n')+1,
                    'end_line': retained_text[:retained_end].count('\n'),
                    'assertions_modified': False},
              'native_engine_execution': False, 'Windows_SEH_executed': False,
              'DLL_built': False, 'game_headset_executed': False,
              'limits': ['Actual production UpdateDetour/Attach/End/Cleanup/Drop paths; native leaves substituted.',
                         'The native Update leaf observes the speed passed to it; host tests do not prove engine animation progress.',
                         'Unknown native hand/root/outer cleanup debts remain retained; no load/UI destruction assumption.',
                         'Linux __finally macro is ordinary host control flow and does not validate Windows SEH.']}
    if (out/'legacy_omnibus_note.json').exists():
        report['legacy_omnibus_fixture_note'] = json.loads((out/'legacy_omnibus_note.json').read_text())
    (out/'RESULT.json').write_text(json.dumps(report, indent=2)+'\n')
    print(f"R96R1 native grasp: {sum(row['passed'] for row in rows)}/{len(rows)} entries passed", flush=True)
    return 0 if report['passed'] else 1


if __name__ == '__main__':
    raise SystemExit(main())
