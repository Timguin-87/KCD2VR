"""Exact retail queued attacker-position command; read-only, no native execution."""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
import argparse,hashlib,json,re,struct
from pathlib import Path
from native_research112 import PE
p=argparse.ArgumentParser(description=__doc__);p.add_argument('--dll',type=Path,required=True);p.add_argument('--out',type=Path,required=True);a=p.parse_args()
pe=PE(a.dll);sha=hashlib.sha256(pe.data).hexdigest()
assert sha=='bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7' and pe.stamp==0x6A350E20
source=read_source(Path(__file__).resolve().parents[1]/'src/vrik/native_motion112.inl');pins=[]
for name,rva in [('position36',0x14095E8),('positionCall36',0xA70FA3),('selfMover36',0xA71B60)]:
 v=re.search(r'unsigned char '+name+r'\[\d+\]\{([^}]+)\}',source)[1]
 data=bytes(int(x,0)for x in v.split(','));assert pe.read(rva,len(data))==data,name
 pins.append({'name':name,'rva':hex(rva),'bytes':data.hex()})
for rva,hexbytes in [
 (0xA70F34,'488d7330488b3e488b6e08'), # command vector begin/end
 (0xA70F44,'0fb60f85c9746c83e901745a83e9017448'), # byte2 ->14095E8 branch
 (0xA70FC2,'4883c730483bfd'), # queue stride0x30
 (0x1409665,'488bd7488bcbe8d87a66ff'), # ordinary fallback A71148
 (0x140972D,'e83a8266ff'), # alternate ->A7196C
 (0xA71262,'e805070000'), # ordinary ->A7196C
 (0xA71980,'33d2498bf9498be8488bf1e8d0010000'), # target0 resolves manager+8 SELF
 (0xA71998,'488b8850020000e8201af7ff'), # actor's +250 manager getter
 (0xA719A7,'488b084c8b4130488bc841ffd0'), # mover Start before late query
 (0xA719DC,'488b8b50020000e8dc19f7ff488b08488b5138488bc8ffd2') # Commit
]:
 data=bytes.fromhex(hexbytes);assert pe.read(rva,len(data))==data,hex(rva)
 pins.append({'rva':hex(rva),'bytes':data.hex()})
assert 0xA70FA8+struct.unpack('<i',pe.read(0xA70FA4,4))[0]==0x14095E8
assert pe.read(0xA70FA8,2)==bytes.fromhex('eb18') # caller ignores returned RAX
a.out.parent.mkdir(parents=True,exist_ok=True)
a.out.write_text(json.dumps({'sha256':sha,'timestamp':'0x6A350E20','pins':pins,'caller':'A70FA8','command_type':2,'queue_stride':'0x30','native_execution':False,'headset':False,'limits':'Proven queued self-position route only; not a global root-motion or procedural-factory patch.'},indent=2)+'\n')
print(f'PASS {len(pins)} queued position pins, exact fingerprint, caller/queue/self mover lineage; no native execution')
