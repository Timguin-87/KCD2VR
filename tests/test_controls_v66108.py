import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
import pathlib, re, unittest
ROOT=pathlib.Path(__file__).resolve().parents[1]
class Controls109(unittest.TestCase):
 def test_openxr_thumbrest_binding(self):
  h=read_source(ROOT/'src/xr_input.h'); c=read_source(ROOT/'src/xr_input.cpp')
  self.assertIn('quick_access_touch',h); self.assertIn('quick_access_touch_action_',h)
  self.assertIn('/user/hand/right/input/thumbrest/touch',c)
  self.assertIn('GetBoolState(quick_access_touch_action_, right_path_',c)
  self.assertIn('right_.quick_access_touch=false',c)
 def test_left_grip_freed(self):
  c=read_source(ROOT/'src/touch_bindings.cpp'); m=read_source(ROOT/'src/mod.cpp')
  self.assertNotIn('const bool modifier=left_grip_down',c)
  self.assertNotIn('LG+LS-LEFT',c); self.assertNotIn('LG+LS-RIGHT',c); self.assertNotIn('LG+LS-DOWN',c)
  self.assertRegex(c,r'const bool quick_touch\s*=\s*right\.quick_access_touch')
  self.assertIn('interaction_ownership111::HandEligible(l.active&&l.aim_active,ls,ln,(gripMask&1u)!=0)',m)
 def test_chords(self):
  c=read_source(ROOT/'src/touch_bindings.cpp'); p=read_source(ROOT/'src/native_vr_pad_premap.cpp')
  self.assertIn('quick_touch && meta_left_dir==Dir::Down',c)
  self.assertIn('R-THUMBREST+LS-DOWN HOLD 0.5s -> VR menu toggle',c)
  self.assertIn('R-THUMBREST+LS-LEFT',c); self.assertIn('R-THUMBREST+LS-RIGHT',c)
  self.assertRegex(p,r'p\.dpad_left\s*=\s*ql;\s*p\.dpad_right\s*=\s*qr;')
  self.assertIn('Up/Down are intentionally NOT emitted',p)
  menu=read_source(ROOT/'src/vr_menu.cpp'); self.assertIn('R thumbrest + LS Down: VR Menu',menu); self.assertIn('Left Grip: physical grab / support',menu)
 def test_l3_sprint_untouched(self):
  c=read_source(ROOT/'src/touch_bindings.cpp'); p=read_source(ROOT/'src/native_vr_pad_premap.cpp')
  self.assertIn('left.thumbstick_down, shift_',c)
  self.assertIn('p.l3=l.thumbstick_down',p)
if __name__=='__main__': unittest.main()
