import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
import importlib.util
from pathlib import Path
import unittest
r=Path(__file__).resolve().parents[1]
s=importlib.util.spec_from_file_location('body_combos',r/'tools/compile_body_combos59c7g.py')
m=importlib.util.module_from_spec(s);s.loader.exec_module(m)
class Recipes(unittest.TestCase):
 def test_generated(self):
  self.assertEqual(m.compile_xml((r/'data/VRBodyCombos.xml').read_bytes()),read_source(r/'src/vrik/body_combo_recipes59c7g.h'))
 def test_unknown_or_unprovable_schema(self):
  d=(r/'data/VRBodyCombos.xml').read_bytes()
  for old,new in [(b'region="knee"',b'body_part="knee"'),(b'gapMs="1500"',b'gapMs="999999"'),
                  (b'equipment="Longsword"',b'equipment="Imaginary"'),(b'region="knee"',b'region="femur"'),
                  (b'region="any"',b'region="face"')]:
   with self.subTest(new=new),self.assertRaises((ValueError,KeyError)):
    m.compile_xml(d.replace(old,new,1))
 def test_dtd(self):
  with self.assertRaises(ValueError):m.compile_xml(b'<!DOCTYPE foo []><VRBodyCombos version="1"/>')
if __name__=='__main__':unittest.main()
