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
    return b,ts,image_base,read,qword

def main():
    ap=argparse.ArgumentParser(); ap.add_argument('whgame',type=Path); a=ap.parse_args()
    b,ts,base,read,qword=parse_pe(a.whgame)
    ck('timestamp',ts==EXPECTED_TS,hex(ts))
    ck('image base',base==0x180000000,hex(base))

    # CDefaultSkeleton primary vtable: slot 1=count accessor, slot 7=name->joint hash lookup.
    cdef=0x406B930
    ck('CDefaultSkeleton slot1 joint count',qword(cdef+1*8)==base+0x1AAF020,hex(qword(cdef+8)-base))
    ck('CDefaultSkeleton slot7 joint name lookup',qword(cdef+7*8)==base+0x78BC4C,hex(qword(cdef+56)-base))
    ck('joint name lookup prologue',read(0x78BC4C,12)==bytes.fromhex('4C 8B C2 4C 8B C9 83 CA FF 41 8A 00'))

    # CSkeletonAnim primary vtable slot 8 is the name-based StartAnimation wrapper.
    csa=0x3A711E0
    ck('CSkeletonAnim slot8 StartAnimation(name)',qword(csa+8*8)==base+0xAA7FA0,hex(qword(csa+64)-base))
    ck('StartAnimation(name) prologue',read(0xAA7FA0,16)==bytes.fromhex('48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57'))
    # Static semantic sanity: wrapper must contain call through self vtable +0x48 near its tail.
    body=read(0xAA7FA0,0xC0)
    ck('StartAnimation wrapper dispatches id-based virtual +0x48',bytes.fromhex('48 8B 06 4C 8B C3 48 8B CE FF 50 48') in body or bytes.fromhex('48 8B 06') in body and bytes.fromhex('FF 50 48') in body)

    # CSystem outer frame anchor.
    csys=0x3DC2858
    ck('CSystem slot5 DoFrame candidate',qword(csys+5*8)==base+0x6678C4,hex(qword(csys+40)-base))
    ck('DoFrame prologue',read(0x6678C4,11)==bytes.fromhex('48 8B C4 48 89 58 10 48 89 70 18'))

    for n,ok,d in CHECKS: print(('PASS' if ok else 'FAIL')+': '+n+(f' [{d}]' if d else ''))
    bad=[x for x in CHECKS if not x[1]]
    print(f'\nRESULT: {len(CHECKS)-len(bad)} passed, {len(bad)} failed')
    return 1 if bad else 0
if __name__=='__main__': sys.exit(main())
