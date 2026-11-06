#!/usr/bin/env python3
from pathlib import Path
import argparse, struct, sys

EXPECTED_TS=0x6A350E20
CHECKS=[]
def ck(name, cond, detail=''):
    CHECKS.append((name, bool(cond), detail))

def parse_pe(path: Path):
    b=path.read_bytes()
    if b[:2]!=b'MZ': raise ValueError('not MZ')
    pe=struct.unpack_from('<I',b,0x3c)[0]
    if b[pe:pe+4]!=b'PE\0\0': raise ValueError('not PE')
    ts=struct.unpack_from('<I',b,pe+8)[0]
    nsec=struct.unpack_from('<H',b,pe+6)[0]
    optsz=struct.unpack_from('<H',b,pe+20)[0]
    opt=pe+24
    image_base=struct.unpack_from('<Q',b,opt+24)[0]
    sec=opt+optsz
    sections=[]
    for i in range(nsec):
        o=sec+i*40
        name=b[o:o+8].rstrip(b'\0').decode('ascii','replace')
        vs,va,rawsz,rawptr=struct.unpack_from('<IIII',b,o+8)
        sections.append((name,va,vs,rawptr,rawsz))
    def rvaoff(rva):
        for _,va,vs,rp,rs in sections:
            if va<=rva<va+max(vs,rs): return rp+(rva-va)
        raise ValueError(f'RVA unmapped: {rva:#x}')
    def read(rva,n):
        o=rvaoff(rva); return b[o:o+n]
    def qword(rva): return struct.unpack('<Q',read(rva,8))[0]
    def dword(rva): return struct.unpack('<I',read(rva,4))[0]
    return b,ts,image_base,read,qword,dword

def rel32_target(read, insn_rva):
    raw=read(insn_rva,5)
    if raw[0]!=0xE8: return None
    rel=struct.unpack('<i',raw[1:5])[0]
    return insn_rva+5+rel

def main():
    ap=argparse.ArgumentParser(); ap.add_argument('whgame',type=Path); a=ap.parse_args()
    b,ts,base,read,qword,dword=parse_pe(a.whgame)
    ck('timestamp',ts==EXPECTED_TS,hex(ts))
    ck('image base',base==0x180000000,hex(base))

    # Existing exact-build anchors kept from V64.10/V65.04.
    cdef=0x406B930
    ck('CDefaultSkeleton slot1 joint count',qword(cdef+1*8)==base+0x1AAF020,hex(qword(cdef+8)-base))
    ck('CDefaultSkeleton slot7 joint name lookup',qword(cdef+7*8)==base+0x78BC4C,hex(qword(cdef+56)-base))
    ck('joint name lookup prologue',read(0x78BC4C,12)==bytes.fromhex('4C 8B C2 4C 8B C9 83 CA FF 41 8A 00'))

    csa=0x3A711E0
    ck('CSkeletonAnim slot8 StartAnimation(name)',qword(csa+8*8)==base+0xAA7FA0,hex(qword(csa+64)-base))
    ck('StartAnimation(name) prologue',read(0xAA7FA0,16)==bytes.fromhex('48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57'))
    body=read(0xAA7FA0,0xC0)
    ck('StartAnimation wrapper dispatches id-based virtual +0x48',bytes.fromhex('FF 50 48') in body)

    csys=0x3DC2858
    ck('CSystem slot5 DoFrame candidate',qword(csys+5*8)==base+0x6678C4,hex(qword(csys+40)-base))
    ck('DoFrame prologue',read(0x6678C4,11)==bytes.fromhex('48 8B C4 48 89 58 10 48 89 70 18'))

    # V65.04a new static F8 proof: this is a CCryAction virtual, not CSystem::DoFrame.
    ccry_vt=0x40472D0
    nexus=0x669C88
    ck('CCryAction vslot +0x58 frame nexus',qword(ccry_vt+0x58)==base+nexus,hex(qword(ccry_vt+0x58)-base))
    ck('CCryAction frame nexus prologue',read(nexus,21)==bytes.fromhex('41 F6 C0 20 0F 85 65 03 00 00 48 8B C4 48 89 58 08 48 89 70 10'))

    col=qword(ccry_vt-8)
    ck('CCryAction vtable complete-object-locator',col==base+0x45E8FE0,hex(col-base))
    td_rva=dword(0x45E8FE0+12)
    name=read(td_rva+16,32).split(b'\0',1)[0]
    ck('CCryAction RTTI name',name==b'.?AVCCryAction@@',name.decode('ascii','replace'))

    # The exact inner chain that the new F8 ladder observes.
    ck('frame nexus phase +0x7E8 callsite',read(0x669D48,16)==bytes.fromhex('48 8B 0D B9 3A 2C 04 48 8B 01 FF 90 E8 07 00 00'))
    ck('frame nexus camera handoff slot +0x40 callsite',read(0x669DF3,25)==bytes.fromhex('48 8B 3D 0E 3A 2C 04 48 8B 4E 48 48 8B 07 C7 45 C0 00 00 00 00 48 8B 58 40'))
    ck('frame nexus renderer-global +0x380 callsite',read(0x669E60,21)==bytes.fromhex('48 8B 0D A1 3A 2C 04 48 85 C9 74 09 48 8B 01 FF 90 80 03 00 00'))
    ck('frame nexus final phase +0x7F8 callsite',read(0x669FB9,16)==bytes.fromhex('48 8B 0D 48 38 2C 04 48 8B 01 FF 90 F8 07 00 00'))
    ck('frame nexus CViewSystem::Update direct call',rel32_target(read,0x669DD7)==0x7F0220,hex(rel32_target(read,0x669DD7) or 0))

    ck('GetOrCreateRenderView prologue',read(0x43EB60,20)==bytes.fromhex('48 89 5C 24 08 57 48 83 EC 30 48 63 C2 48 81 C1 D8 23 00 00'))
    ck('CStandardGraphicsPipeline::Execute prologue',read(0x57278C,16)==bytes.fromhex('48 89 5C 24 10 48 89 74 24 18 57 48 83 EC 30 48'))

    for n,ok,d in CHECKS:
        print(('PASS' if ok else 'FAIL')+': '+n+(f' [{d}]' if d else ''))
    bad=[x for x in CHECKS if not x[1]]
    print(f'\nRESULT: {len(CHECKS)-len(bad)} passed, {len(bad)} failed')
    return 1 if bad else 0
if __name__=='__main__': sys.exit(main())
