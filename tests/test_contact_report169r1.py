import importlib.util
from pathlib import Path
import unittest
spec=importlib.util.spec_from_file_location('contacts169r1',Path(__file__).resolve().parents[1]/'tools/report_contacts169r1.py')
m=importlib.util.module_from_spec(spec);spec.loader.exec_module(m)
B='KCD2VR BUILD '+m.EXPECTED+'\n'
E='V66.169R1 CONTACT_EVENT source=WEAPON_WORLD actor=AA key=12 generation=1 epoch=1 request=1 side=R episode=1 status=CONTACT phase=SLIDE eligible=1 worldCoherent=1 witness=0 hapticQueued=1\n'
class ContactReport(unittest.TestCase):
 def test_current(self):
  r=m.summarize([('new.log',(B+E).encode())]);self.assertTrue(r['currentBuildMarkerPresent']);self.assertEqual(r['sources']['WEAPON_WORLD']['hapticRequestsQueued'],1);self.assertNotIn('hits',r)
 def test_old(self):
  r=m.summarize([('renamed169.log',b'KCD2VR BUILD KCD2VR-V66.164R4-HUD-PERF-FASTPATH\n')]);self.assertFalse(r['currentBuildMarkerPresent']);self.assertEqual(r['sources'],{})
 def test_duplicate(self):
  x=(B+E).encode();r=m.summarize([('a',x),('b',x)]);self.assertEqual(len(r['duplicates']),1);self.assertEqual(r['sources']['WEAPON_WORLD']['records'],1)
 def test_native_not_passive(self):
  r=m.summarize([('a',(B+'V66.116 MATERIAL_EFFECT stage=2 prediction=2 sweep=3 contact=0 nativeReturned=1\n').encode())]);self.assertEqual(r['sources'],{});self.assertEqual(r['nativeMaterialWithExactStackIds'],1)
 def test_unknown(self):
  r=m.summarize([('a',(B+E.replace('status=CONTACT','status=UNKNOWN').replace('worldCoherent=1','worldCoherent=0')).encode())]);self.assertEqual(r['sources']['WEAPON_WORLD']['worldMismatchRecords'],1);self.assertTrue(any('world mismatch' in d for d in r['decisions']))
 def test_absence_explicit(self):
  r=m.summarize([('a',B.encode())]);self.assertIn('NOT_QUERIED',m.markdown(r));self.assertTrue(any('not an OpenXR success' in d for d in r['decisions']))
if __name__=='__main__':unittest.main()
