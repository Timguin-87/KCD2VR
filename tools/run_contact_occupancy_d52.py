#!/usr/bin/env python3
"""Build/run the actual D52 read-only contact observer with explicit native doubles."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import subprocess

ROOT = Path(__file__).resolve().parents[1]

def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--out', type=Path, required=True)
    out = p.parse_args().out.resolve()
    out.mkdir(parents=True, exist_ok=True)
    name = 'contact_occupancy_d52_test'
    fixture = ROOT / 'tests' / (name + '.cpp')
    exe = out / (name + ('.exe' if os.name == 'nt' else ''))
    if os.name == 'nt':
        vcvars = Path(os.environ.get('ProgramFiles', 'C:/Program Files')) / 'Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvars64.bat'
        script = out / 'build.cmd'
        script.write_text('@echo off\ncall "' + str(vcvars) + '" >nul\nif errorlevel 1 exit /b 1\n'
            'cl /nologo /std:c++20 /EHsc /O1 /I"' + str(ROOT / 'src') + '" "' + str(fixture) +
            '" /Fe:"' + str(exe) + '" /Fo:"' + str(out / (name + '.obj')) + '"\nexit /b %errorlevel%\n')
        command = ['cmd', '/d', '/c', str(script)]
    else:
        command = ['g++', '-std=c++20', '-O1', '-pthread', '-I' + str(ROOT / 'src'), str(fixture), '-o', str(exe)]
    build = subprocess.run(command, capture_output=True, text=True)
    (out / 'compile.log').write_text(build.stdout + build.stderr)
    result = {'compile_exit': build.returncode, 'passed': False, 'headset_tested': False, 'native_game_execution': False}
    if build.returncode == 0:
        run = subprocess.run([str(exe)], capture_output=True, text=True)
        (out / 'run.log').write_text(run.stdout + run.stderr)
        result.update(run_exit=run.returncode, summary=run.stdout.strip(), passed=run.returncode == 0)
    paths = ['src/vrik/contact_occupancy_d52.h', 'src/vrik/contact_occupancy_d52.inl', 'src/vrik/coherent_pose.inl', 'src/frik_pose_probe.cpp', 'tests/contact_occupancy_d52_test.cpp']
    result['inputs'] = {s: hashlib.sha256((ROOT / s).read_bytes()).hexdigest() for s in paths}
    (out / 'RESULTS.json').write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps(result, indent=2))
    return 0 if result['passed'] else 1

if __name__ == '__main__':
    raise SystemExit(main())
