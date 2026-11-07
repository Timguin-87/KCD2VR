import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from source_contract5916c3 import assert_current_identity, assert_filtered_pad_route
import unittest
from pathlib import Path
R=Path(__file__).resolve().parents[1]
class V136R3Reload(unittest.TestCase):
 def test_marker(self):
  assert_current_identity(R)
  s=read_source(R/"src/mod.cpp")
  self.assertIn("F11=1 native reload recording only",s)
  self.assertIn("F11=2 explicit manual bow quiver/nock experiment",s)
  self.assertIn("recorders do not own gameplay",s)
 def test_milestones(self):
  h=read_source(R/'src/diagnostics/reload_milestones136r3.h')
  for x in ['bow_load_2nd_over_player','LoadReachedAmmo','LoadReachedWeapon',
            'crossbow_medium_charging_phase1','LoadReachedCocking','SpawnLever','DespawnLever',
            'rifle_charging_phase1','rifle_charging_phase2']:
   self.assertIn(x,h)
 def test_fifo_observer_read_only(self):
  s=read_source(R/'src/diagnostics/playback_probe.inl')
  self.assertIn('V66.136R3-F3 RELOAD_MILESTONE',s)
  self.assertIn('handlerCall=UNPROVEN ammoWrite=NONE',s)
  self.assertIn('ReloadDiagnosticWindow136R3',s)
  self.assertNotIn('LoadWeaponAction(',s)
 def test_attachment_observer_read_only(self):
  s=read_source(R/'src/frik_pose_probe.cpp')
  self.assertIn('V66.136R3-F3 MISSILE_HELPER',s)
  self.assertIn('semantic=OBSERVE_ONLY',s)
  self.assertIn('F3-RELOAD',s)
  self.assertIn('semantics=UNCLASSIFIED',s)
if __name__=='__main__': unittest.main()
