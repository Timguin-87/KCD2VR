#!/usr/bin/env python3
"""D85 kick: build and run tests/native_kick_free_d85_test.cpp.

Native byte slices are extracted from the fingerprinted retail WHGame.dll into
--out (never into the source tree). Rows come from companion/source (the
tables packed into Mods/KCD2VR/Data/KCD2VR.pak); --pak only has to contain the
same members. g++ (Linux/MSYS) or MSVC cl (--cxx cl from a VS prompt).
  python tools/run_native_kick_free_d85.py --out build/kick_d85 --dll <WHGame.dll>
         --tables <Data/Tables.pak> --pak <Mods/KCD2VR/Data/KCD2VR.pak>
"""
import argparse
import json
from pathlib import Path
import subprocess
import sys

p = argparse.ArgumentParser(description=__doc__)
p.add_argument('--out', type=Path, required=True)
p.add_argument('--dll', type=Path, required=True)
p.add_argument('--tables', type=Path, required=True)
p.add_argument('--pak', type=Path, required=True)
p.add_argument('--source', type=Path, default=Path(__file__).resolve().parents[1],
               help='tree with tools/ and companion/source/')
p.add_argument('--cxx', default='g++')
a = p.parse_args()
root = Path(__file__).resolve().parents[1]
out = a.out.resolve()
out.mkdir(parents=True, exist_ok=True)
subprocess.run([sys.executable, str(root / 'tools/prepare_native_kick_selector_h41.py'),
                '--source', str(a.source.resolve()), '--output', str(out), '--pak', str(a.pak.resolve()),
                '--dll', str(a.dll.resolve()), '--tables', str(a.tables.resolve()),
                '--d72-current-source', '--d85-free'], check=True)
test = root / 'tests/native_kick_free_d85_test.cpp'
exe = out / ('native_kick_free_d85.exe' if a.cxx == 'cl' else 'native_kick_free_d85')
if a.cxx == 'cl':
    cmd = ['cl', '/nologo', '/std:c++latest', '/EHsc', '/Od', '/I', str(out), '/I', str(root / 'tests'), str(test), '/Fe' + str(exe)]
else:
    cmd = [a.cxx, '-std=c++20', '-O0', '-w', '-I', str(out), '-I', str(root / 'tests'), str(test), '-o', str(exe)]
build = subprocess.run(cmd, capture_output=True, text=True, cwd=out)
result = {'command': cmd, 'compile_rc': build.returncode, 'build_log': build.stdout + build.stderr,
          'limits': 'Retail selector/query/filter/pick machine code with built rows; weapon definition, player, '
                    'allocation and enum leaves controlled. No game, action manager, animation or damage executed.'}
if build.returncode == 0:
    run = subprocess.run([str(exe), str(out)], capture_output=True, text=True, cwd=out, timeout=120)
    result.update(run_rc=run.returncode, output=run.stdout + run.stderr)
    print(run.stdout + run.stderr)
else:
    print(build.stdout + build.stderr)
(out / 'KICK_FREE_D85_RESULT.json').write_text(json.dumps(result, indent=2) + '\n')
raise SystemExit(result.get('run_rc', build.returncode))
