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
 return ts,base,rd,qw

def main():
 ap=argparse.ArgumentParser();ap.add_argument('whgame',type=Path);a=ap.parse_args();ts,base,rd,qw=parse(a.whgame)
 ck('timestamp',ts==EXPECTED_TS,hex(ts)); ck('image base',base==0x180000000,hex(base))
 ctxvt=0x3BD6180; clipvt=0x3BD6260; paramsvt=0x3BD6220
 ck('IKTargetContext vslot5 Update',qw(ctxvt+5*8)==base+0x145DCBC,hex(qw(ctxvt+40)-base))
 ck('IKTargetContext Update prologue',rd(0x145DCBC,16)==bytes.fromhex('48 8B C4 48 89 58 08 48 89 70 10 48 89 78 18 55'))
 ck('IKTarget clip vslot12 activation delegate',qw(clipvt+12*8)==base+0x145E5F4,hex(qw(clipvt+96)-base))
 ck('IKTarget params serialization vslot4',qw(paramsvt+4*8)==base+0xB77774,hex(qw(paramsvt+32)-base))
 ck('IKTarget activate/copy prologue',rd(0x145DBCC,16)==bytes.fromhex('40 53 48 83 EC 30 48 8B D9 0F 29 74 24 20 48 83'))
 ck('IKTarget bridge prologue',rd(0x145E558,16)==bytes.fromhex('48 89 5C 24 08 48 89 74 24 18 57 48 83 EC 40 48'))
 ck('IKTarget pose writer prologue',rd(0xA7F6D0,16)==bytes.fromhex('48 89 5C 24 08 57 48 83 EC 20 48 8B FA 49 8B D9'))
 ck('IKTarget blend writer prologue',rd(0xA80150,16)==bytes.fromhex('40 53 48 83 EC 30 48 8B DA 0F 29 74 24 20 49 8B'))
 ck('IKTarget target lookup prologue',rd(0xA80DC0,16)==bytes.fromhex('48 8B C4 48 89 58 08 48 89 68 10 48 89 70 18 48'))
 ck('IKTarget context factory prologue',rd(0x1613BC4,16)==bytes.fromhex('48 89 5C 24 08 48 89 74 24 18 48 89 7C 24 20 55'))
 ck('IKTarget clip factory prologue',rd(0x1613D58,16)==bytes.fromhex('48 89 5C 24 18 57 48 83 EC 20 83 3D 07 96 E8 03'))
 ck('IKTarget params ctor prologue',rd(0x1613F44,16)==bytes.fromhex('40 53 48 83 EC 20 48 8D 05 CF 22 5C 02 48 C7 41'))
 # Direct bridge calls establish writer ABIs and the signed widening of the handle CRC.
 bridge=rd(0x145E558,0x98)
 ck('bridge calls pose writer',bytes.fromhex('E8 12 11 62 FF') in bridge)
 ck('bridge calls blend writer',bytes.fromhex('E8 71 1B 62 FF') in bridge)
 ck('bridge sign-extends CRC key',bytes.fromhex('48 63 D8') in bridge)
 for n,ok,d in checks: print(('PASS' if ok else 'FAIL')+': '+n+(f' [{d}]' if d else ''))
 bad=[x for x in checks if not x[1]]; print(f'\nRESULT: {len(checks)-len(bad)} passed, {len(bad)} failed'); return 1 if bad else 0
if __name__=='__main__':sys.exit(main())
