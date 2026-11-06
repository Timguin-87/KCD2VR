#!/usr/bin/env python3
from pathlib import Path
import argparse, struct, sys
EXPECTED_TS=0x6A350E20
checks=[]
def ck(n,c,d=''): checks.append((n,bool(c),d))
def parse(path):
 b=path.read_bytes(); pe=struct.unpack_from('<I',b,0x3c)[0]; ts=struct.unpack_from('<I',b,pe+8)[0]; ns=struct.unpack_from('<H',b,pe+6)[0]; os=struct.unpack_from('<H',b,pe+20)[0]; opt=pe+24; base=struct.unpack_from('<Q',b,opt+24)[0]; sec=opt+os; ss=[]
 for i in range(ns):
  o=sec+i*40; vs,va,rs,rp=struct.unpack_from('<IIII',b,o+8); ss.append((va,max(vs,rs),rp))
 def off(r):
  for va,size,rp in ss:
   if va<=r<va+size:return rp+r-va
  raise ValueError(hex(r))
 def rd(r,n): o=off(r); return b[o:o+n]
 def qw(r): return struct.unpack('<Q',rd(r,8))[0]
 def dw(r): return struct.unpack('<I',rd(r,4))[0]
 return ts,base,rd,qw,dw
def rel32(read,rva):
 x=read(rva,5)
 if x[0]!=0xE8:return None
 return rva+5+struct.unpack('<i',x[1:])[0]
def main():
 ap=argparse.ArgumentParser(); ap.add_argument('whgame',type=Path); a=ap.parse_args(); ts,base,rd,qw,dw=parse(a.whgame)
 ck('timestamp',ts==EXPECTED_TS,hex(ts)); ck('image base',base==0x180000000,hex(base))
 look=0x59B0F0
 ck('LookAt entity target resolver prologue',rd(look,15)==bytes.fromhex('48 89 5C 24 08 48 89 74 24 10 57 48 83 EC 30'))
 ck('resolver tests target+0x18',rd(0x59B0FF,5)==bytes.fromhex('48 83 7A 18 00'))
 ck('resolver calls target vslot +0x10',rd(0x59B10F,9)==bytes.fromhex('48 8B 02 48 8B CA FF 50 10'))
 ck('resolver calls actor capability',rel32(rd,0x59B129)==0x5D26C4,hex(rel32(rd,0x59B129) or 0))
 ck('resolver calls eye/head offset',rel32(rd,0x59B13E)==0x59B180,hex(rel32(rd,0x59B13E) or 0))
 ck('resolver success return AL=1',rd(0x59B167,2)==bytes.fromhex('B0 01'))
 ck('resolver failure return AL=0',rd(0x59B179,2)==bytes.fromhex('32 C0'))
 ck('direct caller 59AE9A',rel32(rd,0x59AE9A)==look,hex(rel32(rd,0x59AE9A) or 0))
 ck('record-type caller 59AF21',rel32(rd,0x59AF21)==look,hex(rel32(rd,0x59AF21) or 0))
 ck('eye offset mode global read',rd(0x59B1AC,6)==bytes.fromhex('8B 05 9E 42 39 04'))
 # CEntity vtable proof: COL immediately before vtable and RTTI type descriptor.
 vt=0x4072C78; col=qw(vt-8); ck('CEntity vtable COL',col==base+0x4626B88,hex(col-base))
 td=dw(0x4626B88+12); name=rd(td+16,32).split(b'\0',1)[0]
 ck('CEntity RTTI name',name==b'.?AVCEntity@@',name.decode('ascii','replace'))
 ck('CEntity vslot2 returns +0x10',qw(vt+16)==base+0x638480,hex(qw(vt+16)-base))
 ck('CEntity vslot2 prologue',rd(0x638480,5)==bytes.fromhex('48 8B 41 10 C3'))
 for n,ok,d in checks: print(('PASS' if ok else 'FAIL')+': '+n+(f' [{d}]' if d else ''))
 bad=[x for x in checks if not x[1]]; print(f'\nRESULT: {len(checks)-len(bad)} passed, {len(bad)} failed'); return 1 if bad else 0
if __name__=='__main__': sys.exit(main())
