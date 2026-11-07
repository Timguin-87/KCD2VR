"""Report/guard regressions, no game calls or inferred headset outcomes."""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
import importlib.util
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]
spec = importlib.util.spec_from_file_location("report_bow169r2", ROOT / "tools/report_bow169r2.py")
report = importlib.util.module_from_spec(spec)
spec.loader.exec_module(report)


class BowReport169R2(unittest.TestCase):
    def test_old_marker_is_not_new_acceptance(self):
        r = report.summarize([("old.log", b"KCD2VR-V66.164R4-HUD-PERF-FASTPATH")])
        self.assertFalse(r["contains_expected_marker"])
        self.assertEqual(r["bow"]["sparse_records"], 0)

    def test_deduplicates_renamed_logs(self):
        raw = b"V66.169R2 SWEEP_SUMMARY TIME_BUDGET=4 ANGULAR_PREFIX=10"
        r = report.summarize([("a.log", raw), ("b.log", raw)])
        self.assertEqual(r["sweeps"]["aggregate_query_call_deltas"], {"TIME_BUDGET": 4, "ANGULAR_PREFIX": 10})
        self.assertEqual(len(r["duplicate_sources"]), 1)

    def test_details_do_not_double_count_summary(self):
        r = report.summarize([("run", b"V66.169R2 SWEEP_COVERAGE reason=TIME_BUDGET\nV66.169R2 SWEEP_SUMMARY TIME_BUDGET=5")])
        self.assertEqual(r["sweeps"]["sparse_detail_counts"]["TIME_BUDGET"], 1)
        self.assertEqual(r["sweeps"]["aggregate_query_call_deltas"]["TIME_BUDGET"], 5)

    def test_final_geometry_and_nan(self):
        raw = b"V66.169R2 BOW_POSE stage=0 geometryReady=1 posePacketPublished=1 rightOwned=1 wristErrorM=0.003 reachExcessM=nan armTorsoProxyGapM=-0.02"
        r = report.summarize([("run", raw)])
        self.assertEqual(r["bow"]["negative_final_torso_proxy_gap_records"], 1)
        self.assertEqual(r["bow"]["sampled_maxima"], {"wristErrorM": .003})

    def test_no_geometry_no_clearance_claim(self):
        raw = b"V66.169R2 BOW_POSE stage=1 geometryReady=0 posePacketPublished=0 rightOwned=0 wristErrorM=0 reachExcessM=0 armTorsoProxyGapM=-1"
        r = report.summarize([("run", raw)])
        self.assertEqual(r["bow"]["rejected_records"], 1)
        self.assertEqual(r["bow"]["negative_final_torso_proxy_gap_records"], 0)

    def test_production_bow_capture_gated_before_proxy_work(self):
        text = read_source(ROOT / "src/vrik/coherent_pose.inl")
        self.assertIn("tx.bowContext169r2=manualPose169r2&&kcd2vr::diagnostics::bow169r2::recording(ts.tick)", text)
        self.assertIn("if(tx.bowContext169r2){auto& r=tx.bowEvidence169r2;", text)
        self.assertNotIn("g_phys_frame6698.read", text)
        self.assertIn("tx.physicalOutput.bowHand169r2", text)


if __name__ == "__main__":
    unittest.main()
