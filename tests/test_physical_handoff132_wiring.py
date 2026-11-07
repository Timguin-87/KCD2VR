import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
import unittest
class PhysicalOwner146(unittest.TestCase):
    def test_waiting_cannot_call_legacy_writer(self):
        s=read_source(Path(__file__).resolve().parents[1].joinpath('src/frik_pose_probe.cpp'))
        dispatch=s[s.index('static void __fastcall EntityAttachmentUpdateDetourV6688'):]
        self.assertNotIn('Waiting&&BridgeControllerToPhysical132',dispatch)
        self.assertIn('if(result==PhysicalAttachmentResult101::Busy)return',dispatch)
        self.assertIn('if(result==PhysicalAttachmentResult101::Placed)',dispatch)
