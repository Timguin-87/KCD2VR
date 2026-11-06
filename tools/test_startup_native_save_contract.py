#!/usr/bin/env python3
"""Exercise the production startup combo, Save handler, and cfg save contract.

Uses real temporary cfg files. Win32 controls/profile/file-operation calls are
explicit host adapters; the DLL, game, Steam and headset are never executed.
"""
import argparse
import hashlib
import json
from pathlib import Path
import subprocess
import host_compiler
from test_fresh_pose_uncapped_menu import block


def extract(root, dest, variant):
    path = root / 'src/subsystems/base/startup_setup_s15.inl'
    text = path.read_text(encoding='utf-8-sig')
    declarations = ['struct Field {', 'static constexpr Field fields[]=', 'struct State {']
    functions = ['static bool Parse(', 'static std::wstring Canonical(',
                 'static std::wstring Read(', 'static bool ReadBool(',
                 'static int ProductionMode(', 'static bool ReadSource(',
                 'static bool LaunchCompatible(', 'static void LoadLaunchOptions(',
                 'static bool SaveSource(', 'static void FillChoices(',
                 'static int SelectedChoice(', 'static void FillQuality(']
    chunks = [block(text, marker) + ';' for marker in declarations]
    chunks.extend(block(text, marker) for marker in functions)
    actual = '\n\n'.join(chunks) + '\n'
    if variant == 'missing_generated_mode':
        old = 'if(launchOptions)lines=graphics_owner31::updateUserCfgR27(lines,selected);'
        new = ('selected.erase(std::remove_if(selected.begin(),selected.end(),'
               '[](const auto& entry){return entry.first=="r_SuperResolution_Mode";}),selected.end());\n' + old)
    elif variant == 'without_final_readback':
        old, new = 'if(ok)ok=selectionSaved(cfg);', '/* Negative control: final verification removed. */'
    else:
        old = new = None
    if old:
        if actual.count(old) != 1:
            raise ValueError('Source contract changed for ' + variant)
        actual = actual.replace(old, new)
    dest.mkdir(parents=True, exist_ok=True)
    (dest / 'startup_native_save_actual.inc').write_text(actual)
    click = ('static int Handle(State& state,int id) { State* s=&state; const HWND w=1;\n' +
             block(text, 'if(id==IDOK||id==201)') + '\nreturn 0; }\n')
    (dest / 'startup_native_save_click.inc').write_text(click)
    return dict(source=str(path), sha256=hashlib.sha256(path.read_bytes()).hexdigest(),
                extracted_sha256=hashlib.sha256(actual.encode()).hexdigest(),
                save_click_sha256=hashlib.sha256(click.encode()).hexdigest(),
                fixture_change=variant)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--source', type=Path, default=Path(__file__).resolve().parents[1])
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    root, output = args.source.resolve(), args.output.resolve()
    if output == root or root in output.parents:
        parser.error('Evidence output must be outside the source tree.')
    results = []
    for variant in ('production', 'missing_generated_mode', 'without_final_readback'):
        dest = output / variant
        provenance = extract(root, dest, variant)
        executable = host_compiler.executable_path(dest / 'startup_native_save_contract')
        command = host_compiler.command(root / 'tests/startup_native_save_contract_test.cpp', executable, [root / 'src', dest])
        build = subprocess.run(command, capture_output=True, text=True)
        (dest / 'build.log').write_text(build.stdout + build.stderr)
        row = dict(variant=variant, provenance=provenance, command=command,
                   build_rc=build.returncode, passed=False)
        if build.returncode == 0:
            run = subprocess.run([str(executable), str(dest / 'files'), variant],
                                 capture_output=True, text=True)
            (dest / 'run.log').write_text(run.stdout + run.stderr)
            row.update(run_rc=run.returncode, stdout=run.stdout, stderr=run.stderr)
            row['passed'] = (run.returncode == 1 and 'FAIL ' in run.stdout) if variant == 'without_final_readback' else run.returncode == 0
        results.append(row)
        print(variant + ': ' + ('PASS' if row['passed'] else 'FAIL'), flush=True)
        if not row['passed']:
            break
    headers = ['src/subsystems/base/startup_graphics_d80.h', 'src/graphics_owner31.h',
               'src/stereo_upscaler_d81r2.h', 'src/subsystems/base/view_defaults_b2.h']
    report = dict(scope='Actual startup combo helpers, full Save/Keep button branch, source reader/selection validation, and complete SaveSource with real cfg files. Window/profile/rename/copy/delete leaves are host adapters. SaveIni is a recording double; pending INI and native Win32 behavior are not executed.',
                  headset_tested=False, game_launched=False,
                  headers={p: hashlib.sha256((root / p).read_bytes()).hexdigest() for p in headers},
                  results=results)
    (output / 'RESULT.json').write_text(json.dumps(report, indent=2) + '\n')
    return 0 if len(results) == 3 and all(row['passed'] for row in results) else 1


if __name__ == '__main__':
    raise SystemExit(main())
