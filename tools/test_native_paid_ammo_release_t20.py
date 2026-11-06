"""Fresh retail byte qualification + actual paid T20 contribution MSVC/ASAN fixture.

Reads WHGame as data only. Does not load a DLL, build the game ASI or install.
All compiler output lives in the explicit external --build-dir.
"""
import argparse
import hashlib
import json
from pathlib import Path
import re
import struct
import subprocess
from native_research112 import PE

EXPECTED_SHA = 'bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7'


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--dll', type=Path, required=True)
    ap.add_argument('--build-dir', type=Path, required=True)
    ap.add_argument('--out', type=Path, required=True)
    args = ap.parse_args()
    root = Path(__file__).resolve().parents[1]
    build = args.build_dir.resolve()
    if build == root or root in build.parents:
        raise SystemExit('Build output must be outside the source checkout')
    build.mkdir(parents=True, exist_ok=True)
    contribution = root / 'src/subsystems/throw_drop/runtime/native_paid_ammo_release_t20.inl'
    stock_equipment = root / 'src/subsystems/throw_drop/runtime/stock_equipment_d65.inl'
    header = root / 'src/subsystems/throw_drop/native_paid_ammo_release_t20.h'
    fixture = root / 'tests/subsystems/throw_drop/native_paid_ammo_release_t20_test.cpp'
    transient = root/'src/subsystems/throw_drop/runtime/native_transient_ammo_t20.inl'
    factory = root/'src/subsystems/throw_drop/runtime/native_ammo_launch_t17.inl'
    source = transient.read_text()+'\n'+contribution.read_text()
    pins = [(int(m[1], 16), bytes(int(x.strip(), 0) for x in m[2].split(',')))
            for m in re.finditer(r'\{(0x[0-9A-Fa-f]+),\{([^}]+)\}\}', source)]
    assert len(pins) >= 10 and all(len(b) == 16 for _, b in pins)
    p = PE(args.dll)
    checks = []

    def ck(name, actual, expected):
        checks.append(dict(name=name, actual=actual, expected=expected, passed=actual == expected))

    ck('exact retail SHA256', hashlib.sha256(p.data).hexdigest(), EXPECTED_SHA)
    ck('exact retail PE timestamp', p.stamp, 0x6A350E20)
    for rva, expected in pins:
        ck(f'actual contribution pin {rva:X}', p.read(rva, len(expected)).hex(), expected.hex())
    ck('WorldInventory no-merge policy live vslot80',
       struct.unpack('<Q', p.read(0x3A83DC0+0x80, 8))[0]-p.base, 0x838AE0)
    for name, table, slot, target in [
            ('native Arrow Init sets entity NO_SAVE', 0x3ECE6F0, 0x38, 0x2AA7B20),
            ('actual CEntity GetFlags DWORD+18', 0x4072C78, 0x30, 0x1A72850),
            ('global entity CanSerialize NO_SAVE consumer', 0x4072C78, 0x448, 0xD59E04)]:
        ck(name, struct.unpack('<Q', p.read(table+slot, 8))[0]-p.base, target)
    # Actual native callsite/record extent, including MOVAPS GUID alignment and
    # exact cleanup. These checks are independent of the fixture's native leaves.
    extra = [
        (0x453435, '0f2802f30f7f4150'),
        (0x45343D, 'c7416025000000'),
        (0x453449, '44894164'),
        (0x4534CC, 'e87fa79e01'),
        (0x2AA01DB, 'e804329bfd'),
        (0x2AA01E0, '4533c9488d542440488bce458d410ae8cc5d9cfd'),
        (0x2AA01FB, '488d8c2418010000e8d8db39ff'),
        (0x2AA0208, '4c8b8424d8000000488d9424d8000000488d8c24d80000004d8b4008e88b2c9bfd'),
        (0x2AA0229, '488b8c24d8000000ba30000000e8555201ff'),
        (0x289F5A6, '448a4d584d8d87180100008a45504180f101498bd48845c8e8356affff'),
        (0xBEAB7E, '488d05cb6c2d0383cfff488901'),
        (0xBE8A63, '488bc8e8fd200000'),
        (0xBE8A75, '488987c8000000'),
        (0xBE8E6C, '488b87c8000000'),
        (0xBE8E84, '488905edeb8904'),
        (0x68C229, '488b0d48b8df04e8ab47d401'),
        (0x3E27B1, '488b88e0000000488b89c8000000e81ce2fe01'),
        (0x479785,'4585c97416443b4a507310'),
        (0x179CF7A,'e83d0000008b5350488bcb03d7e8a8bbccfe'),
        (0x179CFA8,'b001'),
        (0x289F81C,'e86bfcffff488bf84885c00f84b3010000'),
    ]
    for rva, expected in extra:
        ck(f'native ABI/caller {rva:X}', p.read(rva, len(bytes.fromhex(expected))).hex(), expected)
    ranges = []
    for rva, reason in [
            (0x479758, 'native inventory remove-one; partial branch delegates negative count delta, whole branch retires original WUID'),
            (0x179CF50, 'native partial delta pre-notifies, reads current amount, sets amount and post-notifies; no clone/move/merge'),
            (0x179D064, 'native negative delta permission branch returns true'),
            (0x479660, 'whole removal retires original ItemManager registry WUID after native representation cleanup'),
            (0x2895FF8, 'native Ammo payload clone or one-unit transfer from arbitrary current inventory'),
            (0x289F7A0, 'complete native shoot and exact captured factory caller'),
            (0x40B730, 'native held entity cleanup')]:
        start, end = p.function(rva)
        ranges.append(dict(rva_start=hex(start), rva_end_exclusive=hex(end),
                           sha256=hashlib.sha256(p.read(start,end-start)).hexdigest(), reason=reason))
    report = dict(schema=1, native_execution=False, shipped_or_enabled=False,
                  native_checks=checks, native_function_ranges=ranges, fixture=None,
                  limits=['Native leaves are substituted in the host fixture.',
                          'No cost/launch atomicity, cross-save durability, private launcher lifetime or headset acceptance claim.',
                          'Current production carrier includes this provider; retail execution is still not exercised by this host fixture.',
                          'Exact original factory-return transfer is observed before later contact; contact is mocked here, not native damage acceptance.',
                          'Coherently replaced mutable receipts lose the immutable original scope and cannot authorize cleanup or a successful launch.'],
                  input_sha256={str(f.relative_to(root)): hashlib.sha256(f.read_bytes()).hexdigest()
                                for f in [contribution, stock_equipment, header, fixture, transient, factory,
                                          root/'src/native_script_t2.h', root/'src/native_script_t2.cpp']})
    args.out.parent.mkdir(parents=True, exist_ok=True)
    if not all(c['passed'] for c in checks):
        report['all_passed'] = False
        args.out.write_text(json.dumps(report, indent=2)+'\n')
        print(json.dumps([c for c in checks if not c['passed']], indent=2))
        return 1
    (build/'paid_pins_t20.inc').write_text('\n'.join(
        '{0x%X,{%s}},' % (rva, ','.join('0x%02x'%b for b in value)) for rva, value in pins)+'\n')
    factory_text=factory.read_text()
    (build/'factory_capture_paid_t20.inc').write_text(factory_text[factory_text.index('using CreateAmmoFnT17='):factory_text.index('static bool InstallAmmoLaunchT17()')])
    vcvars = Path('C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvars64.bat')
    cmd = build/'run.cmd'
    cmd.write_text('@echo off\ncall "'+str(vcvars)+'" >nul\n'
                   'if errorlevel 1 exit /b %errorlevel%\n'
                   'cd /d "'+str(build)+'"\n'
                   'cl /nologo /std:c++20 /EHsc /Od /Zi /UNDEBUG /fsanitize=address '
                   '/I "'+str(root/'src')+'" /I "'+str(build)+'" '
                   '"'+str(fixture)+'" /Fe:test.exe\n'
                   'if errorlevel 1 exit /b %errorlevel%\n'
                   'test.exe\nexit /b %errorlevel%\n')
    result = subprocess.run(['cmd.exe', '/d', '/c', str(cmd)], text=True,
                            stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    (build/'RUN.log').write_text(result.stdout)
    outputs = [line for line in result.stdout.splitlines() if line.startswith('{"checks":')]
    report['fixture'] = json.loads(outputs[-1]) if outputs else None
    report['fixture_exit_code'] = result.returncode
    report['all_passed'] = result.returncode == 0 and bool(report['fixture'])
    report['run_log'] = str(build/'RUN.log')
    args.out.write_text(json.dumps(report, indent=2)+'\n')
    print(result.stdout)
    print('Native checks: %d/%d' % (sum(c['passed'] for c in checks), len(checks)))
    return 0 if report['all_passed'] else 1


if __name__ == '__main__':
    raise SystemExit(main())
