"""Compile/run the D59 production-header/input fixtures with Windows MSVC ASAN.

Retail script tests additionally require lupa (Lua 5.1) and the supported
game's Scripts.pak. No game process, installation, or INI mutation is involved.
"""
from pathlib import Path
import argparse
import hashlib
import json
import os
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--out', required=True, type=Path)
    ap.add_argument('--lua-path', type=Path)
    ap.add_argument('--only', help='Rerun named cases, preserving other recorded results in the same output folder')
    a = ap.parse_args()
    out = a.out.resolve()
    out.mkdir(parents=True, exist_ok=True)
    vc = Path(os.environ.get('ProgramFiles', 'C:/Program Files')) / 'Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvars64.bat'
    cases = ['pet_gesture_d59_test', 'pet_service_d59_test', 'washing_d59_test',
             'misc_input_d59_test', 'live_settings_d39_test',
             'subsystems/alchemy/gesture_transfer_d59_test',
             'subsystems/alchemy/input67_test', 'subsystems/alchemy/physical_release_repair_test',
             'subsystems/alchemy/ownership_d47_test', 'alchemy_tilt_d56_test',
             'subsystems/alchemy/station_capture_d42_test', 'subsystems/alchemy/gesture_work67_test']
    names = cases + ['pet_native_d59_test.py', 'washing_d59_lua_test.py']
    selected = set(a.only.split(',')) if a.only else set(names)
    assert selected <= set(names), selected - set(names)
    previous = out / 'RESULTS.json'
    rows = [r for r in json.loads(previous.read_text())['results'] if r['test'] not in selected] if a.only and previous.exists() else []
    env = os.environ.copy()
    if a.lua_path:
        env['PYTHONPATH'] = str(a.lua_path.resolve()) + os.pathsep + env.get('PYTHONPATH', '')
    for case in cases:
        if case not in selected:
            continue
        name = Path(case).name
        source = ROOT / 'tests' / (case + '.cpp')
        exe = out / (name + '.exe')
        cmd = out / (name + '.cmd')
        includes = ' '.join('/I"' + str(p) + '"' for p in [ROOT / 'src', ROOT / 'tests/openxr_sdk'])
        cmd.write_text('@echo off\ncall "' + str(vc) + '" >nul\nif errorlevel 1 exit /b 1\n' +
                       'cl /nologo /std:c++20 /EHsc /Od /Zi /UNDEBUG /fsanitize=address /D_CRT_SECURE_NO_WARNINGS ' +
                       includes + ' "' + str(source) + '" /Fe:"' + str(exe) + '" /Fo:"' +
                       str(out / (name + '.obj')) + '" /Fd:"' + str(out / (name + '.pdb')) + '"\n' +
                       'if errorlevel 1 exit /b 1\n"' + str(exe) + '"\nexit /b %errorlevel%\n')
        run = subprocess.run(['cmd', '/d', '/c', str(cmd)], cwd=out, env=env, capture_output=True, text=True)
        (out / (name + '.log')).write_text(run.stdout + run.stderr)
        rows.append(dict(test=case, passed=run.returncode == 0, exit=run.returncode,
                         kind='MSVC_ASAN_COMPILE_LINK_RUN', source_sha256=hashlib.sha256(source.read_bytes()).hexdigest()))
        print(name, 'PASS' if run.returncode == 0 else 'FAIL', (run.stdout + run.stderr)[-1200:], flush=True)
    for name in ['pet_native_d59_test.py', 'washing_d59_lua_test.py']:
        if name not in selected:
            continue
        run = subprocess.run([sys.executable, str(ROOT / 'tests' / name)], cwd=ROOT, env=env,
                             capture_output=True, text=True)
        (out / (name + '.log')).write_text(run.stdout + run.stderr)
        rows.append(dict(test=name, passed=run.returncode == 0, exit=run.returncode,
                         kind='RETAIL_LUA51_ORIGINAL_METHODS_SIMULATED_NATIVE_LEAVES'))
        print(name, 'PASS' if run.returncode == 0 else 'FAIL', (run.stdout + run.stderr)[-1200:], flush=True)
    result = dict(passed=len(rows) == len(names) and all(r['passed'] for r in rows), results=rows,
                  headset='NOT_RUN', game_installation=False,
                  limits='Production source fixtures; native script leaves simulated. No visual/action-result acceptance claim.')
    (out / 'RESULTS.json').write_text(json.dumps(result, indent=2) + '\n')
    return 0 if result['passed'] else 1


if __name__ == '__main__':
    raise SystemExit(main())
