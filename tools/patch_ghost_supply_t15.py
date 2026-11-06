"""Overlay only invisible-supply Lua and dedicated bootstrap into CURRENT PAK."""
from pathlib import Path
import argparse,json,hashlib,zipfile,io
r=Path(__file__).resolve().parents[1]
def sha(b):return hashlib.sha256(b).hexdigest()
def patch(base,output,report):
 if base.resolve()==output.resolve():raise ValueError('output must not overwrite input')
 with zipfile.ZipFile(base) as z:
  if z.testzip():raise ValueError('input CRC')
  infos=z.infolist();before={i.filename:z.read(i) for i in infos}
  if len(before)!=len(infos) or len({n.lower() for n in before})!=len(before):raise ValueError('duplicate members')
 after=dict(before)
 pairs={'Scripts/KCD2VR/native_supply_t2.lua':r/'src/vrik/native_supply_t2.lua','Scripts/Startup/zzkcd2vr_supply_t15.lua':r/'src/subsystems/throw_drop/resources/startup_supply_t15.lua'}
 if 'Scripts/KCD2VR/native_supply_t2.lua' not in before:raise ValueError('current supply member missing')
 for n,p in pairs.items():after[n]=p.read_bytes()
 changed=[n for n in after if before.get(n)!=after[n]]
 assert set(changed)<=set(pairs) and set(before)<=set(after)
 b=io.BytesIO()
 with zipfile.ZipFile(b,'w',zipfile.ZIP_DEFLATED,compresslevel=6) as z:
  for i in infos:z.writestr(i,after[i.filename])
  for n in sorted(set(after)-set(before)):z.writestr(n,after[n])
 raw=b.getvalue();output.parent.mkdir(parents=True,exist_ok=True);output.write_bytes(raw)
 with zipfile.ZipFile(output) as z:
  assert z.testzip() is None
  assert {n:z.read(n) for n in z.namelist()}==after
 result={'base_sha256':sha(base.read_bytes()),'output_sha256':sha(raw),'changed':changed,'unchanged_members':sum(before[n]==after[n] for n in before),'members':len(after),'headset':'NOT_RUN','source':'real inventory; no attachments/slots/timers/clones'}
 report.write_text(json.dumps(result,indent=2));return result
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--base',type=Path,required=True);p.add_argument('--output',type=Path,required=True);p.add_argument('--report',type=Path,required=True);a=p.parse_args();print(json.dumps(patch(a.base,a.output,a.report),indent=2))
