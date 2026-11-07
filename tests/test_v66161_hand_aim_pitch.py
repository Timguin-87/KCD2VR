import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
import unittest
R=Path(__file__).resolve().parents[1]
M=read_source(R/'src/mod.cpp')
class RetiredHandAim162(unittest.TestCase):
    def test_no_hand_angular_publisher(self):
        self.assertNotIn('pitchPublished161', M)
        self.assertNotIn('g_vrik_aim_tick161', M)
        self.assertIn('SetActive(inputWrite);', M)
    def test_view_owner_untouched(self):
        self.assertIn('native_head_rotation_owned=inputWrite && native_input_look_lab::Ready();', M)
    def test_hand_ray_retained(self):
        self.assertIn('interaction_alignment::SetHandRayWorld(true,ox,oy,oz,dx,dy,dz,reach158', M)
        self.assertIn('SetRightGripFreeLookEnabled', M)
    def test_old_ini_cannot_enable_servo(self):
        self.assertIn('VrikHandAimPitch ignored: retired', M)
        self.assertNotIn('vrik_hand_aim_pitch_', M)
if __name__=='__main__': unittest.main()
