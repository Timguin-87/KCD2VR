"""Extract the actual producer/consumer for the portable D53 heading fixture."""
import argparse, hashlib, json
from pathlib import Path
p=argparse.ArgumentParser();p.add_argument('--out',type=Path,required=True);a=p.parse_args()
r=Path(__file__).resolve().parents[1];source=r/'src/camera_hook.cpp';raw=source.read_bytes();text=raw.decode()
parts=[]
for signature in ('static void RoomscaleXrSample47(const XrPose& pose','kcd2vr::roomscale58::Drive CameraHook::RoomscaleDrive('):
 start=text.index(signature);begin=text.index('{',start);end=begin+1;depth=1
 while depth:
  depth+=(text[end]=='{')-(text[end]=='}');end+=1
 parts.append(text[start:end])
a.out.mkdir(parents=True,exist_ok=True)
(a.out/'actual_roomscale_xr_d53.inc').write_text('\n'.join(parts)+'\n')
(a.out/'ROOMSCALE_PROVENANCE.json').write_text(json.dumps({'source':str(source),'sha256':hashlib.sha256(raw).hexdigest(),'runtime':'fixture doubles; no headset'},indent=2))
