"""Reproduce verified instruction-boundary xrefs with optional Capstone 5.

Install Capstone into a research environment only. These are discovery records,
not a runtime ABI declaration; no target code is loaded or executed.
"""
import argparse
import hashlib
import json
from pathlib import Path
import struct
from capstone import Cs, CS_ARCH_X86, CS_MODE_64
from capstone.x86 import X86_OP_MEM, X86_REG_RIP, X86_REG_R8
from native_research112 import PE
from discover_physics113 import EXPECTED

def run(binary,out):
    pe=PE(binary)
    assert hashlib.sha256(pe.data).hexdigest()==EXPECTED and pe.stamp==0x6A350E20
    out.mkdir(parents=True,exist_ok=True)
    dis=Cs(CS_ARCH_X86,CS_MODE_64);dis.detail=True
    targets={0x406ee68:'AddConstraint',0x406ee58:'AddImpulse',0x406e130:'SetPhysicParams',
             0x3a6fac0:'ArticulatedImpulse',0x3a71a90:'RigidImpulse'}
    _,va,size,raw=next(s for s in pe.sections if s[0]==b'.text')
    code=pe.data[raw:raw+size];hits=[]
    def listing(instructions):
        return '\n'.join(f'{x.address-pe.base:X} {x.mnemonic} {x.op_str}'.rstrip() for x in instructions)+'\n'
    for op in (b'\x48\x8d\x0d',b'\x48\x8d\x15',b'\x4c\x8d\x05',b'\x4c\x8d\x0d'):
        at=-1
        while True:
            at=code.find(op,at+1)
            if at<0 or at+7>len(code):break
            target=va+at+7+struct.unpack_from('<i',code,at+3)[0]
            if target not in targets:continue
            rva=va+at;f=pe.function(rva)
            if not f:continue
            ins=list(dis.disasm(pe.read(f[0],f[1]-f[0]),pe.base+f[0]))
            ix=next((i for i,x in enumerate(ins) if x.address==pe.base+rva),None)
            if ix is None:continue
            hit={'name':targets[target],'string_rva':hex(target),'xref_rva':hex(rva),
                 'function':[hex(x) for x in f],'asm':listing(ins[max(0,ix-15):ix+18])}
            if f[0]==0x144dcb4:
                # Registration builds a native member callback object, then
                # associates it with this exact Lua name. Verify its code start.
                for x in reversed(ins[max(0,ix-15):ix]):
                    if x.mnemonic=='lea' and x.operands[0].reg==X86_REG_R8 and x.operands[1].type==X86_OP_MEM and x.operands[1].mem.base==X86_REG_RIP:
                        callback=x.address+x.size+x.operands[1].mem.disp-pe.base
                        region=pe.function(callback)
                        assert region and region[0]==callback
                        hit['registered_callback_rva']=hex(callback)
                        (out/(hit['name']+'.asm')).write_text(listing(dis.disasm(pe.read(*[region[0],region[1]-region[0]]),pe.base+region[0])),encoding='utf-8')
                        break
            hits.append(hit)
    (out/'binding_xrefs.json').write_text(json.dumps(hits,indent=2)+'\n',encoding='utf-8')
    print(json.dumps([{k:v for k,v in h.items() if k!='asm'} for h in hits],indent=2))

if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('--whgame',type=Path,required=True);p.add_argument('--out',type=Path,required=True)
    a=p.parse_args();run(a.whgame,a.out)
