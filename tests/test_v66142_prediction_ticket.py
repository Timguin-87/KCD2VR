import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
import unittest
class NativeStrikeEligibility146(unittest.TestCase):
    def test_request_is_not_native_acceptance(self):
        root=Path(__file__).resolve().parents[1]
        pred=read_source(root/'src/vrik/native_prediction120.inl')
        self.assertIn('native_admission147::predictor(',pred)
        self.assertIn('PredictionAdmissionContract147(b)',pred)
        self.assertNotIn('PASS_VR_COMMIT_142',pred)
        self.assertNotIn('!s.state121.attacking&&!physicalStrike142',pred)
