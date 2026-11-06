"""Run the maintained actual K37 provider/service host fixture with MSVC ASAN.
Native/device/VM/preview/payment leaves are substitutes. No game DLL is loaded.
Optional overlays are for isolated review; ordinary use reads current source.
"""
import argparse
import hashlib
import json
from pathlib import Path
import re
import subprocess
import sys

EXPECTED_RETAIL = 'bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7'
SERVICE = 'src/subsystems/throw_drop/runtime/native_ammo_service_t22.inl'
PROVIDER = 'src/subsystems/throw_drop/runtime/native_private_launcher_t21.inl'
COOLDOWN = '    c.last=ctx.now; // Rate-limit attempted native cue lookups, including a slow/refused one.\n'

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--source-root', type=Path, default=Path(__file__).resolve().parents[1])
    parser.add_argument('--fixture', type=Path)
    parser.add_argument('--production-overlay', type=Path, action='append', default=[])
    parser.add_argument('--dll', type=Path, required=True)
    parser.add_argument('--build-dir', type=Path, required=True)
    parser.add_argument('--out', type=Path, required=True)
    parser.add_argument('--without-cooldown-control', action='store_true')
    args = parser.parse_args()
    root = args.source_root.resolve()
    fixture = args.fixture or root/'tests/subsystems/throw_drop/native_ammo_k37_connected_test.cpp'
    build = args.build_dir.resolve()
    if build == root or root in build.parents:
        raise ValueError('Build output must be outside the source checkout')
    build.mkdir(parents=True, exist_ok=True)
    roots = [p.resolve() for p in args.production_overlay] + [root]
    def selected(rel):
        return next(p/rel for p in roots if (p/rel).is_file())
    provider, service = selected(PROVIDER), selected(SERVICE)
    inputs = {str(p): hashlib.sha256(p.read_bytes()).hexdigest() for p in (provider, service, fixture)}
    sys.path.insert(0, str(root/'tools'))
    from native_research112 import PE
    pe = PE(args.dll)
    assert hashlib.sha256(pe.data).hexdigest() == EXPECTED_RETAIL and pe.stamp == 0x6A350E20
    pins = []
    for m in re.finditer(r'\{(0x[0-9a-fA-F]+),\{([^}]+)\}\}', provider.read_text()):
        rva = int(m[1], 16)
        value = bytes(int(v.strip(), 0) for v in m[2].split(','))
        assert pe.read(rva, len(value)) == value
        pins.append((rva, value))
    text = service.read_text()
    for name, rva in [('slot',0x7F1E3C),('filename',0x1A81E20),('helper',0x51EF4C),('selected',0x2AAD1AD),('worldDisableT26',0xABDFF8),('proxyEnableT26',0xABE028),('getProxyT26',0x5D70C4)]:
        m = re.search(r'char '+name+r'\[\]\{([^}]+)\}', text)
        assert m, name
        value = bytes(int(v.strip(), 0) for v in m[1].split(','))
        assert pe.read(rva, len(value)) == value
        pins.append((rva, value))
    assert len(pins) == 13
    (build/'pins_k37_connected.inc').write_text('\n'.join('{0x%X,{%s}},' % (r, ','.join('0x%02x'%v for v in b)) for r,b in pins)+'\n')
    if args.without_cooldown_control:
        assert text.count(COOLDOWN) == 1
        mutant = build/'control'/SERVICE
        mutant.parent.mkdir(parents=True, exist_ok=True)
        mutant.write_text(text.replace(COOLDOWN, ''))
        roots.insert(0, build/'control')
    includes = ''.join(' /I "'+str(p/'src')+'"' for p in roots)+' /I "'+str(build)+'"'
    command = ('@echo off\ncall "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvars64.bat" >nul\n'
        'if errorlevel 1 exit /b %errorlevel%\n'
        'cl /nologo /std:c++latest /EHsc /Od /Zi /UNDEBUG /fsanitize=address'+includes+' "'+str(fixture)+'" /Fe:test.exe /link /INCREMENTAL:NO >build.log 2>&1\n'
        'if errorlevel 1 exit /b %errorlevel%\n'
        'test.exe >run.log 2>&1\nexit /b %errorlevel%\n')
    (build/'run.cmd').write_text(command)
    result = subprocess.run(['cmd.exe','/d','/c',str(build/'run.cmd')], cwd=build)
    compiler = (build/'build.log').read_text(errors='replace')
    log = (build/'run.log').read_text(errors='replace') if (build/'run.log').exists() else ''
    match = re.search(r'RESULT K37_CONNECTED checks=(\d+)', log)
    expected_control = args.without_cooldown_control and result.returncode != 0 and 'FAILED line=' in log and 'error C' not in compiler
    passed = expected_control if args.without_cooldown_control else result.returncode == 0 and bool(match) and int(match[1]) == 110
    report = dict(passed=bool(passed), exit_code=result.returncode, checks=int(match[1]) if match else None,
        actual_provider=True, actual_service=True, native_execution=False, input_sha256=inputs,
        native_static_pins=len(pins), without_cooldown_control=args.without_cooldown_control,
        expected_control_failure=bool(expected_control), run_log=str(build/'run.log'),
        limits=['VM/device/native ABI, preview/payment and physical leaf effects are substituted.',
                'Host success is not native hold/flight/damage or headset acceptance.'])
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text(json.dumps(report, indent=2)+'\n')
    print(compiler[-3000:]); print(log[-4000:])
    return 0 if passed else 1

if __name__ == '__main__':
    raise SystemExit(main())
