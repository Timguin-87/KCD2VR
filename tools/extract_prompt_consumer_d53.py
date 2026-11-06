import argparse,hashlib,json
from pathlib import Path
p=argparse.ArgumentParser();p.add_argument('--out',type=Path,required=True);a=p.parse_args()
r=Path(__file__).resolve().parents[1];f=r/'src/subsystems/hud_ui_controls/runtime/prompt_device_lock.cpp';raw=f.read_bytes();s=raw.decode().replace("\r\n","\n");start=s.index('void* __fastcall InputDeviceGetD53(');op=s.index('{',start);depth=1;end=op+1
while depth:
 if s[end]=='{':depth+=1
 elif s[end]=='}':depth-=1
 end+=1
a.out.mkdir(parents=True,exist_ok=True);(a.out/'prompt_consumer_d53.inc').write_text(s[start:end]+'\n')
(a.out/'PROMPT_CONSUMER_PROVENANCE.json').write_text(json.dumps({'source':str(f),'sha256':hashlib.sha256(raw).hexdigest(),'native_IO':'explicit fixture doubles; actual consumer control flow'},indent=2))
