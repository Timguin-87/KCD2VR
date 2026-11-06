"""Read-only exact-retail audit. Requires pefile/capstone; never loads the DLL."""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
import argparse, hashlib, json, re

SITES = [
 (0x2A8A838,"PickupRequest165"),(0x2A8A92C,"PickupReturn165"),
 (0x2A8A7EC,"StealRequest165"),(0x2AC0004,"StealReturn165"),
 (0x8D07B0,"HandChange165"),(0x896C6E,"AlchemyStartBegin165"),
 (0x896C92,"AlchemyStart165"),(0xD5AE0A,"AlchemyUseBegin165"),
 (0xD5AD24,"AlchemyUseController165"),(0xD5AD55,"AlchemyTarget165"),
 (0xD5ADAD,"AlchemyUse165"),(0x11EDAD2,"AlchemyQueryBegin165"),
 (0x11EDAF1,"AlchemyQueryController165"),(0x11EDB09,"AlchemyState165"),
 (0x47ECBF,"RemoveRequest165"),(0x47EEBB,"RemoveAccepted165"),
 (0x37CFD2B,"ConstraintRequest165"),(0x37CFD3C,"ConstraintReturn165"),
 (0x712094,"PhysicParamRequest168"),(0x7120AD,"PhysicParamReturn168"),
 (0xB369E3,"PhysicAddSubmit36"),(0xB3784D,"PhysicRemoveSubmit36"),
 (0xB37D81,"PhysicActionReturn36"),
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
    lines = ["// Generated/verified by tools/audit_f5_178r36c3.py; exact WHGame only.",
             "static constexpr LifecycleSite165 kLifecycleSites165[]{"]
    windows, functions = [], {}
    for rva, callback in SITES:
        code = p.get_data(rva, 16)
        lines.append("    {0x%X,{%s},%s}," % (rva, ','.join('0x%02X' % b for b in code), callback))
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
    lines += ["};", ""]
    generated = '\n'.join(lines)
    if generate: source.write_text(generated, encoding='utf-8')
    else: assert read_source(source, encoding='utf-8') == generated, 'Source pins do not match DLL'
    # Return-address joins, owner access and object layouts are taken from these
    # exact call instructions / native loads, not SDK slot indexes.
    expected_calls = {0x2AC0093:0x711B28,0x7120A8:0xB35634,0x7120B9:0x711B5C}
    for site, target in expected_calls.items():
        i = next(md.disasm(p.get_data(site, 5), site))
        assert i.mnemonic == 'call' and i.operands[0].imm == target
    for offset, target in [(0x2B8,0x2A8A838),(0x2E0,0x2A8A7EC)]:
        assert int.from_bytes(p.get_data(0x3EC85E0+offset,8),'little') == p.OPTIONAL_HEADER.ImageBase+target
    out.mkdir(parents=True,exist_ok=True)
    for start, asm in functions.items(): (out/f'{start:X}.asm').write_text(asm,encoding='utf-8')
    result = dict(dll_sha256=SHA,timestamp='0x6A350E20',executed=False,hook_sites=windows,
                  call_edges={hex(k):hex(v) for k,v in expected_calls.items()},
                  henry_owner='pickup actor ID + exact CEntity, manager backlink, exact watched entity pointer/ID; no SDK layouts',
                  limitation='Static proof, not live grabbing/physical constraint acceptance')
    (out/'F5_NATIVE_AUDIT.json').write_text(json.dumps(result,indent=2)+'\n')
    print(f'PASS: {len(SITES)} pins, decoded non-overlap and local branch entries; {len(expected_calls)} call edges; pickable vtable.')

if __name__ == '__main__':
    ap = argparse.ArgumentParser();ap.add_argument('--dll',type=Path,required=True)
    ap.add_argument('--out',type=Path,required=True);ap.add_argument('--generate',action='store_true')
    args=ap.parse_args()
    audit(args.dll,Path(__file__).resolve().parents[1]/'src/diagnostics/interaction_lifecycle_sites165.inl',args.out,args.generate)
