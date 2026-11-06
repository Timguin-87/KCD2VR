import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
import pathlib, unittest
ROOT=pathlib.Path(__file__).resolve().parents[1]
class Cleanup135R1(unittest.TestCase):
    def test_hotkey_cycles_are_short(self):
        s=read_source(ROOT/'src/mod.cpp', errors='ignore')
        self.assertIn('const int next=(cur==3)?4:(cur==4)?5:(cur==5)?0:3',s)
        self.assertIn('const int next=(cur==3)?6:(cur==6)?7:(cur==7)?0:3',s)
        self.assertIn('CTRL_F10_DIRECT',s)
    def test_grace_is_body_relative(self):
        s=read_source(ROOT/'src/vrik/physical_native.inl', errors='ignore')
        self.assertIn('BODY_RELATIVE_LAST_GOOD_POSE',s)
        self.assertIn('record.localWanted',s)
        self.assertIn('g_committed_frame6695.read(body)',s)
    def test_topology_only_same_callback(self):
        s=read_source(ROOT/'src/vrik/native_motion112.inl', errors='ignore')
        p=read_source(ROOT/'src/vrik/native_prediction120.inl', errors='ignore')
        self.assertIn('v.callback==socket->callback',s)
        self.assertIn('v.callback==socket->callback',p)
    def test_npc_owner_resolution(self):
        s=read_source(ROOT/'src/diagnostics/native_hit_probe134.inl', errors='ignore')
        self.assertIn('candidate=owner?Rd<const void*>(owner,0x38,nullptr):nullptr',s)
        self.assertIn('NPC_OWNER_UNRESOLVED',s)
if __name__=='__main__': unittest.main()
