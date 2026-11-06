import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from source_contract5916c3 import assert_current_identity, assert_filtered_pad_route
from pathlib import Path
import unittest
ROOT=Path(__file__).resolve().parents[1]
class V135(unittest.TestCase):
    def test_domains_are_separate(self):
        s=read_source(ROOT/'src/diagnostics/lab_state.h')
        self.assertIn('CombatReference()',s);self.assertIn('InteractionReference()',s)
        self.assertIn('NativeReference() noexcept{return CombatReference();}',s)
        own=read_source(ROOT/'src/vrik/interaction_ownership111.h')
        self.assertIn('(void)f3',own)
    def test_attachment_continuity(self):
        s=read_source(ROOT/'src/vrik/physical_native.inl')
        block=s[s.index('static PhysicalAttachmentResult101 PhysAttachmentResultImpl101'):s.index('static bool PhysCallAction6698')]
        self.assertIn('PhysReplayLast135',block);self.assertIn('antiFlickerGraceMs=80',block)
        self.assertNotIn('EntityById6694(id)',block)
        self.assertIn('three separated verification failures',block)
    def test_native_sweep_no_optional_contact_gate(self):
        s=read_source(ROOT/'src/vrik/native_motion112.inl')
        ctx=s[s.index('static bool NativeMotionContext112'):s.index('static bool NativeAlignmentOwned112')]
        self.assertNotIn('f.output.contactKnown[0]',ctx)
        prep=s[s.index('static bool NativeTrajectoryPrepare112'):]
        self.assertNotIn('g_phys_sockets101.read(sockets)',prep)
        self.assertIn('capture.socket',prep)
    def test_reload_observation_is_f3_read_only(self):
        s=read_source(ROOT/'src/diagnostics/playback_probe.inl')
        self.assertIn('V66.135-F3 WEAPON_LIFECYCLE',s)
        for name in ('CROSSBOW','RIFLE','BOW','completion=NATIVE_STATE_OR_FIFO_ACK_REQUIRED writes=NONE'):
            self.assertIn(name,s)
    def test_marker(self):
        assert_current_identity(ROOT)
if __name__=='__main__':unittest.main()
