"""C33 companion: the pinned C32C1 pak with exactly five members changed (kick zone 5, special-input configs,
physical hit reactions, comment cleanup); all other members byte-identical; deterministic; fails closed."""
from pathlib import Path
import sys,tempfile,unittest,zipfile,hashlib,re
ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'tools'))
import build_companion33 as b
BASE=ROOT/'companion/reference/C32C1/KCD2VR_KICK_D_fulltable_plus_masterstrikes.pak'
CURRENT=ROOT/'companion/Mods/KCD2VR/Data'/BASE.name
class Companion33(unittest.TestCase):
 def test_build_matches_shipped_and_preserves_members(self):
  with tempfile.TemporaryDirectory() as d:
   x,y=Path(d)/'x.pak',Path(d)/'y.pak'
   rx=b.build(BASE,x);ry=b.build(BASE,y)
   self.assertEqual(rx,ry);self.assertEqual(x.read_bytes(),y.read_bytes());self.assertEqual(x.read_bytes(),CURRENT.read_bytes())
   with zipfile.ZipFile(BASE) as old,zipfile.ZipFile(x) as new:
    self.assertEqual(old.namelist(),new.namelist());self.assertEqual(len(new.namelist()),26);self.assertIsNone(new.testzip())
    changed=[n for n in old.namelist() if old.read(n)!=new.read(n)]
    self.assertEqual(sorted(changed),sorted([b.ATTACK,b.ADB,b.CONFIG,b.REACT,b.RPG]))
    att=new.read(b.ATTACK).decode()
    self.assertNotIn('attack_zone_id="3"',att);self.assertNotIn('+aZ3+',att)
    self.assertEqual(len(re.findall(r'attack_type_id="4"[^>]*attack_zone_id="5"',att)),18) # 12 armed + 6 retail unarmed
    adb=new.read(b.ADB).decode();self.assertNotIn('+aZ3+',adb)
    self.assertEqual(adb.count('+aZ5+kick+')-old.read(b.ADB).decode().count('+aZ5+kick+'),12)
    self.assertEqual(new.read(b.CONFIG).decode().count('combat_input_class_id="2" combat_zone_id="5"'),7)
    self.assertIn('physical_coef="0.8"',new.read(b.REACT).decode())
    self.assertNotIn('Overhaul',new.read(b.RPG).decode())
    for n in old.namelist():
     if n not in changed:self.assertEqual(old.read(n),new.read(n),n)
 def test_wrong_input_refuses_without_output(self):
  with tempfile.TemporaryDirectory() as d:
   bad=Path(d)/'bad.pak';bad.write_bytes(BASE.read_bytes()+b' ');out=Path(d)/'o.pak'
   with self.assertRaisesRegex(ValueError,'fingerprint'):b.build(bad,out)
   self.assertFalse(out.exists())
 def test_no_overwrite(self):
  with self.assertRaisesRegex(ValueError,'overwrite'):b.build(BASE,BASE)
if __name__=='__main__':unittest.main()
