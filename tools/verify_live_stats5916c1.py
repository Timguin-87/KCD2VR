#!/usr/bin/env python3
"""Read-only exact-retail pins used by the live attributes/effort adapter.

This verifies static bytes and data layout, not runtime object identity or an
applied native stamina debit. It neither executes the DLL nor changes it.
"""
from __future__ import annotations
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
import argparse
import hashlib
import json
import re
import struct
from pathlib import Path
from native_research112 import PE

EXPECTED_SHA = 'bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7'

def verify(path: Path) -> dict:
    pe = PE(str(path))
    sha = hashlib.sha256(pe.data).hexdigest()
    if pe.stamp != 0x6A350E20 or sha != EXPECTED_SHA:
        raise ValueError('Not the qualified retail DLL: do not use these offsets.')
    root = Path(__file__).resolve().parents[1]
    source = read_source(root / 'src/diagnostics/live_attributes5912.inl')
    checks = []
    def pin(name: str, rva: int, expected: bytes) -> None:
        actual = pe.read(rva, len(expected))
        if actual != expected:
            raise ValueError(f'{name} mismatch at {rva:#x}: {actual.hex()}')
        checks.append({'name': name, 'rva': hex(rva), 'bytes': actual.hex()})
    for name, rva in [('current',0x642B30),('maximum',0x46F1F4),('baseStat',0x469D7C)]:
        match = re.search(r'\b' + name + r'\[\]\{([^}]+)', source)
        if not match:
            raise ValueError('Live-source pin absent: ' + name)
        wanted = bytes(int(x, 16) for x in re.findall(r'0x([0-9A-Fa-f]+)', match.group(1)))
        pin(name, rva, wanted)
    pin('integer_base_stat_indexed_read', 0x469D8C, bytes.fromhex('8b84c16c050000'))
    for offset, target in [(0x270,0x642B30),(0x280,0x46F1F4)]:
        if struct.unpack('<Q',pe.read(0x3F44F28 + offset,8))[0] - pe.base != target:
            raise ValueError(f'Soul virtual slot mismatch: {offset:#x}')
    geom = read_source(root / 'src/diagnostics/geometry_hit178r27.inl')
    helper = geom[geom.index('static int EffortNative5916('):geom.index('static bool ChargeEffort5916(')]
    match = re.search(r'\bpin\[\]\{([^}]+)', helper)
    if not match:
        raise ValueError('Native effort entry pin absent.')
    pin('weapon_raised_native_cost_entry',0xC5BA24,
        bytes(int(x,16) for x in re.findall(r'0x([0-9A-Fa-f]+)',match.group(1))))
    pin('effort_actor_player_link',0xC5BA4A,bytes.fromhex('4c8b80f8010000'))
    pin('effort_player_soul_link',0xC5BAE3,bytes.fromhex('488b9268060000'))
    return {'sha256':sha,'timestamp':hex(pe.stamp),'checks':checks,
        'base_stat_storage':'signed integer, Soul+0x56C+8*index; not float',
        'current_stamina':'native current-state index 1',
        'maximum_stamina':'native maximum-state index 1 (derived maximum)',
        'level_scope':'base Strength/Agility, not buff-adjusted levels',
        'native_cost_scope':'preparation component, not calibrated variable-work/full-swing cost',
        'native_execution':False}

def main() -> None:
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('dll',type=Path)
    parser.add_argument('--out',type=Path)
    args=parser.parse_args()
    try:
        report=verify(args.dll)
    except (OSError,ValueError,KeyError,struct.error) as exc:
        parser.exit(1, f'Verification failed: {exc}\n')
    text=json.dumps(report,indent=2)+'\n'
    print(text,end='')
    if args.out:
        args.out.parent.mkdir(parents=True,exist_ok=True)
        args.out.write_text(text)

if __name__=='__main__':
    main()
