#!/usr/bin/env python3
from pathlib import Path
import argparse,struct,hashlib,sys
EXPECTED_TS=0x6A350E20
EXPECTED_SHA='bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7'
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
 return b,ts,base,rd
def main():
 ap=argparse.ArgumentParser(); ap.add_argument('whgame',type=Path); a=ap.parse_args(); b,ts,base,rd=parse(a.whgame)
 sha=hashlib.sha256(b).hexdigest(); ck('sha256',sha==EXPECTED_SHA,sha); ck('timestamp',ts==EXPECTED_TS,hex(ts)); ck('image base',base==0x180000000,hex(base))
 exact={
  'F5 LookAt resolver +0x59B0F0':(0x59B0F0,'48 89 5c 24 08 48 89 74 24 10 57 48 83 ec 30'),
  'F10 skinning writer +0x5FE2F0':(0x5FE2F0,'48 8b c4 53 55 57 41 54 41 55 41 56 41 57 48 81 ec f0 00 00 00'),
  'F8 camera build +0x865550':(0x865550,'48 8b c4 48 89 58 08 48 89 68 10 48 89 70 18 48 89 78 20 41 56 48 83 ec 30'),
  'F8 pair builder +0x866F6C':(0x866F6C,'48 8b c4 4c 89 48 20 55 53 56 57 41 54 41 56 41 57 48 8d a8 b8 fa ff ff 48 81 ec 10 06 00 00'),
  'F8 CSystem::Render +0x43E464':(0x43E464,'48 8b c4 48 89 58 08 48 89 70 10 48 89 78 18 55 41 56 41 57 48 8d 68 a1 48 81 ec c0 00 00 00'),
  'Shop IsShopOpened +0x2EFE6B0':(0x2EFE6B0,'40 53 48 83 ec 20 48 8b 49 70 48 8b da 41 8b d0'),
  'Shop find-by-type +0x2EFE54C':(0x2EFE54C,'48 89 5c 24 08 48 89 6c 24 10 48 89 74 24 18 57 48 83 ec 20'),
  'Pose getter selector1 +0x53D830':(0x53D830,'48 83 ec 28 44 8b c2 48 83 c1 08 ba 01 00 00 00'),
  'Pose setter selector1 +0x53F4D0':(0x53F4D0,'48 83 ec 28 41 0f 10 08 48 83 c1 08 45 8b 50 18'),
 }
 for n,(r,h) in exact.items():
  want=bytes.fromhex(h); got=rd(r,len(want)); ck(n,got==want,got.hex(' '))
 for n,ok,d in checks: print(('PASS' if ok else 'FAIL')+': '+n+(f' [{d}]' if d else ''))
 bad=[x for x in checks if not x[1]]; print(f'\nRESULT: {len(checks)-len(bad)} passed, {len(bad)} failed'); return 1 if bad else 0
if __name__=='__main__': sys.exit(main())
