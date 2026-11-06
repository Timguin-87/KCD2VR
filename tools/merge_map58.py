"""Offline map-only merge: exact C57R1 parent, reviewed T2 assets, current own Lua."""
from pathlib import Path
import argparse,json
import build_companion32 as archive
import build_map_navigation_t2 as renderer
ROOT=Path(__file__).resolve().parents[1]
BASE_SHA='fce649e55405972ad395981127d24bba625e8a05b3e7a6abdaab9fe3d22e24d6'
REPLACED={'Scripts/Startup/zzkcd2vr_map.lua','Objects/kcd2vr/map32/trosky.mtl','Objects/kcd2vr/map32/kuttenberg.mtl'}
ADDED={renderer.PREFIX+r+'_'+str(i)+'.dds' for r,n in [('trosky',9),('kuttenberg',30)] for i in range(1,n+1)}|{'Libs/UI/KCD2VRMapT2.gfx','Libs/UI/UIElements/KCD2VRMapT2.xml','Scripts/KCD2VR/map_navigation_t2.lua'}
def build(base,donor,output,report):
 base,donor,output,report=map(Path,(base,donor,output,report))
 game=Path('C:/Steam/steamapps/common/KingdomComeDeliverance2').resolve()
 if any(p.resolve().is_relative_to(game) for p in (output,report)):raise ValueError('offline source only')
 oldraw=base.read_bytes();donorraw=donor.read_bytes()
 if archive.sha(oldraw)!=BASE_SHA:raise ValueError('not exact C57R1 companion parent')
 old,source=archive.unpack(oldraw),archive.unpack(donorraw)
 if len(old)!=26 or not REPLACED<=old.keys() or set(source)-set(old)!=ADDED|{'Scripts/KCD2VR/native_supply_t2.lua'}:raise ValueError('unexpected map member set')
 if any(old[k]!=source[k] for k in old if k not in REPLACED):raise ValueError('donor changes unrelated payload')
 if source['Libs/UI/KCD2VRMapT2.gfx']!=renderer.gfx() or source['Libs/UI/UIElements/KCD2VRMapT2.xml']!=renderer.ui_xml():raise ValueError('renderer provenance mismatch')
 merged=dict(old);merged.update({k:source[k] for k in REPLACED|ADDED})
 for member,name in [('Scripts/Startup/zzkcd2vr_map.lua','map_runtime.lua'),('Scripts/KCD2VR/map_navigation_t2.lua','map_navigation_t2.lua')]:merged[member]=(ROOT/'mods/KCD2VRMap'/name).read_bytes()
 if b'native_supply' in merged['Scripts/Startup/zzkcd2vr_map.lua'] or len(merged)!=68:raise ValueError('unexpected stock dependency')
 raw=archive.packed(merged)
 if archive.unpack(raw)!=merged:raise ValueError('archive roundtrip')
 detail=dict(parent_sha256=BASE_SHA,donor_sha256=archive.sha(donorraw),pak_sha256=archive.sha(raw),members=68,replaced=sorted(REPLACED),added=sorted(ADDED),preserved=[dict(member=k,sha256=archive.sha(v))for k,v in old.items()if k not in REPLACED],installed=False,headset_test=False)
 manifest=json.loads(base.with_suffix('.manifest.json').read_text());manifest.update(pak_sha256=detail['pak_sha256'],members=68,map_only_c58r1=detail)
 archive.atomic(output,raw);archive.atomic(output.with_suffix('.manifest.json'),(json.dumps(manifest,indent=2)+'\n').encode());archive.atomic(report,(json.dumps(detail,indent=2)+'\n').encode());return detail
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__)
 for name in ['base','donor','output','report']:p.add_argument('--'+name,type=Path,required=True)
 a=p.parse_args();print(json.dumps(build(a.base,a.donor,a.output,a.report),indent=2))
