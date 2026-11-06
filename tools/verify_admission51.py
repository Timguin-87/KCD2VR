"""Read-only verification of the R51C1 angle and input-observation boundaries."""
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

source = Path(__file__).resolve().parents[1]/'src/vrik/native_admission51.inl'
pins = []
for name, rva, size in [('angle', 0x13D0938, 16), ('gate', 0xD4AD94, 16), ('call', 0x72F8C5, 8)]:
    values = re.search(r'unsigned char '+name+r'\[\]\{([^}]+)\}', read_source(source))[1]
    expected = bytes(int(v, 16) for v in values.split(','))
    assert len(expected) == size and read(rva, size) == expected, name
    pins.append({'name': name, 'rva': hex(rva), 'bytes': expected.hex()})
call = read(0x72F8C5, 8)
assert 0x72F8CA+struct.unpack_from('<i', call, 1)[0] == 0x13D0938
assert call[5:] == bytes.fromhex('0f2fc6')  # returned float compared to measured target angle
assert read(0x72F8B8, 3) == bytes.fromhex('488bd3')  # RDX = target owner, not an attack row
assert read(0x72F8C2, 3) == bytes.fromhex('488bcf')  # RCX = actor
assert read(0x72F8CD, 2) == bytes.fromhex('7631')    # reject angle >= returned threshold
a.out.parent.mkdir(parents=True, exist_ok=True)
a.out.write_text(json.dumps({'dll': str(a.dll), 'sha256': digest, 'timestamp': '0x6A350E20',
    'pins': pins, 'angleCallAbi': 'float(actor,targetOwner); caller return RVA 72F8CA',
    'nativeExecution': False, 'globalCvarWrites': False,
    'limitations': 'Verifies retail bytes/call relationship, not native acceptance or headset behavior.'}, indent=2)+'\n')
print('PASS exact fingerprint, three source pins and actor/target float-call relationship')
