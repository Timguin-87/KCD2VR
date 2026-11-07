import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
import unittest
ROOT=Path(__file__).resolve().parents[1]
COH=read_source(ROOT/'src/vrik/coherent_pose.inl'); MOD=read_source(ROOT/'src/mod.cpp')
class ContactTopology159(unittest.TestCase):
    def test_marker(self): self.assertIn('KCD2VR-V66.174R2-COMBINED-CONTACT-RECOVERY',MOD)
    def test_contact_coverage_log(self):
        self.assertIn('V66.159 HAND_CONTACT',COH)
        self.assertIn('fingerMask159',COH); self.assertIn('stagedContact[side].planeCount',COH)
        self.assertIn('worldProxy=PALM_PLUS_FIVE_FINGERS',COH)
    def test_self_collision_is_gated_proxy(self):
        self.assertIn('collisionMode&&selfContact167',COH)
        self.assertIn('const bool selfContact167=collisionMode;',COH) # explicit contact rung, not diagnostic lease
        self.assertIn('!nativeFingerCapture',COH)
        self.assertIn('freeContact164R1[side]=contactYield164R1[side]==0',COH)
        self.assertIn('if(!freeContact164R1[side])',COH)
        self.assertIn('torsoGap159',COH); self.assertIn('otherHandGap159',COH)
        self.assertIn('selfBodyCollision=TORSO_PROXY otherHandCollision=RUNG_DEPENDENT_VISUAL_PROXY dynamicEntityCollision=QUALIFIED_QUERY_ONLY',COH)
        self.assertIn('kcd2vr::hands59c7f::active()&&geometry164[0]&&geometry164[1]',COH)
        self.assertIn('kcd2vr::hand_shape59c7f::separate',COH)
if __name__=='__main__': unittest.main()
