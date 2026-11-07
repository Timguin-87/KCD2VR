import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from source_contract5916c3 import assert_current_identity, assert_filtered_pad_route
import pathlib, unittest

ROOT = pathlib.Path(__file__).resolve().parents[1]

class ReviewFixes66112R1(unittest.TestCase):
    def test_build_markers_match(self):
        assert_current_identity(ROOT)

    def test_hands_do_not_drive_spine2_reach_lean(self):
        s=read_source(ROOT/"src/vrik/coherent_pose.inl", errors="ignore")
        self.assertNotIn("stagedChest", s)
        self.assertNotIn("stable_reach_excess(stagedChest", s)
        self.assertIn("hands never drive the torso/head foundation", s)

    def test_vertical_safe_zone_is_independent(self):
        s=read_source(ROOT/"src/mod.cpp", errors="ignore")
        self.assertIn("vr_ui_safe_height_.load", s)
        self.assertNotIn("const float safeH=safeW", s)

    def test_theater_and_projection_are_mutually_exclusive(self):
        s=read_source(ROOT/"src/mod.cpp", errors="ignore")
        self.assertIn("theater_presented", s)
        xr=read_source(ROOT/"src/xr_manager.cpp", errors="ignore")
        block=xr[xr.index("const XrCompositionLayerBaseHeader* layers"):xr.index("if(have_flat_laser)")]
        self.assertIn("if (!flat_game_requested)", block)
        self.assertIn("&projection_layer", block)
        self.assertIn("&flat_game_quad", block)
        self.assertIn("one visual owner at a time", block)

if __name__ == "__main__":
    unittest.main()
