"""Compose only reviewed T27 knife Lua and CL-D doors on exact frozen H41 PAK."""
from pathlib import Path
import argparse,copy,hashlib,importlib.util,json,zipfile
ROOT=Path(__file__).resolve().parents[1]
def sha(b):return hashlib.sha256(b).hexdigest()
def main():
 p=argparse.ArgumentParser();p.add_argument('--parent',type=Path,required=True);p.add_argument('--out',type=Path,required=True);p.add_argument('--receipt',type=Path,required=True);a=p.parse_args()
 assert sha(a.parent.read_bytes())=='c938cad79201ebfb8716945a33ee9e4c85f1f9106d986ade18985158adcce021'
 with zipfile.ZipFile(a.parent)as z:old={i.filename:(i,z.read(i))for i in z.infolist()}
 path=ROOT/'src/subsystems/throw_drop/resources/companion_patch_t27.py';spec=importlib.util.spec_from_file_location('t27',path);m=importlib.util.module_from_spec(spec);spec.loader.exec_module(m)
 after,changed=m.plan_members(old)
 member='Scripts/Startup/zzkcd2vr_hand_only_doors_h40.lua';source=ROOT/'mods/KCD2VRDoors/hand_only_h40.lua'
 after[member]=(copy.copy(old[member][0]),source.read_bytes());changed.add(member)
 assert set(after)==set(old) and len(changed)==3
 assert all(after[n][1]==b for n,(_,b) in old.items()if n not in changed)
 a.out.parent.mkdir(parents=True,exist_ok=True)
 with zipfile.ZipFile(a.out,'w')as z:
  for name,(info,data)in after.items():z.writestr(copy.copy(info),data)
 with zipfile.ZipFile(a.out)as z:assert all(z.read(n)==b for n,(_,b)in after.items())
 receipt={'parent_sha256':sha(a.parent.read_bytes()),'pak_sha256':sha(a.out.read_bytes()),'changed':{n:{'before':sha(old[n][1]),'after':sha(after[n][1])}for n in sorted(changed)},'unchanged_members':len(old)-3,'added':[],'removed':[],'native_execution':False,'headset':'NOT_RUN'}
 a.receipt.parent.mkdir(parents=True,exist_ok=True);a.receipt.write_text(json.dumps(receipt,indent=2)+'\n');print(json.dumps(receipt,indent=2))
if __name__=='__main__':main()
