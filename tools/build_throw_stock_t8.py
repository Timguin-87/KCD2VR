"""Build a T8 companion overlay from reviewed H6 content; never install it."""
from pathlib import Path
import argparse, copy, importlib.util, io, json, zipfile
ROOT=Path(__file__).resolve().parents[1]
SPEC=importlib.util.spec_from_file_location("t6",ROOT/"src/subsystems/throw_drop/resources/companion_patch_t6.py")
t6=importlib.util.module_from_spec(SPEC);SPEC.loader.exec_module(t6)
REPLACEMENTS={
 "Scripts/Startup/zzkcd2vr_throwing_assets1.lua":"inventory_delivery_t6.lua",
 "Libs/Tables/rpg/perk__kcd2vr_throwing_assets1.xml":"perk__kcd2vr_throwing_assets1.xml",
 "Libs/Tables/rpg/perk_script__kcd2vr_throwing_assets1.xml":"perk_script__kcd2vr_throwing_assets1.xml"}
def build(base,out):
 base=base.resolve();out=out.resolve()
 t6.plain_path(base);t6.plain_path(out)
 t6.require(not out.exists() and not out.is_relative_to(base) and not base.is_relative_to(out),"new separate output required")
 t6.require("steamapps" not in [x.casefold() for x in out.parts],"game output refused")
 files=t6.read_tree(base);meta=json.loads(files[t6.MARKER]);old=t6.read_pak(files[t6.PAK])
 t6.require(meta["pak_sha256"]==t6.sha(files[t6.PAK]),"base PAK receipt mismatch")
 t6.require(meta["pak_sha256"]=="48c8a565aee01c61ac6ec8aad28c8cae62b16d30024526e8b5ff2a53ac2f388d","requires exact frozen H6/M4 companion")
 t6.validate_definitions(old,files)
 replacement={n:(ROOT/"src/subsystems/throw_drop/resources"/s).read_bytes() for n,s in REPLACEMENTS.items()}
 stream=io.BytesIO()
 with zipfile.ZipFile(stream,"w") as z:
  for n,(info,data) in old.items():z.writestr(copy.copy(info),replacement.get(n,data))
 new=stream.getvalue();after=t6.read_pak(new)
 changed=[n for n in old if old[n][1]!=after[n][1]]
 t6.require(set(changed)==set(REPLACEMENTS),"unexpected patch footprint")
 t6.validate_definitions(after,files)
 files[t6.PAK]=new
 meta["pak_sha256"]=t6.sha(new)
 for row in meta["members_provenance"]:
  if row["path"] in replacement:
   row["sha256"]=t6.sha(replacement[row["path"]]);row.setdefault("sources",[]).append(str(ROOT/"src/subsystems/throw_drop/resources"/REPLACEMENTS[row["path"]]))
 meta["throwing_t8"]={"changed_members":changed,"preserved_members":len(old)-len(changed),"native_damage_adapter":False,"recovery":"explicit once per saved soul, old receipts preserved"}
 files[t6.MARKER]=(json.dumps(meta,indent=2)+"\n").encode()
 # Only existing distribution metadata and three PAK payloads change.
 for n,data in files.items():
  target=out/n;target.parent.mkdir(parents=True,exist_ok=True);target.write_bytes(data)
 report={"base_pak":t6.sha((base/t6.PAK).read_bytes()),"pak_sha256":t6.sha(new),"changed_members":changed,"preserved_members":len(old)-len(changed),"installed":False}
 (out/"T8_PACKAGE_RECEIPT.json").write_text(json.dumps(report,indent=2)+"\n")
 return report
if __name__=="__main__":
 ap=argparse.ArgumentParser();ap.add_argument("--base",type=Path,required=True);ap.add_argument("--out",type=Path,required=True);a=ap.parse_args();print(json.dumps(build(a.base,a.out),indent=2))
