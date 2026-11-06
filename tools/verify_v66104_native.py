#!/usr/bin/env python3
"""Read-only .104 native contract check. Does not load or execute the game.

Usage: python tools/verify_v66104_native.py --whgame PATH --out checks.json
Matching bytes/slots do not validate complete layouts, threading or gameplay.
"""
from __future__ import annotations
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source, read_source_bytes
import argparse
import bisect
import hashlib
import json
from pathlib import Path
import re
import struct
import sys

EXPECTED_SHA = 'bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7'

def inspect(dll: Path, source: Path) -> dict:
    data = dll.read_bytes()
    if len(data) < 64 or data[:2] != b'MZ':
        raise ValueError('Input is not a PE image')
    pe = struct.unpack_from('<I', data, 0x3c)[0]
    if data[pe:pe+4] != b'PE\0\0':
        raise ValueError('Missing PE signature')
    machine, nsec, stamp = struct.unpack_from('<HHI', data, pe+4)
    opt = pe+24
    optsize = struct.unpack_from('<H', data, pe+20)[0]
    if struct.unpack_from('<H', data, opt)[0] != 0x20b:
        raise ValueError('Expected PE32+ x64')
    base = struct.unpack_from('<Q', data, opt+24)[0]
    sections = []
    for i in range(nsec):
        p = opt+optsize+i*40
        _, va, size, raw = struct.unpack_from('<IIII', data, p+8)
        sections.append((va, size, raw))
    def offset(rva: int, size: int = 1) -> int:
        for va, rawsize, raw in sections:
            if va <= rva and rva+size <= va+rawsize:
                where = raw+rva-va
                if where+size <= len(data):
                    return where
        raise ValueError(f'RVA {rva:#x} is not fully file-backed')
    er, es = struct.unpack_from('<II', data, opt+112+3*8)
    if es % 12:
        raise ValueError('Unexpected exception-directory length')
    start = offset(er, es)
    entries = [struct.unpack_from('<III', data, p) for p in range(start, start+es, 12)]
    starts = [x[0] for x in entries]
    text = read_source(source, encoding='utf-8')
    checks = []
    def check(name: str, ok: bool, **extra) -> None:
        checks.append(dict(name=name, passed=bool(ok), **extra))
    check('exact supplied image hash', hashlib.sha256(data).hexdigest() == EXPECTED_SHA)
    check('supported x64 timestamp', machine == 0x8664 and stamp == 0x6A350E20,
          machine=hex(machine), timestamp=hex(stamp))
    for name, var, length, rva in [('RequestAttack', 'req', 24, 0x93A28C),
                                   ('SetAttackZone', 'zone', 24, 0x90F824),
                                   ('DamageDispatch', 'expected', 16, 0x72704C)]:
        match = re.search(r'const unsigned char '+var+r'\['+str(length)+r'\]\{([^}]+)\}', text)
        if not match:
            raise ValueError(f'Cannot extract production fingerprint {var}')
        wanted = bytes(int(v.strip(), 0) for v in match.group(1).split(','))
        actual = data[offset(rva, length):offset(rva, length)+length]
        check(name+' production fingerprint', actual == wanted and len(wanted) == length,
              rva=hex(rva), expected=wanted.hex(), actual=actual.hex())
        ix = bisect.bisect_left(starts, rva)
        exact = ix < len(entries) and entries[ix][0] == rva
        check(name+' exact unwind entry', exact,
              function_range=[hex(v) for v in entries[ix][:2]] if exact else None)
    for slot, target, label in [(74, 0x93A28C, 'RequestAttack'), (73, 0x2756D58, 'IsAttacking')]:
        address = struct.unpack_from('<Q', data, offset(0x3A80610+8*slot, 8))[0]
        check('C_CombatPlayer '+label+' slot', address == base+target,
              slot=slot, expected_rva=hex(target), actual_rva=hex(address-base))
    return dict(build='V66.104-NATIVE-INTENT-WORKBUDGET',
                passed=all(c['passed'] for c in checks), checks=checks,
                dll_sha256=hashlib.sha256(data).hexdigest(),
                production_native_source_sha256=hashlib.sha256(read_source_bytes(source)).hexdigest(),
                limitations='Read-only bytes and selected vtable entries; not full ABI/layout validation, native execution, accepted attacks/hits, or Windows linking.')

def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--whgame', type=Path, required=True)
    parser.add_argument('--out', type=Path, required=True)
    parser.add_argument('--source', type=Path, default=Path(__file__).resolve().parents[1]/'src/vrik/native_combat104.inl')
    args = parser.parse_args()
    try:
        result = inspect(args.whgame, args.source)
        args.out.parent.mkdir(parents=True, exist_ok=True)
        args.out.write_text(json.dumps(result, indent=2)+'\n', encoding='utf-8')
        print(f"{'PASS' if result['passed'] else 'FAIL'} {sum(c['passed'] for c in result['checks'])}/{len(result['checks'])} read-only .104 native contracts")
        return 0 if result['passed'] else 1
    except (OSError, ValueError, struct.error) as exc:
        print(f'Native verification failed: {exc}', file=sys.stderr)
        return 2
if __name__ == '__main__':
    raise SystemExit(main())
