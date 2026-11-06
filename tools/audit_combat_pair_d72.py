#!/usr/bin/env python3
"""Read-only exact-retail audit for D72 pair, endpoint and kick boundaries.

This verifies bytes, RTTI and relative native calls. It does not execute the
pair ABI, animation notifications or the game. The native kick selector has a
separate executable machine-code harness.
"""
from pathlib import Path
import argparse
import hashlib
import json
import re
import struct

from native_research112 import PE

RETAIL_SHA256 = 'bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7'
ROOT = Path(__file__).resolve().parents[1]


def function(source, name):
    match = re.search(r'static [^;{}\n]*\b' + re.escape(name) + r'\([^;{}]*\)[^{;]*\{', source)
    if not match:
        raise ValueError('Missing production definition: ' + name)
    start = match.start()
    end = source.index('{', match.start()) + 1
    depth = 1
    while depth:
        depth += (source[end] == '{') - (source[end] == '}')
        end += 1
    return source[start:end]


def numbers(value):
    return [int(v.strip(), 0) for v in value.split(',') if v.strip()]


def audit(args):
    pe = PE(args.dll)
    records = []

    def check(name, passed, **detail):
        records.append(dict(name=name, passed=bool(passed), **detail))

    seal = hashlib.sha256(pe.data).hexdigest()
    check('Exact independently supplied retail binary', seal == RETAIL_SHA256, actual=seal, expected=RETAIL_SHA256)
    check('PE timestamp', pe.stamp == 0x6A350E20, actual=hex(pe.stamp), expected='0x6a350e20')
    check('Native image base', pe.base == 0x180000000, actual=hex(pe.base))
    sources = {}

    def source(relative):
        path = args.source / relative
        raw = path.read_bytes()
        sources[relative] = hashlib.sha256(raw).hexdigest()
        return raw.decode()

    def pin(label, rva, wanted):
        actual = pe.read(rva, len(wanted))
        check(label, actual == wanted, rva=hex(rva), expected_hex=wanted.hex(), actual_hex=actual.hex())

    # Read the live production arrays, so changes cannot silently leave a
    # detached validation copy passing while the installed pins differ.
    for relative, name, expected_count in (
        ('src/subsystems/masterstrike_combos/ms_reaction_owner_d62c.inl', 'MsPairPinsD72', 8),
        ('src/vrik/native_bridges168.inl', 'ArmFreeKickD72', 4),
    ):
        body = function(source(relative), name)
        array = body.split('pins[]{', 1)[1].split('};', 1)[0]
        rows = re.findall(r'\{(0x[0-9A-Fa-f]+),\s*(\d+),\s*\{([^{}]*)\}\}', array)
        check(name + ' production pin coverage', len(rows) == expected_count, actual=len(rows), expected=expected_count)
        for addr, size, values in rows:
            expected = bytes(numbers(values))
            check(name + ' declared length ' + addr, len(expected) == int(size))
            pin(name + ' ' + addr, int(addr, 16), expected)

    final = source('src/subsystems/masterstrike_combos/physical_finisher_gate_cb1.inl')
    guard = function(source('src/vrik/native_bridges168.inl'), 'NativeFreeKickGuardTypeD84')
    guard_reader = re.search(r'unsigned char use\[\]\{([^{}]*)\}', guard)
    if not guard_reader:
        raise ValueError('Missing current FreeGuard reader pin')
    guard_bytes = bytes(numbers(guard_reader.group(1)))
    check('Current FreeGuard reader includes the virtual getter slot', len(guard_bytes) == 17 and guard_bytes[-3:] == b'\xff\x50\x08')
    pin('D84 current FreeGuard reader', 0x12EDF58, guard_bytes)
    for name, address in (('inputFamilyD72', 0x225923E), ('chargedD72', 0x2259256)):
        match = re.search(r'unsigned char ' + name + r'\[\]\{([^{}]*)\}', final)
        if not match:
            raise ValueError('Missing production final pin: ' + name)
        check(name + ' is wired to native address', bool(re.search(r'pin\(0x' + f'{address:X}' + ',' + name + r',', final)))
        pin(name, address, bytes(numbers(match.group(1))))

    layout = source('src/subsystems/masterstrike_combos/native_pair_d72.h')
    for name, wanted in (
        ('combo', [0x3A8B608, 0x3A993D8, 0x70, 0x80, 0xA0, 0xC0]),
        ('master', [0x3B933B0, 0x3E40740, 0x88, 0x98, 0xF0, 0xA0]),
    ):
        got = numbers(re.search(r'Layout ' + name + r'\{([^{}]*)\}', layout).group(1))
        check(name + ' production layout', got == wanted, actual=[hex(x) for x in got])

    # MSVC complete-object locator -> type descriptor, without guessing from
    # a function name or queue order.
    for vtable, name in (
        (0x3A8B608, 'C_CombatActorActionSyncAttack'),
        (0x3A993D8, 'C_CombatActorActionSyncHit'),
        (0x3B933B0, 'C_CombatActorActionSyncPerfectBlock'),
        (0x3E40740, 'C_CombatActorActionSyncPerfectBlockHit'),
        (0x3A60C88, 'C_CombatAnimAction'),
        (0x3E3FAC8, 'C_CombatActor'),
        (0x3A80610, 'C_CombatPlayer'),
    ):
        locator = struct.unpack('<Q', pe.read(vtable - 8, 8))[0] - pe.base
        descriptor = struct.unpack('<I', pe.read(locator + 12, 4))[0]
        actual = pe.read(descriptor + 16, 160).split(b'\0')[0].decode('ascii')
        expected = '.?AV' + name + '@combatmodule@wh@@'
        check('RTTI ' + name, actual == expected, vtable=hex(vtable), locator=hex(locator), descriptor=hex(descriptor), actual=actual)

    for label, address, encoded in (
        ('Combo victim reciprocal write after accepted native admission', 0xD4B489, '498b064885c074074889b0c0000000'),
        ('Master victim reciprocal write guarded by native admission AL', 0x10F30D3, '488bd6488bcfe8729781ff84c0744f488b06488998a0000000'),
        ('Combo reciprocal teardown clears counterpart and own link', 0xD4B372, '4881c1a0000000e8de84f0ff48c787c000000000000000'),
        ('Fragment entity derives from actor entity owner', 0x90EC8C, '498b80f80100008b4830498bc241898a9c000000'),
        ('Fragment completion starts unset', 0x90EC85, '44889998000000'),
        ('Victim actor and fragment fields', 0xD4B16B, '488d7378e84cc47aff488d7b68'),
        ('Master attacker fragment field', 0xC51CEA, '488d9f98000000488bd0488bcb'),
        ('Native request type uses actual weapon primary/secondary types', 0x12EE09A, '84c0750e8b4734488b5c24304883c4205fc38b4738ebf0'),
    ):
        pin(label, address, bytes.fromhex(encoded))

    for label, address, expected in (
        ('Combo semantic admission', 0xD4B474, 0x90C850),
        ('Master semantic admission', 0x10F30D9, 0x90C850),
        ('Combo native fragment construction', 0xD4B8FF, 0x90EBDC),
        ('Master native fragment construction', 0xC51CE5, 0x90EBDC),
        ('Combo original fragment queue', 0xD4BA16, 0xD4C600),
        ('Master original fragment queue', 0xC51D7A, 0xD4C600),
        ('Free request call in query builder', 0x687C75, 0x12EE05C),
        ('Free request call in readiness probe', 0x12EDFEF, 0x12EE05C),
        ('Free request call in start-pose query', 0x12EDF48, 0x12EE05C),
    ):
        raw = pe.read(address, 5)
        actual = address + 5 + struct.unpack('<i', raw[1:])[0] if raw[0] == 0xE8 else None
        check(label, actual == expected, call_rva=hex(address), actual_target=hex(actual) if actual else None, expected_target=hex(expected))

    # Reproducible static evidence ranges, sealed by the whole retail hash.
    # Their hashes are receipts, not a substitute for executing engine events.
    ranges = []
    for label, start, end in (
        ('Combo native pair admission', 0xD4B398, 0xD4B4F0),
        ('Master native pair admission', 0x10F3030, 0x10F3148),
        ('Combo native fragment creation and queue', 0xD4B794, 0xD4BA30),
        ('Master native fragment creation and queue', 0xC51BB4, 0xC51D7F),
        ('Victim fragment creation and completion binding', 0xD4B104, 0xD4B2C8),
        ('Combat fragment constructor', 0x90EBDC, 0x90ECD8),
        ('Actual equipped native request type getter', 0x12EE05C, 0x12EE0B4),
    ):
        raw = pe.read(start, end-start)
        ranges.append(dict(name=label, start=hex(start), end=hex(end), size=len(raw), sha256=hashlib.sha256(raw).hexdigest()))
    failures = [r for r in records if not r['passed']]
    report = dict(status='PASS_STATIC_RETAIL' if not failures else 'FAIL', checks=len(records), failures=failures,
        retail=str(args.dll.resolve()), retail_sha256=seal, source_sha256=sources, records=records, ranges=ranges,
        limits=['Read-only static native bytes, RTTI and call targets.',
                'Native pair/animation event ABI execution NOT_RUN; game/headset NOT_RUN.',
                'Separate native_kick_d72_test.cpp executes copied native selection/admission routines with declared engine leaves.',
                'The runtime retains original pair admission, event dispatch, costs and effects; bounded speed is not same-frame completion.'])
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text(json.dumps(report, indent=2) + '\n')
    print(f'D84 combat exact-retail static audit: {len(records)-len(failures)}/{len(records)}; pair/game execution NOT_RUN')
    for record in failures:
        print('FAIL', record['name'], json.dumps(record))
    return bool(failures)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--dll', type=Path, required=True)
    parser.add_argument('--out', type=Path, required=True)
    parser.add_argument('--source', type=Path, default=ROOT)
    raise SystemExit(audit(parser.parse_args()))
