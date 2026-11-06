import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from source_contract5916c3 import assert_current_identity, assert_filtered_pad_route
from pathlib import Path
import unittest

ROOT=Path(__file__).resolve().parents[1]
FRIK=read_source(ROOT/'src/frik_pose_probe.cpp')
PHYS=read_source(ROOT/'src/vrik/physical_native.inl')
MOD=read_source(ROOT/'src/mod.cpp')

class SingleAuthority157(unittest.TestCase):
    def test_marker(self):
        assert_current_identity(ROOT)
        self.assertIn('g_phys_authority_drop157', FRIK)

    def test_retired_controller_bridge_stays_disabled_in_waiting(self):
        dispatch=FRIK[FRIK.index('static void __fastcall EntityAttachmentUpdateDetourV6688'):]
        self.assertNotIn('Waiting&&BridgeControllerToPhysical132', dispatch)
        self.assertIn('g_phys_waiting_native157.fetch_add', dispatch)
        self.assertIn('policy=ONE_WRITER_NO_CONTROLLER_BRIDGE', FRIK)

    def test_weapon_drop_reason_counters(self):
        self.assertIn('g_phys_authority_drop157[11]', PHYS)
        for reason in (1,2,3,4,5,10):
            self.assertIn(f'PhysAuthorityDrop157({reason})', PHYS)
        self.assertIn('authorityReason=!haveFrame?6:!frameIdentity?7:!frame.output.active[side]?8:9', PHYS)
        self.assertIn('PhysAuthorityDrop157(authorityReason)', PHYS)

    def test_pose_fallback_counters_are_observation_only(self):
        self.assertIn('g_pose_authority_drop157[6]', FRIK)
        self.assertIn('g_pose_authority_drop157[authorityReason157].fetch_add', FRIK)
        self.assertIn('result=NATIVE_POSE_FALLBACK', FRIK)

    def test_no_grace_extension(self):
        self.assertIn('const std::uint64_t maxAge=handKnown?80ull:60ull;', PHYS)
        self.assertNotIn('250ull', PHYS[PHYS.index('static bool PhysReplayLast135'):PHYS.index('static PhysicalAttachmentResult101 PhysAttachmentResultImpl101')])

if __name__ == '__main__': unittest.main()
