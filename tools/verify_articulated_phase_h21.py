"""Read-only exact DLL proof for the typed T4 articulated-body bridge."""
from pathlib import Path
import argparse
import hashlib
import json
import re
import struct

SHA = 'bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7'
ROOT = Path(__file__).resolve().parents[1]

def verify(binary):
    blob = binary.read_bytes()
    assert hashlib.sha256(blob).hexdigest() == SHA, 'wrong retail binary'
    pe = struct.unpack_from('<I', blob, 0x3c)[0]
    count = struct.unpack_from('<H', blob, pe + 6)[0]
    optional = struct.unpack_from('<H', blob, pe + 20)[0]
    image = struct.unpack_from('<Q', blob, pe + 24 + 24)[0]
    sections = [struct.unpack_from('<IIII', blob, pe + 24 + optional + i * 40 + 8) for i in range(count)]
    def read(rva, size):
        for virtual_size, va, raw_size, raw in sections:
            if va <= rva and rva + size <= va + min(virtual_size, raw_size):
                return blob[raw + rva - va:raw + rva - va + size]
        raise AssertionError(hex(rva))
    source = (ROOT / 'src/subsystems/physics/runtime/native_articulated_phase_h21.inl').read_text()
    pins = {'status': 0x8a9348, 'action': 0xa99df0, 'addBranch': 0xa99e5f,
            'addDelegate': 0x226fb2a, 'removeDelegate': 0xa9a978, 'statusDelegate': 0x8a9cc0}
    for name, rva in pins.items():
        body = re.search(r'constexpr unsigned char ' + name + r'\[\]\{([^}]+)\}', source).group(1)
        pin = bytes(int(s, 16) for s in re.findall(r'0x[0-9A-Fa-f]+', body))
        assert pin and read(rva, len(pin)) == pin, name
    slots = {8: 0x1a72e50, 0x10: 0x5311c4, 0x18: 0x531f84, 0x30: 0x8a9348, 0x38: 0xa99df0, 0x50: 0x5d6f44}
    for slot, target in slots.items():
        assert struct.unpack('<Q', read(0x3a48b30 + slot, 8))[0] == image + target
    # Articulated action5 delegates before the normal queue helper; action6 is
    # not2/4/10 and falls through to the rigid-base Action with safe=1.
    assert read(0xa99e14, 3) == bytes.fromhex('83 3a 05')
    six = bytes.fromhex('41 bd 02 00 00 00 45 39 2f 74 6d 45 8d 65 02 45 39 27 74 21 41 83 3f 0a 0f 84 05 02 00 00 45 8d 45 ff 49 8b d7 48 8b cf')
    assert read(0xa9a950, len(six)) == six
    for rva, encoded in [(0x8a9362, '83 3a 06'), (0x8a9393, '0f 85 8d 01 00 00'),
                         (0x8a9526, '83 3a 1c 75 3e'), (0x8a9569, '83 3a 08 0f 85 4e 07 00 00'),
                         (0x8a6d04, '83 3f 1a 0f 85 4c 01 00 00'), (0x8a67f0, '89 47 04')]:
        expected = bytes.fromhex(encoded)
        assert read(rva, len(expected)) == expected
    # Full exact SHA plus direct call-target checks accompany the documented
    # disassembly; never select slots from a generic third-party SDK layout.
    for call, target in [(0x226fb2a, 0xac0d18), (0xa9a978, 0xac0d18), (0x8a9cc0, 0x8a6c90)]:
        assert read(call, 1) == b'\xe8'
        assert call + 5 + struct.unpack('<i', read(call + 1, 4))[0] == target
    return {'retail_sha256': SHA, 'articulated_vft': '0x3A48B30',
            'vtable_slots': {hex(k): hex(v) for k, v in slots.items()},
            'type5_delegate': 'A99E5F ->226FB2A ->AC0D18, original safe parameter',
            'type6_delegate': 'A9A978 ->AC0D18, safe=1',
            'type26_status_delegate': '8A9CC0 ->8A6C90',
            'part': 'type1 raw part at+4, index=-1;8A68EA scans current part ids;8A67F0 writes actual id',
            'limits': 'Static ABI proof only. Corpse death/main/relinquish/source freshness belongs to the H18 native caller. No headset acceptance.'}

if __name__ == '__main__':
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('binary', type=Path)
    print(json.dumps(verify(p.parse_args().binary), indent=2))
