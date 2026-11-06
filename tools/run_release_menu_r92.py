"""MSVC host checks of menu state and launcher config preservation. No game DLL."""
from pathlib import Path
import argparse, hashlib, json, subprocess
from host_compiler import command, executable_path

root = Path(__file__).resolve().parents[1]
p = argparse.ArgumentParser()
p.add_argument('--out', type=Path, required=True)
p.add_argument('--compiler', default='cl')
a = p.parse_args()
out = a.out.resolve(); out.mkdir(parents=True, exist_ok=True)
results = []
for name in ('release_menu_r92', 'release_body_height_r98', 'release_launcher_r92', 'startup_launch_options_d80'):
    exe = executable_path(out / name)
    cmd = command(root / 'tests' / (name + '_test.cpp'), exe,
                  [root / 'src', root / 'deps/DetourModKit/include', root / 'tests/openxr_sdk'], a.compiler)
    if a.compiler.lower() in ('cl', 'cl.exe'):
        cmd[cmd.index('/std:c++20')] = '/std:c++latest'
        cmd.append('/DWIN32_LEAN_AND_MEAN')
        if name != 'release_menu_r92':
            cmd.extend(['user32.lib', 'shell32.lib', 'gdi32.lib'])
    built = subprocess.run(cmd, capture_output=True, text=True, cwd=out)
    exe.with_suffix('.build.log').write_text(built.stdout + built.stderr)
    if built.returncode:
        raise SystemExit(built.stdout + built.stderr)
    ran = subprocess.run([str(exe)], capture_output=True, text=True, cwd=out)
    exe.with_suffix('.run.log').write_text(ran.stdout + ran.stderr)
    results.append({'test': name, 'exit': ran.returncode, 'output': ran.stdout + ran.stderr})
    print(ran.stdout, flush=True)
    if ran.returncode: raise SystemExit(ran.returncode)
files = ['src/release_menu_r92.h', 'src/subsystems/experimental/live_settings_d39.h',
         'src/vrik/motion_trials_d67.h', 'src/subsystems/base/startup_setup_s15.inl',
         'tests/release_menu_r92_test.cpp', 'tests/release_launcher_r92_test.cpp',
         'tests/startup_launch_options_d80_test.cpp']
(out / 'RESULT.json').write_text(json.dumps({'passed': True, 'results': results,
    'sha256': {f: hashlib.sha256((root / f).read_bytes()).hexdigest() for f in files},
    'limits': 'Host menu reducers and launcher config functions, no headset UI or DLL.'}, indent=2) + '\n')
