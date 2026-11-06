"""Read-only verification of the two diagnostic bool properties. Never executes WHGame."""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
import argparse, hashlib, json, re, struct
from pathlib import Path
from native_research112 import PE

ap = argparse.ArgumentParser()
ap.add_argument('--dll', type=Path, required=True)
ap.add_argument('--out', type=Path, required=True)
args = ap.parse_args()
pe = PE(args.dll)
digest = hashlib.sha256(pe.data).hexdigest()
assert digest == 'bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7'
assert pe.stamp == 0x6A350E20
source = read_source(Path(__file__).resolve().parents[1] / 'src/diagnostics/geometry_hit178r27.inl')
vft = int(re.search(r'kDefenseBoolVft=(0x[0-9A-F]+)', source)[1], 16)
getter = int(re.search(r'kDefenseBoolGetter=(0x[0-9A-F]+)', source)[1], 16)
pin = bytes(int(x.strip(), 16) for x in re.search(r'kDefenseBoolGetterEntry\[\]\{([^}]+)', source)[1].split(','))
assert pe.read(getter, len(pin)) == pin == bytes.fromhex('0fb64108c3')
assert struct.unpack('<Q', pe.read(vft+8, 8))[0] == pe.base + getter
pins = {
    0x10F1102: '488d2d9795a102',  # constructor selects same property vtable
    0x10F13FC: '4889af00030000',  # +300 property
    0x10F1421: '4488a708030000',  # +308 BYTE initialization
    0x10F1751: '4889af80060000',  # +680 property
    0x10F176F: '4488a788060000',  # +688 BYTE initialization
    0xD56D72: '488d8f80060000488b01ff5008',  # native hit reads +680 via +8 getter
    0xD56D83: '8845c6',  # retains AL, not a 32-bit integer/float
}
for rva, raw in pins.items():
    assert pe.read(rva, len(bytes.fromhex(raw))) == bytes.fromhex(raw), hex(rva)
assert 0x10F1102+7+struct.unpack('<i', pe.read(0x10F1105,4))[0] == vft
result = dict(sha256=digest, timestamp=hex(pe.stamp), vtable=hex(vft), getter=hex(getter),
              getterBytes=pin.hex(), pins={hex(k):v for k,v in pins.items()},
              semantics='BYTE payload at property+8; following three bytes are not part of the flag.',
              scope='Recorder-only diagnostic, UNKNOWN on type/read/pin refusal; no combat decisions or writes.')
args.out.parent.mkdir(parents=True, exist_ok=True)
args.out.write_text(json.dumps(result, indent=2)+'\n')
print('PASS exact retail SHA/timestamp, source getter pin, vtable target, constructors and native consumer; no native execution')
