import unittest,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools'))
import analyze_v6694 as report
class ControllerReportTests(unittest.TestCase):
 def test_vanilla_reference(self):self.assertEqual(report.analyze('')['recipe_profile_annotation'],'supplied-retail')
 def test_no_false_success(self):
  r=report.analyze('V66.94-F12 CONTROL_STARTED mode=2 nativeReadback=1 visibleProof=USER_REQUIRED')['controller_core']
  self.assertEqual(r['reported_weapon_control_started'],1);self.assertEqual(r['weapon_visual_success'],'USER_OBSERVATION_REQUIRED')
 def test_loaded_cache(self):self.assertTrue(report.analyze('V66.94-FINGER REFERENCES_LOADED source=NATIVE_CURVE_CACHE')['controller_core']['reported_cache_loaded'])
 def test_complete_array_is_not_endpoint(self):
  s='\n'.join(f'V66.94-FINGER RAW_REFERENCE side=R category=3 sample=9 index={i} count=2 joint={i+5} q=(0,0,0,1)' for i in range(2))
  g=report.analyze(s)['controller_core']['raw_reference_groups'][0]
  self.assertTrue(g['complete_logged_array']);self.assertFalse(g['authored_endpoint_verified'])
 def test_missing_array_row(self):
  r=report.analyze('V66.94-FINGER RAW_REFERENCE side=L category=1 sample=2 index=0 count=2')['controller_core']['raw_reference_groups'][0]
  self.assertFalse(r['complete_logged_array'])
 def test_duplicate_array_row(self):
  s='V66.94-FINGER RAW_REFERENCE side=L category=1 sample=2 index=0 count=2\n'*2
  self.assertFalse(report.analyze(s)['controller_core']['raw_reference_groups'][0]['complete_logged_array'])
 def test_channels_not_mixed(self):
  s='V66.94-FINGER RAW_REFERENCE side=L category=1 sample=2 index=0 count=2\nV66.94-FINGER RAW_REFERENCE side=R category=1 sample=2 index=1 count=2'
  groups=report.analyze(s)['controller_core']['raw_reference_groups'];self.assertEqual(len(groups),2);self.assertFalse(any(g['complete_logged_array'] for g in groups))
 def test_old_placeholder_unresolved(self):
  r=report.analyze('V66.89-ANIM-START role=HENRY name="CAF filepath disabled"')
  self.assertEqual(r['controller_core']['unresolved_clip_rows'],1)
 def test_retain_f5_exact_target(self):
  r=report.analyze("V66.90-F5-ACTION REQUEST seq=1 targetId=90626 name='door_l_b_o'")
  self.assertEqual(r['controller_core']['native_event_counts']['F5-ACTION REQUEST'],1)
 def test_retain_f3_context(self):
  r=report.analyze('V66.90-F3-ENRICH eventSeq=2 participantRole=UNCLASSIFIED')
  self.assertEqual(r['controller_core']['native_event_counts']['F3-ENRICH'],1)
 def test_no_combo_negative_inference(self):self.assertIn('not evidence that no combo occurred',report.markdown(report.analyze('')))
 def test_render_report(self):self.assertIn('V66.94 controller-core',report.markdown(report.analyze('V66.94-F3 COVERAGE callbacks=100 enriched=20 throttled=15')))
if __name__=='__main__':unittest.main()
