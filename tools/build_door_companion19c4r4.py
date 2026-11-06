"""Build only the authored additive door script; no retail AnimDoor replacement."""
from pathlib import Path
import hashlib,json,zipfile
ROOT=Path(__file__).resolve().parents[1]
def build():
 data=(ROOT/"mods/KCD2VRDoors/door_runtime.lua").read_bytes()
 target=ROOT/"companion/Mods/KCD2VRDoors/Data/KCD2VR_Doors.pak"
 target.parent.mkdir(parents=True,exist_ok=True)
 with zipfile.ZipFile(target,"w",zipfile.ZIP_DEFLATED,compresslevel=9) as z:
  info=zipfile.ZipInfo("Scripts/Startup/zzkcd2vr_doors.lua",(2026,10,19,0,0,0));info.compress_type=zipfile.ZIP_DEFLATED;z.writestr(info,data)
 report={"sha256":hashlib.sha256(target.read_bytes()).hexdigest(),"lua_sha256":hashlib.sha256(data).hexdigest(),"retail_script_replaced":False,"requires_exact_asi_private_getter_protocol":1966,"companion_revision":66,"headset_acceptance":"NOT_RUN"}
 print(json.dumps(report,indent=2));return report
if __name__=="__main__":build()
