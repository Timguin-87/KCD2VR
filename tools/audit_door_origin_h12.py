#!/usr/bin/env python3
"""Read-only exact-retail proof for the H12 original-once point-input seam."""
from pathlib import Path
import argparse,hashlib,json,re,struct
p=argparse.ArgumentParser(description=__doc__)
p.add_argument('--native',type=Path,default=Path('C:/Steam/steamapps/common/KingdomComeDeliverance2/Bin/Win64MasterMasterSteamPGO/WHGame.dll'))
p.add_argument('--out',type=Path,required=True)
a=p.parse_args();root=Path(__file__).resolve().parents[1]
raw=a.native.read_bytes();pe=struct.unpack_from('<I',raw,0x3C)[0]
count=struct.unpack_from('<H',raw,pe+6)[0];timestamp=struct.unpack_from('<I',raw,pe+8)[0];optional=pe+24
optional_size=struct.unpack_from('<H',raw,pe+20)[0];sections=[]
for n in range(count):
 h=optional+optional_size+n*40;_,va,size,offset=struct.unpack_from('<IIII',raw,h+8);sections.append((va,va+size,offset))
def read(rva,size):
 for lo,hi,off in sections:
  if lo<=rva and rva+size<=hi:return raw[off+rva-lo:off+rva-lo+size]
 raise ValueError(hex(rva))
source=(root/'src/interaction_door_origin_h12.inl').read_bytes();text=source.decode();checks=[]
expected='bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7'
checks.append({'name':'exact_retail_sha256','passed':hashlib.sha256(raw).hexdigest()==expected})
checks.append({'name':'exact_retail_timestamp','passed':timestamp==0x6A350E20})
for name,rva in [('call',0x640117),('leaf',0xFBB860),('contains',0x4033AC),('inverse',0x63FFDD)]:
 t=re.search(r'unsigned char '+name+r'\[\]\{([^}]+)\}',text)[1]
 want=bytes(int(v,16) for v in re.findall(r'0x([0-9A-Fa-f]{2})',t));actual=read(rva,len(want))
 checks.append({'name':name,'rva':hex(rva),'bytes':len(want),'expected':want.hex(),'actual':actual.hex(),'passed':actual==want})
for site,target in [(0x640117,0xFBB860),(0xFBB86A,0x4033AC)]:
 b=read(site,5);resolved=site+5+struct.unpack_from('<i',b,1)[0]
 checks.append({'name':'exact_relative_call_'+hex(site),'passed':b[0]==0xE8 and resolved==target,'resolved':hex(resolved)})
report={'source_sha256':hashlib.sha256(source).hexdigest(),'native_sha256':hashlib.sha256(raw).hexdigest(),'checks':checks,'native_execution':False,
 'scope':'MID640117 copied source + FBB860 original-once point replacement; native AL/distance, bounds, object-policy60, ranking and Use preserved',
 'limitations':['Native body-forward broadphase unchanged','Palm outside exact AABB follows original view-facing fallback','Native Lua curve/hinge/readback still requires runtime acceptance']}
a.out.parent.mkdir(parents=True,exist_ok=True);a.out.write_text(json.dumps(report,indent=2)+'\n')
print('door_origin_h12 retail/source pins: %d/%d PASS'%(sum(c['passed'] for c in checks),len(checks)))
raise SystemExit(0 if all(c['passed'] for c in checks) else 1)
