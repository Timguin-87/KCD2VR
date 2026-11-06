#!/usr/bin/env python3
"""D85 UPSCALERS: check the XeSS binding's native pins against a WHGame.dll.

Parses the literal pin table, export names and IAT slots from
src/native_xess_stereo_d85.cpp and compares them with the supplied image:
PE timestamp/size, every pinned byte run, and that each IAT slot is the
libxess.dll import of the same name. Also checks that every libxess import is
called from exactly one call [rip+disp32] site, the pinned one.
"""
from pathlib import Path
import argparse,hashlib,json,re,struct,sys
p=argparse.ArgumentParser();p.add_argument('--dll',type=Path,required=True);p.add_argument('--out',type=Path);a=p.parse_args()
root=Path(__file__).resolve().parents[1];src=(root/'src/native_xess_stereo_d85.cpp').read_text()
d=a.dll.read_bytes();base=0x180000000
e=struct.unpack_from('<I',d,0x3c)[0];nsec=struct.unpack_from('<H',d,e+6)[0];opt=struct.unpack_from('<H',d,e+20)[0]
stamp=struct.unpack_from('<I',d,e+8)[0];size=struct.unpack_from('<I',d,e+24+56)[0]
secs=[]
for i in range(nsec):
 o=e+24+opt+40*i;secs.append((d[o:o+8].rstrip(b'\0'),*struct.unpack_from('<IIII',d,o+8)))
def off(rva):
 for n,vs,va,rs,ro in secs:
  if va<=rva<va+rs:return ro+rva-va
 raise KeyError(hex(rva))
checks=[];ok=True
def check(name,cond):
 global ok;checks.append({'check':name,'pass':bool(cond)});ok&=bool(cond)
check('timestamp 0x6A350E20',stamp==0x6A350E20);check('SizeOfImage 0x5B2D000',size==0x5B2D000)
pins=re.findall(r'\{0x([0-9A-Fa-f]+),\{((?:0x[0-9a-f]{2},?)+)\}\}',src)
check('seven pins parsed',len(pins)==7)
for rva,bs in pins:
 want=bytes(int(x,16) for x in bs.split(','));have=d[off(int(rva,16)):off(int(rva,16))+len(want)]
 check(f'pin {rva} bytes',have==want)
# import table: libxess thunk names
imp_rva=struct.unpack_from('<I',d,e+24+120)[0];o=off(imp_rva);names={}
while True:
 lookup,_,_,name,first=struct.unpack_from('<IIIII',d,o)
 if not lookup and not first:break
 dll=d[off(name):d.index(b'\0',off(name))].decode()
 if dll.lower()=='libxess.dll':
  k=0
  while True:
   ent=struct.unpack_from('<Q',d,off(lookup)+8*k)[0]
   if not ent:break
   nm=d[off(ent+2):d.index(b'\0',off(ent+2))].decode();names[first+8*k]=nm;k+=1
 o+=20
exports=re.search(r'kExports\[5\]\{([^}]*)\}',src).group(1);exports=re.findall(r'"([^"]+)"',exports)
iat=[int(x,16) for x in re.findall(r'0x([0-9A-F]+)',re.search(r'kIat\[5\]\{([^}]*)\}',src).group(1))]
check('five exports and IAT slots',len(exports)==5 and len(iat)==5)
for name,slot in zip(exports,iat):check(f'IAT {hex(slot)} is libxess!{name}',names.get(slot)==name)
# single call site per import (call qword ptr [rip+disp32] = FF 15)
text=[s for s in secs if s[0]==b'.text'][0];t=d[text[4]:text[4]+text[3]];sites={}
i=t.find(b'\xff\x15')
while i!=-1:
 tgt=text[2]+i+6+struct.unpack_from('<i',t,i+2)[0]
 if tgt in names:sites.setdefault(tgt,[]).append(text[2]+i)
 i=t.find(b'\xff\x15',i+1)
pinned={int(r,16) for r,_ in pins}
for slot in iat:check(f'{names.get(slot)} single pinned call site',len(sites.get(slot,[]))==1 and sites[slot][0] in pinned)
res={'dll_sha256':hashlib.sha256(d).hexdigest(),'passed':ok,'checks':checks,'call_sites':{names[k]:[hex(base+x) for x in v] for k,v in sites.items()}}
if a.out:a.out.write_text(json.dumps(res,indent=2)+'\n')
print(json.dumps({'passed':ok,'failed':[c['check'] for c in checks if not c['pass']],'checks':len(checks),'call_sites':res['call_sites']},indent=1))
sys.exit(0 if ok else 1)
