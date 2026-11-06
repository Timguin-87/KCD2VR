
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
import unittest
from pathlib import Path

ROOT=Path(__file__).resolve().parents[1]

class V66115R3VerticalBoundaries(unittest.TestCase):
    def test_upper_and_lower_controls_are_wired_separately(self):
        menu=read_source(ROOT/"src/vr_menu.cpp", encoding="utf-8")
        self.assertIn('L"Upper HUD boundary"',menu)
        self.assertIn('L"Lower HUD boundary"',menu)
        self.assertIn("begin_drag(kV4UiTopBoundary,DragControl::UiTopBoundary)",menu)
        self.assertIn("begin_drag(kV4UiBottomBoundary,DragControl::UiBottomBoundary)",menu)

    def test_geometry_keeps_center_and_separate_spans(self):
        src=read_source(ROOT/"src/ui_native_canvas.h", encoding="utf-8")
        self.assertIn("fit.centerX=960.f;fit.centerY=540.f",src)
        self.assertIn("fit.upperSpan=",src)
        self.assertIn("fit.lowerSpan=",src)
        self.assertIn("ny<0.f?fit.upperSpan:fit.lowerSpan",src)
        self.assertNotIn(".5f*(fitTop+fitBottom)",src)

    def test_uniform_mode_fixes_vertical_center(self):
        mod=read_source(ROOT/"src/mod.cpp", encoding="utf-8")
        self.assertIn("vr_ui_top_boundary_.load", mod)
        self.assertIn("vr_ui_bottom_boundary_.load", mod)
        self.assertIn("// authored vertical center fixed; top/bottom boundaries own Y", mod)

    def test_new_boundaries_are_persistent(self):
        mod=read_source(ROOT/"src/mod.cpp", encoding="utf-8")
        self.assertIn('"VrUiTopBoundary"', mod)
        self.assertIn('"VrUiBottomBoundary"', mod)

if __name__=="__main__":
    unittest.main()
