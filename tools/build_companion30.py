"""C30 ordinary-Talk companion; preserve all 16 C28 members and add two Lua assets."""
from pathlib import Path
import argparse,hashlib,json,os,tempfile,zipfile
import build_dialogue_lua30 as lua
ROOT=Path(__file__).resolve().parents[1]
BASE_SHA='cb574f4a9d1d453378acedabab880fab7172cf85d30ae0839bac064079c9ddac'

def build(base:Path,scripts:Path,out:Path):
 if base.resolve()==out.resolve():raise ValueError('output must not overwrite baseline')
 if hashlib.sha256(base.read_bytes()).hexdigest()!=BASE_SHA:raise ValueError('C28 companion fingerprint mismatch')
 with zipfile.ZipFile(scripts) as z:added={n:lua.patch(n,z.read(n)) for n in lua.INPUTS}
 for n,data in added.items():
  if data!=(ROOT/'companion/source'/n).read_bytes():raise ValueError('authored Lua differs from exact recipe')
 with zipfile.ZipFile(base) as z:
  if z.testzip() is not None:raise ValueError('baseline CRC')
  names=z.namelist()
  if len(names)!=len(set(names)) or any(n in names for n in added):raise ValueError('duplicate/unexpected Lua member')
  old={n:z.read(n) for n in names}
 out.parent.mkdir(parents=True,exist_ok=True)
 fd,tmp=tempfile.mkstemp(prefix='C30.',suffix='.tmp',dir=out.parent);os.close(fd)
 try:
  with zipfile.ZipFile(tmp,'w',zipfile.ZIP_DEFLATED,compresslevel=9) as z:
   for n,data in list(old.items())+list(added.items()):
    info=zipfile.ZipInfo(n,(2026,10,16,0,0,0));info.compress_type=zipfile.ZIP_DEFLATED;info.external_attr=0o644<<16;z.writestr(info,data)
  with zipfile.ZipFile(tmp) as z:
   assert z.testzip() is None and z.namelist()==names+list(added)
   for n,data in old.items():assert z.read(n)==data,n
   for n,data in added.items():assert z.read(n)==data,n
  os.replace(tmp,out)
 finally:
  if os.path.exists(tmp):os.unlink(tmp)
 return {'base_sha256':BASE_SHA,'pak_sha256':hashlib.sha256(out.read_bytes()).hexdigest(),
  'preserved_members':[{'name':n,'sha256':hashlib.sha256(d).hexdigest()} for n,d in old.items()],
  'added_members':[{'name':n,'sha256':hashlib.sha256(d).hexdigest()} for n,d in added.items()],
  'table_changes':False,'native_hooks_added':False,'installed':False,'headset_validation':False}

def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--base',type=Path,required=True);p.add_argument('--scripts',type=Path,required=True);p.add_argument('--out',type=Path,required=True);a=p.parse_args()
 result=build(a.base,a.scripts,a.out);a.out.with_suffix('.manifest.json').write_text(json.dumps(result,indent=2)+'\n');print(json.dumps(result,indent=2))
if __name__=='__main__':main()
