import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from source_contract5916c3 import assert_current_identity, assert_filtered_pad_route
import unittest
from pathlib import Path
R=Path(__file__).resolve().parents[1]
class V137(unittest.TestCase):
 def test_marker(self):
  assert_current_identity(R)
 def test_deferred_compose(self):
  s=read_source(R/'src/vrik/coherent_pose.inl')
  for x in ['BuildEssential137','ComposeFull137','g_compose_defer137','pending137','essential137','composedPerSolve=']:
   self.assertIn(x,s)
  self.assertLess(s.index('ComposeFull137(s)'),s.index('// Sparse invariants:'))
 def test_neutral_is_full_before_defer(self):
  s=read_source(R/'src/vrik/coherent_pose.inl')
  a=s.index('if(!Compose6689(s))return RejectCoherent6693')
  b=s.index('s.deferPassengers137=g_compose_defer137',a)
  self.assertLess(a,b)
 def test_pose_equivalence_fixture(self):
  s=(R/'tests/pose_adapter_test.cpp').read_text()
  self.assertIn('V137 deferred passenger compose is bit-identical to full compose',s)
if __name__=='__main__': unittest.main()
