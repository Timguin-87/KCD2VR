"""Read-only, exact-retail verification of the focus-FOV setting and consumer.

Requires pefile and capstone. Does not hook the game or authorize raw writes.
Usage: python tools/audit_focus58c2.py WHGame.dll OUTPUT_DIRECTORY
"""
from pathlib import Path
import argparse
import hashlib
import json
import struct
import pefile
import capstone


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('binary', type=Path)
    ap.add_argument('output', type=Path)
    args = ap.parse_args()
    data = args.binary.read_bytes()
    sha = hashlib.sha256(data).hexdigest()
    if sha != 'bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7':
        raise SystemExit('Unsupported WHGame: no evidence generated')
    pe = pefile.PE(data=data)
    assert pe.FILE_HEADER.TimeDateStamp == 0x6A350E20
    assert pe.get_data(0x3EA3A30, 26) == b'wh_pl_FocusFOVMaxDecrease\0\0'[:26]  # verified literal
    pins = {
        0x15A87E8: '498d9658010000',  # registration storage: receiver + 0x158
        0x15A880E: '488d0d1bb28f02',  # setting name
        0x8B9AF7: 'e8e412bfff',      # getter
        0x8B9B03: 'f30f109858010000',# setting value
        0x8B9B0B: 'f30f591d75097e03',# degrees to radians
        0x8B9B13: 'f30f5cf3',        # reduced endpoint
        0x8B9B32: 'e8f1d2baff',      # interpolation
        0x4AAE07: '488d05b2101305',  # getter returns singleton
    }
    for rva, raw in pins.items():
        expected = bytes.fromhex(raw)
        assert pe.get_data(rva, len(expected)) == expected, hex(rva)
    degree_constant = struct.unpack('<f', pe.get_data(0x8B9B13 + 0x37E0975, 4))[0]
    assert abs(degree_constant - 0.01745329252) < 1e-7
    cs = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_64)
    args.output.mkdir(parents=True, exist_ok=True)
    regions = {
        'register_15A880E.asm': (0x15A87BE, 0x15A8844),
        'proven_consumer_8B999C.asm': (0x8B999C, 0x8B9B77),
        'getter_4AADE0.asm': (0x4AADE0, 0x4AAE18),
    }
    for name, (start, end) in regions.items():
        ins = cs.disasm(pe.get_data(start, end-start), pe.OPTIONAL_HEADER.ImageBase+start)
        text = '\n'.join(f'{i.address-pe.OPTIONAL_HEADER.ImageBase:08X} {i.bytes.hex():28} {i.mnemonic:8} {i.op_str}'.rstrip() for i in ins)
        (args.output/name).write_text(text+'\n', encoding='utf-8')
    # Decode the caller from its PE unwind function start, not a guessed nearby byte.
    caller = next(e.struct for e in pe.DIRECTORY_ENTRY_EXCEPTION
                  if e.struct.BeginAddress <= 0x1064611 < e.struct.EndAddress)
    instructions = list(cs.disasm(pe.get_data(caller.BeginAddress, caller.EndAddress-caller.BeginAddress),
                                 pe.OPTIONAL_HEADER.ImageBase+caller.BeginAddress))
    index = next(i for i,v in enumerate(instructions) if v.address-pe.OPTIONAL_HEADER.ImageBase == 0x1064611)
    text = '\n'.join(f'{i.address-pe.OPTIONAL_HEADER.ImageBase:08X} {i.bytes.hex():28} {i.mnemonic:8} {i.op_str}'.rstrip() for i in instructions[max(0,index-8):index+2])
    (args.output/'registration_receiver_1064611.asm').write_text(text+'\n', encoding='utf-8')
    report = dict(binary=str(args.binary), sha256=sha, timestamp='0x6A350E20',
                  setting='wh_pl_FocusFOVMaxDecrease', string_rva='0x3EA3A30',
                  registration_name_load='0x15A880E', field='0x158',
                  getter='0x4AADE0', singleton='0x55DBEC0', storage='0x55DC018',
                  proven_consumer={'function':'0x8B999C', 'end':'0x8B9B77',
                                   'getter_call':'0x8B9AF7','value_load':'0x8B9B03',
                                   'subtract':'0x8B9B13','interpolation':'0x8B9B32'},
                  degree_constant=degree_constant,
                  pins={hex(r):v for r,v in pins.items()},
                  conclusion='Zero makes the focus-FOV interpolation endpoints equal. This does not establish all authored camera translations or every book camera.',
                  runtime_access='Existing fingerprint-qualified named console Get/Set wrappers only. No raw write to the storage RVA; no new native hook or SDK vtable.',
                  rejected_search_candidates=['0x666D24 and 0x135F914: nearby vtable displacement 0x158 is not evidence of a focus-setting read.'])
    (args.output/'provenance.json').write_text(json.dumps(report, indent=2)+'\n', encoding='utf-8')
    print('PASS exact-retail focus FOV registration, pins, units and consumer')


if __name__ == '__main__':
    main()
