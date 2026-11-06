#!/usr/bin/env python3
"""Verify the exact retail selector before interpreting its H36 lifecycle."""
import argparse
import hashlib
import json
import re
import subprocess
from pathlib import Path
from native_research112 import PE

HERE = Path(__file__).resolve().parents[1]
EXPECTED = 'bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7'
PINS = {
    0x8EEA04: '4883ec284c8bc1e8f4f8000084c0740983fa037404418950404883c428c3',
    0x8FE304: '488b4138488b8890090000488b054a78a20448398160020000740332c0c34839816802000075f4b001c3',
}


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--dll', type=Path, required=True)
    p.add_argument('--out', type=Path, required=True)
    a = p.parse_args()
    out = a.out.resolve()
    out.mkdir(parents=True, exist_ok=True)
    pe = PE(a.dll)
    sha = hashlib.sha256(pe.data).hexdigest()
    pin_source = (HERE / 'src/retail_build112.cpp').read_text()
    pinned = re.search(r'constexpr std::array<unsigned char,32> expected\{(.*?)\};', pin_source, re.S).group(1)
    source_sha = ''.join(re.findall(r'0x([0-9a-fA-F]{2})', pinned)).lower()
    assert sha == EXPECTED == source_sha and pe.stamp == 0x6A350E20 and pe.base == 0x180000000
    assert pe.function(0x8EEA04) == (0x8EEA04, 0x8EEA22)
    rows = []
    for rva, expected in PINS.items():
        size = len(bytes.fromhex(expected))
        actual = pe.read(rva, size).hex()
        assert actual == expected, hex(rva)
        command = ['objdump', '-d', '-Mintel', '--start-address=' + hex(pe.base + rva),
                   '--stop-address=' + hex(pe.base + rva + size), str(a.dll.resolve())]
        d = subprocess.run(command, capture_output=True, text=True, check=True)
        (out / ('rva_' + format(rva, 'X') + '.asm')).write_text(d.stdout)
        rows.append({'rva': hex(rva), 'length': size, 'full_bytes': actual,
                     'pdata_bounds': pe.function(rva), 'command': command})
    report = {
        'passed': True, 'dll_sha256': sha, 'runtime_pinned_sha256': source_sha,
        'dll_bytes': len(pe.data), 'timestamp': hex(pe.stamp), 'image_base': hex(pe.base),
        'native_execution': False, 'pins': rows,
        'proven_facts': [
            '8EEA04 calls only 8FE304, tests its AL result and excludes set 3, then conditionally writes manager+0x40.',
            '8FE304 loads manager+0x38, actor+0x990, and compares held WUIDs +0x260/+0x268 with the global empty sentinel; it has no calls or writes.',
            'A normal returned SelectSet has no deferred attachment or world job to retire. Exact selected-set readback is still required before the same command Draw.',
            'An abnormal call is not given a normal-return receipt by D48; actual draw retirement still requires original native membership and binding evidence.',
        ],
        'limits': 'Static exact-image proof. Does not execute game callbacks, test a headset, or establish which historical runtime episode traversed this branch.'
    }
    (out / 'SELECTOR_PROOF.json').write_text(json.dumps(report, indent=2) + '\n')
    print('PASS exact retail selector: 30-byte synchronous setter + 42-byte read-only empty-hand predicate; native execution NONE')


if __name__ == '__main__':
    main()
