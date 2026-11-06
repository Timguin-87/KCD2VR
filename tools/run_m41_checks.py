"""Current-source M41 presenter/reader/Lua/bootstrap/native-book regression suite.

No game process is started. Native engine leaves are explicitly substituted.
The optional --control-no-model-gate runs a deliberate negative control after
the unmodified production-source tests and requires the control to fail.
"""
from pathlib import Path
import hashlib
import json
import subprocess
import sys
from m41_test_support import options


args = options('')
lane = Path(__file__).resolve().parent
forward = ['--source-root', str(args.source_root), '--out', str(args.out)]
for flag, value in [('--vcvars', args.vcvars), ('--lua-path', args.lua_path), ('--pak', args.pak)]:
    if value is not None:
        forward.extend([flag, str(value)])
for script in ['test_m41_presenter.py', 'test_m41_right_reader.py', 'test_m41_bootstrap.py']:
    subprocess.run([sys.executable, '-B', str(lane / script), *forward], check=True)
if args.control_no_model_gate:
    subprocess.run([sys.executable, '-B', str(lane / 'test_m41_presenter.py'),
                    *forward, '--control-no-model-gate'], check=True)

result = {name: json.loads((args.out / name / 'RESULT.json').read_text())
          for name in ['presenter', 'right_reader', 'lua', 'bootstrap']}
assert 'checks=33 failures=0' in result['presenter']['result']
assert '17/17' in result['right_reader']['result']
assert result['lua']['checks'] == 42
assert result['bootstrap']['actual_static_bootstrap_with_preserved_native_modules'] == 9
inputs = [
    'src/subsystems/vrmap/runtime/held_map_m1.cpp',
    'src/subsystems/vrmap/runtime/held_map_feed_t2.cpp',
    'src/subsystems/vrmap/committed_pose_service.inl',
    'src/subsystems/vrmap/static_paper_m41.h',
    'mods/KCD2VRMap/map_runtime.lua',
    'mods/KCD2VRMap/map_catalog_static_m41.lua',
    'mods/KCD2VRMap/map_navigation_t2.lua',
    'mods/KCD2VRMap/map_native_live_h28.lua',
    'mods/KCD2VRMap/map_native_read_h25.lua',
    'tools/test_native_map_live_h28.py',
    'tools/test_map_pose_reader_m39.py',
]
report = dict(result='PASS', source_root=str(args.source_root), cases=result,
              source_sha256={p: hashlib.sha256((args.source_root / p).read_bytes()).hexdigest() for p in inputs},
              native_leaves='SUBSTITUTED', headset='NOT_RUN', image_rendering='NOT_RUN',
              windows_dll_link='NOT_PERFORMED_BY_HOST_FIXTURES')
if args.control_no_model_gate:
    report['negative_control'] = json.loads((args.out / 'presenter_control_no_model_gate/RESULT.json').read_text())
(args.out / 'M41_RESULTS.json').write_text(json.dumps(report, indent=2) + '\n', encoding='utf-8')
print('M41 current-source checks PASS: presenter33, reader17, Lua42, bootstrap9, retained native-book fixture')
