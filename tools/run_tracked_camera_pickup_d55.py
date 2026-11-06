"""Run current production camera-reader tests under MSVC, optionally ASan."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--out', type=Path, required=True)
    p.add_argument('--asan', action='store_true')
    a = p.parse_args()
    out = a.out.resolve()
    out.mkdir(parents=True, exist_ok=True)
    subprocess.run([sys.executable, str(ROOT/'tools/extract_tracked_camera_pickup_d55.py'), '--out', str(out)], check=True)
    vc = Path(os.environ.get('ProgramFiles', 'C:/Program Files'))/'Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvars64.bat'
    args = ['cl', '/nologo', '/std:c++20', '/EHsc', '/O1', '/UNDEBUG']
    if a.asan:
        args += ['/fsanitize=address', '/Zi']
    args += ['/I'+str(d) for d in [ROOT/'src', out, ROOT/'tests/syntax_shims', ROOT/'tests/openxr_sdk']]
    args += ['/Fe:camera_test.exe', '/Fo:camera_test.obj', '/Fd:camera_test.pdb', str(ROOT/'tests/tracked_camera_pickup_d55_test.cpp')]
    command = '@echo off\ncall "'+str(vc)+'" >nul\nif errorlevel 1 exit /b 1\n'
    command += subprocess.list2cmdline(args)+'\nif errorlevel 1 exit /b 1\ncamera_test.exe > camera_test.log 2>&1\nexit /b %errorlevel%\n'
    (out/'run.cmd').write_text(command, encoding='utf-8')
    result = subprocess.run(['cmd', '/d', '/c', str(out/'run.cmd')], cwd=out, capture_output=True, text=True)
    (out/'compile.log').write_text(result.stdout+result.stderr, encoding='utf-8')
    log = (out/'camera_test.log').read_text() if (out/'camera_test.log').exists() else ''
    groups = [dict(name=n, passed=int(g), checks=int(t)) for n,g,t in re.findall(r'^(tracked_\w+)=(\d+)/(\d+)', log, re.M)]
    files = ['src/subsystems/vrik/tracked_camera_service.inl','src/vrik/camera_hold_e1.h',
             'src/vrik/tracked_frame51.h','src/diagnostics/interaction_cycle_h4.h',
             'tests/tracked_camera_pickup_d55_test.cpp','tools/extract_tracked_camera_pickup_d55.py']
    report = dict(compiler='MSVC', address_sanitizer=a.asan, exit_code=result.returncode,
                  passed=result.returncode==0 and len(groups)==4 and all(g['passed']==g['checks'] for g in groups),
                  groups=groups, total_checks=sum(g['checks'] for g in groups),
                  native_leaves='MOCKED', headset='NOT_RUN', final_camera_pixels='NOT_OBSERVED',
                  source_sha256={f:hashlib.sha256((ROOT/f).read_bytes()).hexdigest() for f in files},
                  extraction_report='CAMERA_EXTRACTION_D55.json')
    (out/'CAMERA_RESULTS_D55.json').write_text(json.dumps(report, indent=2)+'\n', encoding='utf-8')
    print(json.dumps(report, indent=2))
    if result.returncode:
        print(result.stdout+result.stderr)
    return 0 if report['passed'] else 1


if __name__ == '__main__':
    raise SystemExit(main())
