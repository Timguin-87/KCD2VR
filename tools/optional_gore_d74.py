"""Build optional gore separately from the preserved, hash-qualified D73 donor.
No installation. No change to native eligibility or the gore rendering pipeline.
"""
from pathlib import Path
import hashlib,json,os,zipfile
from companion_sources_d39 import GORE_SOURCE_PAIRS

def sha(p):
 h=hashlib.sha256()
 with p.open('rb') as f:
  for b in iter(lambda:f.read(1024*1024),b''):h.update(b)
 return h.hexdigest()
def atomic(p,b):
 p.parent.mkdir(parents=True,exist_ok=True);q=p.with_name(p.name+'.tmp');q.write_bytes(b);os.replace(q,p)
README=b'Optional adult gore mod. Copy only this Mods/kcd2vr_gore folder to the game Mods folder if wanted. Main companion contains no gore. Use only this one gore PAK; keep older gore packages outside active Mods. Restart the game after adding/removing this optional mod. Eligibility remains native; this packaging repair does not prove all visual/limb outcomes.\n'
def validate_optional(root):
 expected={'Data/kcd2vr_gore.pak','KCD2VR_GORE_D74.json','mod.manifest'}
 actual={p.relative_to(root).as_posix() for p in root.rglob('*') if p.is_file()}
 if actual!=expected:raise ValueError('Unexpected or missing optional gore distribution member: '+str(actual^expected))
 receipt=json.loads((root/'KCD2VR_GORE_D74.json').read_text())
 if sha(root/'Data/kcd2vr_gore.pak')!=receipt['pak_sha256']:raise ValueError('Optional gore PAK hash mismatch')
 if sha(root/'mod.manifest')!=receipt['inputs']['manifest_sha256']:raise ValueError('Optional gore manifest hash mismatch')
 return receipt

def prepare_optional(source,out,pak,meta,names,marker):
 if len(names)!=13602:raise ValueError('D74 optional gore donor partition changed; review required')
 root=out/'OptionalGore/Mods/kcd2vr_gore';target=root/'Data/kcd2vr_gore.pak';record=root/'KCD2VR_GORE_D74.json'
 replacement={member:(source/path).read_bytes() for path,member in GORE_SOURCE_PAIRS.items()}
 if not set(replacement)<=names:raise ValueError('Optional gore source mapping is not in the qualified donor')
 manifest=(source/'companion/source/gore/mod.manifest').read_bytes()
 inputs={'build_marker':marker,'parent_pak_sha256':meta['pak_sha256'],
  'manifest_sha256':hashlib.sha256(manifest).hexdigest(),
  'source_members':{n:hashlib.sha256(b).hexdigest() for n,b in replacement.items()}}
 expected={r['path']:r['sha256'] for r in meta['members_provenance'] if r['path'] in names}
 final_expected={**expected,**inputs['source_members']}
 try:
  cached=validate_optional(root)
  if cached['inputs']==inputs and {r['path']:r['sha256'] for r in cached['members']}==final_expected:
   atomic(out/'OptionalGore/README_OPTIONAL_GORE.txt',README)
   return
 except (OSError,ValueError,KeyError):pass
 target.parent.mkdir(parents=True,exist_ok=True);tmp=target.with_suffix('.pak.tmp');rows=[]
 with zipfile.ZipFile(pak) as zin,zipfile.ZipFile(tmp,'w') as zout:
  for entry in zin.infolist():
   name=entry.filename
   if name not in names:continue
   original=zin.read(entry)
   if hashlib.sha256(original).hexdigest()!=expected[name]:raise ValueError('Gore donor member hash mismatch: '+name)
   data=replacement.get(name,original);zout.writestr(entry,data)
   rows.append({'path':name,'sha256':hashlib.sha256(data).hexdigest(),'bytes':len(data)})
 with zipfile.ZipFile(tmp) as z:
  if z.testzip() is not None or set(z.namelist())!=names:raise ValueError('Optional gore output invalid')
 os.replace(tmp,target);atomic(root/'mod.manifest',manifest)
 atomic(record,(json.dumps({'inputs':inputs,'pak_sha256':sha(target),'members':rows,'headset':'NOT_RUN'},indent=2)+'\n').encode())
 atomic(out/'OptionalGore/README_OPTIONAL_GORE.txt',README)
 validate_optional(root)


# The normal source/build has no adult payload. A user may explicitly supply an
# already sealed standalone optional package; no implicit donor or network fetch.
def stage_external_optional(supplied,out):
 import shutil
 supplied=Path(supplied).resolve();out=Path(out).resolve()
 target=out/'OptionalGore';root=target/'Mods/kcd2vr_gore'
 paths=('Mods/kcd2vr_gore/Data/kcd2vr_gore.pak','Mods/kcd2vr_gore/KCD2VR_GORE_D74.json',
        'Mods/kcd2vr_gore/mod.manifest','README_OPTIONAL_GORE.txt')
 if supplied==target or supplied.is_relative_to(target) or target.is_relative_to(supplied):
  raise ValueError('Optional package input and staging must be separate')
 def copy_stream(src,dest,expected=None):
  dest.parent.mkdir(parents=True,exist_ok=True);tmp=dest.with_name(dest.name+'.optional.tmp')
  try:
   with tmp.open('wb') as f:shutil.copyfileobj(src,f,1024*1024)
   if expected is not None and sha(tmp)!=expected:raise ValueError('Optional input member hash mismatch: '+dest.name)
   os.replace(tmp,dest)
  finally:
   if tmp.exists():tmp.unlink()
 if supplied.is_file():
  with zipfile.ZipFile(supplied) as z:
   names=z.namelist()
   if len(names)!=len(paths) or set(names)!=set(paths):raise ValueError('Unexpected standalone optional package layout')
   receipt=json.loads(z.read(paths[1]));manifest=z.read(paths[2])
   if hashlib.sha256(manifest).hexdigest()!=receipt['inputs']['manifest_sha256']:
    raise ValueError('Optional input manifest hash mismatch')
   # Stream and qualify the large archive before publishing it to staging.
   expected={paths[0]:receipt['pak_sha256'],paths[2]:receipt['inputs']['manifest_sha256']}
   for name in paths:
    with z.open(name) as f:copy_stream(f,target/name,expected.get(name))
 else:
  if not all((supplied/name).is_file() for name in paths):raise ValueError('Optional source must be the standalone package root')
  receipt=validate_optional(supplied/'Mods/kcd2vr_gore')
  expected={paths[0]:receipt['pak_sha256'],paths[2]:receipt['inputs']['manifest_sha256']}
  for name in paths:
   with (supplied/name).open('rb') as f:copy_stream(f,target/name,expected.get(name))
 return validate_optional(root)
