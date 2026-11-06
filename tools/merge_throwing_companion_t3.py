"""Merge only the frozen T3 map modules and supply into exact C58R1 main.
Requires an explicit immutable parent PAK; never writes any game directory.
All66 unrelated parent payloads remain byte-identical. Output is69 members.
"""
from pathlib import Path
import argparse,copy,hashlib,json,tempfile,zipfile
ROOT=Path(__file__).resolve().parents[1]
BASE_SHA='77c40150b40ea19393694da6bf7e2ce07bb519dbb629bff690cf78c1e3016bfb'
def sha(b):return hashlib.sha256(b).hexdigest()
def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--base',required=True,type=Path)
 p.add_argument('--out',type=Path,default=ROOT/'companion/Mods/KCD2VR/Data/KCD2VR_KICK_D_fulltable_plus_masterstrikes.pak')
 args=p.parse_args();base=args.base.resolve();target=args.out.resolve()
 if base==target:raise ValueError('base and output must be distinct')
 # A source/artifact build cannot be redirected at the installed game.
 if ROOT not in target.parents:raise ValueError('output must stay in candidate source')
 raw=base.read_bytes()
 if sha(raw)!=BASE_SHA:raise ValueError('not the exact immutable C58R1 parent PAK')
 with zipfile.ZipFile(base) as z:
  assert z.testzip() is None
  infos=z.infolist();names=z.namelist();members={n:z.read(n) for n in names}
 assert len(names)==len(set(names))==68
 overlay={
  'Scripts/Startup/zzkcd2vr_map.lua':(ROOT/'mods/KCD2VRMap/map_runtime.lua').read_bytes(),
  'Scripts/KCD2VR/map_navigation_t2.lua':(ROOT/'mods/KCD2VRMap/map_navigation_t2.lua').read_bytes(),
  'Scripts/KCD2VR/native_supply_t2.lua':(ROOT/'src/vrik/native_supply_t2.lua').read_bytes()}
 assert b'Scripts/KCD2VR/native_supply_t2.lua' in overlay['Scripts/Startup/zzkcd2vr_map.lua'],'startup has no protected supply load'
 assert b'kcd2vr_supply_native_t2' in overlay['Scripts/KCD2VR/native_supply_t2.lua']
 added=set(overlay)-set(members);replaced=set(overlay)&set(members)
 assert added=={'Scripts/KCD2VR/native_supply_t2.lua'} and len(replaced)==2
 expected={**members,**overlay};target.parent.mkdir(parents=True,exist_ok=True)
 with tempfile.NamedTemporaryFile(dir=target.parent,suffix='.pak.tmp',delete=False) as temp:tempPath=Path(temp.name)
 try:
  with zipfile.ZipFile(tempPath,'w') as z:
   for info in infos:z.writestr(copy.copy(info),expected[info.filename])
   for name in sorted(added):
    info=zipfile.ZipInfo(name,(2026,10,18,0,0,0));info.compress_type=zipfile.ZIP_DEFLATED
    z.writestr(info,expected[name],compresslevel=9)
  with zipfile.ZipFile(tempPath) as z:
   assert z.testzip() is None and len(z.namelist())==69 and set(z.namelist())==set(expected)
   for name,data in expected.items():assert z.read(name)==data,name
  target.write_bytes(tempPath.read_bytes())
 finally:tempPath.unlink(missing_ok=True)
 receipt={'parent_sha256':BASE_SHA,'pak_sha256':sha(target.read_bytes()),'members':69,
  'preserved_members':[{'member':n,'sha256':sha(members[n])} for n in sorted(set(members)-replaced)],
  'replaced_members':{n:{'before':sha(members[n]),'after':sha(overlay[n])} for n in sorted(replaced)},
  'added_members':[{'member':n,'sha256':sha(overlay[n])} for n in sorted(added)],
  'native_headset_acceptance':'NOT_RUN','installed':False}
 assert len(receipt['preserved_members'])==66
 out=ROOT/'review/T3/THROWING_PAK_PRESERVATION.json';out.parent.mkdir(parents=True,exist_ok=True);out.write_text(json.dumps(receipt,indent=2)+'\n')
 manifestPath=target.with_suffix('.manifest.json')
 manifest=json.loads(manifestPath.read_text()) if manifestPath.exists() else {}
 manifest.update(pak_sha256=receipt['pak_sha256'],members=69,installed=False)
 manifest['t3_map_supply']=receipt
 manifestPath.write_text(json.dumps(manifest,indent=2)+'\n')
 print(json.dumps({'pak':str(target),'sha256':receipt['pak_sha256'],'members':69,'preserved':66,'replaced':sorted(replaced),'added':sorted(added)},indent=2))
if __name__=='__main__':main()
