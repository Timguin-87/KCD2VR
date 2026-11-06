"""Read-only verification of the existing alignment query, record layout and caller."""
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

source = read_source(Path(__file__).resolve().parents[1]/'src/vrik/native_motion112.inl')
pins = []
for name, rva in [('query', 0x9E37A8), ('alignmentCall', 0x9E2D50)]:
    values = re.search(r'unsigned char '+name+r'\[\d+\]\{([^}]+)\}', source)[1]
    expected = bytes(int(v, 0) for v in values.split(','))
    assert read(rva, len(expected)) == expected, name
    pins.append({'name': name, 'rva': hex(rva), 'bytes': expected.hex()})
call = read(0x9E2D50, 13)
assert 0x9E2D55+struct.unpack_from('<i', call, 1)[0] == 0x9E37A8
assert 0x9E2D5D+struct.unpack_from('<i', call, 9)[0] == 0x9E2E99
# Constructor copies target entity ID, kind byte, clip pointer into a 32-byte
# entry and appends to manager +58/+60/+68. No SDK layout is assumed.
for rva, hexbytes in [
    (0x14098EF, '488b82f80100008b5030'),
    (0x14098F9, '8a44247888442430'),
    (0x1409901, '488b8424800000008954242c498943f0'),
    (0x1409911, '488d4158488b4808483b4810'),
    (0x1409928, '4883400820')]:
    expected = bytes.fromhex(hexbytes)
    assert read(rva, len(expected)) == expected, hex(rva)
    pins.append({'rva': hex(rva), 'bytes': expected.hex()})
a.out.parent.mkdir(parents=True, exist_ok=True)
a.out.write_text(json.dumps({'dll': str(a.dll), 'sha256': digest, 'timestamp': '0x6A350E20',
    'pins': pins, 'queryReturnCaller': '0x9E2D55', 'falseBranch': '0x9E2E99',
    'nativeExecution': False,
    'limits': 'An alignment-setup boundary, not proof that all authored root motion stops. Live exact-Henry/current-action guards remain required.'}, indent=2)+'\n')
print('PASS exact fingerprint, query/caller pins and alignment registration layout')
