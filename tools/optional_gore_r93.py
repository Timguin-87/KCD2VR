"""Optional D77 source overlay, used only when the user explicitly supplies its assets.
No asset is generated or replaced. Exact donor hashes supersede its stale D74 receipt.
"""
from pathlib import Path
import hashlib,json,os,tempfile,zipfile
from companion_sources_d39 import GORE_SOURCE_PAIRS
from optional_gore_d74 import atomic,sha,validate_optional,stage_external_optional,README


def prepare_current_optional(source,out,supplied,marker):
 source,out,supplied=Path(source),Path(out),Path(supplied).resolve()
 proof=json.loads((source/'mods/KCD2VRGore/r93r3/D77_DONOR.json').read_text())
 # A sealed external ZIP keeps its existing strict layout/hash validation.
 # The original D77 folder has a stale receipt; it is qualified by full PAK
 # and manifest hashes below, never by trusting that stale receipt.
 if supplied.is_file():
  if supplied.suffix.lower()!='.zip':raise ValueError('Supply the D77 folder or a sealed optional ZIP')
  staged=out/'optional-input-r93'
  stage_external_optional(supplied,staged)
  donor=staged/'OptionalGore/Mods/kcd2vr_gore'
 else:
  donor=next((p for p in (supplied/'Mods/kcd2vr_gore',supplied/'OptionalGore/Mods/kcd2vr_gore',supplied) if (p/'Data/kcd2vr_gore.pak').is_file()),None)
  if donor is None:raise ValueError('Optional D77 gore PAK not found in supplied folder')
 pak=donor/'Data/kcd2vr_gore.pak';manifest=donor/'mod.manifest'
 if sha(pak)!=proof['pak_sha256'] or sha(manifest)!=proof['manifest_sha256']:
  raise ValueError('Optional gore is not the exact reviewed D77 donor; assets must be reviewed before overlay')
 replacement={member:(source/path).read_bytes() for path,member in GORE_SOURCE_PAIRS.items()}
 if set(replacement)!={proof['source_member']}:raise ValueError('R93 optional overlay must replace only the reviewed main Lua')
 root=out/'OptionalGore/Mods/kcd2vr_gore';target=root/'Data/kcd2vr_gore.pak'
 if pak.resolve()==target.resolve():raise ValueError('Optional donor and output must be separate')
 inputs={'build_marker':marker,'parent_pak_sha256':proof['pak_sha256'],
  'manifest_sha256':proof['manifest_sha256'],'source_members':{n:hashlib.sha256(b).hexdigest() for n,b in replacement.items()},
  'donor_version':proof['donor_version'],'stale_donor_receipt_ignored':True}
 try:
  cached=validate_optional(root)
  if cached['inputs']==inputs:
   atomic(out/'OptionalGore/README_OPTIONAL_GORE.txt',README)
   return cached
 except (OSError,ValueError,KeyError):pass
 target.parent.mkdir(parents=True,exist_ok=True)
 fd,tmp=tempfile.mkstemp(dir=target.parent,prefix='gore-r93-',suffix='.tmp');os.close(fd)
 rows=[]
 try:
  with zipfile.ZipFile(pak) as zin,zipfile.ZipFile(tmp,'w') as zout:
   names=zin.namelist()
   # The hash-qualified D77 donor contains105 case-alias empty directories.
   # Preserve them verbatim; files remain unique under game path normalization.
   files=[i.filename.replace('\\','/').lower() for i in zin.infolist() if not i.is_dir()]
   if len(names)!=proof['members'] or len(set(names))!=len(names) or len(set(files))!=len(files) or any(i.is_dir() and i.file_size!=0 for i in zin.infolist()):
    raise ValueError('D77 member identity changed')
   if hashlib.sha256(zin.read(proof['source_member'])).hexdigest()!=proof['lua_sha256']:raise ValueError('D77 source Lua identity changed')
   for name,digest in proof['required_members'].items():
    if hashlib.sha256(zin.read(name)).hexdigest()!=digest:raise ValueError('Required D77 wrapper/cap source changed: '+name)
   for entry in zin.infolist():
    data=replacement.get(entry.filename)
    if data is None:data=zin.read(entry)
    zout.writestr(entry,data)
    rows.append({'path':entry.filename,'sha256':hashlib.sha256(data).hexdigest(),'bytes':len(data)})
  with zipfile.ZipFile(tmp) as z:
   if z.testzip() is not None or z.namelist()!=names:raise ValueError('Optional overlay output failed verification')
  os.replace(tmp,target)
 finally:
  if os.path.exists(tmp):os.unlink(tmp)
 atomic(root/'mod.manifest',manifest.read_bytes())
 record={'inputs':inputs,'pak_sha256':sha(target),'members':rows,'headset':'NOT_RUN'}
 atomic(root/'KCD2VR_GORE_D74.json',(json.dumps(record,indent=2)+'\n').encode())
 atomic(out/'OptionalGore/README_OPTIONAL_GORE.txt',README)
 return validate_optional(root)
