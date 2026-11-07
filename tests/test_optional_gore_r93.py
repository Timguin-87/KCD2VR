"""Tiny synthetic asset set exercises the real strict optional source overlay."""
import hashlib,json,sys,tempfile,unittest,zipfile
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools'))
from optional_gore_r93 import prepare_current_optional
from optional_gore_d74 import validate_optional
from companion_sources_d39 import GORE_SOURCE_PAIRS
class Overlay(unittest.TestCase):
 def test_exact_source_only_overlay_and_unknown_donor_refusal(self):
  with tempfile.TemporaryDirectory(prefix='kcd2vr-gore-r93-') as temp:
   root=Path(temp);source=root/'source';donor=root/'donor/Mods/kcd2vr_gore';out=root/'out'
   pak=donor/'Data/kcd2vr_gore.pak';pak.parent.mkdir(parents=True)
   member=next(iter(GORE_SOURCE_PAIRS.values()));script=next(iter(GORE_SOURCE_PAIRS))
   original={member:b'old Lua', 'objects/cap.fly.cdf':b'unchanged wrapper', 'objects/limb.skin':b'unchanged model', 'objects/':b'', 'Objects/':b''}
   with zipfile.ZipFile(pak,'w') as z:
    for name,b in original.items():z.writestr(name,b)
   manifest=donor/'mod.manifest';manifest.write_bytes(b'D77 test manifest')
   proof={'pak_sha256':hashlib.sha256(pak.read_bytes()).hexdigest(),'manifest_sha256':hashlib.sha256(manifest.read_bytes()).hexdigest(),
    'lua_sha256':hashlib.sha256(original[member]).hexdigest(),'source_member':member,'members':len(original),'donor_version':'synthetic fixture',
    'required_members':{'objects/cap.fly.cdf':hashlib.sha256(original['objects/cap.fly.cdf']).hexdigest()}}
   pp=source/'mods/KCD2VRGore/r93r3/D77_DONOR.json';pp.parent.mkdir(parents=True);pp.write_text(json.dumps(proof))
   sp=source/script;sp.parent.mkdir(parents=True,exist_ok=True);sp.write_bytes(b'new Lua')
   # Deliberately stale old receipt is not used to authenticate the donor.
   (donor/'KCD2VR_GORE_D74.json').write_text('{"pak_sha256":"stale"}')
   result=prepare_current_optional(source,out,root/'donor','R93-test')
   self.assertEqual(result,validate_optional(out/'OptionalGore/Mods/kcd2vr_gore'))
   with zipfile.ZipFile(out/'OptionalGore/Mods/kcd2vr_gore/Data/kcd2vr_gore.pak') as z:
    self.assertEqual(set(z.namelist()),set(original))
    for name,b in original.items():self.assertEqual(z.read(name),b'new Lua' if name==member else b)
   self.assertEqual(hashlib.sha256(pak.read_bytes()).hexdigest(),proof['pak_sha256'])
   self.assertEqual(len(result['members']),5) # both empty directory aliases were preserved
   self.assertEqual(prepare_current_optional(source,out,root/'donor','R93-test'),result)
   manifest.write_bytes(b'wrong manifest')
   with self.assertRaisesRegex(ValueError,'exact reviewed D77'):prepare_current_optional(source,root/'bad',root/'donor','R93-test')
   manifest.write_bytes(b'D77 test manifest');pak.write_bytes(pak.read_bytes()+b'changed archive')
   with self.assertRaisesRegex(ValueError,'exact reviewed D77'):prepare_current_optional(source,root/'bad',root/'donor','R93-test')
   # Even with an exact fixture hash, ambiguous file paths and nonempty
   # directory aliases are refused. This never broadens asset admission.
   for extra in [{'OBJECTS/CAP.FLY.CDF':b'conflicting file'},{'Objects/':b'nonempty directory'}]:
    payload={**original,**extra}
    with zipfile.ZipFile(pak,'w') as z:
     for name,b in payload.items():z.writestr(name,b)
    proof['pak_sha256']=hashlib.sha256(pak.read_bytes()).hexdigest();proof['members']=len(payload);pp.write_text(json.dumps(proof))
    with self.assertRaisesRegex(ValueError,'member identity changed'):prepare_current_optional(source,root/'bad',root/'donor','R93-test')
if __name__=='__main__':unittest.main()
