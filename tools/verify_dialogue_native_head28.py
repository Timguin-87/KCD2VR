"""Read-only exact-retail proof of the native View -> original HEAD getter chain.

No function is called, no game memory/file is modified. SDK names are not used
to derive slots: the expected entries and code bytes come from this retail PE.
"""
import argparse
import hashlib
import json
import struct
from pathlib import Path
from native_research112 import PE

SHA = 'bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7'
PINS = {
    0x6411AC: '40534883ec208bda33d2e8095ff9ff',
    0x53D830: '4883ec28448bc24883c108ba01000000e83b1d0000496bd01c4803c24883c428c3',
    0x1A81D90: '488b8168020000c3',
    0x1AA6BC0: '488d8198060000c3',
    0x641250: '48895c2408574883ec20488d99480200008bfa85d2781a488b03488bcbff50083bf8730d488b038bd7488bcbff5050eb07',
    0x641224: '6683fa3c770d480fbfc20fb784417c010000c3',
    0x8A49D0: '488b81700a0000c3',
    0x94D0D1: '33d2488b4818488b4918488b01ff9028030000',
    0x94D214: 'ffd2ba2d000000488bc8e8353fcfff0fbfd0488bceffd3',
    0x1A74B40: '8b4160c3',
}
ENTRIES = [
    (0x3AA4B98, 0x6E8, 0x1A81D90),
    (0x4072C78, 0x328, 0x6411AC),
    (0x40602F8, 0x28, 0x1A7FB10),
    (0x40602F8, 0x38, 0x1AA6BC0),
    (0x40602F8, 0x58, 0x8A49D0),
    (0x3A71028, 0xD8, 0x641250),
    (0x3A71510, 0x8, 0x1A74B40),
    (0x3A71510, 0x10, 0x53F550),
    (0x3A71510, 0x50, 0x53D830),
    (0x406B930, 0xA0, 0x641224),
]

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('whgame', type=Path)
    parser.add_argument('output', type=Path)
    args = parser.parse_args()
    pe = PE(args.whgame)
    digest = hashlib.sha256(pe.data).hexdigest()
    assert pe.stamp == 0x6A350E20 and digest == SHA, 'exact retail fingerprint mismatch'
    pins = []
    for rva, expected in PINS.items():
        actual = pe.read(rva, len(bytes.fromhex(expected))).hex()
        assert actual == expected, f'native leaf mismatch {rva:X}'
        pins.append({'rva':hex(rva), 'bytes':actual, 'verified':True})
    entries = []
    for vft, offset, target in ENTRIES:
        actual = struct.unpack('<Q', pe.read(vft + offset, 8))[0] - pe.base
        assert actual == target, f'native vft mismatch {vft:X}+{offset:X}'
        entries.append({'vft':hex(vft),'byteOffset':hex(offset),'targetRva':hex(actual),'verified':True})
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps({'whgame':str(args.whgame),'sha256':digest,
        'timestamp':hex(pe.stamp),'pins':pins,'entries':entries,'nativeExecuted':False,
        'writes':False,'scope':'original local player HEAD10 through native View only'}, indent=2)+'\n', encoding='utf-8')
    print(f'dialogue native head28: exact hash + {len(pins)} code pins + {len(entries)} native entries PASS (read-only)')

if __name__ == '__main__': main()
