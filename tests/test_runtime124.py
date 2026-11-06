import sys
import unittest
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]/'tools'))
from analyze_runtime124 import extract


class Runtime124(unittest.TestCase):
    def test_missing_records_are_not_success(self):
        r = extract('KCD2VR BUILD actual', '')
        self.assertEqual(r['runtime_identification']['markers'], ['actual'])
        self.assertIsNone(r['budget_skips_max'])
        self.assertTrue(r['missing_diagnostics'])

    def test_material_serial_requires_session_and_preserves_fallback(self):
        r = extract('', '\n'.join([
            'V66.116 MATERIAL_EFFECT session=1 serial=1 stage=1 row="hit_stopped_sword_slash_player" surfaceId=138 effectId=0',
            'V66.116 MATERIAL_EFFECT session=1 serial=1 stage=2 effectId=196 nativeReturned=1',
            'V66.116 MATERIAL_EFFECT session=2 serial=1 stage=2 effectId=7 nativeReturned=1',
        ]))
        self.assertEqual(len(r['material_dispatches']), 2)
        self.assertEqual(r['material_dispatches']['1:1']['lookup'][0]['effectId'], '0')
        self.assertEqual(r['material_dispatches']['1:1']['execute'][0]['effectId'], '196')
        self.assertEqual(r['native_damage_summary']['records'], 0)

    def test_candidate_copy_does_not_invent_damage(self):
        r = extract('', 'V66.122 CONTACT_PATH session=1 sequence=1 HenryRole=SOURCE seenMask=31 stopMask=0 candidateCopied=1 raw136=00')
        self.assertEqual(r['reference_contact_paths']['candidate_copy_records'], 1)
        self.assertEqual(r['native_damage_summary']['positive_health'], 0)


if __name__ == '__main__': unittest.main()
