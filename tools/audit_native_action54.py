"""Exact-retail static evidence for the separate C54 native-cost proposal.

This validates byte/call sites. It neither executes nor modifies game code.
"""
import argparse,hashlib,json,struct
from pathlib import Path
from native_research112 import PE
ap=argparse.ArgumentParser();ap.add_argument('--dll',required=True,type=Path);ap.add_argument('--out',required=True,type=Path);a=ap.parse_args()
p=PE(a.dll);digest=hashlib.sha256(p.data).hexdigest()
assert p.stamp==0x6A350E20 and digest=='bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7'
calls={0x915931:0x91571C,0x9158DB:0x91590C,0xD4AA93:0xD4B06C,0xD4B093:0x939310,0xD4B0CD:0x90C850}
for site,target in calls.items():
    code=p.read(site,5);assert code[0]==0xE8 and site+5+struct.unpack_from('<i',code,1)[0]==target
assert list(p.calls(0x91571C))==[0x915931]
assert list(p.calls(0x91590C))==[0x9158DB,0x2755291]
assert p.read(0x91590C,16)==bytes.fromhex('48895c2408574883ec20488bd9488bfa')
assert p.read(0x91576A,7)==bytes.fromhex('488d05e7c41403') # constructor's final MainAttack vtable3A61C58
out={'dllSha256':digest,'timestamp':'0x6A350E20','ordinaryVft':'0x3A61C58',
     'sites':{f'0x{s:X}':f'0x{t:X}' for s,t in calls.items()},
     'constructorDirectCallers':['0x915931'],'factoryDirectCallers':['0x9158DB','0x2755291'],
     'dispatchReturn':'0xD4B0D2',
     'interpretation':'Actual owned-release action and factory generation are independent of the latest-intent clock match. This static evidence does not prove runtime preparation cost coverage or headset acceptance.'}
a.out.parent.mkdir(parents=True,exist_ok=True);a.out.write_text(json.dumps(out,indent=2)+'\n');print(json.dumps(out,indent=2))
