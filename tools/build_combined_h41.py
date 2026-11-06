"""Build reviewed H41 overlays on immutable H40; no game installation."""
from pathlib import Path
import sys,json,hashlib,zipfile,copy,argparse
ROOT=Path(__file__).resolve().parents[1]
def sha(b):return hashlib.sha256(b).hexdigest()
def main():
 ap=argparse.ArgumentParser();ap.add_argument('--parent',type=Path,required=True);ap.add_argument('--receipt',type=Path,required=True);a=ap.parse_args()
 sys.path.insert(0,str(ROOT/'src/subsystems/throw_drop/resources'))
 import companion_patch_t26 as t
 native=t.prior.prior.Native(Path('C:/Steam/steamapps/common/KingdomComeDeliverance2'))
 original=t.prior.base.read_pak(a.parent.read_bytes())
 after,changed,removed,detail=t.plan_members(original,native)
 owned={'Scripts/Startup/zzkcd2vr_phys.lua':'mods/kcd2vr_tests/zzkcd2vr_phys.lua','Scripts/Startup/zzkcd2vr_hand_only_doors_h40.lua':'mods/KCD2VRDoors/hand_only_h40.lua','Scripts/Startup/zzkcd2vr_grip_stow_cb1.lua':'mods/KCD2VRStow/zzkcd2vr_grip_stow_cb1.lua'}
 for name,source in owned.items():
  info=copy.copy(after[name][0]) if name in after else zipfile.ZipInfo(name,(2026,10,23,0,0,0))
  info.compress_type=zipfile.ZIP_DEFLATED;after[name]=(info,(ROOT/source).read_bytes())
 out=ROOT/'companion/C63_UNIFIED/Mods/KCD2VR';pak=out/'Data/KCD2VR.pak'
 def writezip(path,entries):
  with zipfile.ZipFile(path,'w')as z:
   for n,(i,b)in entries.items():z.writestr(i,b)
  with zipfile.ZipFile(path)as z:
   assert z.testzip()is None
   assert all(z.read(n)==b for n,(i,b)in entries.items())
 writezip(pak,after)
 loc=[]
 for path in sorted((out/'Localization').glob('*.pak')):
  with zipfile.ZipFile(path)as z:entries={i.filename:(i,z.read(i))for i in z.infolist()}
  target='text_ui__kcd2vr_throwing_assets1.xml';i,b=entries[target];entries[target]=(i,t.plan_localization(b))
  preserved={n:sha(b)for n,(i,b)in entries.items()if n!=target};writezip(path,entries)
  loc.append({'path':str(path.relative_to(ROOT)),'sha256':sha(path.read_bytes()),'changed_member':target,'preserved_members':preserved})
 # Canonical source pairs must describe the actual selected runtime, never old loaders.
 pairs={'src/subsystems/throw_drop/resources/startup_supply_t22.lua':t.prior.STARTUP_MEMBER,'src/subsystems/throw_drop/resources/startup_reservation_t22.lua':t.prior.base.SUPPLY_MEMBER,'src/subsystems/throw_drop/resources/inventory_delivery_t26.lua':t.STARTUP,'src/subsystems/throw_drop/resources/perk__kcd2vr_throwing_assets1.xml':t.PERKS,'src/subsystems/throw_drop/resources/perk_script__kcd2vr_throwing_assets1.xml':t.SCRIPTS}
 for source,member in pairs.items():
  path=ROOT/source
  if source.endswith('.xml'):path.write_bytes(after[member][1])
  else:assert path.read_bytes()==after[member][1],source
 meta=json.loads((out/'KCD2VR_UNIFIED63.json').read_text())
 meta.update(candidate='H41_CLAUDE_T26_REVIEWED_INTEGRATION',pak_sha256=sha(pak.read_bytes()),members=len(after),installed=False)
 meta['members_provenance']=[{'path':n,'sha256':sha(b),'source':'H41 reviewed composition'}for n,(i,b)in after.items()]
 meta['distribution_files']={str(p.relative_to(out)).replace('\\','/'):sha(p.read_bytes())for p in out.rglob('*')if p.is_file()and p.name not in ('KCD2VR.pak','KCD2VR_UNIFIED63.json')}
 (out/'KCD2VR_UNIFIED63.json').write_text(json.dumps(meta,indent=2)+'\n')
 changed=sorted(n for n in original.keys()&after.keys()if original[n][1]!=after[n][1]);added=sorted(after.keys()-original.keys());removed=sorted(original.keys()-after.keys())
 receipt={'parent_sha256':sha(a.parent.read_bytes()),'pak_sha256':sha(pak.read_bytes()),'members':len(after),'changed':changed,'added':added,'removed':removed,'unchanged':len(original)-len(changed)-len(removed),'T26':detail,'localizations':loc,'runtime_gore_added':False,'console_probes_added':['bounded one-shot physics only; no hand-gameplay claim'],'headset':'NOT_RUN'}
 a.receipt.parent.mkdir(parents=True,exist_ok=True);a.receipt.write_text(json.dumps(receipt,indent=2)+'\n');print(json.dumps(receipt,indent=2))
if __name__=='__main__':main()
