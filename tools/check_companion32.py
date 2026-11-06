"""Read-only installation check; canonical disk bytes are not mount-order proof."""
from pathlib import Path
import argparse, hashlib, json, zipfile
R=Path(__file__).resolve().parents[1]
NAME='KCD2VR_KICK_D_fulltable_plus_masterstrikes.pak'
def inspect(game):
 expected=R/'companion/Mods/KCD2VR/Data'/NAME
 path=game/'Mods/KCD2VR/Data'/NAME
 sha=lambda b:hashlib.sha256(b).hexdigest()
 result={'canonical_path':str(path),'expected_sha256':sha(expected.read_bytes()),'installed':path.exists(),
  'scope':'CANONICAL_DISK_BYTES_NOT_NATIVE_MOUNT_OR_OVERRIDE_PROOF','writes':'NONE'}
 if path.exists():
  b=path.read_bytes();result['actual_sha256']=sha(b);result['exact_current_main']=b==expected.read_bytes()
  with zipfile.ZipFile(path) as z,zipfile.ZipFile(expected) as current:
   result['member_count']=len(z.namelist());result['lua_members']=[n for n in z.namelist() if n.lower().endswith('.lua')]
   result['missing_required_current_members']=[n for n in current.namelist() if n not in z.namelist()]
 return result
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--game',type=Path,required=True);p.add_argument('--out',type=Path);a=p.parse_args()
 report=inspect(a.game);text=json.dumps(report,indent=2)+'\n'
 if a.out:a.out.parent.mkdir(parents=True,exist_ok=True);a.out.write_text(text)
 print(text);raise SystemExit(0 if report.get('exact_current_main') else 2)
