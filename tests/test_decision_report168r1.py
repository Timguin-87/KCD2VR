from pathlib import Path
import io
import sys
import tempfile
import unittest
sys.path.insert(0,str(Path(__file__).parents[1]/'tools'))
import report_session168 as base
import report_decisions168r1 as report
class DecisionReportTests(unittest.TestCase):
 def inspect(self,body):
  item=base.inspect_stream(io.BytesIO(body.encode()),'test.log',100000)
  return report.decisions({'files':[item],'duplicates_not_recounted':[],'errors':[]})
 def test_old_log_not_new_acceptance(self):
  d=self.inspect('KCD2VR BUILD KCD2VR-V66.164R4-HUD-PERF-FASTPATH\nMASTER_FACTORY nativeGroupMatch=1 vrPermit=1\n')
  self.assertEqual(d['current_build_files'],0)
  self.assertEqual(next(x for x in d['decisions'] if x['gap']=='thrust_only_counter_coverage')['recorded_count'],0)
 def test_query_does_not_complete_kick(self):
  d=self.inspect('KCD2VR BUILD '+report.EXPECTED_BUILD+'\nCLOSE_QUERY type=4\n')
  r=next(x for x in d['decisions'] if x['gap']=='armed_R3_kick_selection')
  self.assertEqual(r['recorded_count'],1);self.assertEqual(r['status'],'IMPLEMENTATION_REQUIRED')
 def test_factory_is_not_success(self):
  d=self.inspect('KCD2VR BUILD '+report.EXPECTED_BUILD+'\nMASTER_FACTORY nativeGroupMatch=1 vrPermit=1\n')
  self.assertEqual(next(x for x in d['decisions'] if x['gap']=='thrust_only_counter_coverage')['status'],'OBSERVED_NOT_PROVEN')
 def test_hud_bypass_and_partial_capture(self):
  d=self.inspect('KCD2VR BUILD '+report.EXPECTED_BUILD+'\nV66.168 CAPTURE domain=HUD session=2 state=BEGIN\nV66.168R1 HUD_COVERAGE phase=SAMPLE contendedUnmasked=1\n')
  self.assertEqual(d['partial_current_captures'],['test.log']);self.assertEqual(len(d['hud_unmasked_bypass_observed']),1)
 def test_lifecycle_does_not_prove_constraint(self):
  d=self.inspect('KCD2VR BUILD '+report.EXPECTED_BUILD+'\nF8_LIFECYCLE constraint=1\n')
  self.assertEqual(next(x for x in d['decisions'] if x['gap']=='real_held_object')['status'],'IMPLEMENTATION_REQUIRED')
 def test_full_hash_duplicates_not_recounted(self):
  with tempfile.TemporaryDirectory() as folder:
   a=Path(folder)/'a.log';b=Path(folder)/'b.log'
   a.write_text('KCD2VR BUILD '+report.EXPECTED_BUILD+'\nCLOSE_QUERY type=4\n');b.write_bytes(a.read_bytes())
   d=report.decisions(base.inspect_paths([a,b]))
   self.assertEqual(d['current_build_files'],1);self.assertEqual(len(d['source_review']['duplicates_not_recounted']),1)
if __name__=='__main__':unittest.main()
