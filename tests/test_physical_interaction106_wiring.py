import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from source_contract5916c3 import assert_current_identity, assert_filtered_pad_route
import unittest
from pathlib import Path
R=Path(__file__).resolve().parents[1]
class Wiring106(unittest.TestCase):
 def test_marker(self):
  assert_current_identity(R)
 def test_native_interactor_scope(self):
  m=read_source(R/'src/mod.cpp'); h=read_source(R/'src/interaction_alignment.h'); c=read_source(R/'src/interaction_alignment.cpp')
  self.assertIn('RequestPhysicalInteractionUse()',m)
  self.assertIn('const float reach158=interaction57c1::HandReach',m); self.assertIn('SetHandRayWorld(true,ox,oy,oz,dx,dy,dz,reach158,interaction57c1::HandRadius,side)',m)
  self.assertIn('float max_reach_m=0.0f, float acquisition_radius_m=0.0f',h)
  self.assertIn('packet.limit=maxReach',c); self.assertIn('packet.radius=radius',c)
  self.assertIn('g_hand_packets112.publish(packet)',c)
  self.assertIn('native_reach',c); self.assertIn('useDx * reach',c)
 def test_f5_is_use_not_fake_action(self):
  f=read_source(R/'src/frik_pose_probe.cpp'); m=read_source(R/'src/mod.cpp')
  self.assertIn('LIVE_HAND_USE+OBSERVE',f)
  self.assertIn('This does not call StartInteractiveActionByName',m)
  self.assertIn('g_interaction_original6689(actor,name,targetId,force,speed)',f)
  self.assertNotIn('PhysicalGrabImplemented=true',read_source(R/'src/diagnostics/lab_state.h'))
 def test_grip_conflict_guards(self):
  m=read_source(R/'src/mod.cpp'); p=read_source(R/'src/vrik/physical_interaction.h')
  self.assertIn('std::fabs(r.thumb_x)<0.28f',m)
  self.assertIn('std::fabs(l.thumb_x)<0.28f',m)
  self.assertIn('in.hand[i].empty&&in.hand[i].neutral',p)
  self.assertIn('use_at_ms=in.now_ms+20ull',p)
if __name__=='__main__': unittest.main()
