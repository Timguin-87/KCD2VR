"""Offline exact-retail proof for event-only original material transfer; no DLL execution."""
import argparse,hashlib,json,re,struct
from pathlib import Path
from native_research112 import PE
p=argparse.ArgumentParser();p.add_argument('--dll',type=Path,required=True);p.add_argument('--out',type=Path,required=True);a=p.parse_args()
pe=PE(a.dll);sha=hashlib.sha256(pe.data).hexdigest()
assert sha=='bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7' and pe.stamp==0x6A350E20
root=Path(__file__).resolve().parents[1];source=(root/'src/subsystems/gore/native_material_d54.inl').read_text();rows=[]
for match in re.finditer(r'\{(0x[0-9A-Fa-f]+),(\d+),\{([^}]+)\}\}',source):
 rva=int(match[1],16);n=int(match[2]);expected=bytes(int(x.strip(),0) for x in match[3].split(','));actual=pe.read(rva,n)
 rows.append(dict(rva=hex(rva),expected=expected.hex(),actual=actual.hex(),match=expected==actual))
slots=source[source.index('static constexpr Slot slots[]'):source.index('// GetCharacter is already')]
for match in re.finditer(r'\{(0x[0-9A-Fa-f]+),(0x[0-9A-Fa-f]+),(0x[0-9A-Fa-f]+)\}',slots):
 vt,off,fn=(int(x,16) for x in match.groups());actual=struct.unpack('<Q',pe.read(vt+off,8))[0]-pe.base
 rows.append(dict(vtable=hex(vt),slot=hex(off),function=hex(fn),actual=hex(actual),match=actual==fn))
result={'retail_sha256':sha,'timestamp':hex(pe.stamp),'checks':rows,'passed':all(x['match'] for x in rows),'native_execution':False,
 'semantics':['CSKINAttachment replacement pointers: object+0x10+8*lod, six LODs; effective fallback CSkin+0x10 array, stride0xB0 material+8.','CMatInfo GetName=slot0x40 reads +0x28; name is only wrapper hint, not runtime tint proof.','SetReplacementMaterial5AF4EC -> smart-pointer assignment6BCF48: incoming AddRef(slot10), old Release(slot18).','Strong original/source refs held across transfer and rollback; writes only to token-named mod helper skin.','GetCharacter6411AC may contain own qualified hook; runtime accepts existing retained exact-entry witness.']}
a.out.parent.mkdir(parents=True,exist_ok=True);a.out.write_text(json.dumps(result,indent=2)+'\n');print(json.dumps({'passed':result['passed'],'checks':len(rows),'out':str(a.out)}));raise SystemExit(0 if result['passed'] else 1)
