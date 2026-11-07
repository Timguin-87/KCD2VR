"""Offline tests for actual report parser; fixtures are synthetic, not game evidence."""
import importlib.util,json,unittest
from pathlib import Path
R=Path(__file__).resolve().parents[1]
spec=importlib.util.spec_from_file_location('report',R/'tools/analyze_v6690.py');m=importlib.util.module_from_spec(spec);spec.loader.exec_module(m)
BASE='''V66.90-PLAYBACK SNAP tick=1000 epoch=1 seq=1 role=HENRY entries=2
V66.90-PLAYBACK CLIP tick=1000 epoch=1 seq=1 role=HENRY id=2 activated=1 progression=ADVANCED name="combat_stab"
V66.90-PLAYBACK CLIP tick=1000 epoch=1 seq=1 role=HENRY id=3 activated=0 progression=FIRST_SAMPLE name="combat_sync_death"
V66.90-F3-CONTACT_CONTEXT tick=1100 epoch=1 eventSeq=5 entity=22
'''
class ReportTests(unittest.TestCase):
 def test_active_queued(self):
  r=m.analyze(BASE);d={c['name']:c for c in r['clips']};self.assertEqual(d['combat_stab']['activated_samples'],1);self.assertEqual(d['combat_sync_death']['activated_samples'],0);self.assertEqual(d['combat_sync_death']['queued_samples'],1)
 def test_correlates_only_prior(self):
  r=m.analyze(BASE);self.assertEqual(r['events'][0]['prior_playback']['age_ms'],100);self.assertEqual(r['events'][0]['prior_playback']['activated_names'],['combat_stab'])
 def test_no_future(self):
  r=m.analyze(BASE.replace('tick=1100','tick=900'));self.assertIsNone(r['events'][0]['prior_playback'])
 def test_no_old(self):
  r=m.analyze(BASE.replace('tick=1100','tick=1700'));self.assertIsNone(r['events'][0]['prior_playback'])
 def test_no_other_epoch(self):
  r=m.analyze(BASE.replace('tick=1100 epoch=1','tick=1100 epoch=2'));self.assertIsNone(r['events'][0]['prior_playback'])
 def test_gap_invalidates(self):
  s=BASE.replace('V66.90-F3-CONTACT_CONTEXT','V66.90-PLAYBACK GAP tick=1050 epoch=1 role=HENRY reason=CHANGED\nV66.90-F3-CONTACT_CONTEXT');r=m.analyze(s);self.assertIsNone(r['events'][0]['prior_playback'])
 def test_missing_lines_incomplete(self):
  r=m.analyze(BASE.replace('entries=2','entries=3'));self.assertTrue(r['snapshots'][0]['header']['incomplete']);self.assertIsNone(r['events'][0]['prior_playback'])
 def test_orphan(self):
  r=m.analyze('\n'.join(BASE.splitlines()[1:]));self.assertTrue(r['snapshots'][0]['header']['incomplete'])
 def test_truncation(self):
  r=m.analyze(BASE+'V66.90-PLAYBACK TRUNCATED seq=1 emitted=2 total=40\n');self.assertTrue(r['snapshots'][0]['header']['incomplete'])
 def test_static_not_native(self):
  c={'clips':{'combat_stab':[{'fragment':'CombatAttackSyncDeathGen'}]}};r=m.analyze(BASE,c)
  row=next(x for x in r['clips'] if x['name']=='combat_stab');self.assertEqual(row['static_fragment_candidates'],['CombatAttackSyncDeathGen']);self.assertEqual(row['native_finisher_eligibility'],'UNKNOWN');self.assertEqual(row['runtime_fragment'],'UNKNOWN')
 def test_no_health_inference(self):
  r=m.analyze(BASE);self.assertEqual(r['events'][0]['native_accepted_hit'],'UNKNOWN');self.assertEqual(r['events'][0]['native_finisher_eligibility'],'UNKNOWN')
 def test_start_is_not_playback(self):
  r=m.analyze("V66.89-ANIM-START success=1 role=HENRY name='anything'\n");self.assertEqual(r['snapshot_count'],0);self.assertEqual(r['clips'][0]['activated_samples'],0);self.assertIn('do not establish',m.markdown(r))
 def test_queue_drops(self):
  r=m.analyze(BASE+'V66.89-DIAGNOSTICS droppedTotal=50\nV66.89-DIAGNOSTICS droppedTotal=55\n');self.assertEqual(r['log_queue_dropped_total'],55)
 def test_quoted_spaces(self):
  self.assertEqual(m.fields('name="animation with spaces" id=1')['name'],'animation with spaces')
 def test_unknown_sample_serial_not_used(self):
  s=BASE.replace('tick=1100','tick=1100 playbackSeq=888');r=m.analyze(s);self.assertEqual(r['events'][0]['prior_playback']['seq'],'1')
 def test_json_serializable(self):
  self.assertEqual(json.loads(json.dumps(m.analyze(BASE)))['snapshot_count'],1)
if __name__=='__main__':unittest.main()
