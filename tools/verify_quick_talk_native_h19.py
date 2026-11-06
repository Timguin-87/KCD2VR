"""Read-only exact-retail proof for the dedicated chat observer. Standard Python only."""
from pathlib import Path
import argparse
import hashlib
import json
import re
import struct

EXPECTED = 'bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7'
ROOT = Path(__file__).resolve().parents[1]

def verify(path):
    data = path.read_bytes()
    assert hashlib.sha256(data).hexdigest() == EXPECTED, 'wrong retail binary'
    pe = struct.unpack_from('<I', data, 0x3C)[0]
    count, = struct.unpack_from('<H', data, pe + 6)
    optional, = struct.unpack_from('<H', data, pe + 20)
    section = pe + 24 + optional
    sections = [struct.unpack_from('<IIII', data, section + i * 40 + 8) for i in range(count)]
    def read(rva, size):
        for virtual_size, va, raw_size, raw in sections:
            if va <= rva and rva + size <= va + min(virtual_size, raw_size):
                return data[raw + rva - va:raw + rva - va + size]
        raise AssertionError(hex(rva))
    source = (ROOT / 'src/subsystems/hud_ui_controls/runtime/quick_talk_native_h19.inl').read_text()
    pins = {'entry': 0x6160EC, 'input': 0x616128, 'selector': 0x61615A}
    for name, rva in pins.items():
        body = re.search(r'constexpr unsigned char ' + name + r'\[\]\{([^}]+)\}', source).group(1)
        expected = bytes(int(x, 16) for x in re.findall(r'0x[0-9A-Fa-f]+', body))
        assert expected and read(rva, len(expected)) == expected, name
    assert read(0x3A42518, 17) == b'chat_interaction\0'
    assert read(0x3A3E084, 7) == b'dialog\0'
    assert read(0x616109, 7) == bytes.fromhex('48 8d 15 08 c4 42 03')
    assert 0x616109 + 7 + struct.unpack('<i', read(0x61610C, 4))[0] == 0x3A42518
    # Dedicated open/close helpers call this exact chat-map wrapper.
    for call in (0x616753, 0x617A72):
        assert read(call, 1) == b'\xe8'
        assert call + 5 + struct.unpack('<i', read(call + 1, 4))[0] == 0x6160EC
    return {'retail_sha256': EXPECTED, 'pins': {k: hex(v) for k, v in pins.items()},
            'dedicated_map': 'chat_interaction', 'dialogue_branch': 'self+35 == 0 excluded',
            'native_calls': ['616753 close', '617A72 open'],
            'scope': 'static byte/control-flow proof; not live UI or headset acceptance'}

if __name__ == '__main__':
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('binary', type=Path)
    a = p.parse_args()
    print(json.dumps(verify(a.binary), indent=2))
