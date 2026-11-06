"""Exact-retail function listings for the V114 bow/equipment/contact review.

Addresses are RVAs. This never loads or calls game code. Capstone is a research
dependency only. Function ranges come from the image's exception directory.
"""
import argparse
import hashlib
import json
from pathlib import Path
from capstone import Cs, CS_ARCH_X86, CS_MODE_64
from native_research112 import PE
from discover_physics113 import EXPECTED


def run(binary, out, addresses):
    pe = PE(binary)
    assert hashlib.sha256(pe.data).hexdigest() == EXPECTED
    assert pe.stamp == 0x6A350E20
    out.mkdir(parents=True, exist_ok=True)
    dis = Cs(CS_ARCH_X86, CS_MODE_64)
    records = []
    for address in addresses:
        rva = int(address, 0)
        region = pe.function(rva)
        if not region:
            raise ValueError(f'No unwind function for {rva:X}')
        instructions = list(dis.disasm(pe.read(region[0], region[1]-region[0]), pe.base+region[0]))
        record = {'requested':hex(rva), 'function':[hex(v) for v in region],
                  'first16':pe.read(region[0],16).hex(),
                  'call_candidates':[hex(v) for v in pe.calls(region[0])],
                  'pointers':[hex(v) for v in pe.pointers(region[0])]}
        records.append(record)
        (out/f'{rva:X}.asm').write_text('\n'.join(
            f'{x.address-pe.base:08X} {x.bytes.hex():32s} {x.mnemonic} {x.op_str}'.rstrip()
            for x in instructions)+'\n', encoding='utf-8')
    (out/'latest.json').write_text(json.dumps(records, indent=2)+'\n',encoding='utf-8')
    print(json.dumps(records, indent=2))


if __name__ == '__main__':
    p=argparse.ArgumentParser()
    p.add_argument('--whgame',type=Path,required=True)
    p.add_argument('--out',type=Path,required=True)
    p.add_argument('addresses',nargs='+')
    a=p.parse_args()
    run(a.whgame,a.out,a.addresses)
