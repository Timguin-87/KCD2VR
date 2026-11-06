"""Exercise the actual optional Lua screen gate with explicit native UI/CVar leaves."""
from pathlib import Path
import unittest
from lupa.lua51 import LuaRuntime
ROOT=Path(__file__).resolve().parents[1]
SOURCE=ROOT/'mods/KCD2VRGore/r93r3/zzkcd2vr_gore.lua'
class ScreenGate(unittest.TestCase):
 def test_actual_screen_gate(self):
  s=SOURCE.read_text();a=s.index('local function f2OnR94()');b=s.index('local function try(',a);chunk=s[a:b]
  for expr,expected in [('0',0),('1',1),('2',0),('true',0),('"1"',0),('nil',0),('error("native read failed")',0)]:
   with self.subTest(expr=expr):
    l=LuaRuntime();l.execute('screens=0;logs=0;Game={SendInfoText=function()screens=screens+1 end};function log(s)logs=logs+1 end;System={GetCVar=function(name)assert(name=="kcd2vr_door_capture_f2");return '+expr+' end}')
    l.execute(chunk+'\nsay("failure")');self.assertEqual(l.eval('screens'),expected);self.assertEqual(l.eval('logs'),1)
  for setup in ['System=nil','System={}']:
   l=LuaRuntime();l.execute(setup+';logs=0;function log(s)logs=logs+1 end');l.execute(chunk+'\nsay("failure")');self.assertEqual(l.eval('logs'),1)
if __name__=='__main__':unittest.main()
