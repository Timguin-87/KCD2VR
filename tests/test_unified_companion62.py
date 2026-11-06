import importlib.util, tempfile, unittest, zipfile
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
spec=importlib.util.spec_from_file_location('unified62',ROOT/'tools/build_unified_companion62.py');module=importlib.util.module_from_spec(spec);spec.loader.exec_module(module)
class UnifiedCompanion62(unittest.TestCase):
 def test_case_insensitive_path_conflict_refused(self):
  with tempfile.TemporaryDirectory() as t:
   a,b=Path(t)/'a.pak',Path(t)/'b.pak'
   with zipfile.ZipFile(a,'w')as z:z.writestr('Scripts/Startup/map.lua',b'current')
   with zipfile.ZipFile(b,'w')as z:z.writestr('scripts/startup/MAP.lua',b'old')
   with self.assertRaisesRegex(ValueError,'Conflicting member'):module.collect([a,b])
 def test_current_payload_preservation_and_real_legacy_conflict(self):
  main=ROOT/'companion/Mods/KCD2VR/Data/KCD2VR_KICK_D_fulltable_plus_masterstrikes.pak'
  old=ROOT/'companion/Mods/KCD2VRMap/Data/KCD2VR_Map.pak'
  with self.assertRaisesRegex(ValueError,'Conflicting member'):module.collect([main,old])
  with tempfile.TemporaryDirectory()as t:
   out=Path(t)/'new';report=module.build(out)
   self.assertEqual(report['members'],71)
   with zipfile.ZipFile(main)as original,zipfile.ZipFile(out/'Mods/KCD2VR/Data/KCD2VR.pak')as current:
    for n in original.namelist():self.assertEqual(original.read(n),current.read(n),n)
    self.assertEqual(len(current.namelist()),len({n.lower()for n in current.namelist()}))
    self.assertIn('Scripts/Startup/zzkcd2vr_doors.lua',current.namelist())
    self.assertIn('Scripts/Startup/zzkcd2vr_lab.lua',current.namelist())
   self.assertTrue((out/'Mods/KCD2VR/Localization/English_xml.pak').is_file())
if __name__=='__main__':unittest.main()
