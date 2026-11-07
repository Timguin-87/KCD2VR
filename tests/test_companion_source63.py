"""Byte-exact source-distribution contracts, using the actual reviewed package."""
from pathlib import Path
import sys,unittest,tempfile,json
from contextlib import redirect_stdout
import io
ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'tools'))
import verify_companion_source63 as v
from package_release_helpers_r19 import prepare_companion
class SourceDistribution63(unittest.TestCase):
 @classmethod
 def setUpClass(cls):
  # The embedded C63 PAK is a donor. Validate the actual release preparation,
  # including current source overrides, instead of asserting it is a finished
  # package or restoring intentionally separate source files into the donor.
  cls.temp=tempfile.TemporaryDirectory(prefix='kcd2vr-source63-')
  cls.addClassCleanup(cls.temp.cleanup)
  cls.prepared=Path(cls.temp.name)/'prepared'
  with redirect_stdout(io.StringIO()):prepare_companion(ROOT,cls.prepared)
  cls.entries={n:(ROOT/n).read_bytes() for n in [*v.PAIRS,'tools/install_unified_companion63.ps1']}
  for p in cls.prepared.rglob('*'):
   if p.is_file() and p.name!='patch2_companion_inputs.json':
    cls.entries[v.PACKAGE+p.relative_to(cls.prepared).as_posix()]=p.read_bytes()
  # Installer is distributed at the source package root, outside Mods.
  cls.entries[v.PACKAGE+'install_unified_companion63.ps1']=(ROOT/v.PACKAGE/'install_unified_companion63.ps1').read_bytes()
 def test_reviewed_current_bytes(self):
  r=v.verify_source(self.entries);self.assertEqual(r['source_pak_pairs'],len(v.PAIRS));self.assertGreaterEqual(r['pak_members'],len(v.PAIRS))
 def test_normalized_manifest_rejected(self):
  e=dict(self.entries);name=v.PACKAGE+'Mods/KCD2VR/mod.manifest';e[name]=e[name].replace(b'\r\n',b'\n')
  with self.assertRaisesRegex(ValueError,'hash mismatch: mod.manifest'):v.verify_source(e)
 def test_source_lua_normalization_rejected(self):
  e=dict(self.entries);name='mods/KCD2VRMap/map_runtime.lua';e[name]=e[name].replace(b'\r\n',b'\n')
  with self.assertRaisesRegex(ValueError,'Source/PAK byte mismatch'):v.verify_source(e)
 def test_missing_member_rejected(self):
  e=dict(self.entries);del e[v.PACKAGE+'Mods/KCD2VR/mod.manifest']
  with self.assertRaisesRegex(ValueError,'Incomplete/extra'):v.verify_source(e)
 def test_extra_old_pak_rejected(self):
  e=dict(self.entries);e[v.PACKAGE+'Mods/KCD2VR/Data/old.pak']=b'old'
  with self.assertRaisesRegex(ValueError,'Incomplete/extra'):v.verify_source(e)
 def test_installer_pair_exact(self):
  e=dict(self.entries);e['tools/install_unified_companion63.ps1']+=b' '
  with self.assertRaisesRegex(ValueError,'installer bytes differ'):v.verify_source(e)
if __name__=='__main__':unittest.main()
