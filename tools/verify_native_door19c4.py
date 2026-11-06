"""Verify compiled door leaf pins against the exact retail image; never executes it."""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
import argparse,hashlib,json,re,struct
from pathlib import Path
from native_research112 import PE

def verify(dll,source):
 p=PE(str(dll)); errors=[]; checks=[]
 sha=hashlib.sha256(p.data).hexdigest()
 assert sha=='bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7'
 assert p.stamp==0x6A350E20
 text=read_source(source/'src/core/native_door_pins19c4.h', encoding='utf-8-sig')
 pins={name:bytes(int(x,16) for x in re.findall(r'0x([0-9a-fA-F]+)',body)) for name,body in re.findall(r'constexpr unsigned char (\w+)\[\]\{([^}]+)\}',text)}
 assert len(pins)==7, 'Missing compiled native door pins'
 for r,name in re.findall(r'\{0x([0-9A-Fa-f]+),(\w+),sizeof\(\w+\)\}',text):
  at=int(r,16);wanted=pins[name];ok=p.read(at,len(wanted))==wanted
  checks.append({'rva':hex(at),'name':name,'ok':ok,'actual':p.read(at,len(wanted)).hex(),'compiled':wanted.hex()})
  if not ok:errors.append(name)
 for v,off,fn in [(0x3A711E0,0xB8,0xAA8448),(0x3A711E0,0xE8,0xAB1124),(0x40602F8,0x28,0x1A7FB10),(0x4077048,0x70,0x4B4F98)]:
  actual=struct.unpack('<Q',p.read(v+off,8))[0]-p.base;ok=actual==fn
  checks.append({'vtable':hex(v),'slot':hex(off),'native':hex(actual),'ok':ok})
  if not ok:errors.append(hex(v+off))
 return {'sha256':sha,'timestamp':hex(p.stamp),'checks':checks,'passed':not errors,'errors':errors}
if __name__=='__main__':
 a=argparse.ArgumentParser();a.add_argument('dll',type=Path);a.add_argument('--source',type=Path,default=Path(__file__).resolve().parents[1]);a.add_argument('--out',type=Path);args=a.parse_args()
 result=verify(args.dll,args.source);content=json.dumps(result,indent=2);print(content)
 if args.out:args.out.parent.mkdir(parents=True,exist_ok=True);args.out.write_text(content+'\n',encoding='utf-8')
 raise SystemExit(not result['passed'])
