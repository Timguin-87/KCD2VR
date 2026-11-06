"""Read-only exact-retail combat research; never load or execute the game DLL.

Requires pefile and capstone. Emits selected principal .pdata ranges, decoded
call edges, and XML counts/hashes. Cold fragments and virtual targets are not
implicitly resolved. These artifacts are evidence, not declarations of ABI.
"""
from pathlib import Path
import argparse
import collections
import hashlib
import json
import xml.etree.ElementTree as ET
import zipfile

SHA = 'bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7'
FUNCTIONS = (0xC5235C, 0x72EFDC, 0xD53610, 0xD568B0, 0xD499D8,
             0xD49AB4, 0xD4921C, 0xD49C28, 0x90C850, 0xD542E0, 0xD4CB40, 0x89B68C, 0x7F2DD4, 0x1ED6AD0)
CALLS = {0xC523AC: 0x72EFDC, 0xC5252F: 0xD53610, 0xD536B7: 0xD568B0,
         0xD49A7B: 0xD49AB4, 0xD49B66: 0xD4921C, 0xD49B82: 0xD49C28,
         0x90C86F: 0x9CC3D4}
TABLES = ('combat_action_hit', 'combat_action_block', 'combat_action_perfect_block',
          'combat_action_sync_attack', 'combat_action_sync_hit',
          'combat_action_sync_perfect_block_hit', 'combat_action_attack',
          'combat_action_type', 'combat_action_type_mapping', 'combat_hit_origin',
          'combat_hit_type', 'combat_attack_config', 'hit_reaction', 'hit_reaction_type')

def main():
    import pefile
    from capstone import Cs, CS_ARCH_X86, CS_MODE_64, CS_OP_IMM
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--game', required=True, type=Path)
    ap.add_argument('--out', required=True, type=Path)
    a = ap.parse_args()
    dll = a.game / 'Bin/Win64MasterMasterSteamPGO/WHGame.dll'
    data = dll.read_bytes()
    assert hashlib.sha256(data).hexdigest() == SHA, 'Unsupported retail DLL'
    pe = pefile.PE(data=data, fast_load=True)
    assert pe.FILE_HEADER.TimeDateStamp == 0x6A350E20
    pe.parse_data_directories(directories=[pefile.DIRECTORY_ENTRY['IMAGE_DIRECTORY_ENTRY_EXCEPTION']])
    md = Cs(CS_ARCH_X86, CS_MODE_64)
    md.detail = True
    a.out.mkdir(parents=True, exist_ok=True)
    report = {'dll': str(dll), 'sha256': SHA, 'timestamp': '0x6A350E20',
              'native_execution': False, 'functions': {}, 'calls': {}, 'tables': {}}
    for rva in FUNCTIONS:
        if rva == 0x1ED6AD0:
            start, end = rva, 0x1ED6B45  # switch leaf: final LEA/RET, before its jump table
            assert pe.get_data(end-1, 1) == b'\xc3'
        else:
            e = next(x.struct for x in pe.DIRECTORY_ENTRY_EXCEPTION
                     if x.struct.BeginAddress <= rva < x.struct.EndAddress)
            start, end = e.BeginAddress, e.EndAddress
        code = pe.get_data(start, end-start)
        ins = list(md.disasm(code, start))
        report['functions'][hex(rva)] = {
            'start': hex(start), 'end': hex(end),
            'sha256': hashlib.sha256(code).hexdigest(), 'instructions': len(ins),
            'scope': 'principal .pdata range only; cold paths / vtables not resolved'}
        (a.out / f'{rva:X}.asm').write_text('\n'.join(
            f'{i.address:08X} {i.bytes.hex()} {i.mnemonic} {i.op_str}'.rstrip() for i in ins)+'\n')
    # UI input values select animation labels, not perfect-block acceptance.
    import struct
    report['pbslot_labels'] = {}
    for value in range(1, 14):
        target = struct.unpack('<I', pe.get_data(0x1ED6B48+(value-1)*4, 4))[0]
        body = pe.get_data(target, 8)
        assert body[:3] == bytes.fromhex('488d05') and body[7] == 0xc3
        string_rva = target+7+struct.unpack('<i', body[3:7])[0]
        label = pe.get_data(string_rva, 80).split(b'\0')[0].decode('ascii')
        report['pbslot_labels'][str(value)] = {'branch': hex(target), 'string_rva': hex(string_rva), 'label': label}
    for site, target in CALLS.items():
        i = next(md.disasm(pe.get_data(site, 5), site))
        assert i.mnemonic == 'call' and i.operands[0].type == CS_OP_IMM and i.operands[0].imm == target
        report['calls'][hex(site)] = hex(target)
    # r14d is zeroed first; the compiler materializes 8 with LEA, not MOV.
    assert pe.get_data(0xC52382, 3) == bytes.fromhex('4533f6')
    assert pe.get_data(0xC523A8, 4) == bytes.fromhex('418d5608')
    report['phase_precondition'] = {'site': '0xC523A8', 'value': 8, 'callee': '0x72EFDC'}
    # This selector is a leaf without its own .pdata boundary. Bound its exact
    # two returns explicitly, rather than including the following function.
    selector = pe.get_data(0xC52DF4, 0x18)
    report['reaction_selector'] = {'rva': '0xC52DF4', 'size': len(selector),
        'sha256': hashlib.sha256(selector).hexdigest(), 'enum_mapping': 'unresolved'}
    (a.out/'C52DF4.asm').write_text('\n'.join(f'{i.address:08X} {i.bytes.hex()} {i.mnemonic} {i.op_str}'.rstrip()
        for i in md.disasm(selector, 0xC52DF4))+'\n')
    with zipfile.ZipFile(a.game/'Data/Tables.pak') as z:
        for name in z.namelist():
            if not name.lower().endswith('.xml'): continue
            stem = Path(name).stem
            if stem not in TABLES and stem != 'CombatCombos__generated': continue
            raw = z.read(name)
            root = ET.fromstring(raw)
            row_tag = 'CombatCombo' if stem == 'CombatCombos__generated' else stem
            rows = list(root.iter(row_tag))
            entry = {'sha256': hashlib.sha256(raw).hexdigest(), 'bytes': len(raw),
                     'rows': len(rows), 'row_tag': row_tag}
            if stem == 'CombatCombos__generated':
                children = list(root)
                entry['root_tag'] = root.tag
                entry['child_tag_counts'] = dict(collections.Counter(x.tag for x in children))
            fields = ('action_type_id','mn_fragment_id','hasRagdoll','hasAlignProcclip',
                      'hit_type_id','attack_type_id')
            entry['counts'] = {k: dict(collections.Counter(r.get(k, '<absent>') for r in rows))
                              for k in fields if any(k in r.attrib for r in rows)}
            if stem == 'combat_action_hit':
                entry['ragdoll_by_fragment'] = dict(collections.Counter(
                    r.get('mn_fragment_id','')+' / '+r.get('hasRagdoll','<absent>') for r in rows))
            report['tables'][name] = entry
    (a.out/'audit.json').write_text(json.dumps(report, indent=2)+'\n')
    print(f'PASS: exact DLL, {len(CALLS)} direct edges, phase=8, {len(report["tables"])} XML summaries; native execution NONE')

if __name__ == '__main__':
    main()
