"""Merge native printed-map resources into the sole C31 companion.

Inputs are pinned; all old members stay byte-identical. No minimap/native map UI
or new combat table is installed. Localization contains only our two new keys.
"""
from pathlib import Path, PurePosixPath
import argparse, hashlib, io, json, os, re, tempfile, zipfile
import xml.etree.ElementTree as ET
ROOT=Path(__file__).resolve().parents[1]
BASE_SHA='ea14f16b851631a7ef8bc020666622d3deed3d58bf3e26674749e201e27ff7d9'
OVERLAY_SHA='08f53ded4bde01515ef085f80e742f0000e131c89f41fe3b692b42b81d193f7e'
MAP_MEMBERS={'Libs/Tables/item/item__kcdvr_map.xml','Scripts/Startup/zzkcd2vr_map.lua'} | {
 'Objects/kcd2vr/map32/'+r+'.'+ext for r in ('trosky','kuttenberg') for ext in ('cdf','dds','mtl')}
def sha(b):return hashlib.sha256(b).hexdigest()
def unpack(raw):
 with zipfile.ZipFile(io.BytesIO(raw)) as z:
  names=z.namelist()
  if len(names)!=len({n.lower() for n in names}) or z.testzip():raise ValueError('duplicate/corrupt archive')
  for n in names:
   p=PurePosixPath(n)
   if p.is_absolute() or '..' in p.parts or '\\' in n or ':' in n:raise ValueError('unsafe archive path')
  return {n:z.read(n) for n in names}
def packed(members):
 b=io.BytesIO()
 with zipfile.ZipFile(b,'w',zipfile.ZIP_DEFLATED,compresslevel=9) as z:
  for n,d in members.items():
   i=zipfile.ZipInfo(n,(2026,10,16,0,0,0));i.compress_type=zipfile.ZIP_DEFLATED;i.external_attr=0o100644<<16;z.writestr(i,d)
 return b.getvalue()
def atomic(path,data):
 path.parent.mkdir(parents=True,exist_ok=True)
 fd,tmp=tempfile.mkstemp(prefix='C32.',suffix='.tmp',dir=path.parent)
 try:
  with os.fdopen(fd,'wb') as f:f.write(data)
  os.replace(tmp,path)
 finally:
  if os.path.exists(tmp):os.unlink(tmp)
def build(base,overlay,out,locales):
 if out.resolve() in {base.resolve(),overlay.resolve()}:raise ValueError('output overwrites evidence input')
 b,o=base.read_bytes(),overlay.read_bytes()
 if sha(b)!=BASE_SHA or sha(o)!=OVERLAY_SHA:raise ValueError('pinned C31/map32 fingerprint mismatch')
 old,added=unpack(b),unpack(o)
 if len(old)!=18 or set(added)!=MAP_MEMBERS or {n.lower() for n in old}&{n.lower() for n in added}:raise ValueError('unexpected/competing members')
 if added['Scripts/Startup/zzkcd2vr_map.lua']!=(ROOT/'mods/KCD2VRMap/map_runtime.lua').read_bytes():raise ValueError('map Lua differs from current producer')
 table=ET.fromstring(added['Libs/Tables/item/item__kcdvr_map.xml'])
 rows=table.findall('ItemClasses/Document')
 if len(rows)!=1 or rows[0].get('Id')!='5f3c2a71-8e4d-4b6a-9c1e-2d7f0b9a4e63':raise ValueError('unqualified map item')
 loc=(ROOT/'mods/KCD2VRMap/text_ui__kcdvr_map.xml').read_bytes();keys=[]
 for row in ET.fromstring(loc).findall('Row'):
  cells=row.findall('Cell')
  if len(cells)!=3 or not all(c.text for c in cells):raise ValueError('missing localization cell')
  keys.append(cells[0].text)
 if keys!=['ui_nm_kcd2vr_map','ui_in_kcd2vr_map'] or b'blank' in loc.lower():raise ValueError('wrong map localization keys/text')
 languages=sorted(set(locales))
 if not languages or any(not re.fullmatch(r'[A-Za-z_]+_xml\.pak',n) for n in languages):raise ValueError('invalid native localization filename')
 result=packed(old|added);actual=unpack(result)
 if any(actual[n]!=d for n,d in (old|added).items()):raise ValueError('payload preservation failed')
 atomic(out,result)
 localized=packed({'text_ui__kcdvr_map.xml':loc})
 for n in languages:atomic(out.parent.parent/'Localization'/n,localized)
 manifest={'base_sha256':BASE_SHA,'map_overlay_sha256':OVERLAY_SHA,'pak_sha256':sha(result),
  'preserved_members':[{ 'name':n,'sha256':sha(d)} for n,d in old.items()],
  'added_members':[{ 'name':n,'sha256':sha(d)} for n,d in added.items()],
  'localization':{'languages':languages,'pak_sha256':sha(localized),'xml_sha256':sha(loc),'fallback_text':'English'},
  'combat_table_changes':False,'external_minimap':False,'native_map_markers':False,'installed':False,'headset_acceptance':False}
 atomic(out.with_suffix('.manifest.json'),(json.dumps(manifest,indent=2)+'\n').encode())
 return manifest
def main():
 p=argparse.ArgumentParser(description=__doc__)
 p.add_argument('--base',type=Path,default=ROOT/'companion/reference/C31/KCD2VR_KICK_D_fulltable_plus_masterstrikes.pak')
 p.add_argument('--overlay',type=Path,required=True);p.add_argument('--out',type=Path,default=ROOT/'companion/Mods/KCD2VR/Data/KCD2VR_KICK_D_fulltable_plus_masterstrikes.pak')
 p.add_argument('--localization',type=Path,required=True);a=p.parse_args()
 print(json.dumps(build(a.base,a.overlay,a.out,[f.name for f in a.localization.glob('*_xml.pak')]),indent=2))
if __name__=='__main__':main()
