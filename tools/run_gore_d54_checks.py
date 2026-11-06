"""Focused MSVC/ASan tests for gore materials, event format and existing cut leases."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import subprocess

ROOT = Path(__file__).resolve().parents[1]
NAMES = ('gore_material_d54_test', 'gorepak50_test', 'gore_skin_path_d52_test',
         'gore_lifecycle_d52_test', 'gore_visibility59_test')


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--out', type=Path, required=True)
    p.add_argument('--asan', action='store_true')
    a = p.parse_args()
    out = a.out.resolve()
    out.mkdir(parents=True, exist_ok=True)
    vc = Path(os.environ.get('ProgramFiles', 'C:/Program Files'))/'Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvars64.bat'
    lines = ['@echo off', 'call "'+str(vc)+'" >nul', 'if errorlevel 1 exit /b 1']
    for name in NAMES:
        lines += ['cl /nologo /std:c++20 /EHsc /O1 /UNDEBUG '+('/fsanitize=address /Zi ' if a.asan else '')+
                  '/I"'+str(ROOT/'src')+'" "'+str(ROOT/'tests'/(name+'.cpp'))+'" '+
                  '/Fe:"'+str(out/(name+'.exe'))+'" /Fo:"'+str(out/(name+'.obj'))+'" '+
                  '/Fd:"'+str(out/(name+'.pdb'))+'"',
                  'if errorlevel 1 exit /b 1',
                  '"'+str(out/(name+'.exe'))+'" > "'+str(out/(name+'.run.log'))+'" 2>&1',
                  'if errorlevel 1 exit /b 1']
    lines += ['exit /b 0']
    command = out/'run.cmd'
    command.write_text('\n'.join(lines)+'\n')
    result = subprocess.run(['cmd', '/d', '/c', str(command)], cwd=out, capture_output=True, text=True)
    (out/'compile.log').write_text(result.stdout+result.stderr)
    inputs = ['src/vrik/gore_material_d54.h','src/vrik/gorepak50.h','src/vrik/gore_skin_path_d52.h',
              'src/vrik/gore_visibility59.h','src/subsystems/gore/cut_lifecycle_d52.inl']
    inputs += ['tests/'+n+'.cpp' for n in NAMES]
    report = dict(compiler='MSVC', address_sanitizer=a.asan, exit_code=result.returncode,
                  passed=result.returncode==0, native_calls='MOCKED', headset='NOT_RUN',
                  source_sha256={n:hashlib.sha256((ROOT/n).read_bytes()).hexdigest() for n in inputs},
                  results={n:(out/(n+'.run.log')).read_text() if (out/(n+'.run.log')).exists() else 'NOT_RUN' for n in NAMES})
    (out/'RESULTS.json').write_text(json.dumps(report, indent=2)+'\n')
    print(json.dumps(report, indent=2))
    return result.returncode


if __name__ == '__main__':
    raise SystemExit(main())
