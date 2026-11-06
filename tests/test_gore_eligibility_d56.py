"""Execute the shipped Lua positive-male gate; no native game API calls."""
import unittest
from pathlib import Path
from lupa.lua51 import LuaRuntime
ROOT=Path(__file__).resolve().parents[1]
class Eligibility(unittest.TestCase):
 def runtime(self):
  l=LuaRuntime(unpack_returned_tuples=True)
  l.execute('''entities={};logs={};enum_humanGender={male=1,female=2};
  System={LogAlways=function(s)table.insert(logs,s)end,GetEntity=function(id)return entities[id]end};
  Script={SetTimer=function()error('unexpected effect timer')end};
  victim={id=42,human={},soul={GetGender=function()return 1 end}};entities[42]=victim
  ''')
  l.execute((ROOT/'mods/KCD2VRGore/r93r3/zzkcd2vr_gore.lua').read_text())
  return l
 def answer(self,l):return l.eval("kcd2vr_gore_eligibility_d56('D56|719|42')")
 def test_positive_male(self):self.assertEqual(self.answer(self.runtime()),'D56|719|42|MALE_HUMAN')
 def test_every_other_gender_refused(self):
  for value in ['0','2','3','nil',"'1'",'false']:
   l=self.runtime();l.execute('victim.soul.GetGender=function()return '+value+' end')
   self.assertEqual(self.answer(l),'D56|719|42|REFUSED')
 def test_no_missing_metadata_equality(self):
  for change in ['victim.human=nil','victim.soul=nil','enum_humanGender=nil','enum_humanGender.male=nil','enum_humanGender.male=0','victim.soul.GetGender=nil']:
   l=self.runtime();l.execute(change);self.assertEqual(self.answer(l),'D56|719|42|REFUSED')
 def test_getter_fault_or_identity_change(self):
  for code in ["error('unavailable')",'entities[42]={id=42};return 1','victim.human={};return 1','victim.soul={};return 1']:
   l=self.runtime();l.execute('victim.soul.GetGender=function() '+code+' end');self.assertEqual(self.answer(l),'D56|719|42|REFUSED')
 def test_actual_cut_entry_refuses_before_any_effect(self):
  for code in ['victim.soul.GetGender=function()return 2 end','victim.human=nil']:
   l=self.runtime();l.execute(code+";KCD2VR_Gore.Cut{target=42,cut='head',energy=1}")
   self.assertTrue(any('ELIGIBILITY' in str(x) for x in l.globals().logs.values()))
   self.assertIsNone(l.eval('KCD2VR_Gore.done[42]'))
 def test_bad_packets(self):
  l=self.runtime()
  for text in ['','D56|1|0','D56|1|42|MALE_HUMAN','D56|x|42','D56|1|42;malicious']:
   self.assertEqual(l.globals().kcd2vr_gore_eligibility_d56(text),'D56|BAD_PACKET')
if __name__=='__main__':unittest.main()
