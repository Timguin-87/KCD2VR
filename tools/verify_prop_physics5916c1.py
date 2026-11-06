#!/usr/bin/env python3
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
import argparse,hashlib,json,re,struct
from pathlib import Path
from native_research112 import PE
p=argparse.ArgumentParser();p.add_argument('dll');p.add_argument('--out');a=p.parse_args()
e=PE(a.dll);sha=hashlib.sha256(e.data).hexdigest()
assert e.stamp==0x6A350E20 and sha=='bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7'
s=read_source(Path(__file__).resolve().parents[1]/'src/vrik/prop_native_pins5916c1.inl')
checks=[]
for rva,n,raw in re.findall(r'\{0x([0-9A-F]+),(\d+),\{([^}]+)\}\}',s):
 rva=int(rva,16);n=int(n);b=bytes(int(x,16) for x in re.findall(r'0x([0-9A-Fa-f]+)',raw))
 assert len(b)==n and e.read(rva,n)==b
 checks.append({'rva':hex(rva),'bytes':b.hex()})
assert len(checks)==7
for offset,target in ((0x30,0x8A6C90),(0x38,0xAC0D18)):
 assert struct.unpack('<Q',e.read(0x3A48DC0+offset,8))[0]-e.base==target
# Setter consumes the same linear/angular fields that status publishes.
assert e.read(0x2279C5F,5)==bytes.fromhex('f20f10460c')
assert e.read(0x2279CD1,5)==bytes.fromhex('f20f104618')
assert e.read(0x8A72A2,9)==bytes.fromhex('8b83c402000089474c')
r={'sha256':sha,'timestamp':hex(e.stamp),'pins':checks,'rigid_vtable':'0x3A48DC0','status_slot':'0x30','action_slot':'0x38',
   'status_mass_offset':'0x4C','velocity_type':10,'status_type':8,'native_execution':False}
t=json.dumps(r,indent=2);print(t)
if a.out:Path(a.out).write_text(t+'\n')
