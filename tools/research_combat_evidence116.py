"""Offline retail evidence, not a runtime ABI or permission to call a function.

Searches RIP-relative references and direct calls, then validates their instruction
boundaries against the PE unwind function. Records bytes, source hash and complete
function listings so findings can be checked without trusting a string-name guess.
"""
import argparse
import hashlib
import json
import re
import struct
from pathlib import Path

from capstone import Cs, CS_ARCH_X86, CS_MODE_64
from capstone.x86 import X86_OP_MEM, X86_REG_RIP
from native_research112 import PE
from discover_physics113 import EXPECTED


def run(args):
    pe = PE(args.whgame)
    assert hashlib.sha256(pe.data).hexdigest() == EXPECTED
    assert pe.stamp == 0x6A350E20
    args.out.mkdir(parents=True, exist_ok=True)
    dis = Cs(CS_ARCH_X86, CS_MODE_64)
    dis.detail = True
    functions, targets, references, calls = {}, {}, [], []

    def decode(rva):
        region = pe.function(rva)
        if not region:
            return None, []
        if region[0] not in functions:
            functions[region[0]] = list(dis.disasm(
                pe.read(region[0], region[1] - region[0]), pe.base + region[0]))
        return region[0], functions[region[0]]

    pattern = re.compile(args.strings, re.I) if args.strings else None
    for section, va, size, raw in pe.sections:
        if section == b'.text' or not pattern:
            continue
        for match in re.finditer(rb'[\x20-\x7e]{3,}\x00', pe.data[raw:raw+size]):
            value = match[0][:-1].decode('ascii')
            if pattern.search(value):
                targets[va + match.start()] = value
    for arg in args.target:
        rva = int(arg, 0)
        targets[rva] = 'explicit ' + hex(rva)
    _, va, size, raw = next(s for s in pe.sections if s[0] == b'.text')
    code = pe.data[raw:raw+size]
    # Candidate search only. The full decoded operand must point to the target.
    prefixes = [bytes((rex, op)) for rex in (0x48, 0x4c) for op in (0x8d, 0x8b)]
    prefixes += [bytes((0xf3, 0x0f, op)) for op in (0x10, 0x59, 0x58, 0x5c, 0x5e, 0x5d, 0x5f)]
    for opcode in (prefix + bytes((m,)) for prefix in prefixes for m in range(5, 64, 8)):
        start = -1
        while True:
            start = code.find(opcode, start + 1)
            length = len(opcode) + 4
            if start < 0 or start + length > len(code):
                break
            target = va + start + length + struct.unpack_from('<i', code, start+len(opcode))[0]
            if target not in targets:
                continue
            rva = va + start
            function, instructions = decode(rva)
            ins = next((i for i in instructions if i.address == pe.base + rva), None)
            if ins and any(o.type == X86_OP_MEM and o.mem.base == X86_REG_RIP
                           and ins.address + ins.size + o.mem.disp == pe.base + target
                           for o in ins.operands):
                references.append(dict(target=hex(target), text=targets[target],
                                       rva=hex(rva), function=hex(function), bytes=ins.bytes.hex()))
    for arg in args.call:
        target = int(arg, 0)
        for rva in pe.calls(target):
            function, instructions = decode(rva)
            ins = next((i for i in instructions if i.address == pe.base + rva), None)
            if ins and ins.mnemonic == 'call' and ins.bytes[0] == 0xe8:
                calls.append(dict(target=hex(target), rva=hex(rva), function=hex(function), bytes=ins.bytes.hex()))
    for arg in args.rva:
        rva = int(arg, 0)
        function, _ = decode(rva)
        if function is None:
            raise ValueError('No unwind boundary for requested function: ' + arg)
    for rva, instructions in functions.items():
        lines = []
        for ins in instructions:
            notes = []
            for op in ins.operands:
                if op.type != X86_OP_MEM or op.mem.base != X86_REG_RIP:
                    continue
                target = ins.address + ins.size + op.mem.disp - pe.base
                note = ''
                try:
                    b = pe.read(target, 180).split(b'\0')[0]
                    if len(b) > 2 and all(32 <= c < 127 for c in b):
                        note = repr(b.decode('ascii'))
                except ValueError:
                    pass
                notes.append(f'RVA={target:X} {note}'.rstrip())
            lines.append((f'{ins.address-pe.base:08X} {ins.bytes.hex():32s} {ins.mnemonic} {ins.op_str}'
                          + (' ; ' + '; '.join(notes) if notes else '')).rstrip())
        (args.out / f'{rva:X}.asm').write_text('\n'.join(lines)+'\n', encoding='utf-8')
    result = dict(sha256=EXPECTED, timestamp=hex(pe.stamp),
                  search=dict(strings=args.strings, targets=args.target, calls=args.call, rvas=args.rva),
                  targets={hex(k): v for k, v in targets.items()}, references=references, calls=calls,
                  limitations=['Not an exhaustive cross-reference search.',
                               'Static instructions do not prove live receiver identity, eligibility or ownership.'])
    (args.out / 'index.json').write_text(json.dumps(result, indent=2)+'\n', encoding='utf-8')
    print(json.dumps(dict(targets=len(targets), references=len(references), calls=calls, functions=len(functions))))


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--whgame', required=True, type=Path)
    parser.add_argument('--out', required=True, type=Path)
    parser.add_argument('--strings', default='')
    for flag in ('target', 'call', 'rva'):
        parser.add_argument('--'+flag, action='append', default=[])
    run(parser.parse_args())
