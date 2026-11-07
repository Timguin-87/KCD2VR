import sys,unittest
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools'))
import analyze_v6695 as r
class ContinuityTests(unittest.TestCase):
 def test_version(self):self.assertEqual(r.analyze('')['build'],'V66.95')
 def test_vanilla(self):self.assertEqual(r.analyze('')['recipe_profile_annotation'],'supplied-retail')
 def test_first_apply_not_final(self):
  c=r.analyze('V66.95-F12 CONTROL_STARTED mode=3 nativeReadback=1\nV66.95-F12 STATUS applied=1')['continuity_contact']
  self.assertEqual(c['session_end_count'],0);self.assertEqual(c['reported_application_count_at_session_ends'],0);self.assertFalse(c['all_active_intervals_covered'])
 def test_session_end_count(self):
  c=r.analyze('V66.95-F12 SESSION_END applied=30 restorePending=1\nV66.95-F12 SESSION_END applied=12 restorePending=0')['continuity_contact']
  self.assertEqual(c['reported_application_count_at_session_ends'],42);self.assertEqual(c['session_ends_without_pending_restore'],1)
 def test_no_sum_cumulative_hits(self):
  c=r.analyze('V66.92R1-COLLISION STATUS state=QUERY_RETURNED_CONTACT_UNPROVEN queries=10 hits=3 faults=0\nV66.92R1-COLLISION STATUS state=QUERY_RETURNED_CONTACT_UNPROVEN queries=30 hits=4 faults=0')['continuity_contact']
  self.assertEqual(c['latest_cumulative_collision_status']['hits'],'4');self.assertFalse(c['collision_guaranteed'])
 def test_fault_not_clear(self):
  s=r.markdown(r.analyze('V66.92R1-COLLISION STATUS state=BLOCKED_NATIVE_FAULT queries=1 hits=0 faults=1'))
  self.assertIn('BLOCKED_NATIVE_FAULT',s);self.assertIn('not proof',s)
 def test_owner_is_not_accepted(self):
  c=r.analyze('V66.95-F3-OWNER_BOUNDARY gate=ACTOR_TYPE accepted=0')['continuity_contact']
  self.assertEqual(len(c['owner_boundary_records']),1);self.assertFalse(c['owner_mismatch_is_player_acceptance'])
 def test_preserve_literal_lines(self):
  p='V66.95-FINGER CURVE_REFERENCE_READY side=L count=22';a=r.analyze('header\n'+p)
  self.assertEqual(a['controller_core']['records'][0]['message'],p);self.assertEqual(a['controller_core']['records'][0]['source_line'],2)
 def test_old_logs_supported(self):self.assertEqual(r.analyze('V66.94-FINGER CURVE_REFERENCE_READY side=R count=22')['controller_core']['reported_reference_ready'],1)
 def test_native_request_not_grab(self):
  a=r.analyze("V66.90-F5-ACTION REQUEST seq=1 targetId=7 name='door_l_b_o'")
  self.assertEqual(a['controller_core']['native_event_counts']['F5-ACTION REQUEST'],1);self.assertFalse(a['continuity_contact']['physical_grabbing_implemented'])
 def test_gap_details(self):
  g=r.analyze('V66.90-PLAYBACK GAP reason=INVALID_FIELD field=1 layer=4 entry=0')['continuity_contact']['playback_gaps'][0]
  self.assertEqual(g['field'],'1');self.assertEqual(g['layer'],'4')
 def test_complete_raw_not_endpoint(self):
  s='\n'.join(f'V66.95-FINGER RAW_REFERENCE side=R category=0 sample=5 index={i} count=2 joint={i}' for i in range(2))
  g=r.analyze(s)['controller_core']['raw_reference_groups'][0]
  self.assertTrue(g['complete_logged_array']);self.assertFalse(g['authored_endpoint_verified'])
if __name__=='__main__':unittest.main()
