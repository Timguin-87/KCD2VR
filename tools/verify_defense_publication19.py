#!/usr/bin/env python3
"""Exact-retail, read-only provenance of the phase-4 diagnostic offsets. No native execution."""
import argparse,hashlib,json,struct
from pathlib import Path
parser=argparse.ArgumentParser();parser.add_argument('--dll',type=Path,default=Path(r'C:/Steam/steamapps/common/KingdomComeDeliverance2/Bin/Win64MasterMasterSteamPGO/WHGame.dll'));parser.add_argument('--out',type=Path)
a=parser.parse_args();data=a.dll.read_bytes();sha=hashlib.sha256(data).hexdigest();assert sha=='bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7',sha
pe=struct.unpack_from('<I',data,0x3c)[0];assert data[pe:pe+4]==b'PE\0\0';sections=struct.unpack_from('<H',data,pe+6)[0];assert struct.unpack_from('<I',data,pe+8)[0]==0x6A350E20
opt=pe+24;section_start=opt+struct.unpack_from('<H',data,pe+20)[0]
def read(rva,n):
 for j in range(sections):
  at=section_start+j*40;vs,va,rs,rp=struct.unpack_from('<IIII',data,at+8)
  if va<=rva and rva+n<=va+rs:return data[rp+rva-va:rp+rva-va+n]
 raise AssertionError(hex(rva))
pins=[
 (0xC55D8E,'837c245004','native defense callback tests new phase4'),
 (0xC55DB9,'e80ea5adff','callback calls7302CC'),
 (0xC55DBE,'4883b80801000000','pending gate is on7302CC RESULT+108'),
 (0x7302D6,'488b4128','7302CC cached Defense+28 models'),
 (0x7302F9,'488b8348020000488b4048','derive models from actor+248 ->automation+48'),
 (0x10EF309,'488d0570b1a102488907','automation main vft3B0A480'),
 (0x10EF3D4,'488d8f18010000','embedded Defense atautomation+118'),
 (0x10EF257,'488d0532b1a102488903','Defense vft3B0A390'),
 (0x10EF49D,'48897748','constructor stores models atautomation+48'),
 (0x10EEBE2,'48899fa8010000','models constructor stores owner+1A8'),
 (0x947210,'488b4108c3','exact C_CombatTarget entity+8 read'),
]
rows=[]
for rva,expected,meaning in pins:
 b=bytes.fromhex(expected);actual=read(rva,len(b));assert actual==b,(hex(rva),actual.hex(),expected);rows.append({'rva':hex(rva),'bytes':b.hex(),'meaning':meaning})
assert struct.unpack('<Q',read(0x3A796F0+8,8))[0]==0x180947210
result={'dll':str(a.dll),'sha256':sha,'timestamp':'0x6A350E20','pins':rows,'total':len(rows)+1,'result':'PASS','scope':'observation only; no ABI call or forced outcome'}
if a.out:a.out.write_text(json.dumps(result,indent=2))
print(f"PASS defense_publication19: {len(rows)+1} exact native pins; phase4 ->models108 + CE8 diagnostic")
