"""Read-only audit of the exact retail predictor and attacking-property getter.

No new native function calls or ABI fields are introduced by V122. This report
records why correlation is observation, distinct from the active-attack write
gate. Static control flow alone cannot identify the old run's failed predicate.
"""
import argparse
import hashlib
import json
from pathlib import Path
import struct
from capstone import Cs, CS_ARCH_X86, CS_MODE_64
from native_research112 import PE
from research_prediction119 import EXPECTED


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--whgame', type=Path, required=True)
    ap.add_argument('--out', type=Path, required=True)
    args = ap.parse_args()
    pe = PE(args.whgame)
    assert hashlib.sha256(pe.data).hexdigest() == EXPECTED and pe.stamp == 0x6A350E20
    cs = Cs(CS_ARCH_X86, CS_MODE_64)
    # Getter is an exact retail method, not an imported SDK virtual index.
    assert struct.unpack('<Q', pe.read(0x3A80610 + 73*8, 8))[0] == pe.base + 0x2756D58
    assert struct.unpack('<Q', pe.read(0x3B0A6A0 + 8, 8))[0] == pe.base + 0x1A7DE10
    expected = {
        0x2756D58: ('mov', 'rcx, qword ptr [rcx + 0x210]'),
        0x2756D5F: ('add', 'rcx, 0x2d0'),
        0x2756D69: ('jmp', 'qword ptr [rax + 8]'),
        0x1A7DE10: ('movzx', 'eax, byte ptr [rcx + 8]'),
        0x10F1102: ('lea', 'rbp, [rip + 0x2a19597]'),
        0x10F13D0: ('mov', 'qword ptr [rdi + 0x2d0], rbp'),
        0x10F13F5: ('mov', 'byte ptr [rdi + 0x2d8], r12b'),
        0x10F1BAE: ('mov', 'qword ptr [rdi + 0xce0], rbx'),
        0xA7ECA0: ('cmp', 'byte ptr [rax + 0x411], 0'),
        0xA7ECB0: ('mov', 'cl, byte ptr [rax + 0x2d0]'),
        0xA7CDB1: ('cmp', 'byte ptr [rax + 0x308], 0'),
        0xA7CDC1: ('add', 'rcx, 0x450'),
        0xA7CDE2: ('mov', 'byte ptr [rbx + 0x10], dil'),
    }
    for rva, pair in expected.items():
        ins = next(cs.disasm(pe.read(rva, 16), rva))
        assert (ins.mnemonic, ins.op_str) == pair, (hex(rva), ins.mnemonic, ins.op_str)
    assert 0x10F1102 + 7 + 0x2A19597 == 0x3B0A6A0
    calls = {0xA7ECC7: 0xA7CD4C, 0xA7CDE9: 0xA7C624, 0xA7CE45: 0x12D5AD4}
    for site, target in calls.items():
        raw = pe.read(site, 5)
        assert raw[0] == 0xE8 and site + 5 + struct.unpack('<i', raw[1:])[0] == target
    ranges = [(0x2756D58, 0x2756D6C), (0x1A7DE10, 0x1A7DE15),
              (0x10F10CC, 0x10F1147), (0x10F13A4, 0x10F1429),
              (0x10F1BAE, 0x10F1BB5), (0xA7EBBC, 0xA7ED54),
              (0xA7CD4C, 0xA7CE4F)]
    args.out.mkdir(parents=True, exist_ok=True)
    records = []
    for begin, end in ranges:
        raw = pe.read(begin, end-begin)
        listing = '\n'.join(f'{i.address:08X} {i.mnemonic} {i.op_str}'.rstrip() for i in cs.disasm(raw, begin)) + '\n'
        name = f'{begin:X}.asm'
        (args.out/name).write_text(listing, encoding='utf-8')
        records.append(dict(rva=f'{begin:X}', bytes=len(raw), sha256=hashlib.sha256(raw).hexdigest(), listing=name))
    report = dict(result='PASS', sha256=EXPECTED, timestamp='6A350E20',
                  verified_instructions=len(expected), verified_calls=len(calls), ranges=records,
                  findings=[
                      'The native IsAttacking getter reads CombatState+2D8. The existing raw observation is correct.',
                      'A7EBBC selects prediction using action-local flags; these are a different object from CombatState.',
                      'A7CD4C performs native eligibility/bookkeeping, calls prediction and consumes its original result.',
                      'V122 permits read-only pivot acquisition before IsAttacking, but still requires it for substitution.',
                      'No new native call, offset write, damage request or eligibility override is introduced.'
                  ], limitations=[
                      'No live scheduling or accepted-hit proof; the old context=336 counter cannot identify its failed gate.',
                      'Called functions can perform additional eligibility checks. Absence of a direct flag read is not proof of absence of all native checks.'
                  ])
    (args.out/'static-contract.json').write_text(json.dumps(report, indent=2)+'\n', encoding='utf-8')
    print(json.dumps({k: report[k] for k in ('result', 'verified_instructions', 'verified_calls')}))


if __name__ == '__main__':
    main()
