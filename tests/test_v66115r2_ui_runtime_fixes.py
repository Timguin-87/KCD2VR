import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
import pathlib, unittest
ROOT=pathlib.Path(__file__).resolve().parents[1]
class V66115R2UiRuntimeFixes(unittest.TestCase):
    def test_uniform_hud_delegates_fixed_center_geometry(self):
        s=read_source(ROOT/"src/flashui_probe.cpp", errors="ignore")
        body=s[s.index("bool ApplyUniformReadableHudLayout"):s.index("bool ApplyUniformDialogueLayout")]
        self.assertIn("ComputeCentered",body)
        self.assertIn("(void)offsetX;(void)offsetY",body)
        self.assertNotIn('FindMovieClip(hud,"MasterSwitch"',body)

    def test_uniform_mode_reapplies_live_controls(self):
        s=read_source(ROOT/"src/mod.cpp", errors="ignore")
        for token in ["old_ui_compactness","old_ui_center_x","old_ui_center_y","old_ui_dialogue_scale"]:
            self.assertIn(token,s)
        self.assertIn("std::fabs(old_ui_safe_width-ui_safe_width)",s)
        self.assertIn("std::fabs(old_ui_safe_height-ui_safe_height)",s)
    def test_dialogue_does_not_overwrite_captured_parent_geometry(self):
        m=read_source(ROOT/"src/mod.cpp", errors="ignore")
        body=m[m.index("enum class HudLayoutOwner"):m.index("const int f3_view_marriage_mode")]
        self.assertNotIn("ApplyUniformDialogueLayout",body)
        self.assertIn("ApplyUniformReadableHudLayout",body)

    def test_context_b_is_edge_pulsed(self):
        s=read_source(ROOT/"src/touch_bindings.cpp", errors="ignore")
        self.assertIn("V66.115R2-CONTEXT B TAP -> native Escape/Back pulse",s)
        self.assertIn("V66.115R2-CONTEXT B HOLD 700ms -> native Escape/Back pulse",s)
    def test_theater_keeps_projection_under_quad(self):
        s=read_source(ROOT/"src/xr_manager.cpp", errors="ignore")
        block=s[s.index("const XrCompositionLayerBaseHeader* layers"):s.index("if(have_flat_laser)")]
        self.assertIn("projection_layer",block)
        self.assertIn("flat_game_requested",block)
        self.assertIn("flat_game_quad",block)
    def test_theater_exit_no_pair_wait(self):
        s=read_source(ROOT/"src/theater_exit113.h", errors="ignore")
        self.assertNotIn("completed <= baseline",s)
        self.assertIn("return nativeTheater",s)
if __name__=="__main__": unittest.main()
