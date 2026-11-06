"""Read-only exact retail collision ABI/pin verification. No injection."""
import argparse,hashlib,json,re,struct
from pathlib import Path
from native_research112 import PE
parser=argparse.ArgumentParser();parser.add_argument('--whgame',required=True);parser.add_argument('--out',type=Path,required=True);args=parser.parse_args()
p=PE(args.whgame);sha=hashlib.sha256(p.data).hexdigest();assert sha=='bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7' and p.stamp==0x6A350E20
root=Path(__file__).resolve().parents[1];header=root/'src/subsystems/physics/runtime/native_weapon_contact_event_h24.h';s=header.read_text();checks=[]
for match in re.finditer(r'\{0x([0-9A-Fa-f]+),\{([^}]+)\},(\d+)\}',s):
 rva=int(match[1],16);n=int(match[3]);expected=bytes(int(x.strip(),0) for x in match[2].split(','));actual=p.read(rva,n);assert actual==expected[:n],hex(rva);checks.append({'rva':hex(rva),'bytes':actual.hex()})
assert len(checks)==7
entry=bytes.fromhex('48895c240848896c2418565741564883ec30');assert p.read(0xBA7168,len(entry))==entry and p.function(0xBA7168)==(0xBA7168,0xBA7286)
for slot,fn in [(0x210,0xE1BF10),(0x218,0xE1C028),(0xA8,0x52950C)]:assert struct.unpack('<Q',p.read(0x407FC80+slot,8))[0]-p.base==fn
assert p.read(0xE1D712,7).hex()=='4c8d05a3c21000'
args.out.parent.mkdir(parents=True,exist_ok=True);args.out.write_text(json.dumps({'sha256':sha,'timestamp':hex(p.stamp),'status':'STATIC_ABI_AND_PINS_VERIFIED_NO_RETAIL_EXECUTION','producer_pdata':['0xBA7168','0xBA7286'],'abi':'int __fastcall(world,flags,EventPhysCollision*,optionalQueuedOut); all four registers used by native; no call from mod','event_size':'0xA8','event_id':2,'log_collision_flag':'0x02000000','pins':checks,'physical_id':'world407FC80+A8 ->52950C; producer writes event+38 from pEntity1 ID; never other0ID whenownside1','body_fields':{'pEntity':'0x10/0x18','foreign':'0x20/0x28','foreignType':'0x30/0x34 signed native int16extended','point':'0x3C','normal':'0x48','velocity':'0x54/0x60','mass':'0x6C/0x70','part':'0x74/0x78','material':'0x7C/0x7E','penetration':'0x84','normalImpulse':'0x88','radius':'0x8C'},'causality':'Producer stamps actual occurrence; logged callback diagnostic only. Unique per-incarnation cookie must remain nonreused. Consumer must join exact current native target/part/gesture before RPG consequences.'},indent=2))
print('PASS exact retail collision producer ABI/7pins/worldslots/fingerprint')
