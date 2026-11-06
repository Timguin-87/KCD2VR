import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
import unittest
ROOT=Path(__file__).parents[1]
def text(p):return read_source(ROOT/p)
class Capture168R1Tests(unittest.TestCase):
 def test_capture_keys_have_no_mode_setters_or_shift_gate(self):
  s=text('src/mod.cpp')
  for key,end in [('hud','// V66.128 F1:'),('combat','// V66.85 AA OWNERSHIP'),('interaction','frik_pose_probe::MiscInteractionDiagTick();')]:
   p=s[s.index('if(capturePress.'+key+')'):];p=p[:p.index(end)]
   for bad in ('shift_down','CycleHolsterCombatDiagMode','CycleMiscInteractionDiagMode','SetWeaponControlMode(',
               'SetCombatCensusMode(', 'CancelFocusedTests167(', 'f2_ui_layers_test=!'):
    self.assertNotIn(bad,p,key)
  self.assertNotIn('f2_ui_layers_test=!f2_ui_layers_test',s)
  # C7 adds an OPTIONAL capture-only chord. Plain F2 retains its existing ladder;
  # F3/F5 do not gain modifier requirements. The chord cannot mutate HUD/gameplay.
  self.assertNotIn('SHIFT_F3',s);self.assertNotIn('SHIFT_F5',s)
  capture=s[s.index('if(capturePress.hud&&'):s.index('}else if(capturePress.hud)')]
  self.assertIn('SHIFT_F2_CAPTURE_ONLY',capture)
  self.assertIn('Domain::Hud,!was',capture)
  for forbidden in ('hudLadder36.Press','SetHudExperiment36','SetDesktopMirror36',
                    'vr_hud_reapply_requested_', 'SetWeaponControlMode', 'SaveMenuSettings'):
   self.assertNotIn(forbidden,capture)

 def test_capture_api_does_not_change_physics_or_modes(self):
  s=text('src/frik_pose_probe.cpp');b=s[s.index('unsigned SetCombatObservation168r1('):s.index('void SetHolsterCombatDiagMode(')]
  for bad in ('SetHolsterCombatDiagMode(', 'SetMiscInteractionDiagMode(', 'SetCombatCensusMode(', 'SetWeaponControlMode(',
              'g_pose_epoch6689.fetch_add', 'NativeInputArm117(', 'NativeMotionArm112(', 'PhysCallAction'):
   self.assertNotIn(bad,b)
  self.assertIn('ArmCombatInput101(false)',b)
 def test_capture_is_not_native_reference(self):
  self.assertNotIn('captureOnly',text('src/diagnostics/lab_state.h'))
  s=text('src/diagnostics/native_hit_probe134.inl')
  ref=next(x for x in s.splitlines() if 'const bool reference=' in x)
  self.assertNotIn('CaptureOnly168r1',ref)
 def test_hud_transition_owner_removes_render_contention_path(self):
  s=text('src/flashui_probe.cpp')
  self.assertNotIn('HudRender162r1(',s);self.assertNotIn('inline_at(std::move(req),&HudRender',s)
  self.assertIn('g_visibility170.Update',s);self.assertIn('renderHook=OFF',s)
 def test_reference_observation_eligibility_not_trajectory_authority(self):
  p=text('src/vrik/physical_native.inl')
  self.assertIn('F11Capture167(1)||capture::captureOnly168r1(now)',p)
  self.assertIn('ReferenceSweepOwner113(collisions)',p)
  self.assertIn('const bool replaced=local&&NativeTrajectoryPrepare112',p)
  self.assertIn('replaced?&vrPrevious:previous,replaced?&vrCurrent:current,dt',p)
  self.assertIn('!PredictionCapture120()&&!cc::captureOnly168r1(now)',text('src/vrik/material_observer116.inl'))
if __name__=='__main__':unittest.main()
