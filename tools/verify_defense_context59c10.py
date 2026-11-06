"""Read-only retail proof for native row-derived VR defense announcements."""
from pathlib import Path
import argparse, hashlib, json, struct
from native_research112 import PE

parser = argparse.ArgumentParser()
parser.add_argument('dll')
parser.add_argument('--output')
args = parser.parse_args()
p = PE(args.dll)
expected = 'bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7'
assert hashlib.sha256(p.data).hexdigest() == expected and p.stamp == 0x6A350E20
pins = {
    '12EDEF4': '48895c24084889742410574883ec30',
    '90D6D0': '4883ec38448bca488bd144394908',
    '1A72970': '8b4108c3',
    '939E02': 'e8ed409b00',
    '939E21': '488b8338010000488d8e500100008b68088bd5e89738fdff',
    '911B73': '488d8e50010000488b11ff5208',
    '911BAD': 'e852260000',
    '909C08': '488d8890030000488b11ff520884c0',
    '9E43AC': '4883ec38f30f104108488bd10f2ec1',
    '1A7DE00': 'f30f104108c3',
    '9142BE': '498b8e100200004881c150040000488b01ff5008',
    '914314': '0f2f0d315c78037208488bcbe82bb1ab01',
    '9087AB': '488d4b28e8b8fb0c0084c074edba01000000488bcbe88bc10000',
    '23CF450': '40534883ec20f30f11491c488bd94883c1280f57d2e8ca7560fec6431b01',
}
for rva, data in pins.items():
    want = bytes.fromhex(data)
    assert p.read(int(rva, 16), len(want)) == want, rva
assert struct.unpack('<Q', p.read(0x3B0A718 + 8, 8))[0] == p.base + 0x1A72970
assert struct.unpack('<Q', p.read(0x3B0A6C8 + 8, 8))[0] == p.base + 0x1A72970
assert struct.unpack('<Q', p.read(0x3B0A790 + 8, 8))[0] == p.base + 0x1A7DE00
assert struct.unpack('<f', p.read(0x4099F4C, 4))[0] == 0.0  # zero DOES arm an immediate-expiry timer
assert struct.unpack('<f', p.read(0x409A2EC, 4))[0] == -1.0  # native timing reset value
functions = {}
for address in (0x12EDEF4, 0x12EDF54, 0x12EE05C, 0x90FD50, 0x90FF10,
                0x910134, 0x90D6D0, 0x939CD8, 0x911B24, 0x914204,
                0x909BC8, 0x909E68, 0x908790, 0x908A9C, 0x9089AC,
                0x9E43AC, 0x9E3B80, 0xD4E1C8, 0xA73A04, 0xB27958,
                0x23CF450, 0x9D6A34, 0x9D836C, 0x914950):
    span = p.function(address)
    assert span and span[0] == address, hex(address)
    functions[f'{address:X}'] = {'start': span[0], 'end': span[1],
        'sha256': hashlib.sha256(p.read(span[0], span[1] - span[0])).hexdigest()}
result = {'dllSha256': expected, 'timestamp': '6A350E20', 'pins': pins,
          'functions': functions, 'result': 'PASS', 'nativeExecution': 'NONE'}
if args.output:
    Path(args.output).write_text(json.dumps(result, indent=2) + '\n')
print(f'PASS: {len(pins)} native instruction pins, two exact int-property getters and one float getter, '
      f'{len(functions)} .pdata function hashes; no native execution')
