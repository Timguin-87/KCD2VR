#!/usr/bin/env python3
"""Reproduce the R59.6 review from a PE file and the saved runtime log; no native execution."""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
import argparse
from collections import Counter
import hashlib
import json
from pathlib import Path
import re
import struct


def audit(dll, log, root):
    b = dll.read_bytes()
    digest = hashlib.sha256(b).hexdigest()
    if digest != 'bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7':
        raise ValueError('Unsupported retail SHA-256; this audit must not label other binaries verified')
    pe = struct.unpack_from('<I', b, 0x3c)[0]
    count = struct.unpack_from('<H', b, pe + 6)[0]
    timestamp = struct.unpack_from('<I', b, pe + 8)[0]
    if timestamp != 0x6A350E20:
        raise ValueError('Unsupported timestamp')
    section = pe + 24 + struct.unpack_from('<H', b, pe + 20)[0]
    sections = [struct.unpack_from('<IIII', b, section + 40*i + 8) for i in range(count)]

    def entry(rva, n):
        for size, start, raw_size, raw in sections:
            if start <= rva and rva + n <= start + raw_size:
                return b[raw+rva-start:raw+rva-start+n]
        raise ValueError(f'Unbacked RVA {rva:x}')

    source = read_source(root/'src/diagnostics/geometry_hit178r27.inl')
    pins = []
    for name in ('React', 'OnHit', 'Down', 'Kind', 'Release', 'Dispatch', 'DeathPred'):
        symbol = 'kOuter' + name
        rva = int(re.search(r'\b'+symbol+r'=0x([0-9A-Fa-f]+)', source)[1], 16)
        array = re.search(symbol+r'Entry\[\]\{([^}]+)', source)[1]
        want = bytes(int(x, 16) for x in re.findall(r'0x([0-9a-fA-F]+)', array))
        actual = entry(rva, len(want))
        pins.append(dict(name=symbol, rva=f'{rva:X}', expected=want.hex(), actual=actual.hex(), match=want==actual))
    blob = log.read_bytes()
    lines = blob.decode('utf-8', errors='replace').splitlines()
    evidence = {}
    for name, token in {
        'dispatch_hook_installed':'REACTION_RECEIPTS ',
        'outer_rejected':'GEOM_OUTER rejected=',
        'direct_hits':' VR_HIT ',
        'native_reload_calls':' RELOAD_NATIVE_CALL ',
        'lifecycle_end':'F8_LIFECYCLE_END ',
        'impulse_returns':' PHYS_IMPULSE ',
    }.items():
        evidence[name] = [[i+1, x] for i, x in enumerate(lines) if token in x]
    fields = lambda line: dict(re.findall(r'(\w+)=([^\s]+)', line))
    effects, stages, outcomes, perfect = Counter(), Counter(), Counter(), []
    for i, line in enumerate(lines):
        if 'NATIVE_SERVICE seq=' in line:
            f = fields(line)
            stages[f.get('stage')] += 1
            if f.get('stage') == '4':
                effects[f.get('requestedEffectDelta')] += 1
            if f.get('stage') == '7' and f.get('flags') == '0x7':
                perfect.append([i+1, line])
        if 'GEOM_HIT serial=' in line:
            f = fields(line)
            outcomes[f.get('rejected', 'RESULT')] += 1
    return dict(retail_sha256=digest, timestamp=hex(timestamp), pins=pins,
        all_original_pins_match=all(x['match'] for x in pins),
        scope='Disk bytes and recorded outcomes only; no live patched-memory or headset execution',
        log_sha256=hashlib.sha256(blob).hexdigest(), line_count=len(lines),
        native_service_stages=stages, requested_effects=effects, geom_records=outcomes,
        perfect_block_hit_return_records=perfect, evidence=evidence)


if __name__ == '__main__':
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--dll', required=True, type=Path)
    p.add_argument('--log', required=True, type=Path)
    p.add_argument('--out', required=True, type=Path)
    a=p.parse_args()
    result=audit(a.dll, a.log, Path(__file__).resolve().parents[1])
    a.out.parent.mkdir(parents=True, exist_ok=True)
    a.out.write_text(json.dumps(result, indent=2)+'\n', encoding='utf-8')
    print(f"Original entry pins: {sum(x['match'] for x in result['pins'])}/7. Native execution: NONE.")
    raise SystemExit(0 if result['all_original_pins_match'] else 1)
