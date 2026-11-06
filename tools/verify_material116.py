"""Verify the new observer against retail bytes, without loading or executing it."""
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
    source = read_source(ROOT/'src/vrik/material_observer116.inl')
    arrays = source.split('constexpr unsigned char bytes[][16]{', 1)[1].split('};', 1)[0]
    expected = [bytes(int(x, 16) for x in re.findall(r'0x([0-9a-f]+)', row))
                for row in re.findall(r'\{([^{}]+)\}', arrays)]
    rvas = [0xB24A0C, 0x52AA38, 0x52A9A4]
    assert len(expected) == len(rvas)
    entries = []
    for rva, fingerprint in zip(rvas, expected):
        assert len(fingerprint) == 16 and pe.read(rva, 16) == fingerprint, hex(rva)
        assert pe.function(rva)[0] == rva, 'hook must begin at the runtime-function entry'
        entries.append(dict(rva=hex(rva), bytes=fingerprint.hex(), match=True))
    pointers = []
    for slot, target in [(0x4056C88+0x20, 0x60C044), (0x4056C88+0x30, 0x52AA38),
                         (0x4056C88+0x38, 0x1545F3C), (0x4056C88+0x50, 0x52A9A4),
                         (0x3C25040, 0xB24CA0)]:
        actual = struct.unpack('<Q', pe.read(slot, 8))[0]-pe.base
        assert actual == target, (hex(slot), hex(actual), hex(target))
        pointers.append(dict(slot_rva=hex(slot), function_rva=hex(target), match=True))
    # Additional procedural collision caller discovered in this retail image.
    call = 0x27C1690
    assert pe.read(call, 1) == b'\xe8'
    assert call+5+struct.unpack('<i', pe.read(call+1, 4))[0] == 0xA7A124
    return dict(sha256=EXPECTED, timestamp=hex(pe.stamp), hooks=entries, pointers=pointers,
                procedural_sweep_return='0x27c1695', result='PASS',
                limitation='Static bytes and prototypes only; no live hook or headset validation')


if __name__ == '__main__':
    p = argparse.ArgumentParser(); p.add_argument('--whgame', type=Path, required=True)
    p.add_argument('--out', type=Path, required=True); a = p.parse_args()
    result = verify(a.whgame); a.out.parent.mkdir(parents=True, exist_ok=True)
    a.out.write_text(json.dumps(result, indent=2)+'\n', encoding='utf-8')
    print(json.dumps(result, indent=2))
