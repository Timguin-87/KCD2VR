"""Map-only overlay: direct retail paper playback and persistent material UI."""
from pathlib import Path
import argparse,json
import build_companion32 as archive
import build_map_navigation_t2 as renderer
ROOT=Path(__file__).resolve().parents[1]
def plan(before):
 after=dict(before)
 for n in ['Libs/UI/ApseInventoryList.gfx','Libs/UI/UIElements/ApseInventoryList.xml']:
  if n in before:raise ValueError('Inventory adapter present; reconcile instead of silently carrying M30')
 replacements={path:(ROOT/'mods/KCD2VRMap'/source).read_bytes() for path,source in [('Scripts/Startup/zzkcd2vr_map.lua','map_runtime.lua'),('Scripts/KCD2VR/map_navigation_t2.lua','map_navigation_t2.lua'),('Scripts/KCD2VR/map_presentation_m31.lua','map_presentation_m31.lua')]}
 replacements['Libs/UI/KCD2VRMapT2.gfx']=renderer.gfx()
 for path,data in replacements.items():
  if any(n.casefold()==path.casefold() and n!=path for n in before):raise ValueError('owned map path alias')
  after[path]=data
 changed=sorted(n for n,v in after.items() if before.get(n)!=v)
 assert set(changed)<=set(replacements)
 assert all(after[n]==v for n,v in before.items() if n not in replacements)
 return after,changed

def build(base,output,report):
 if base.resolve()==output.resolve():raise ValueError('frozen input required')
 raw=base.read_bytes();before=archive.unpack(raw);after,changed=plan(before)
 archive.atomic(output,archive.packed(after))
 result={'base_sha256':archive.sha(raw),'pak_sha256':archive.sha(output.read_bytes()),'changed':changed,'preserved_members':sum(after[n]==v for n,v in before.items()),'members':len(after),'inventory_or_menu_asset_changes':False,'native_chr_caf_mtl_changes':False,'native_game_or_headset':'NOT_RUN'}
 archive.atomic(report,(json.dumps(result,indent=2)+'\n').encode());return result
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__)
 for n in ['base','output','report']:p.add_argument('--'+n,type=Path,required=True)
 a=p.parse_args();print(json.dumps(build(a.base,a.output,a.report),indent=2))
