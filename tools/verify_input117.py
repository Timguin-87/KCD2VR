"""Check input-lifecycle contracts against the pinned retail PE; never load it."""
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
from discover_physics113 import EXPECTED

ROOT = Path(__file__).resolve().parents[1]


def verify(binary):
    pe = PE(binary)
    if hashlib.sha256(pe.data).hexdigest() != EXPECTED or pe.stamp != 0x6A350E20:
        raise ValueError('Unsupported WHGame fingerprint')
    source = read_source(ROOT/'src/vrik/native_input117.inl')
    block = source.split('const Pin pins[]{', 1)[1].split('};', 1)[0]
    entries = []
    for rva, values in re.findall(r'\{(0x[0-9A-F]+),\{([^}]+)\}\}', block):
        address = int(rva, 16)
        data = bytes(int(v.strip(), 0) for v in values.split(','))
        assert len(data) == 16 and pe.read(address, 16) == data, rva
        assert pe.function(address)[0] == address, rva
        entries.append(dict(rva=rva, bytes=data.hex(), match=True))
    assert len(entries) == 6
    calls = []
    for call, target in [(0x27C08E1, 0xD4AAC4), (0x27C0911, 0xD4A99C),
                         (0x27C0993, 0xD4AAC4), (0x27C094D, 0xD4A99C),
                         (0xD4A7FF, 0x939C84)]:
        assert pe.read(call, 1) == b'\xe8'
        assert call+5+struct.unpack('<i', pe.read(call+1, 4))[0] == target
        calls.append(dict(call=hex(call), target=hex(target), return_rva=hex(call+5)))
    assert pe.read(0x27C08CE, 4) == bytes.fromhex('83793800')  # prepare only from IDLE
    assert pe.read(0x27C08FE, 4) == bytes.fromhex('83793801')  # release commits only HELD
    assert pe.read(0x27C0916, 7) == bytes.fromhex('c7433800000000')
    assert pe.read(0xD4A804, 14) == bytes.fromhex('b80200000089433889433c894340')
    return dict(sha256=EXPECTED, timestamp=hex(pe.stamp), entry_fingerprints=entries,
                callsites=calls, cancelled_latch_reset='PASS', result='PASS',
                limitation='Static contracts only; live dispatch and native outcomes still require headset validation.')


if __name__ == '__main__':
    p = argparse.ArgumentParser();p.add_argument('--whgame', type=Path, required=True)
    p.add_argument('--out', type=Path, required=True);a = p.parse_args()
    result = verify(a.whgame);a.out.parent.mkdir(parents=True, exist_ok=True)
    a.out.write_text(json.dumps(result, indent=2)+'\n', encoding='utf-8')
    print(json.dumps(dict(result=result['result'], pins=len(result['entry_fingerprints']), calls=len(result['callsites']))))
