import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
import unittest
class TrackingBounds146(unittest.TestCase):
    def test_producer_and_consumer_match(self):
        r=Path(__file__).resolve().parents[1]
        a=read_source(r/'src/frik_pose_probe.cpp')
        b=read_source(r/'src/vrik/tracking_grace111.h')
        self.assertIn('out.head_good_tick,60',a)
        self.assertIn('prev.head_valid,prev.head,prev.hq,prev.head_good_tick,60',b)
        self.assertIn('g_head_valid_tick.load(std::memory_order_relaxed),60',a)
        self.assertIn('out.left_good_tick,80',a)
        self.assertIn('out.right_good_tick,80',a)
