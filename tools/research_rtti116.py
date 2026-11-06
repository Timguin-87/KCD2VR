"""Recover named retail RTTI vtables and annotate functions; offline only."""
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


def run(binary, out, requested):
    pe=PE(binary);assert hashlib.sha256(pe.data).hexdigest()==EXPECTED and pe.stamp==0x6A350E20
    out.mkdir(parents=True,exist_ok=True)
    dis=Cs(CS_ARCH_X86,CS_MODE_64);dis.detail=True
    def listing(rva):
        region=pe.function(rva)
        # An explicit known call/vtable target without unwind info can be a leaf.
        start=region[0] if region else rva;size=region[1]-start if region else 512
        lines=[]
        for ins in dis.disasm(pe.read(start,size),pe.base+start):
            notes=[]
            for op in ins.operands:
                if op.type==X86_OP_MEM and op.mem.base==X86_REG_RIP:
                    target=ins.address+ins.size+op.mem.disp-pe.base
                    try:
                        b=pe.read(target,300).split(b'\0')[0]
                        note=repr(b.decode('ascii')) if len(b)>2 and all(32<=c<127 for c in b) else ''
                        notes.append(f'RVA={target:X} {note}')
                    except ValueError:pass
            lines.append((f'{ins.address-pe.base:08X} {ins.mnemonic} {ins.op_str}'+(' ; '+ '; '.join(notes) if notes else '')).rstrip())
            if not region and ins.mnemonic=='ret':break
        (out/f'{rva:X}.annotated.asm').write_text('\n'.join(lines)+'\n',encoding='utf-8')
    records=[]
    for section,va,size,raw in pe.sections:
        if section==b'.text':continue
        for match in re.finditer(rb'\.\?AV[^\x00]{5,200}\x00',pe.data[raw:raw+size]):
            name=match[0][:-1].decode('ascii',errors='replace')
            if not any(s in name for s in ['CMaterialEffects@@','CSurfaceTypeManager@@','C_ActorMaterialEffects@','C_PlayerMaterialEffects@','WeaponSolver','WeaponProcSolver']):continue
            descriptor=va+match.start()-16;needle=struct.pack('<I',descriptor)
            for _,cv,cs,cr in pe.sections:
                at=-1;blob=pe.data[cr:cr+cs]
                while True:
                    at=blob.find(needle,at+1)
                    if at<12:break
                    col=cv+at-12
                    if at+12>len(blob):continue
                    fields=struct.unpack_from('<6I',blob,at-12)
                    if fields[0]!=1 or fields[5]!=col:continue
                    for pointer in pe.pointers(col):
                        vft=pointer+8;methods=[]
                        for slot in range(48):
                            method=struct.unpack('<Q',pe.read(vft+8*slot,8))[0]-pe.base
                            if not any(n==b'.text' and av<=method<av+sz for n,av,sz,_ in pe.sections):break
                            methods.append(dict(byte_offset=hex(slot*8),rva=hex(method)))
                        records.append(dict(name=name,descriptor=hex(descriptor),locator=hex(col),object_offset=fields[1],vtable=hex(vft),methods=methods))
    (out/'material_rtti.json').write_text(json.dumps(records,indent=2)+'\n',encoding='utf-8')
    for arg in requested:listing(int(arg,0))
    print(json.dumps(records))


if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('--whgame',type=Path,required=True);p.add_argument('--out',type=Path,required=True);p.add_argument('rvas',nargs='*')
    a=p.parse_args();run(a.whgame,a.out,a.rvas)
