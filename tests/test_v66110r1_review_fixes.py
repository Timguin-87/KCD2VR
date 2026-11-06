import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from source_contract5916c3 import assert_current_identity, assert_filtered_pad_route
from pathlib import Path
import unittest

R = Path(__file__).resolve().parents[1]

class ReviewFixes66110R1(unittest.TestCase):
    def test_build_markers_match(self):
        assert_current_identity(R)

    def test_intent_translation_does_not_rotate_with_head(self):
        s = read_source(R/"src/vrik/coherent_pose.inl")
        a = s.index("// R1: subtract common translation in tracking axes.")
        b = s.index("PhysInputWorld6698", a)
        block = s[a:b]
        self.assertIn("XrToCryV(Sub(side?ts.left:ts.right,ts.head))", block)
        # R32C1 corrects orientation to native wrist-local. Translation still
        # subtracts HMD position in tracking axes and is never HMD-rotated.
        self.assertIn("pi.intentHands[side]={PQ(intentHand),PV(rawP)}", block)
        self.assertNotIn("ts.hq", block)
        self.assertNotIn("QRotateV(intentHeadInv", block)
        self.assertNotIn("QMul(intentHeadInv,rawQ)", block)

    def test_collision_owner_is_refreshed(self):
        s = read_source(R/"src/vrik/physical_native.inl")
        a = s.index("g_phys_equipment6698.publish(e)")
        b = s.index("n.player=e.player", a)
        block = s[a:b]
        self.assertNotIn("n.player!=e.player||!g_phys_collision_owner110", block)
        self.assertIn("Read(static_cast<unsigned char*>(actor)+0x218,collisions)", block)
        self.assertIn("g_phys_collision_owner110.store(nullptr", block)

if __name__ == "__main__":
    unittest.main()
