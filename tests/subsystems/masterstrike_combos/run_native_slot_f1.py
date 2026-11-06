"""Compile/run the actual module query and native callbacks; never load the game."""
import argparse
import hashlib
import json
from pathlib import Path
import subprocess

root = Path(__file__).resolve().parents[3]
parser = argparse.ArgumentParser()
parser.add_argument('--build-dir', type=Path, required=True)
parser.add_argument('--report', type=Path, required=True)
parser.add_argument('--asan', action='store_true')
parser.add_argument('--vcvars', type=Path, default=Path('C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvars64.bat'))
args = parser.parse_args()
build = args.build_dir.resolve()
build.mkdir(parents=True, exist_ok=True)
inputs = [root / 'src/subsystems/masterstrike_combos' / name for name in (
    'module.h', 'native_combo_query_f1.h', 'native_flow_controls.inl', 'lineage_policy.h', 'native_combo_grace.inl')]
inputs += [Path(__file__).resolve(), Path(__file__).with_name('native_slot_f1_test.cpp')]
hashes = lambda: {str(p.relative_to(root)).replace('\\', '/'): hashlib.sha256(p.read_bytes()).hexdigest() for p in inputs}
before = hashes()
command = build / 'run_native_slot_f1.cmd'
test = inputs[-1]
options = ' /fsanitize=address' if args.asan else ''
command.write_text('@echo off\n'
    f'call "{args.vcvars}" >nul\n'
    'if errorlevel 1 exit /b 1\n'
    f'cl /nologo /std:c++20 /EHsc /UNDEBUG /Od /Zi /MD{options} /I "{root / "src"}" '
    f'/Fo"{build / "native_slot_f1.obj"}" /Fd"{build / "native_slot_f1.pdb"}" '
    f'/Fe"{build / "native_slot_f1.exe"}" "{test}"\n'
    'if errorlevel 1 exit /b 1\n'
    f'"{build / "native_slot_f1.exe"}"\n')
run = subprocess.run(['cmd.exe', '/d', '/c', str(command)], cwd=build, text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
after = hashes()
receipt = {'schema': 1, 'pass': run.returncode == 0 and before == after,
    'returncode': run.returncode, 'actual_production_query_and_callbacks': True,
    'native_runtime_executed': False, 'headset_acceptance': False, 'asan': args.asan,
    'input_sha256': before, 'inputs_unchanged': before == after,
    'explicit_fixture_leaves': ['native memory/known-copy', 'committed frame/tracking publications',
        'selection/held-weapon native admission', 'release-history native identity',
        'original native trigger/manager', 'native TimerStart (real function-pointer call)'],
    'coverage': ['old ordering native phase16 before hit', '500/1500ms inclusive finite boundary',
        'late-hit and duplicate no refresh', 'native4/64 close', 'NPC/OFF/DIRECT passthrough',
        '33 current binding/frame/tracking negative gates', '7 revoke-and-return cases',
        'unknown pre/post snapshot', 'nested64/teardown', 'replaced sameaction admission',
        'replaced action/release and body epoch', 'native target override before/after'],
    'stdout': run.stdout}
args.report.parent.mkdir(parents=True, exist_ok=True)
args.report.write_text(json.dumps(receipt, indent=2) + '\n')
print(run.stdout, end='')
print('PASS' if receipt['pass'] else 'FAIL', args.report)
raise SystemExit(0 if receipt['pass'] else 1)
