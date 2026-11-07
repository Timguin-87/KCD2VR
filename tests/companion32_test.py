from pathlib import Path
import importlib.util, tempfile, unittest, zipfile
R=Path(__file__).resolve().parents[1]
spec=importlib.util.spec_from_file_location('companion32',R/'tools/build_companion32.py');m=importlib.util.module_from_spec(spec);spec.loader.exec_module(m)
B=R/'companion/reference/C31/KCD2VR_KICK_D_fulltable_plus_masterstrikes.pak'
O=R/'review/C32/KCD2VR_MAP32_OVERLAY.pak'
C=R/'companion/reference/C32C1'/B.name # C34: the main pak is now the C33 kick/reaction build on top of this
L=R/'companion/Mods/KCD2VR/Data'/B.name
class Companion32(unittest.TestCase):
 def test_actual_shipped_payload_and_preservation(self):
  manifest=__import__('json').loads(C.with_suffix('.manifest.json').read_text())
  with tempfile.TemporaryDirectory() as td:
   out=Path(td)/'Mods/KCD2VR/Data'/B.name
   result=m.build(B,O,out,manifest['localization']['languages'])
   self.assertEqual(result,manifest);self.assertEqual(out.read_bytes(),C.read_bytes())
   old,new=m.unpack(B.read_bytes()),m.unpack(out.read_bytes())
   self.assertEqual(len(old),18);self.assertEqual(len(new),26)
   for n,d in old.items():self.assertEqual(new[n],d,n)
   for lang in result['localization']['languages']:
    self.assertEqual((out.parent.parent/'Localization'/lang).read_bytes(),(L.parent.parent/'Localization'/lang).read_bytes())
 def test_bad_inputs_create_no_output(self):
  with tempfile.TemporaryDirectory() as td:
   bad=Path(td)/'bad.pak';bad.write_bytes(B.read_bytes()+b' ');out=Path(td)/'output.pak'
   with self.assertRaisesRegex(ValueError,'fingerprint'):m.build(bad,O,out,['English_xml.pak'])
   self.assertFalse(out.exists())
   with self.assertRaisesRegex(ValueError,'invalid native localization'):m.build(B,O,out,['../English_xml.pak'])
   self.assertFalse(out.exists())
 def test_overwrite_and_unsafe_archive_refuse(self):
  with self.assertRaisesRegex(ValueError,'overwrites'):m.build(B,O,B,['English_xml.pak'])
  for members in ({'../x':b'x'},{'A':b'1','a':b'2'}):
   with self.assertRaises(ValueError):m.unpack(m.packed(members))
if __name__=='__main__':unittest.main()
