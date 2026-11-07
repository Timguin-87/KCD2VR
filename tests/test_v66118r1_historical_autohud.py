
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
import unittest
ROOT=Path(__file__).resolve().parents[1]
MOD=read_source(ROOT/"src/mod.cpp", errors="ignore")
PROBE=read_source(ROOT/"src/flashui_probe.cpp", errors="ignore")
XR=read_source(ROOT/"src/xr_manager.cpp", errors="ignore")
MENU=read_source(ROOT/"src/vr_menu.cpp", errors="ignore")
INI=(ROOT/"KCD2VR.ini").read_text(errors="ignore")

class HistoricalAutoHud(unittest.TestCase):
    def test_v6522_uniform_math(self):
        self.assertNotIn("ApplyAutoVrLayout(true",MOD)
        self.assertIn("ApplyUniformReadableHudLayout(true,geometry",MOD)
        self.assertIn("uniform?HudLayoutOwner::UniformReadable",MOD)
    def test_overlay_counter_transform_exists(self):
        self.assertIn("ApplyCorrectedMonoGlobalUiOverlayExclusions",PROBE)
        fn=PROBE.split("ApplyCorrectedMonoGlobalUiOverlayExclusions",1)[1].split("LastAutoLayoutStageName",1)[0]
        for n in ("Fader","Vignette","RatioStrips","PopUpBackground"):
            self.assertIn(n,fn)
        self.assertIn("1.0f/uniformScale",fn)
    def test_semantic_fallback_off_when_historical_owner_requested(self):
        self.assertIn("f4_vrhud_safe_desired && !theater_presented;",MOD)
        self.assertNotIn("ApplyCorrectedMonoGlobalUi(true",MOD)
    def test_theater_is_single_owner(self):
        block=XR[XR.index("const XrCompositionLayerBaseHeader* layers"):XR.index("if(have_flat_laser)")]
        self.assertIn("if (!flat_game_requested)",block)
        self.assertNotIn("underneath Theater",block)
    def test_menu_and_defaults(self):
        self.assertIn("Manual HUD placement and visibility",MENU)
        self.assertIn("VrAutoCorrectHud = false",INI) # retired option cannot select another owner

if __name__=="__main__":
    unittest.main()
