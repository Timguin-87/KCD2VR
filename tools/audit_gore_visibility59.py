"""Read-only exact-retail proof of Hide(true/false)'s three visibility bits."""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
import argparse,hashlib,json,re,struct
from pathlib import Path
EXPECTED='bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7'
def audit(dll):
 b=Path(dll).read_bytes();h=hashlib.sha256(b).hexdigest()
 if h!=EXPECTED:raise ValueError('Not the reviewed WHGame retail SHA256')
 pe=struct.unpack_from('<I',b,0x3c)[0]
 if struct.unpack_from('<I',b,pe+8)[0]!=0x6A350E20:raise ValueError('Retail timestamp differs')
 count=struct.unpack_from('<H',b,pe+6)[0];optional=struct.unpack_from('<H',b,pe+20)[0]
 def at(rva,n):
  for i in range(count):
   p=pe+24+optional+i*40;size,va,rawsize,raw=struct.unpack_from('<IIII',b,p+8)
   if va<=rva and rva+n<=va+rawsize:return b[raw+rva-va:raw+rva-va+n]
  raise ValueError('RVA is outside file-backed section')
 source=read_source(Path(__file__).resolve().parents[1]/'src/subsystems/gore/native_gore.inl')
 records=[]
 for rva,offset in [(0x5AF5E8,8),(0x5ADA68,16)]:
  expected=bytes.fromhex('448b41')+bytes([offset])+bytes.fromhex('418bc025fffff8ff4181c80000070085d2440f44c0448941')+bytes([offset])+bytes.fromhex('c3')
  actual=at(rva,len(expected))
  if actual!=expected:raise ValueError('Native visibility function bytes differ')
  for chunkRva,chunk in [(rva,at(rva,16)),(rva+16,at(rva+16,16))]:
   match=re.search(r'\{0x%X,\{([^}]+)\}\}'%chunkRva,source,re.I)
   declared=bytes(int(x.strip(),16) for x in match[1].split(',')) if match else b''
   if declared!=chunk:raise ValueError('Runtime pin does not cover audited branch')
  records.append({'rva':hex(rva),'flags_offset':offset,'size':len(actual),'bytes':actual.hex(),
   'true':'flags | 0x70000','false':'flags & 0xFFF8FFFF','abi':'existing Hide(this,unsigned), not a new virtual slot'})
 return {'dll':str(Path(dll).resolve()),'sha256':h,'timestamp':'0x6A350E20','functions':records,
   'restore_requires':'all original0x70000 bits clear; same current entity/character/attachment/name; unchanged owned flags',
   'corpse_body_mapping':'NOT_USED','runtime_pins':'4/4 PASS'}
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--dll',required=True);p.add_argument('--output')
 a=p.parse_args();r=audit(a.dll);text=json.dumps(r,indent=2)
 if a.output:Path(a.output).write_text(text+'\n',encoding='utf-8')
 print(text)
