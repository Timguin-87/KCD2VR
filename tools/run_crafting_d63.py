"""D63 production acquisition/wash adapters and optional D62C negative controls.

The acquisition fixture imports the actual workstation selection adapter and
calls the production crafting State. The Lua fixture loads the SHA-pinned
retail trigger scripts; only engine leaf functions are simulated. No DLL or
game is loaded, and no headset/native-effect acceptance is claimed.
"""
from pathlib import Path
import argparse
import hashlib
import json
import os
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--out', required=True, type=Path)
    ap.add_argument('--scripts-pak', required=True, type=Path)
    ap.add_argument('--baseline-source', type=Path, help='Read-only D62C source with src/ and companion/')
    ap.add_argument('--python-deps', type=Path, help='Optional local installation of lupa.lua51')
    ap.add_argument('--compiler', default='g++')
    ap.add_argument('--no-sanitize', action='store_true')
    a = ap.parse_args()
    out = a.out.resolve()
    out.mkdir(parents=True, exist_ok=True)
    env = dict(os.environ, ASAN_OPTIONS='detect_leaks=0')
    if a.python_deps:
        env['PYTHONPATH'] = str(a.python_deps.resolve()) + os.pathsep + env.get('PYTHONPATH', '')
    rows = []

    def run_case(name, command, negative=False, expected_failure=None):
        run = subprocess.run(command, cwd=ROOT, env=env, capture_output=True, text=True)
        text = run.stdout + run.stderr
        (out / (name + '.log')).write_text(text)
        passed = run.returncode == 0 if not negative else run.returncode != 0 and expected_failure in text
        rows.append(dict(test=name, command=command, exit=run.returncode, passed=passed,
                         negative_control=negative, output=text))
        print(name, 'PASS' if passed else 'FAIL', text[-1800:], flush=True)
        return passed

    def compile_case(name, source, source_root, adapter=None):
        exe = out / name
        cmd = [a.compiler, '-std=c++20', '-O1', '-g', '-fno-omit-frame-pointer',
               '-I' + str(source_root / 'src')]
        if not a.no_sanitize:
            cmd += ['-fsanitize=address,undefined']
        if adapter:
            cmd += ['-DD59R1_SELECTION_ADAPTER="' + str(adapter) + '"',
                    '-DKCD2VR_INPUT67_HEADER="' + str(source_root / 'src/subsystems/alchemy/input67.h') + '"']
        cmd += [str(source), '-o', str(exe)]
        return exe if run_case(name + '_compile', cmd) else None

    for label, source_root in [('d63', ROOT), ('d62c', a.baseline_source.resolve() if a.baseline_source else None)]:
        if source_root is None:
            continue
        negative = label == 'd62c'
        text = (source_root / 'src/vrik/semi_workstations46.h').read_text()
        begin = text.index('            alchemy_input19c4::Selection selected{};\n')
        end = text.index('            in.action=', begin)
        fragment = text[begin:end]
        assert 'lookupSelected(c.alchemy,c.lifetime,now,in.nativeSelected,selected)' in fragment
        adapter = out / (label + '_production_selection_adapter.inc')
        adapter.write_text(fragment)
        exe = compile_case(label + '_acquisition', ROOT / 'tests/subsystems/alchemy/acquisition_d63_test.cpp', source_root, adapter)
        if exe:
            run_case(label + '_acquisition', [str(exe)], negative, 'D63 acquisition FAIL:')
        exe = compile_case(label + '_wash_service', ROOT / 'tests/washing_d59_test.cpp', source_root)
        if exe:
            run_case(label + '_wash_service', [str(exe)], negative, 'wash::active.load()&&count("COMMIT")==0')
        run_case(label + '_wash_retail_lua', [sys.executable, str(ROOT / 'tests/washing_d59_lua_test.py'),
                 str(a.scripts_pak.resolve()), str(source_root / 'companion/source/Scripts/Startup/zzkcd2vr_wash_d59.lua')],
                 negative, 'D63: retail Animation trigger without link must be eligible')

    sources = ['src/vrik/alchemy_input19c4.h', 'src/subsystems/alchemy/input67.h',
               'src/vrik/semi_workstations46.h', 'src/subsystems/alchemy/bridge67.h',
               'src/subsystems/interactions/washing_gesture_d59.h', 'src/subsystems/interactions/washing_d59.h',
               'companion/source/Scripts/Startup/zzkcd2vr_wash_d59.lua']
    report = dict(passed=all(r['passed'] for r in rows), results=rows,
                  sources_sha256={p: digest(ROOT / p) for p in sources},
                  scripts_pak_sha256=digest(a.scripts_pak),
                  sanitizer='none' if a.no_sanitize else 'address,undefined',
                  native_engine='NOT_RUN', headset='NOT_RUN',
                  limits='Recorded native hint identities/types with controlled stable completed-frame publications and geometry; actual adapter-to-State input requests, not native pickup/effect success. Actual retail Lua with simulated engine leaves.')
    if a.baseline_source:
        report['baseline_sources_sha256'] = {p: digest(a.baseline_source / p) for p in sources}
    (out / 'RESULTS.json').write_text(json.dumps(report, indent=2) + '\n')
    return 0 if report['passed'] else 1


if __name__ == '__main__':
    raise SystemExit(main())
