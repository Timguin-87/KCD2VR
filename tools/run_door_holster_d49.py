#!/usr/bin/env python3
"""Compile the D49 actual door+holster interleaving and retained admission tests.

On Windows this uses MSVC and links host fixtures, not the game DLL. Native
engine/Lua leaves are explicit doubles. Results are not headset acceptance.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import subprocess

ROOT = Path(__file__).resolve().parents[1]
CASES = ['door_holster_query_d49_test', 'holster_instant_d44_test',
         'door_throw_admission_h41_test', 'd48_door_packet_test']

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--out', type=Path, required=True)
    args = parser.parse_args()
    out = args.out.resolve()
    out.mkdir(parents=True, exist_ok=True)
    rows = []
    for name in CASES:
        fixture = ROOT / 'tests' / (name + '.cpp')
        exe = out / (name + ('.exe' if os.name == 'nt' else ''))
        if os.name == 'nt':
            compiler = Path(os.environ.get('ProgramFiles', 'C:/Program Files')) / 'Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvars64.bat'
            script = out / (name + '.cmd')
            script.write_text('@echo off\ncall "' + str(compiler) + '" >nul\n'
                'if errorlevel 1 exit /b 1\n'
                'cl /nologo /std:c++20 /EHsc /O1 /I"' + str(ROOT / 'src') + '" "' + str(fixture) +
                '" /Fe:"' + str(exe) + '" /Fo:"' + str(out / (name + '.obj')) + '"\n'
                'exit /b %errorlevel%\n', encoding='utf-8')
            command = ['cmd', '/d', '/c', str(script)]
        else:
            command = ['g++', '-std=c++20', '-O1', '-pthread', '-I' + str(ROOT / 'src'), str(fixture), '-o', str(exe)]
        compile_result = subprocess.run(command, capture_output=True, text=True)
        (out / (name + '.compile.log')).write_text(compile_result.stdout + compile_result.stderr)
        row = {'test': name, 'compile_exit': compile_result.returncode, 'native_game_execution': False}
        if compile_result.returncode == 0:
            result = subprocess.run([str(exe)], capture_output=True, text=True)
            row['run_exit'] = result.returncode
            row['summary'] = result.stdout.strip()
            (out / (name + '.log')).write_text(result.stdout + result.stderr)
        row['passed'] = row.get('run_exit') == 0
        rows.append(row)
        print(('PASS ' if row['passed'] else 'FAIL ') + name + '\n' + row.get('summary', compile_result.stdout + compile_result.stderr))
    inputs = [ROOT / 'tests' / (name + '.cpp') for name in CASES]
    inputs += [ROOT / 'src/subsystems/doors/hand_only_h40.h', ROOT / 'src/subsystems/doors/hand_only_h40_service.inl', ROOT / 'src/subsystems/throw_drop/holster_gestures.inl']
    report = {'passed': all(row['passed'] for row in rows), 'results': rows,
              'inputs': {str(p.relative_to(ROOT)): hashlib.sha256(p.read_bytes()).hexdigest() for p in inputs},
              'native_game_execution': False, 'headset_tested': False}
    (out / 'RESULTS.json').write_text(json.dumps(report, indent=2) + '\n')
    return 0 if report['passed'] else 1

if __name__ == '__main__':
    raise SystemExit(main())
