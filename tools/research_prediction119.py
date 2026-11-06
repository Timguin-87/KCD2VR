"""Offline, exact-retail prediction/collider evidence. Does not patch the game.

Requires Capstone for reproducible listings. Unwind entries are NOT necessarily
whole functions: the pose callback below has split ranges and is bounded by its
verified final return explicitly. This report is not a runtime ABI declaration.
"""
import argparse
import hashlib
import json
import struct
from pathlib import Path

from capstone import Cs, CS_ARCH_X86, CS_MODE_64
from native_research112 import PE

EXPECTED = 'bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7'
CALLS = {
    0xA7EA3A: 0x981858, 0xA7EA49: 0xA7EAB4, 0xA7EA65: 0xA7CEE0,
    0xA7EA71: 0xA7EBBC, 0xA7ECC7: 0xA7CD4C, 0xA7CDE9: 0xA7C624,
    0xA7CE45: 0x12D5AD4, 0xA7C968: 0xA7CA90, 0xA7C990: 0xA7CCCC,
    0xA7C9BC: 0xA7A124, 0xA7CD18: 0xA7A49C, 0xA7A575: 0xA7A8D8,
    0x1E59160: 0x1E58E70, 0xA7A873: 0xA7A90C,
}
FUNCTIONS = (
    0x981858, 0xA7EA18, 0xA7EBBC, 0xA7CD4C, 0xA7C624,
    0xA7CA90, 0xA7CCCC, 0xA7A49C, 0xA7A8D8, 0xA7A90C,
    0x1E59150, 0x1E58E70, 0x12D5AD4, 0xA7CEE0,
)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--whgame', type=Path, required=True)
    ap.add_argument('--out', type=Path, required=True)
    args = ap.parse_args()
    pe = PE(args.whgame)
    digest = hashlib.sha256(pe.data).hexdigest()
    if digest != EXPECTED or pe.stamp != 0x6A350E20:
        raise SystemExit('Unsupported WHGame fingerprint; no report generated')
    calls = []
    for caller, expected in CALLS.items():
        raw = pe.read(caller, 5)
        actual = caller + 5 + struct.unpack('<i', raw[1:])[0]
        if raw[0] != 0xE8 or actual != expected:
            raise SystemExit(f'Call mismatch at {caller:X}')
        calls.append(dict(call_rva=f'{caller:X}', return_rva=f'{caller+5:X}',
                          target_rva=f'{actual:X}', bytes=raw.hex()))
    # Retail callback thunk slot, recovered from its construction at A7CCF9.
    callback = struct.unpack('<Q', pe.read(0x3E5DF40 + 0x10, 8))[0] - pe.base
    if callback != 0x1E59150 or pe.read(0x1E59123, 1) != b'\xc3':
        raise SystemExit('Pose callback/extended return mismatch')
    args.out.mkdir(parents=True, exist_ok=True)
    dis = Cs(CS_ARCH_X86, CS_MODE_64)
    functions = []
    for rva in FUNCTIONS:
        region = pe.function(rva)
        if rva == 0x1E58E70:
            start, end = rva, 0x1E59124  # includes both returns/split unwind ranges
        elif region:
            start, end = region
        else:
            start, end = rva, rva + 512
        raw = pe.read(start, end-start)
        insns = list(dis.disasm(raw, pe.base+start))
        if not region and rva != 0x1E58E70:
            ret = next(i for i, ins in enumerate(insns) if ins.mnemonic == 'ret')
            insns = insns[:ret+1]
            end = insns[-1].address + insns[-1].size - pe.base
            raw = pe.read(start, end-start)
        listing = ''.join(f'{i.address-pe.base:08X} {i.mnemonic} {i.op_str}'.rstrip()+'\n'
                          for i in insns)
        name = f'{rva:X}.asm'
        (args.out/name).write_text(listing, encoding='utf-8')
        functions.append(dict(requested_rva=f'{rva:X}', start=f'{start:X}', end=f'{end:X}',
                              binary_range_sha256=hashlib.sha256(raw).hexdigest(), listing=name))
    report = dict(
        whgame_sha256=digest, timestamp=f'{pe.stamp:X}', result='PASS',
        calls=calls, callback_vtable_rva='3E5DF40', callback_slot_bytes='10',
        callback_target_rva=f'{callback:X}', functions=functions,
        limitations=[
            'Static call/field evidence only; no native runtime execution or mutation.',
            '981858 lazily resolves actor+2B4 when its value is 4; enum meaning is unproven.',
            'Prediction geometry setup and sweep must share the same pose and interval.',
            'No same-frame tracked pose/callback/subdivision correlation is established.',
            'A non-null sweep or copied native contact is not proof of accepted damage.',
        ],
    )
    (args.out/'prediction-contract.json').write_text(json.dumps(report, indent=2)+'\n', encoding='utf-8')
    print(json.dumps(dict(result='PASS', calls=len(calls), functions=len(functions),
                         callback='3E5DF40+10 -> 1E59150 -> 1E58E70')))


if __name__ == '__main__':
    main()
