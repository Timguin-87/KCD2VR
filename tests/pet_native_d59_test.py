"""Execute original retail Dog/GetActions/OnBonding and descriptor builder.
Only native leaves are fakes. These are source/contract tests, not headset proof.
"""
from pathlib import Path
import sys,unittest,zipfile,hashlib
ROOT=Path(__file__).resolve().parents[1]
if (ROOT/'test_deps').exists():sys.path.insert(0,str(ROOT/'test_deps'))
from lupa.lua51 import LuaRuntime
SCRIPTS=Path('C:/Steam/steamapps/common/KingdomComeDeliverance2/Data/Scripts.pak')
class PetNativeD59(unittest.TestCase):
 @classmethod
 def setUpClass(cls):
  with zipfile.ZipFile(SCRIPTS) as z:
   cls.dog=z.read('Scripts/Entities/AI/Dog.lua')
   cls.actions=z.read('Scripts/Utils/InteractorAction.lua')
  assert hashlib.sha256(cls.dog).hexdigest()=='a1f56bb3dd31047bb4bea9e4004c72be6098e08fcf5f0342a33ed8e5a7f453cb'
  cls.adapter=(ROOT/'companion/source/Scripts/Startup/zzkcd2vr_petting_d59.lua').read_text()
 def setUp(self):
  self.lua=LuaRuntime()
  self.lua.execute('table.Merge=function()end;EntityCommon={MakeSpawnable=function()end};BasicAIActions={};BasicAnimal={};BasicAI={}')
  self.lua.execute(self.actions.decode());self.lua.execute(self.dog.decode());self.lua.execute(self.adapter)
  self.lua.execute('''
calls=0;selected=nil;allow=true;alive=true;conscious=true;bondable=true;canBond=true;pickpocket=false
player={id=10,actor={CanInteractWith=function(self,id)return allow end},human={IsPickpocketing=function()return pickpocket end}}
dog=setmetatable({id=20,actor={IsUnconscious=function()return not conscious end,GetHealth=function()return alive and 1 or 0 end,CanTalk=function()return false end},dog={IsBondable=function()return bondable end,CanBondNow=function()return canBond end,Bond=function(self,id)calls=calls+1;selected=id end},AddAnimalLootAction=function()return {} end},{__index=Dog})
System={GetEntity=function(id)if id==10 then return player elseif id==20 then return dog end end}
''')
  self.call=self.lua.globals().kcd2vr_pet_d59
 def test_exact_descriptor_native_entry_once(self):
  self.assertEqual(self.call('PET|1|10|20'),'PET|1|20|NATIVE_HANDLER_RETURNED')
  self.assertEqual(self.lua.globals().calls,1);self.assertEqual(self.lua.globals().selected,20)
  self.assertEqual(self.call('PET|1|10|20'),'PET|1|20|NATIVE_HANDLER_RETURNED');self.assertEqual(self.lua.globals().calls,1)
 def test_native_refusals(self):
  for i,condition in enumerate(('allow','alive','conscious','bondable','canBond'),1):
   with self.subTest(condition=condition):
    self.lua.execute(condition+'=false')
    self.assertEqual(self.call(f'PET|{i}|10|20'),f'PET|{i}|20|NATIVE_INELIGIBLE')
    self.lua.execute(condition+'=true')
  self.lua.execute('pickpocket=true')
  self.assertEqual(self.call('PET|6|10|20'),'PET|6|20|NATIVE_INELIGIBLE');self.assertEqual(self.lua.globals().calls,0)
 def test_missing_or_wrong_dog_player(self):
  self.assertIn('NO_CURRENT_PLAYER',self.call('PET|1|11|20'))
  self.assertIn('NOT_NATIVE_DOG',self.call('PET|2|10|21'))
  self.lua.execute('dog.dog=nil')
  self.assertIn('NOT_NATIVE_DOG',self.call('PET|3|10|20'));self.assertEqual(self.lua.globals().calls,0)
 def test_handler_throw_after_native_side_effect_not_retried(self):
  self.lua.execute('dog.dog.Bond=function(self,id)calls=calls+1;error("after entry")end')
  self.assertIn('NATIVE_UNKNOWN_NO_RETRY',self.call('PET|1|10|20'))
  self.assertIn('NATIVE_UNKNOWN_NO_RETRY',self.call('PET|1|10|20'));self.assertEqual(self.lua.globals().calls,1)
 def test_same_serial_identity_change_and_stale(self):
  self.call('PET|2|10|20')
  self.assertIn('SERIAL_IDENTITY_CHANGED',self.call('PET|2|10|21'))
  self.assertIn('STALE',self.call('PET|1|10|20'));self.assertEqual(self.lua.globals().calls,1)
 def test_descriptor_target_replacement_cancels(self):
  self.lua.execute('dog.dog.CanBondNow=function()System.GetEntity=function(id)if id==10 then return player end end;return true end')
  self.assertIn('IDENTITY_CHANGED',self.call('PET|1|10|20'));self.assertEqual(self.lua.globals().calls,0)
 def test_native_getter_exception_cancels(self):
  self.lua.execute('dog.dog.CanBondNow=function()error("getter")end')
  self.assertIn('DESCRIPTOR_UNAVAILABLE',self.call('PET|1|10|20'));self.assertEqual(self.lua.globals().calls,0)
 def test_native_descriptor_query_restores_global_output(self):
  self.lua.execute('prior={sentinel=42};output=prior;priorInst={sentinel=43};inst=priorInst')
  self.call('PET|1|10|20')
  self.assertTrue(self.lua.eval('output==prior'))
  self.assertTrue(self.lua.eval('inst==priorInst'))
 def test_invalid_packets_do_nothing(self):
  for p in ('','PET|0|10|20','PET|9007199254740992|10|20','PET|1|0|20','PET|1|10|20|EXTRA','use','PET|1|10|4294967296'):
   with self.subTest(packet=p):self.assertEqual(self.call(p),'PET|BAD')
  self.assertEqual(self.lua.globals().calls,0)
if __name__=='__main__':unittest.main(verbosity=2)
