from pathlib import Path
import importlib.util, unittest
P=Path(__file__).resolve().parents[1]/"tools/report_kick_capture_v66101r1.py"
spec=importlib.util.spec_from_file_location("kick_report",P);report=importlib.util.module_from_spec(spec);spec.loader.exec_module(report)
def row(kind,tick,name,session=1,flags=0,actor="AAA",ident=3):
 return f'V66.101R1-COMBAT_CAPTURE session={session} kind={kind} tick={tick} sample=5 epoch=1 actor={actor} mode=2 value=0 id={ident} layer=0 phase=0.5 flags={flags} keys=0/0 grip=1/1 trigger=0/0 name="{name}" meaning=OBSERVED_OR_REQUESTED_NOT_ACCEPTED_HIT'
class KickReport(unittest.TestCase):
 def test_empty_no_false_success(self):
  self.assertEqual(report.analyze([])["kick_observations"],[])
 def test_heavy_static_not_special_or_armed_permission(self):
  s=report.STATIC_KICK;self.assertEqual(s['input_class_name'],'attack_heavy');self.assertEqual(s['armed_kick_permission'],'NOT_ESTABLISHED')
 def test_actual_names_with_different_native_object_types(self):
  out=report.analyze([row(0,1000,'attack_primary'),row(1,1050,'1d-'+report.KICK,actor='BBB')]);k=out['kick_observations'][0]
  self.assertEqual(len(k['prior_native_input_records']),1);self.assertEqual(k['accepted_hit'],'UNKNOWN')
 def test_capture_sessions_do_not_cross(self):
  out=report.analyze([row(0,1000,'attack_primary',session=1),row(1,1050,report.KICK,session=2)]);self.assertFalse(out['kick_observations'][0]['prior_native_input_records'])
 def test_fifo_rows_deduplicated_with_progress_retained(self):
  out=report.analyze([row(1,1050,report.KICK),row(1,1100,report.KICK,flags=4),row(1,1200,report.KICK,flags=4)])
  self.assertEqual(len(out['kick_observations']),1);self.assertEqual(len(out['kick_observations'][0]['fifo_progress_records']),2)
 def test_punch_request_not_hit_and_drop_report(self):
  out=report.analyze([row(4,1000,'NATIVE_PRIMARY_RELEASE'),"V66.101R1-COMBAT_CAPTURE dropped=9 boundedQueue=512"])
  self.assertEqual(len(out['punch_requests']),1);self.assertFalse(out['kick_observations']);self.assertEqual(out['last_reported_drops'],9)
 def test_malformed_ignored(self):
  self.assertEqual(report.parse(['garbage',row(1,100,report.KICK).replace('kind=1','kind=BAD')]),[])
if __name__=='__main__':unittest.main()
