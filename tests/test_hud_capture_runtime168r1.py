"""V170 replaces the draw-mask fixture with an event-only production owner.

The C++ hud_transition170 target tests policy behavior over 30,000 steady samples.
These guards check wiring and exclusion of the retired render path.
"""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
import unittest
ROOT=Path(__file__).parents[1]
class HudCaptureRuntimeTests(unittest.TestCase):
 def test_no_hud_render_mutation_or_timer(self):
  source=read_source(ROOT/'src/flashui_probe.cpp')
  for retired in ('HudRender162r1(', 'InstallHudRender162r1(', 'HudClock166(', 'mask.Begin(', 'g_hidden_tick162r1'):
   self.assertNotIn(retired,source)
  self.assertIn('g_visibility170.Update(io,',source)
  self.assertIn('if(!report.changed)return;',source)
 def test_manual_mode_and_visibility_controls(self):
  mod=read_source(ROOT/'src/mod.cpp');menu=read_source(ROOT/'src/vr_menu.cpp')
  self.assertIn('const bool requested_auto_hud=state.auto_correct_hud;',mod)
  self.assertIn('PtIn(kV4UiAuto',menu)
  self.assertNotIn('ApplyAutoVrLayout(true',mod)
  self.assertNotIn('"VrManualHudVersion","171",explicit_ini',mod)
  self.assertIn('flashui_probe::UpdateHudVisibility170(false,userHidden170',mod)
  for label in ('Center point','Status icons','Health size','Manual HUD placement and visibility'):
   self.assertIn(label,menu)
  self.assertNotIn('PtIn(kV4AutoHud',menu)
  self.assertNotIn('PtIn(kV4HudNativeCanvas',menu)
 def test_prompts_never_hidden_and_no_wrist_or_hint_refresh(self):
  source=read_source(ROOT/'src/ui_hud_transition170.h');mod=read_source(ROOT/'src/mod.cpp')
  self.assertIn('if(!group)continue;',source)
  self.assertIn('const bool wristMode=false;',mod)
  self.assertNotIn('last_hint_reassert_ms',mod)
  self.assertNotIn('flashui_probe::PublishContextualHudMask',mod)
  self.assertIn('if(!native_ui_snapshot.Open(ui_native_state::Loading)&&!native_ui_snapshot.Open(ui_native_state::Progress))',mod)
if __name__=='__main__':unittest.main()
