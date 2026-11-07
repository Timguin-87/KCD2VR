import importlib.util
from pathlib import Path
import unittest

spec = importlib.util.spec_from_file_location('runtime122', Path(__file__).resolve().parents[1]/'tools/analyze_runtime122.py')
runtime122 = importlib.util.module_from_spec(spec)
spec.loader.exec_module(runtime122)


class RuntimeReport122(unittest.TestCase):
    def test_native_candidate_is_not_damage(self):
        result=runtime122.analyze([
            'CONTACT_PATH session=1 sequence=1 stopMask=1 candidateCopied=0',
            'CONTACT_PATH session=1 sequence=2 stopMask=0 candidateCopied=1'])
        self.assertEqual(result['reference_contact_paths'],dict(records=2,candidate_copy_records=1,stop_masks={'1':1,'0':1}))
        self.assertNotIn('accepted_damage',result)

    def test_missing_return_is_not_null_body(self):
        result=runtime122.analyze([
            'INTERACTION_BODY session=1 requestSeq=1 stage=WATCH',
            'INTERACTION_BODY session=1 requestSeq=2 stage=WATCH',
            'INTERACTION_BODY session=1 requestSeq=2 stage=RETURN body=0 callerRva=ABC'])
        self.assertEqual(result['reference_interaction_watches']['1:1']['native_return_records'],0)
        self.assertEqual(result['reference_interaction_watches']['1:2']['bodies'],['0'])
    def test_missing_is_not_success(self):
        result = runtime122.analyze(['V66.120 PREDICTION reason=2 context=336'])
        self.assertEqual(len(result['missing_diagnostics']), 4)
        self.assertEqual(result['prediction_first_failure_totals'], {})
        self.assertIsNone(result['camera_minus_solved_z_range'])

    def test_totals_are_not_sample_counts(self):
        result = runtime122.analyze([
            'PREDICTION_WITNESS gates={CONTACT_DEFERRED:12,NATIVE_NOT_ATTACKING:30}',
            'PREDICTION_WITNESS gates={CONTACT_DEFERRED:15,NATIVE_NOT_ATTACKING:30}',
        ])
        self.assertEqual(result['prediction_first_failure_totals'], {'CONTACT_DEFERRED': 15, 'NATIVE_NOT_ATTACKING': 30})

    def test_eye_bone_offset_and_unknown_root(self):
        result = runtime122.analyze([
            'CAMERA_BODY targetErrorM=0.16 ageMs=16 cameraMinusSolvedZ=-0.11',
            'CAMERA_BODY targetErrorM=0.02 ageMs=31 cameraMinusSolvedZ=0.09',
            'WEAPON_WORLD currentWorldRead=0 rootDeltaM=-1 weaponWorldGapM=-1 publishDeltaM=0',
        ])
        self.assertEqual(result['camera_minus_solved_z_range'], [-0.11, 0.09])
        self.assertEqual(result['largest_observed']['targetErrorM']['value'], .16)
        self.assertNotIn('rootDeltaM', result['largest_observed'])

    def test_cost_is_weighted_and_scopes_separate(self):
        result = runtime122.analyze([
            'V66.122 PERF stage=EQUIPMENT_LIVE calls=100 avgUs=100 maxUs=300 pageQueries=200',
            'V66.122 PERF stage=EQUIPMENT_LIVE calls=10 avgUs=1000 maxUs=4000 pageQueries=20',
            'V66.122 PERF stage=EQUIPMENT_METADATA calls=2 avgUs=2000 maxUs=3000 pageQueries=100',
        ])
        self.assertAlmostEqual(result['performance']['EQUIPMENT_LIVE']['weighted_avg_wall_us'], 20000/110)
        self.assertEqual(result['performance']['EQUIPMENT_LIVE']['page_queries'], 220)
        self.assertEqual(result['performance']['EQUIPMENT_METADATA']['calls'], 2)
