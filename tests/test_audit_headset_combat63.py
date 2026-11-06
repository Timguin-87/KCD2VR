"""Keep mode ownership and evaluation/application distinctions in log audits."""
import importlib.util
from pathlib import Path
import tempfile
import unittest

spec = importlib.util.spec_from_file_location('combat63', Path(__file__).resolve().parents[1] / 'tools/audit_headset_combat63.py')
module = importlib.util.module_from_spec(spec)
spec.loader.exec_module(module)


class CombatAudit63(unittest.TestCase):
    def test_actual_mode_receipt_overrides_old_main_hotkey(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory)
            (path/'x-main.log').write_text('[2026-10-19 01:00:00.000] [WARNING] :: F4_PRESS2_VR_DIRECT_CONTACT_LAB\n')
            (path/'x-diag.log').write_text(
                '[2026-10-19 01:00:00.000] V66.175 MODE state=F4_PRESS2_VR_DIRECT_CONTACT_LAB\n'
                '[2026-10-19 01:00:01.000] V66.175 MODE state=F4_PRESS1_SYNCHRONIZED_NATIVE_ACTION_VR_CONTACT\n'
                '[2026-10-19 01:00:02.000] V66.178R30 GEOM_HIT serial=1 returned=1\n')
            report = module.audit(path, 'x')
            self.assertEqual(report['eventRecords']['HYBRID']['GEOM_HIT'], 1)
            self.assertNotIn('GEOM_HIT', report['eventRecords'].get('DIRECT', {}))

    def test_evaluation_is_not_a_completed_native_application(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory)
            (path/'x-main.log').write_text('')
            (path/'x-diag.log').write_text(
                '[2026-10-19 01:00:00.000] V66.175 MODE state=F4_PRESS1_SYNCHRONIZED_NATIVE_ACTION_VR_CONTACT\n'
                '[2026-10-19 01:00:01.000] C59 SUBMIT_HIT producer=COLLISION effect=EVALUATION applyNative=0 returned=1 attacker=1(PLAYER) target=2 blocked=1 perfectBlock=1\n'
                '[2026-10-19 01:00:02.000] C59 SUBMIT_HIT producer=COLLISION effect=APPLICATION applyNative=1 returned=1 attacker=1(PLAYER) target=2 blocked=1 perfectBlock=0\n')
            report = module.audit(path, 'x')
            applications = report['nativeApplications']['HYBRID']['HenryAttacker']
            self.assertEqual(applications['records'], 1)
            self.assertEqual(applications['blocked'], 1)
            self.assertNotIn('perfectBlock', applications)
            self.assertEqual(report['eventRecords']['HYBRID']['SUBMIT_HIT'], 2)


if __name__ == '__main__':
    unittest.main()
