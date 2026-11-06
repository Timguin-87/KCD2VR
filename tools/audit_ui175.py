#!/usr/bin/env python3
"""Verify the supplied retail bytes used by V175 HUD visibility qualification.

This is an offline PE/asset audit. It does not load or execute WHGame and cannot
establish headset acceptance or coverage of every native/ActionScript writer.
Only hashes and small code/vtable pins are emitted, never the supplied assets.
"""
import argparse
import hashlib
import json
import struct
from pathlib import Path

EXPECTED_SHA256 = 'bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7'


class PE:
    def __init__(self, path):
        self.path = Path(path)
        self.data = self.path.read_bytes()
        pe = struct.unpack_from('<I', self.data, 0x3c)[0]
        if self.data[pe:pe + 4] != b'PE\0\0':
            raise ValueError('Not a PE file')
        self.machine, count, self.timestamp = struct.unpack_from('<HHI', self.data, pe + 4)
        optional_size = struct.unpack_from('<H', self.data, pe + 20)[0]
        self.image_base = struct.unpack_from('<Q', self.data, pe + 24 + 24)[0]
        section_offset = pe + 24 + optional_size
        self.sections = [struct.unpack_from('<4I', self.data, section_offset + 40 * i + 8)
                         for i in range(count)]

    def read(self, rva, size):
        for _, section_rva, raw_size, raw_offset in self.sections:
            delta = rva - section_rva
            if 0 <= delta and delta + size <= raw_size:
                return self.data[raw_offset + delta:raw_offset + delta + size]
        raise ValueError(f'RVA is not backed by file bytes: {rva:#x}+{size:#x}')


def audit(path):
    pe = PE(path)
    checks = []

    def check(label, rva, expected):
        actual = pe.read(rva, len(expected))
        checks.append({'name': label, 'rva': hex(rva), 'size': len(expected),
                       'expected': expected.hex(), 'actual': actual.hex(),
                       'passed': actual == expected})

    def pointer(label, rva, target):
        check(label, rva, struct.pack('<Q', pe.image_base + target))

    def relative(label, rva, target, opcode=0xe8):
        check(label, rva, bytes([opcode]) + struct.pack('<i', target - rva - 5))

    def rtti(label, vtable, name):
        col = struct.unpack('<Q', pe.read(vtable - 8, 8))[0] - pe.image_base
        descriptor = struct.unpack('<I', pe.read(col + 12, 4))[0]
        check(label, descriptor + 16, name.encode('ascii') + b'\0')

    prologue = bytes.fromhex('48 89 5c 24 08 48 89 74 24 10 57 48 83 ec 20')
    check('IUIElement::FindMovieClip entry', 0x7F4C18, prologue)
    check('CFlashVariableObject destructor entry', 0x7F5FA0, prologue)
    check('CFlashVariableObject::SetDisplayInfo entry', 0x7F4520,
          bytes.fromhex('48 8b c4 48 89 58 10 48 89 70 18'))
    check('CFlashVariableObject::GetDisplayInfo entry', 0x7F4784,
          bytes.fromhex('48 8b c4 48 89 58 18 55 56 57'))
    pointer('IUIElement FindMovieClip vslot', 0x4049BD0 + 0x238, 0x7F4C18)
    pointer('Flash wrapper SetDisplayInfo vslot', 0x3A55BA0 + 0xC8, 0x7F4520)
    pointer('Flash wrapper GetDisplayInfo vslot', 0x3A55BA0 + 0xD0, 0x7F4784)
    pointer('Flash wrapper SetVisible vslot', 0x3A55BA0 + 0x108, 0x7F4B0C)
    pointer('GFx character SetVisible vslot', 0x3DA0440 + 0x198, 0x1C51240)
    check('GFx character handle resolver reads handle+8', 0x1C99B00,
          bytes.fromhex('48 8b 41 08 4c 8b c2 48 85 c0 75 0c'))
    check('GFx destructor invalidates handle+8 through character+0xD0', 0x1C944ED,
          bytes.fromhex('48 8b 81 d0 00 00 00 48 85 c0 74 08 48 c7 40 08 00 00 00 00'))
    check('GFx display-info bit6 dispatches bool to vslot+0x198', 0x1C2FEBF,
          bytes.fromhex('0f b6 86 e0 00 00 00 c0 e8 06 a8 01 74 10 48 8b 07 '
                        '48 8b cf 0f b6 56 30 ff 90 98 01 00 00'))
    check('GFx SetVisible updates bit0 and preserves remaining flags', 0x1C51240,
          bytes.fromhex('44 8b 81 e8 00 00 00 41 8b c0 83 e0 fe 41 83 c8 01 '
                        '84 d2 44 0f 44 c0 44 89 81 e8 00 00 00 e9 7d 8a 04 00'))
    check('C_Minigame accepted-start entry (whole instructions, 16 bytes)', 0x9F0970,
          bytes.fromhex('48 89 5c 24 08 57 48 83 ec 40 80 79 2c 00 8b fa'))
    check('C_Minigame accepted-start writes active+2C', 0x9F098B, bytes.fromhex('c6 43 2c 01'))
    check('C_Minigame stop entry (whole instructions, 18 bytes)', 0x9F1150,
          bytes.fromhex('40 53 48 83 ec 40 48 8b 41 18 48 8d 54 24 20 48 8b d9'))
    check('C_Minigame stop resets active+2C', 0x9F1193, bytes.fromhex('c6 43 2c 00'))
    check('C_Minigame destructor entry (whole instructions, 22 bytes)', 0x9F0E60,
          prologue + bytes.fromhex('48 8d 05 f2 96 07 03'))
    check('C_Minigame constructor binds its owner at +18', 0x9F0F36, bytes.fromhex('48 89 51 18'))
    check('C_Minigame constructor starts inactive', 0x9F0F74, bytes.fromhex('c6 43 2c 00'))
    for name, vtable, type_method, kind in [
        ('C_Alchemy', 0x3F60B08, 0x1A78EB0, 3),
        ('C_Blacksmithing', 0x3A6A390, 0x1A99DC0, 12),
        ('C_Reading', 0x3F749E0, 0x1A74A40, 2),
        ('C_Sharpening', 0x3F74BA0, 0x41A6A0, 1),
    ]:
        rtti(name + ' primary-vtable RTTI', vtable, f'.?AV{name}@playermodule@wh@@')
        pointer(name + ' native type vslot', vtable, type_method)
        check(name + ' native type constant', type_method, bytes([0xb0, kind, 0xc3]))
    rtti('Supported local C_Player primary-vtable RTTI', 0x3AA4B98, '.?AVC_Player@entitymodule@wh@@')
    relative('Alchemy accepted entry reaches common start', 0x897E41, 0x9F0970)
    relative('Blacksmithing accepted entry reaches common start', 0xB67535, 0x9F0970)
    relative('Reading entry reaches common start', 0x2EB1524, 0x9F0970)
    relative('Sharpening entry reaches common start', 0x2EB13D7, 0x9F0970)
    pointer('Sharpening inherited minigame dispatch', 0x3F74C90, 0x9F0A44)
    relative('Alchemy stop reaches common cleanup', 0x9F1DB7, 0x9F0160)
    relative('Blacksmithing stop reaches common cleanup', 0xB68600, 0x9F0160)
    relative('Reading stop reaches common cleanup', 0x2EB26F0, 0x9F0160)
    relative('Common cleanup tailcalls stop', 0x9F0176, 0x9F1150, 0xe9)
    relative('Base deleting destructor calls actual destructor', 0x2E8283B, 0x9F0E60)
    # These are all direct start callers found in the supplied .text. Each was
    # disassembled: RAX is overwritten, or another call occurs, before any use.
    # Retain the bytes for re-audit without pretending Python is a disassembler.
    start_callers = [0x897E41, 0xB67535, 0x19B534C, 0x20279DA, 0x2E5AF8B,
                     0x2E972B5, 0x2E975AC, 0x2E97C3C, 0x2EB0FB0, 0x2EB13D7,
                     0x2EB1524, 0x2EB2440]
    for caller in start_callers:
        relative('Audited void-return start caller ' + hex(caller), caller, 0x9F0970)
    digest = hashlib.sha256(pe.data).hexdigest()
    identity = {'sha256': digest, 'expected_sha256': EXPECTED_SHA256,
                'machine': hex(pe.machine), 'timestamp': hex(pe.timestamp),
                'image_base': hex(pe.image_base), 'bytes': len(pe.data)}
    identity['passed'] = (digest == EXPECTED_SHA256 and pe.machine == 0x8664 and
                          pe.timestamp == 0x6A350E20 and pe.image_base == 0x180000000)
    return {'artifact': pe.path.name, 'identity': identity, 'checks': checks,
            'start_return_contract': {
                'signature': 'void(void* minigame, uint32_t target)',
                'basis': 'Manual disassembly of common return and every known direct caller; RAX unused before clobber',
                'caller_windows': [{'rva': hex(c), 'bytes': pe.read(c, 40).hex()} for c in start_callers],
                'absolute_start_pointer_references': pe.data.count(struct.pack('<Q', pe.image_base + 0x9F0970))},
            'passed': identity['passed'] and all(x['passed'] for x in checks),
            'native_execution': False, 'headset_acceptance': False,
            'script_timeline_visibility_coverage': 'UNMEASURED'}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--dll', required=True)
    parser.add_argument('--out', help='Optional JSON evidence file')
    args = parser.parse_args()
    result = audit(args.dll)
    encoded = json.dumps(result, indent=2) + '\n'
    if args.out:
        output = Path(args.out)
        output.parent.mkdir(parents=True, exist_ok=True)
        output.write_text(encoded)
    print(f"{'PASS' if result['passed'] else 'FAIL'} {result['artifact']}: "
          f"identity + {sum(x['passed'] for x in result['checks'])}/{len(result['checks'])} pins; "
          'offline audit only, native execution/headset acceptance unclaimed')
    return 0 if result['passed'] else 1


if __name__ == '__main__':
    raise SystemExit(main())
