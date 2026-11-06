#!/usr/bin/env python3
"""Read-only verifier for the C23 master-strike counted-window pins. No DLL execution."""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
import argparse,hashlib,json,re,struct
EXPECTED='bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7'
p=argparse.ArgumentParser(description=__doc__);p.add_argument('--dll',required=True);p.add_argument('--out',required=True);a=p.parse_args()
b=Path(a.dll).read_bytes();u16=lambda o:struct.unpack_from('<H',b,o)[0];u32=lambda o:struct.unpack_from('<I',b,o)[0]
pe=u32(0x3c);assert b[:2]==b'MZ' and b[pe:pe+4]==b'PE\0\0'
timestamp=u32(pe+8);optional=pe+24
sections=optional+u16(pe+20);mapping=[]
for i in range(u16(pe+6)):
 o=sections+40*i;vs,va,rs,rp=struct.unpack_from('<IIII',b,o+8);mapping.append((va,rs,rp))
def at(rva,n):
 for va,size,pos in mapping:
  if va<=rva and rva+n<=va+size:return b[pos+rva-va:pos+rva-va+n]
 raise ValueError(hex(rva))
root=Path(__file__).resolve().parents[1];source=read_source(root/'src/vrik/native_master_window23.h')
checks=[]
for rva,raw,size in re.findall(r'\{(0x[0-9A-Fa-f]+),"([^"]+)",(\d+)\}',source):
 expected=bytes(int(x,16) for x in re.findall(r'\\x([0-9a-fA-F]{2})',raw));assert len(expected)==int(size)
 actual=at(int(rva,16),len(expected));checks.append(dict(rva=rva,expected=expected.hex(),actual=actual.hex(),passed=actual==expected))
def branch(site,opcode_len,target):
 rel=struct.unpack('<i',at(site+opcode_len-4,4))[0];actual=site+opcode_len+rel
 checks.append(dict(branch=hex(site),target=hex(target),actual=hex(actual),passed=actual==target))
branch(0xD4ADC6,6,0x22B780B)   # jg into the kind5 cold block
branch(0x1899D16,5,0x909FC4)   # open tail-call: +8 increment
branch(0x1899D3E,5,0x909FF4)   # close tail-call: +8 decrement
for name,rva,expected in [('open +8 increment',0x909FC8,'448b41084c8bca488bd1418d400189'),('close +8 decrement',0x909FF8,'83690801'),
                          ('open reads actor+210',0x1899D04,'488b8910020000'),('close reads actor+210',0x1899D2C,'488b8910020000'),
                          ('D4AD94 reads current actor+210',0x0D4ADB8,'488b8110020000')]:
 actual=at(rva,len(expected)//2).hex();checks.append(dict(name=name,rva=hex(rva),expected=expected,actual=actual,passed=actual==expected))
sha=hashlib.sha256(b).hexdigest();passed=sha==EXPECTED and timestamp==0x6A350E20 and len(checks)==11 and all(c['passed'] for c in checks)
r=dict(dll=str(Path(a.dll).resolve()),sha256=sha,timestamp=hex(timestamp),checks=checks,passed=passed,nativeExecuted=False,nativeWritten=False)
out=Path(a.out);out.parent.mkdir(parents=True,exist_ok=True);out.write_text(json.dumps(r,indent=2)+'\n')
print(json.dumps(r,indent=2));raise SystemExit(0 if passed else 1)
