"""Record exact supplied retail/reference evidence; never modifies game files."""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
import hashlib
import json
import re
import struct
import sys
import xml.etree.ElementTree as ET

ROOT=Path(__file__).resolve().parents[1]
GAME=Path('C:/Steam/steamapps/common/KingdomComeDeliverance2')
OUT=ROOT/'review/V66.162R1'

def sha(p):
    return hashlib.sha256(p.read_bytes()).hexdigest()

def main():
    # Supply a directory containing pefile/capstone, or install them normally.
    if len(sys.argv)>1:
        sys.path.insert(0,sys.argv[1])
    import pefile
    from capstone import Cs, CS_ARCH_X86, CS_MODE_64
    OUT.mkdir(parents=True,exist_ok=True)
    dll=GAME/'Bin/Win64MasterMasterSteamPGO/WHGame.dll'
    pe=pefile.PE(str(dll),fast_load=True)
    digest=sha(dll)
    assert digest=='bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7'
    assert pe.FILE_HEADER.TimeDateStamp==0x6a350e20
    base=pe.OPTIONAL_HEADER.ImageBase
    rva=0x1006468
    entry=pe.get_data(rva,14)
    assert entry.hex()=='40534883ec204883795800488bd9'
    source=read_source(ROOT/'src/flashui_probe.cpp')
    signature=re.search(r'constexpr unsigned char bytes\[\]=\{([^}]+)\};',source[source.index('bool VerifyHudRender162r1'):]).group(1)
    assert bytes(int(x.strip(),16) for x in signature.split(','))==entry
    slots={hex(n):hex(struct.unpack('<Q',pe.get_data(0x4049bd0+n,8))[0]-base) for n in (0xc0,0xc8)}
    assert all(v==hex(rva) for v in slots.values())
    disasm=[]
    for ins in Cs(CS_ARCH_X86,CS_MODE_64).disasm(pe.get_data(rva,128),rva):
        disasm.append(f'{ins.address:08x} {ins.mnemonic} {ins.op_str}')
        if ins.mnemonic=='ret':
            break
    profile=GAME/'IPL gamedata.pak/Libs/Config/defaultProfile.xml'
    xml=ET.parse(profile).getroot()
    maps={m.attrib.get('name'):m for m in xml.iter('actionmap')}
    hdr=read_source(ROOT/'src/ui_back_maps135r2.h')
    names=re.findall(r'"([^"]+)"',hdr.split('Minigames{',1)[1].split('};',1)[0])
    assert len(names)==31
    assert all(n in maps for n in names), [n for n in names if n not in maps]
    alchemy=ET.tostring(maps['alchemy'],encoding='unicode')
    reference=Path('C:/Users/turcs/Downloads/Dynamic Hud 3452 2.7 2026-08-23T16-47Z GR90GCTeV.7z')
    refroot=ROOT.parent/'dynamic-reference/dynamic_hud'
    refs={str(p.relative_to(refroot)).replace('\\','/'):sha(p) for p in refroot.rglob('*') if p.is_file()}
    asi=ROOT/'build/release/RelWithDebInfo/KCD2VR.asi'
    assert b'KCD2VR-V66.162R1-CONTEXTUAL-HUD-NATIVE-MINIGAMES' in asi.read_bytes()
    evidence=dict(retail_dll=dict(path=str(dll),sha256=digest,timestamp=hex(pe.FILE_HEADER.TimeDateStamp),
                   hud_vtable_rva='0x4049bd0',forwarder_rva=hex(rva),entry_hex=entry.hex(),vtable_slots=slots,
                   forwarding_body_disassembly=disasm, runtime_validated=False),
                  native_profile=dict(path=str(profile),sha256=sha(profile),activity_maps=names,alchemy_xml=alchemy),
                  reference=dict(path=str(reference),sha256=sha(reference),extracted_files_sha256=refs,redistributed=False),
                  output_asi=dict(sha256=sha(asi),marker_verified=True,headset_verified=False),
                  interpretation='HUD-only forwarding draw scope inferred from exact retail code; not an SDK ABI transplant.')
    (OUT/'static_evidence.json').write_text(json.dumps(evidence,indent=2),encoding='utf-8')
    print(json.dumps(dict(retail_sha256=digest,activity_maps=len(names),asi_sha256=sha(asi),evidence=str(OUT/'static_evidence.json'))))

if __name__=='__main__':
    main()
