#!/usr/bin/env python3
"""Read-only exact retail schema proof for the current articulated owner join."""
from pathlib import Path
import hashlib,json,mmap,re,struct,sys
root=Path(__file__).resolve().parents[1]
if len(sys.argv)!=3:raise SystemExit('usage: verify_articulated_target_owner_h8.py WHGame.dll OUTPUT.json')
image=Path(sys.argv[1]);header=root/'src/diagnostics/articulated_target_owner_h8.h';text=header.read_text();expected='bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7'
arrays={n:bytes(int(v,16)for v in re.findall(r'0x([0-9A-Fa-f]+)',b))for n,b in re.findall(r'unsigned char (p\d+)\[\]\{([^}]+)\}',text)}
pins=re.findall(r'\{(0x[0-9A-Fa-f]+),(p\d+),sizeof \2\}',text);rows=[]
hash_=hashlib.sha256()
with image.open('rb')as f:
 for b in iter(lambda:f.read(4*1024*1024),b''):hash_.update(b)
 f.seek(0);m=mmap.mmap(f.fileno(),0,access=mmap.ACCESS_READ);pe=struct.unpack_from('<I',m,0x3c)[0];stamp=struct.unpack_from('<I',m,pe+8)[0];size=struct.unpack_from('<H',m,pe+20)[0];count=struct.unpack_from('<H',m,pe+6)[0];base=struct.unpack_from('<Q',m,pe+48)[0];sections=[]
 for i in range(count):
  _,va,raw,offset=struct.unpack_from('<IIII',m,pe+24+size+i*40+8);sections.append((va,raw,offset))
 def read(rva,size):
  for va,raw,offset in sections:
   if va<=rva and rva+size<=va+raw:return m[offset+rva-va:offset+rva-va+size]
  raise ValueError(hex(rva))
 for at,name in pins:
  expectedBytes=arrays[name];actual=read(int(at,16),len(expectedBytes));rows.append({'rva':at,'expected':expectedBytes.hex(),'actual':actual.hex(),'match':actual==expectedBytes})
 slots=[]
 for at,target in [(0x40602F8+0x38,0x1AA6BC0),(0x3A71028+0x30,0x1A77FD0)]:
  actual=struct.unpack('<Q',read(at,8))[0]-base;slots.append({'rva':hex(at),'target':hex(actual),'expected':hex(target),'match':actual==target})
 m.close()
result={'source_header':header.relative_to(root).as_posix(),'source_sha256':hashlib.sha256(header.read_bytes()).hexdigest(),'image':str(image),'image_sha256':hash_.hexdigest(),'image_timestamp':hex(stamp),'pins':rows,'vft_methods':slots,'pass':len(rows)==7 and hash_.hexdigest()==expected and stamp==0x6A350E20 and all(x['match']for x in rows+slots),'meaning':'Current GetCharacter(0) direct-before-slot and pose+698/getmain+108 read schema; no native call/hook/status/physics mutation. Backlink +210 semantic proof is separately retained in POSE_BACKLINK_EXACT_RETAIL_PROOF.json.'}
Path(sys.argv[2]).write_text(json.dumps(result,indent=2)+'\n');print('articulated_owner_exact_retail',result['pass'],len(rows),len(slots));raise SystemExit(0 if result['pass']else 1)
