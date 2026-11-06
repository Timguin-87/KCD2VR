import importlib.util
import math
from pathlib import Path
import unittest

spec = importlib.util.spec_from_file_location('timing', Path(__file__).resolve().parents[1] / 'tools/combat_event_timing_d60.py')
timing = importlib.util.module_from_spec(spec)
spec.loader.exec_module(timing)


class TimingTests(unittest.TestCase):
    def test_sign_flip_is_not_movement(self):
        r = timing.angular_peak([[0, 0, 0, 0, 1], [1, 0, 0, 0, -1]], 1/30)
        self.assertEqual(r['peak']['radiansPerSecond'], 0)

    def test_sparse_keys_report_real_frame_and_time(self):
        q = math.sqrt(0.5)
        r = timing.angular_peak([[10, 0, 0, 0, 2], [40, 0, 0, q, q]], 1/60)
        self.assertEqual(r['peak']['keyFrameEnd'], 40)
        self.assertAlmostEqual(r['peak']['radiansPerSecond'], math.pi)

    def test_velocity_uses_key_duration(self):
        q = math.sqrt(0.5)
        r = timing.angular_peak([[0, 0, 0, 0, 1], [30, 0, 0, q, q], [31, 0, 0, 1, 0]], 1/30)
        self.assertEqual(r['peak']['keyFrameEnd'], 31)

    def test_invalid_key_data_does_not_invent_zero_contact(self):
        r = timing.angular_peak([[0, 0, 0, 0, 0], [0, 0, 0, 0, 1]], 1/30)
        self.assertEqual(r['status'], 'NO_VALID_INTERVAL')
        self.assertEqual(r['invalidIntervals'], 1)

    def test_empty_hands_and_exact_path_join(self):
        clip = {'path': 'A/B.CAF', 'startFrame': 10, 'endFrame': 70, 'secondsPerFrame': 1/60, 'joints': []}
        r = timing.join_clip('clip', clip, {'a/b.caf': [{'name': 'AnimationHit', 'time': '.2'}]}, {}, {})
        self.assertEqual(r['localHandMotion'], {})
        self.assertEqual(r['authoredFrameSpanSeconds'], 1)
        self.assertEqual(r['cafEventAttributesRaw'][0]['time'], '.2')
        self.assertEqual(r['retailFragments'], [])

    def test_procedure_timing_remains_raw_per_layer(self):
        raw = b'<AnimDB><FragmentList><CombatHitComboGen><Fragment GUID="x"><AnimLayer><Animation name="clip"/></AnimLayer><ProcLayer><Blend ExitTime=".3"/><Procedural type="AnimEvent"><ProceduralParams><EventName value="SyncHit"/></ProceduralParams></Procedural><Blend ExitTime=".5"/><Procedural type=""/></ProcLayer></Fragment></CombatHitComboGen></FragmentList></AnimDB>'
        row = timing.fragment_index(raw)['clip'][0]
        self.assertEqual(row['procedures'][1]['precedingBlendRaw']['ExitTime'], '.5')
        self.assertEqual(row['procedures'][0]['parameters'][0]['EventName']['value'], 'SyncHit')


if __name__ == '__main__':
    unittest.main()
