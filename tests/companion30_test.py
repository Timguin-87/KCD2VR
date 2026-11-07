"""C30/C31 current companion: exact old members, exact two Lua additions, fail closed."""
from pathlib import Path
import sys,tempfile,unittest,zipfile,hashlib
ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'tools'))
import build_companion30 as builder
BASE=ROOT/'companion/reference/C28/KCD2VR_KICK_D_fulltable_plus_masterstrikes.pak'
CURRENT=ROOT/'companion/reference/C31'/BASE.name # immutable C30/C31 artifact; C32 has its own additive merge test
SCRIPTS=Path('C:/Steam/steamapps/common/KingdomComeDeliverance2/Data/Scripts.pak')
class Companion30(unittest.TestCase):
 def test_actual_artifact_preservation_determinism(self):
  with tempfile.TemporaryDirectory() as d:
   a,b=Path(d)/'a.pak',Path(d)/'b.pak'
   ra=builder.build(BASE,SCRIPTS,a);rb=builder.build(BASE,SCRIPTS,b)
   self.assertEqual(ra,rb);self.assertEqual(a.read_bytes(),b.read_bytes())
   self.assertEqual(a.read_bytes(),CURRENT.read_bytes())
   self.assertEqual(ra['pak_sha256'],hashlib.sha256(CURRENT.read_bytes()).hexdigest())
   with zipfile.ZipFile(BASE) as old,zipfile.ZipFile(a) as new:
    self.assertEqual(len(old.namelist()),16);self.assertEqual(len(new.namelist()),18)
    self.assertEqual(new.namelist(),old.namelist()+list(builder.lua.INPUTS));self.assertIsNone(new.testzip())
    for n in old.namelist():self.assertEqual(new.read(n),old.read(n),n)
    for n in builder.lua.INPUTS:self.assertEqual(new.read(n),(ROOT/'companion/source'/n).read_bytes(),n)
 def test_changed_base_refuses_without_output(self):
  with tempfile.TemporaryDirectory() as d:
   bad=Path(d)/'bad.pak';bad.write_bytes(BASE.read_bytes()+b' ');out=Path(d)/'out.pak'
   with self.assertRaisesRegex(ValueError,'fingerprint'):builder.build(bad,SCRIPTS,out)
   self.assertFalse(out.exists())
 def test_overwrite_baseline_refuses(self):
  before=BASE.read_bytes()
  with self.assertRaisesRegex(ValueError,'overwrite'):builder.build(BASE,SCRIPTS,BASE)
  self.assertEqual(BASE.read_bytes(),before)
 def test_changed_retail_lua_refuses_without_output(self):
  with tempfile.TemporaryDirectory() as d:
   bad=Path(d)/'Scripts.pak';out=Path(d)/'out.pak'
   with zipfile.ZipFile(SCRIPTS) as old,zipfile.ZipFile(bad,'w') as z:
    for n in builder.lua.INPUTS:z.writestr(n,old.read(n)+b' ')
   with self.assertRaises(ValueError):builder.build(BASE,bad,out)
   self.assertFalse(out.exists())
if __name__=='__main__':unittest.main()
