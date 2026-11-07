"""Actual working-PAK merge checks; no game installation."""
from pathlib import Path
import sys,tempfile,unittest,zipfile
ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'tools'))
import build_companion28 as builder
BASE=Path('C:/Steam/steamapps/common/KingdomComeDeliverance2/Mods/KCD2VR/Data/KCD2VR_KICK_D_fulltable_plus_masterstrikes.pak')
SCRIPTS=Path('C:/Steam/steamapps/common/KingdomComeDeliverance2/Data/Scripts.pak')
class Companion28(unittest.TestCase):
 def test_actual_members_preserved_and_deterministic(self):
  with tempfile.TemporaryDirectory() as d:
   a,b=Path(d)/'a.pak',Path(d)/'b.pak'
   ra=builder.build(BASE,SCRIPTS,a);rb=builder.build(BASE,SCRIPTS,b)
   self.assertEqual(a.read_bytes(),b.read_bytes());self.assertEqual(ra,rb)
   with zipfile.ZipFile(BASE) as old,zipfile.ZipFile(a) as new:
    self.assertEqual(new.namelist(),old.namelist()+[builder.MEMBER]);self.assertIsNone(new.testzip())
    for n in old.namelist():self.assertEqual(new.read(n),old.read(n),n)
    self.assertEqual(new.read(builder.MEMBER),(ROOT/'companion/source'/builder.MEMBER).read_bytes())
   # C28's immutable comparison artifact; C30 has its own current-PAK fixture.
   shipped=ROOT/'companion/reference/C28'/BASE.name
   self.assertEqual(a.read_bytes(),shipped.read_bytes())
 def test_refuses_changed_base(self):
  with tempfile.TemporaryDirectory() as d:
   bad=Path(d)/'bad.pak';bad.write_bytes(BASE.read_bytes()+b' ')
   with self.assertRaises(ValueError):builder.build(bad,SCRIPTS,Path(d)/'out.pak')
   self.assertFalse((Path(d)/'out.pak').exists())
 def test_cannot_overwrite_base(self):
  with self.assertRaises(ValueError):builder.build(BASE,SCRIPTS,BASE)
if __name__=='__main__':unittest.main()
