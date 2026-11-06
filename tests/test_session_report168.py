import importlib.util
from pathlib import Path
import tempfile
import unittest
import zipfile

spec = importlib.util.spec_from_file_location("report168", Path(__file__).parents[1] / "tools/report_session168.py")
r = importlib.util.module_from_spec(spec)
spec.loader.exec_module(r)

class Session168Tests(unittest.TestCase):
    def test_content_identity_and_causality_limits(self):
        text = """[2026-10-07 00:00:00.000] KCD2VR BUILD KCD2VR-V66.164R4-HUD-PERF-FASTPATH
[2026-10-07 00:00:01.000] V66.27-LIVE-PUBLISH pair=10
[2026-10-07 00:00:02.000] V66.27-LIVE-PUBLISH pair=110
[2026-10-07 00:00:02.100] V66.134 SUBMIT_HIT target=99 hitInfo{target=0 dmg=0.00 death=0} masterStrike=0 prediction=232 sweep=233 redirected=1
[2026-10-07 00:00:02.200] V66.134 SUBMIT_HIT target=99 hitInfo{target=99 dmg=12.00 death=1} masterStrike=0 prediction=0 sweep=0 redirected=0
[2026-10-07 00:00:02.210] V66.134 PROCESS_HIT target=99 hitInfo{target=99 dmg=12.00 death=1}
"""
        with tempfile.TemporaryDirectory() as d:
            p = Path(d) / "V999_newest.log";p.write_bytes(text.encode('utf-8')) # identical bytes on Windows and Linux
            z = Path(d) / "duplicate.zip"
            with zipfile.ZipFile(z, "w") as f:f.writestr("old.log", text)
            report = r.inspect_paths([p,z]);self.assertEqual(len(report["files"]),1)
            self.assertEqual(len(report["duplicates_not_recounted"]),1)
            one = report["files"][0]
            self.assertIn("V66.164R4",one["build_markers"][0])
            self.assertEqual(one["cadence"]["weighted_pairs_per_second"],100)
            self.assertEqual(one["counts"]["submit_hit_records"],2)
            self.assertEqual(one["counts"]["environment_submit_records"],1)
            self.assertEqual(one["counts"]["positive_damage_without_tracked_prediction_ids"],1)
            self.assertNotIn("positive_damage_with_tracked_prediction_ids",one["counts"])

    def test_native_master_permission_is_not_success(self):
        c=r.Collector("synthetic")
        c.feed("[2026-10-07 00:00:00.000] V66.168 CAPTURE domain=COMBAT session=1 state=BEGIN",1)
        c.feed("[2026-10-07 00:00:00.100] V66.168 MASTER_FACTORY nativeGroupMatch=1 vrPermit=1 reject=NONE",2)
        c.feed("[2026-10-07 00:00:00.110] V66.168 MASTER_FACTORY nativeGroupMatch=1 vrPermit=0 reject=NO_THRUST",3)
        out=c.result()
        self.assertEqual(out["counts"]["master_factory_vr_permit"],1)
        self.assertEqual(out["counts"]["master_factory_additional_vr_rejection"],1)
        self.assertNotIn("decoded_masterstrike_flag_records",out["counts"])
        self.assertEqual(len(out["captures_open_at_file_end"]),1)
        self.assertTrue(any("File ends" in x for x in out["warnings"]))
        c.feed("[2026-10-07 00:03:00.000] V66.168 CAPTURE domain=COMBAT session=1 state=EXPIRED",4)
        self.assertFalse(c.result()["captures_open_at_file_end"])

    def test_limits_and_errors_do_not_claim_complete(self):
        import io
        out=r.inspect_stream(io.BytesIO(b"x"*100),"large",10)
        self.assertFalse(out["complete_read"])
        self.assertEqual(out["hash_scope"],"PREFIX_ONLY")
        report=r.inspect_paths([Path("/definitely_missing168/test.log")])
        self.assertEqual(len(report["errors"]),1)

if __name__ == "__main__":unittest.main()
