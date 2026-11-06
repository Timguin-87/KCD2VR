"""Exact retail evidence checks for Lua door getters; no runtime writes."""
from pathlib import Path
import sys,hashlib,json,struct
from native_research112 import PE
ROOT=Path(__file__).resolve().parents[1]
def main():
 native=Path(sys.argv[1]) if len(sys.argv)>1 else Path("C:/Steam/steamapps/common/KingdomComeDeliverance2/Bin/Win64MasterMasterSteamPGO/WHGame.dll")
 p=PE(native);m=json.loads((ROOT/"review/C19C4R4/door-native/PROVENANCE.json").read_text());checks=0
 assert hashlib.sha256(p.data).hexdigest()==m["retail_sha256"];checks+=1
 assert p.stamp==int(m["timestamp"],16);checks+=1
 for group in ["functions","registration_slices"]:
  for address,v in m[group].items():
   assert hashlib.sha256(p.read(int(address,16),v["size"])).hexdigest()==v["sha256"],address;checks+=1
 for address,target in m["slots"].items():
  assert struct.unpack("<Q",p.read(int(address,16),8))[0]-p.base==int(target,16);checks+=1
 print("door Lua getter retail evidence: %d/%d PASS"%(checks,checks))
if __name__=="__main__":main()
