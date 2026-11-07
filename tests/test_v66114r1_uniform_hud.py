
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]
MOD = read_source(ROOT / "src/mod.cpp", encoding="utf-8", errors="ignore")
PROBE = read_source(ROOT / "src/flashui_probe.cpp", encoding="utf-8", errors="ignore")
CANVAS = read_source(ROOT / "src/ui_native_canvas.h", encoding="utf-8", errors="ignore")
MENU = read_source(ROOT / "src/vr_menu.cpp", encoding="utf-8", errors="ignore")

class UniformHud115Review(unittest.TestCase):
    def test_uniform_mode_uses_readable_parent_transaction_not_master_switch(self):
        assert "ApplyUniformReadableHudLayout" in MOD
        fn = PROBE.split("bool ApplyUniformReadableHudLayout", 1)[1].split("bool ApplyAutoVrLayout", 1)[0]
        assert "g_canvas.Run" in fn
        assert "MasterSwitch" not in fn

    def test_fullscreen_effects_are_not_uniform_hud_anchors(self):
        anchors = CANVAS.split("inline constexpr std::array<const char*,11> Anchors",1)[1].split("};",1)[0]
        for name in ("fader","vignette","ratioStrips","popUp","bbc","cccursor"):
            assert f'"{name}"' not in anchors

    def test_manual_and_requested_f2_parent_owner_preserve_exclusions(self):
        assert "Manual HUD placement and visibility" in MENU
        assert "PtIn(kV4AutoHud" not in MENU
        assert "PtIn(kV4HudNativeCanvas" not in MENU
        assert "const bool requested_auto_hud=state.auto_correct_hud;" in MOD
        assert "PtIn(kV4UiAuto" in MENU
        call = MOD.split("ok=flashui_probe::ApplyWholeReadableHud44c2(", 1)[1].split(";", 1)[0]
        for value in ("vr_ui_safe_width_", "vr_ui_safe_height_", "vr_ui_top_boundary_", "vr_ui_bottom_boundary_"):
            assert value + ".load()" in call

    def test_whole_root_autohud_is_disabled_in_production(self):
        assert "ApplyAutoVrLayout(true" not in MOD
        assert "ApplyCorrectedMonoGlobalUiOverlayExclusions(true" not in MOD
        assert "uniform?HudLayoutOwner::UniformReadable" in MOD
        assert "ComputeCentered" in PROBE

    def test_child_writers_are_excluded_in_uniform_mode(self):
        assert 'last_hint_reassert_ms' not in MOD
        assert 'const bool wristMode=false;' in MOD

    def test_manual_layout_preserves_element_and_prompt_controls(self):
        for control in ('kV4UiElementTab','kV4UiPromptTab','kV4UiApply','kHudStatus170'):
            assert f'PtIn({control}' in MENU
        assert 'vr_hud_native_canvas_fit_.store(false)' in MOD
        assert 'vr_auto_correct_hud_.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrAutoCorrectHud",false))' in MOD

if __name__ == '__main__':
    unittest.main()
