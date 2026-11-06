from pathlib import Path
import sys,zipfile,json,hashlib
import argparse
a=argparse.ArgumentParser();a.add_argument('--parent',type=Path,required=True);a.add_argument('--localization',type=Path,required=True);a.add_argument('--game',type=Path,default=Path('C:/Steam/steamapps/common/KingdomComeDeliverance2'));a.add_argument('--out',type=Path,required=True);args=a.parse_args()
S=Path(__file__).resolve().parents[1];R=S/'src/subsystems/throw_drop/resources';sys.path[:0]=[str(R),str(S/'companion/optional_throwing_assets1')]
import companion_patch_t26 as t
from build_asset_pack import Native
p=args.parent
with zipfile.ZipFile(p) as z:members={i.filename:(i,z.read(i)) for i in z.infolist()}
n=Native(args.game);out,changed,removed,report=t.plan_members(members,n)
assert len(report['shop_presets'])==19,len(report['shop_presets']);assert not removed
root=t.parse(out[t.PRESETS][1]);rows={r.get('Name'):r for r in root.find('InventoryPresets')}
for name in report['shop_presets']:
 old=[t.prior.prior.canonical(c) for c in n.presets[name]]
 actual=[t.prior.prior.canonical(c) for c in rows[name] if not(c.tag=='InventoryPresetRef' and c.get('Name')==t.STOCK)]
 assert actual==old,name
assert not any(c.get('Name') in (t.AXE,'recipe_'+t.AXE) for c in rows[t.STOCK])
loc=args.localization
with zipfile.ZipFile(loc) as z:
 old=z.read('text_ui__kcd2vr_throwing_assets1.xml');new=t.plan_localization(old)
 a=t.parse(old);b=t.parse(new)
 for ra,rb in zip(a,b):
  if list(ra)[0].text=='ui_nm_kcd2vr_throwing_dagger1':assert all(c.text=='Throwing knife' for c in list(rb)[1:])
  else:assert t.prior.prior.canonical(ra)==t.prior.prior.canonical(rb)
report.update({'parent':str(p),'changed':sorted(changed),'removed':sorted(removed),'untouched':len(members)-len([m for m in changed if m in members]),'member_count':len(out),'shop_original_children_preserved':True,'localization_only_owned_name_cells':True,'archive_written':False})
args.out.parent.mkdir(parents=True,exist_ok=True);args.out.write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2))
