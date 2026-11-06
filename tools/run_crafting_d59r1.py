"""Focused portable D59R1 crafting checks against the current production source.

No game files or build DLL are changed. GCC/Clang C++20 required. The selection
adapter is extracted verbatim from the current workstation integration.
"""
from pathlib import Path
import argparse
import hashlib
import json
import os
import subprocess

ROOT = Path(__file__).resolve().parents[1]


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--out', required=True, type=Path)
    ap.add_argument('--compiler', default='g++')
    ap.add_argument('--no-sanitize', action='store_true')
    ap.add_argument('--extract-only', action='store_true',
                    help='Write the current production adapter for CMake without compiling or running tests')
    ap.add_argument('--baseline-source', type=Path,
                    help='Optional immutable D59 source to run the same smith regression and expect its recorded failure')
    a = ap.parse_args()
    out = a.out.resolve()
    out.mkdir(parents=True, exist_ok=True)
    text = (ROOT / 'src/vrik/semi_workstations46.h').read_text()
    begin = text.index('            alchemy_input19c4::Selection selected{};\n')
    end = text.index('            in.action=', begin)
    fragment = text[begin:end]
    assert 'lookupSelected(c.alchemy,c.lifetime,now,in.nativeSelected,selected)' in fragment
    assert 'selectionSnapshot' not in fragment
    assert 'selectedTypeOnly(c.alchemy,c.lifetime,now,typeOnly)' in fragment
    assert 'nativeTypeOnlyTick=in.nativeSelectedTypeOnly<21u?typeOnly.tick:0' in fragment
    adapter = out / 'production_selection_adapter.inc'
    adapter.write_text(fragment)
    if a.extract_only:
        return 0
    cases = [
        'subsystems/alchemy/selection_stamp_d59r1_test',
        'subsystems/alchemy/gesture_transfer_d59_test',
        'subsystems/alchemy/native_agree_w4_test',
        'smith_native_height_d59r1_test',
        'smith_stroke_wiring_d57_test',
    ]
    results = []
    for case in cases:
        source = ROOT / 'tests' / (case + '.cpp')
        exe = out / Path(case).name
        command = [a.compiler, '-std=c++20', '-O1', '-g', '-fno-omit-frame-pointer',
                   '-I' + str(ROOT / 'src'),
                   '-DD59R1_SELECTION_ADAPTER="' + adapter.as_posix() + '"']
        if not a.no_sanitize:
            command += ['-fsanitize=address,undefined']
        command += [str(source), '-o', str(exe)]
        built = subprocess.run(command, capture_output=True, text=True)
        output = built.stdout + built.stderr
        result = {'test': case, 'source_sha256': hashlib.sha256(source.read_bytes()).hexdigest(),
                  'compile_exit': built.returncode, 'run_exit': None, 'passed': False}
        if built.returncode == 0:
            env = dict(os.environ, ASAN_OPTIONS='detect_leaks=0')
            run = subprocess.run([str(exe)], capture_output=True, text=True, env=env)
            output += run.stdout + run.stderr
            result.update(run_exit=run.returncode, passed=run.returncode == 0)
        (out / (Path(case).name + '.log')).write_text(output)
        result['output'] = output
        results.append(result)
        print(case, 'PASS' if result['passed'] else 'FAIL', output[-1200:], flush=True)
    summary = {'passed': all(r['passed'] for r in results), 'results': results,
               'production_adapter_sha256': hashlib.sha256(fragment.encode()).hexdigest(),
               'production_sources_sha256': {
                   path: hashlib.sha256((ROOT / path).read_bytes()).hexdigest()
                   for path in ['src/vrik/alchemy_input19c4.h', 'src/vrik/semi_workstations46.h',
                                'src/vrik/semi_smith46.h', 'src/subsystems/alchemy/input67.h']},
               'sanitizers': 'none' if a.no_sanitize else 'address,undefined; leak scanning disabled for sandbox /proc restrictions',
               'native_engine': 'NOT_RUN', 'headset': 'NOT_RUN'}
    if a.baseline_source:
        baseline = a.baseline_source.resolve() / 'src/vrik/semi_smith46.h'
        source = ROOT / 'tests/smith_native_height_d59r1_test.cpp'
        exe = out / 'smith_native_height_d59_baseline'
        command = [a.compiler, '-std=c++20', '-O1', '-g', '-fno-omit-frame-pointer',
                   '-DD59R1_SMITH_HEADER="' + baseline.as_posix() + '"']
        if not a.no_sanitize:
            command += ['-fsanitize=address,undefined']
        command += [str(source), '-o', str(exe)]
        built = subprocess.run(command, capture_output=True, text=True)
        output = built.stdout + built.stderr
        counterfactual = {'header': str(baseline), 'compile_exit': built.returncode,
                          'run_exit': None, 'recorded_failure_reproduced': False}
        if built.returncode == 0:
            run = subprocess.run([str(exe)], capture_output=True, text=True,
                                 env=dict(os.environ, ASAN_OPTIONS='detect_leaks=0'))
            output += run.stdout + run.stderr
            counterfactual['run_exit'] = run.returncode
            counterfactual['recorded_failure_reproduced'] = run.returncode == 1 and \
                'FAIL: complete cycle permits continuous second lift' in output
        counterfactual['output'] = output
        (out / 'smith_native_height_d59_baseline.log').write_text(output)
        summary['baseline_counterfactual'] = counterfactual
        summary['passed'] &= counterfactual['recorded_failure_reproduced']
        print('D59 baseline reproduces missing second stroke:',
              counterfactual['recorded_failure_reproduced'], flush=True)
    (out / 'RESULTS.json').write_text(json.dumps(summary, indent=2) + '\n')
    return 0 if summary['passed'] else 1


if __name__ == '__main__':
    raise SystemExit(main())
