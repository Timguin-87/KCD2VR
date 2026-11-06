"""Read-only verification of the R50C2 clock boundary in the exact retail DLL."""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
import argparse, hashlib, json, re, struct
from pathlib import Path

p = argparse.ArgumentParser(description=__doc__)
p.add_argument('--dll', type=Path, required=True)
p.add_argument('--out', type=Path, required=True)
a = p.parse_args()
raw = a.dll.read_bytes()
digest = hashlib.sha256(raw).hexdigest()
assert digest == 'bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7'
pe = struct.unpack_from('<I', raw, 0x3c)[0]
assert raw[pe:pe+4] == b'PE\0\0' and struct.unpack_from('<I', raw, pe+8)[0] == 0x6A350E20
n, optional = struct.unpack_from('<H', raw, pe+6)[0], struct.unpack_from('<H', raw, pe+20)[0]
sections = [struct.unpack_from('<IIII', raw, pe+24+optional+i*40+8) for i in range(n)]
def read(rva, length):
    for _, va, size, off in sections:
        if va <= rva and rva+length <= va+size:
            return raw[off+rva-va:off+rva-va+length]
    raise ValueError(hex(rva))

header = Path(__file__).resolve().parents[1]/'src/vrik/attack_clock50c2.h'
records = []
for address, values in re.findall(r'\{(0x[0-9A-F]+),\{([^}]+)\}\}', read_source(header)):
    expected = bytes(int(v, 16) for v in values.split(','))
    assert len(expected) == 16 and read(int(address, 16), 16) == expected, address
    records.append({'rva': address, 'bytes': expected.hex()})
assert len(records) == 14
base = struct.unpack_from('<Q', raw, pe+24+24)[0]
assert struct.unpack('<Q', read(0x3A61C58+0x1C8, 8))[0] == base+0xB27308
assert struct.unpack('<Q', read(0x3A61C58+0x80, 8))[0] == base+0x1A72810
assert read(0x1A72810, 4) == bytes.fromhex('8b413cc3')  # action channel, NOT attack phase
assert read(0x9E3C16, 14) == bytes.fromhex('0f28c80f28e8f30f594b100f57f6')
a.out.parent.mkdir(parents=True, exist_ok=True)
a.out.write_text(json.dumps({'dll': str(a.dll), 'sha256': digest, 'timestamp': '0x6A350E20',
    'nativeExecution': False, 'pins': records, 'vtableStartAndChannelGetter': True,
    'hookSite': '9E3C16: RBX is coordinator, XMM0 native dt; no register write',
    'writes': 'coordinator +50/+54 and three 0x14-byte record +C/+10 fields only',
    'limitations': 'Static contracts/host math do not establish runtime admission, hit reaction or headset acceptance.'}, indent=2)+'\n')
print('PASS 14 native instruction pins, two native vtable entries, channel getter and exact retail fingerprint')
