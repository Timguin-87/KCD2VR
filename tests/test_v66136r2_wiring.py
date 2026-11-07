import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from source_contract5916c3 import assert_current_identity, assert_filtered_pad_route
from pathlib import Path
import unittest
R=Path(__file__).resolve().parents[1]
class V136R2(unittest.TestCase):
 def test_marker(self):
  assert_current_identity(R)
 def test_native_haptic_is_post_accept_collision_only(self):
  s=read_source(R/'src/diagnostics/native_hit_probe134.inl')
  self.assertIn('ok&&callerRva==0xD536BCull&&aok&&IsPlayer(ae)',s)
  self.assertIn('hitTarget=Rd<std::uint32_t>(hitInfo,0x28,0u)',s)
  self.assertIn('NotifyNativeWeaponImpact136R2',s)
  self.assertNotIn('g_submit_original?',s[s.index('NotifyNativeWeaponImpact136R2')-250:s.index('NotifyNativeWeaponImpact136R2')])
 def test_deferred_contact_feedback_does_not_invent_plane(self):
  s=read_source(R/'src/vrik/physical_weapon.h')
  block=s[s.index('V66.144: do NOT turn an unknown/deferred obstruction'):s.index('// Avoid a constraint demanding impossible anatomy')]
  self.assertIn('contact.known&&contact.limited115&&allowed<.999',block)
  self.assertIn('out.impactSpeed[side]',block)
  self.assertNotIn('contact.normal*',block)
 def test_native_feedback_requires_fresh_owned_melee(self):
  s=read_source(R/'src/frik_pose_probe.cpp');a=s.index('void NotifyNativeWeaponImpact136R2');b=s.index('void SetWeaponConstraints134',a);block=s[a:b]
  for x in ('g_weapon_control_mode.load','now-frame.input.tick>120','frame.output.owned[s]','frame.profiles[s].melee()'):
   self.assertIn(x,block)
if __name__=='__main__':unittest.main()
