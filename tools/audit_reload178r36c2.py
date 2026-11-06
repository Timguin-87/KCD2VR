"""Read-only exact-retail audit. Requires pefile/capstone; never loads the DLL."""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
import argparse, hashlib, json, re

SITES = [
    (0x18468BC, "ReloadAutomaticRequest19c3"),
    (0x2896174, "ReloadActionAdmission64"),
    (0x289CCD0, "ReloadDispatch36"), (0x2886BCE, "ReloadConstruct36"),
    (0x28952CC, "ReloadDestroy36"), (0x289595C, "ReloadHandBegin36"),
    (0x28959D8, "ReloadHandCreate36"), (0x40B730, "ReloadHandRemove36"),
    (0x2895A18, "ReloadHandItems36"), (0x28959C7, "ReloadHandEnd36"),
    (0x289C988, "ReloadTransfer36"), (0x2A8A0C0, "ReloadAmmoBind36"),
    (0x2A8A030, "ReloadNockBegin36"), (0x2A8A0AD, "ReloadNockEnd36"),
    (0x2895F47, "ReloadCocking36"), (0x289D0B4, "ReloadFinalize47C2"),
]
SHA = "bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7"

def audit(dll, source, out, generate=False):
    import pefile
    from capstone import Cs, CS_ARCH_X86, CS_MODE_64, CS_OP_IMM
    data = dll.read_bytes()
    assert hashlib.sha256(data).hexdigest() == SHA, "Wrong retail DLL"
    p = pefile.PE(data=data, fast_load=True)
    assert p.FILE_HEADER.TimeDateStamp == 0x6A350E20
    p.parse_data_directories(directories=[pefile.DIRECTORY_ENTRY['IMAGE_DIRECTORY_ENTRY_EXCEPTION']])
    bounds = [(e.struct.BeginAddress, e.struct.EndAddress) for e in p.DIRECTORY_ENTRY_EXCEPTION]
    md = Cs(CS_ARCH_X86, CS_MODE_64); md.detail = True
    lines = ["// Generated/verified by tools/audit_reload178r36c2.py; exact WHGame only.",
             f"static const std::array<ReloadSite36,{len(SITES)}> kReloadSites36{{{{"]
    windows, functions = [], {}
    for rva, callback in SITES:
        code = p.get_data(rva, 16)
        lines.append("    {0x%X, {{%s}}, %s}," % (rva, ','.join('0x%02X' % b for b in code), callback))
        start, end = next((s,e) for s,e in bounds if s <= rva < e)
        ins = list(md.disasm(p.get_data(start, end-start), start))
        assert any(i.address == rva for i in ins), hex(rva)
        patch_end = rva
        for i in ins:
            if i.address < rva: continue
            patch_end = i.address+i.size
            if patch_end-rva >= 16: break
        assert patch_end-rva >= 16 and patch_end <= end
        for i in ins:
            if i.mnemonic.startswith('j') and i.operands[0].type == CS_OP_IMM:
                target = i.operands[0].imm
                assert not (rva < target < patch_end and not rva <= i.address < patch_end), (hex(rva),hex(i.address),hex(target))
        windows.append(dict(rva=hex(rva), callback=callback, function=hex(start), end=hex(end),
                            conservative_16_byte_decode=patch_end-rva, pin=code.hex()))
        functions[start] = '\n'.join(f'{i.address:08X} {i.bytes.hex()} {i.mnemonic} {i.op_str}'.rstrip() for i in ins)+'\n'
    for a in windows:
        for b in windows:
            if a is b: continue
            assert not int(a['rva'],16) <= int(b['rva'],16) < int(a['rva'],16)+a['conservative_16_byte_decode']
    lines += ["}};", ""]
    generated = '\n'.join(lines)
    if generate: source.write_text(generated, encoding='utf-8')
    else: assert read_source(source, encoding='utf-8') == generated, 'Source pins do not match DLL'
    # Return-address joins, owner access and object layouts are taken from these
    # exact call instructions / native loads, not SDK slot indexes.
    expected_calls = {0x2895998:0x28959D8,0x28959C2:0x40B730,0x289C93B:0x289595C,
                      0x289C952:0x289C988,0x289C9DB:0x2A8A0C0,0x2A8A0FC:0x2A8A030,
                      0x289598A:0x9115E0,0x28959B4:0x9115E0,0x2895F42:0x9115E0,0x289D0E2:0x289C930,0x289D0F3:0x2895F30,
                      0x2AAA00D:0x18468BC,0x1846A4F:0x2896174,0x289618D:0x2886B24,0x2AA9E33:0x7F3310}
    for site, target in expected_calls.items():
        i = next(md.disasm(p.get_data(site, 5), site))
        assert i.mnemonic == 'call' and i.operands[0].imm == target
    assert int.from_bytes(p.get_data(0x3E78E78,8),'little') == p.OPTIONAL_HEADER.ImageBase+0x289CCD0
    # The automatic-request receiver is the actor's equipment (type 4), NOT
    # C5F5EC's missile-item component (item + 0x58). Pin the producing getter,
    # its preserved RSI data flow, and the callee's weapon load independently.
    receiver_pins = {
        0x7F3304:'488b8980020000e900000000', # player+280 -> same type-4 getter
        0x7F3322:'b204', 0x7F3327:'ff5070', 0x7F3340:'3c04',
        0x2AA9E2C:'488b8980020000', 0x2AA9E3C:'488bf0',
        0x2AAA001:'488bce', 0x2AAA008:'4533c0b201',
        0x18468D6:'488b4940',
        0xC5F5F2:'ba02000000', 0xC5F60B:'488b4358',
    }
    for site, hx in receiver_pins.items():
        assert p.get_data(site,len(bytes.fromhex(hx))) == bytes.fromhex(hx), hex(site)
    from capstone.x86_const import X86_REG_RSI
    receiver_flow = list(md.disasm(p.get_data(0x2AA9E3F,0x2AAA012-0x2AA9E3F),0x2AA9E3F))
    assert receiver_flow[-1].address == 0x2AAA00D
    assert all(X86_REG_RSI not in i.regs_access()[1] for i in receiver_flow), 'equipment RSI overwritten'
    out.mkdir(parents=True,exist_ok=True)
    for start, asm in functions.items(): (out/f'{start:X}.asm').write_text(asm,encoding='utf-8')
    result = dict(dll_sha256=SHA,timestamp='0x6A350E20',executed=False,hook_sites=windows,
                  call_edges={hex(k):hex(v) for k,v in expected_calls.items()},
                  receiver_chain=dict(kind='actor equipment type4, not missile-item component',pins={hex(k):v for k,v in receiver_pins.items()},rsi_preserved=True),
                  candidate_owner='exact fresh Henry equipment/current bow',henry_owner='native actor return/argument plus exact C_Player/CEntity identity',
                  limitation='Static proof, not live reload/physical constraint acceptance')
    (out/'RELOAD_NATIVE_AUDIT.json').write_text(json.dumps(result,indent=2)+'\n')
    print(f'PASS: {len(SITES)} pins, decoded non-overlap and local branch entries; {len(expected_calls)} call edges; dispatcher vtable; equipment receiver chain.')

if __name__ == '__main__':
    ap = argparse.ArgumentParser();ap.add_argument('--dll',type=Path,required=True)
    ap.add_argument('--out',type=Path,required=True);ap.add_argument('--generate',action='store_true')
    args=ap.parse_args()
    audit(args.dll,Path(__file__).resolve().parents[1]/'src/diagnostics/reload_lifecycle_sites178r36c2.inl',args.out,args.generate)
