"""Instruction-validated string xrefs in the SHA-pinned retail image. No calls."""
import argparse
import hashlib
import json
import re
import struct
from pathlib import Path
from capstone import Cs, CS_ARCH_X86, CS_MODE_64
from native_research112 import PE
from discover_physics113 import EXPECTED


def run(binary, out, targets=()):
    pe = PE(binary)
    assert hashlib.sha256(pe.data).hexdigest() == EXPECTED and pe.stamp == 0x6A350E20
    out.mkdir(parents=True, exist_ok=True)
    strings = {}
    pattern = re.compile(r'(materialeffect|surfacetype|hit_passed|hit_stopped|sword_stone|sword_wood|hitwall|hit_wall|attack_rebound|recoil)', re.I)
    for section, va, size, raw in pe.sections:
        if section == b'.text':
            continue
        for match in re.finditer(rb'[\x20-\x7e]{5,}\x00', pe.data[raw:raw+size]):
            value = match[0][:-1].decode('ascii')
            if pattern.search(value):
                strings[va+match.start()] = value
    dis = Cs(CS_ARCH_X86, CS_MODE_64)
    for target in targets:
        rva=int(target,0);strings[rva]='EXPLICIT_TARGET '+hex(rva)
    _, va, size, raw = next(s for s in pe.sections if s[0] == b'.text')
    code = pe.data[raw:raw+size]
    functions, references = {}, []
    prefixes=[bytes((rex, op)) for rex in (0x48,0x4c) for op in (0x8d,0x8b)]
    prefixes += [bytes((0xf3,0x0f,op)) for op in (0x10,0x59,0x58,0x5c,0x5e,0x5d,0x5f)]
    for opcode in (prefix+bytes((modrm,)) for prefix in prefixes for modrm in range(5,64,8)):
        start = -1
        while True:
            start = code.find(opcode, start+1)
            length=len(opcode)+4
            if start < 0 or start+length > len(code):
                break
            target = va+start+length+struct.unpack_from('<i', code, start+len(opcode))[0]
            if target not in strings:
                continue
            rva = va+start
            region = pe.function(rva)
            if not region:
                continue
            if region[0] not in functions:
                functions[region[0]] = list(dis.disasm(pe.read(region[0], region[1]-region[0]), pe.base+region[0]))
            instructions = functions[region[0]]
            ix = next((i for i, ins in enumerate(instructions) if ins.address == pe.base+rva), None)
            if ix is None:
                continue
            references.append(dict(string=strings[target], string_rva=hex(target), xref=hex(rva), function=hex(region[0])))
    used = {int(r['function'], 16) for r in references}
    for rva in sorted(used):
        (out/f'{rva:X}.asm').write_text('\n'.join(
            f'{i.address-pe.base:08X} {i.bytes.hex():32s} {i.mnemonic} {i.op_str}'.rstrip()
            for i in functions[rva])+'\n', encoding='utf-8')
    result = dict(sha256=EXPECTED, timestamp=hex(pe.stamp), strings={hex(k):v for k,v in strings.items()}, references=references)
    (out/'material_xrefs.json').write_text(json.dumps(result, indent=2)+'\n', encoding='utf-8')
    print(json.dumps(dict(strings=len(strings), references=len(references), functions=len(used))))


if __name__ == '__main__':
    p=argparse.ArgumentParser();p.add_argument('--whgame',type=Path,required=True);p.add_argument('--out',type=Path,required=True);p.add_argument('--target',action='append',default=[])
    a=p.parse_args();run(a.whgame,a.out,a.target)
