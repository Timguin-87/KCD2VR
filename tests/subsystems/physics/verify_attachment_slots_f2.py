"""Pin the actual retail slot writer/name table; read WHGame.dll, never load it."""
from pathlib import Path
import argparse
import hashlib
import json
import struct
import sys
root=Path(__file__).resolve().parents[3]
sys.path.insert(0,str(root/'tools'))
from native_research112 import PE
ap=argparse.ArgumentParser(description=__doc__)
ap.add_argument('--dll',required=True,type=Path)
ap.add_argument('--report',required=True,type=Path)
a=ap.parse_args()
p=PE(a.dll)
digest=hashlib.sha256(p.data).hexdigest()
assert digest=='bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7'
pins={0x1085280:'488d34db44397cb72c',0x10852CB:'4c8d0db66f0203',
      0x10852D2:'4d8b0cd9',0x10852F2:'44897cb72c',0x1085F74:'40534883ec20488b4920'}
for rva,raw in pins.items():assert p.read(rva,len(bytes.fromhex(raw)))==bytes.fromhex(raw),hex(rva)
assert 0x10852D2+struct.unpack('<i',p.read(0x10852CE,4))[0]==0x40AC288
names=[]
for i,expected in enumerate(['slt_0','slt_1','entity_right','entity_left','hand_right','hand_left']):
    pointer=struct.unpack('<Q',p.read(0x40AC288+i*8,8))[0]
    name=p.read(pointer-p.base,len(expected)+1)
    assert name==expected.encode()+b'\0'
    names.append({'index':i,'model_entity_offset':hex(0x2C+i*0x24),'name_pointer_rva':hex(0x40AC288+i*8),'name':expected})
report={'result':'PASS','dll_sha256':digest,'native_file_executed':False,
        'instruction_pins':{hex(k):v for k,v in pins.items()},'native_slots':names,
        'old_mismatch':'model+2C is slot0; name40AC2A8 is slot4; native writer/lookup pair the same index',
        'scope':'retail read-only layout proof, not engine/collider/headset acceptance'}
a.report.parent.mkdir(parents=True,exist_ok=True)
a.report.write_text(json.dumps(report,indent=2)+'\n')
print('PASS:5 retail instruction pins and6 exact native attachment slot/name pairs')
