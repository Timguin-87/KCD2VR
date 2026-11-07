"""Actual D77-successor Lua consumes the C++ emitted native handover event."""
import os,unittest
from pathlib import Path
from lupa.lua51 import LuaRuntime,LuaError
ROOT=Path(__file__).resolve().parents[1]
SOURCE=ROOT/'mods/KCD2VRGore/r93r3/zzkcd2vr_gore.lua'
CHUNK=Path(os.environ['R93_GORE_CHUNK']).read_text() if os.environ.get('R93_GORE_CHUNK') else None
@unittest.skipUnless(CHUNK, 'run tools/run_gore_r93.py to generate the actual C++ event')
class Handover(unittest.TestCase):
 def runtime(self):
  l=LuaRuntime(unpack_returned_tuples=True)
  l.execute('''
   entities={};logs={};timers={};made={};removed={};enum_humanGender={male=1,female=2};nextId=100
   System={LogAlways=function(s)table.insert(logs,s)end,GetEntity=function(id)return entities[id]end,
    RemoveEntity=function(id)removed[id]=true;entities[id]=nil end}
   Script={SetTimer=function(ms,fn)table.insert(timers,fn)end}
   victim={id=42,human={},soul={GetGender=function()return 1 end}};entities[42]=victim
   function victim:GetBonePos()return {x=0,y=0,z=1}end
   function victim:GetWorldPos()return {x=0,y=0,z=0}end
   function victim:DestroyAttachment(slot,name)made[#made+1]='destroy:'..name end
   function victim:HideAttachment(slot,name,hidden)assert(hidden==false);made[#made+1]='show:'..name end
   function System.SpawnEntity(p)
    nextId=nextId+1;local h={id=nextId,pos=p.position,paths={}};entities[h.id]=h
    function h:LoadCharacter(slot,path)self.paths[slot]=path end
    function h:IsSlotValid()return true end;function h:IsSlotCharacter()return true end
    function h:SetSlotPos()end;function h:DrawSlot()end;function h:GetWorldPos()return self.pos end
    function h:SetPhysicParams()end;function h:AddImpulse()end;function h:Physicalize()end
    return h
   end
  ''')
  l.execute(SOURCE.read_text());l.execute('KCD2VR_Gore.stumpMode=0;KCD2VR_Gore.limbCaps=false')
  return l
 def test_native_event_has_live_pieces_and_cleanup_ack(self):
  l=self.runtime();l.execute(CHUNK)
  self.assertEqual(l.eval('#KCD2VR_Gore.limb41Leases[victim].made'),2)
  self.assertTrue(any('LIMB41 flying ra ok=true layers=2/2' in str(x) for x in l.globals().logs.values()))
  self.assertEqual(l.eval('entities[101].paths[1]'),'objects/characters/humans/male/body/body_arms_kcdgrap.fly.cdf')
  l.execute('KCD2VR_Gore.ReleaseAll()');self.assertIsNone(l.eval('KCD2VR_Gore.limb41Leases[victim]'))
  self.assertEqual(list(l.globals().made.values()),['destroy:kcd2vr_cut_body_arms','show:body_arms','destroy:kcd2vr_cut_gloves','show:gloves'])
 def test_missing_capability_never_calls_old_cut(self):
  l=self.runtime();l.execute('KCD2VR_Gore.nativeLimbHandoverR93=nil;KCD2VR_Gore.Cut=function()error("should not call")end')
  with self.assertRaisesRegex(LuaError,'capability missing'):l.execute(CHUNK)
  self.assertIsNone(l.eval('KCD2VR_Gore.limb41Leases[victim]'))
 def test_missing_script_and_protocol_refused(self):
  for change in ['KCD2VR_Gore=nil','KCD2VR_Gore.protocol=2']:
   l=self.runtime();l.execute(change)
   with self.assertRaisesRegex(LuaError,'capability missing'):l.execute(CHUNK)
 def test_rejected_or_throwing_cut_is_not_acknowledged(self):
  for body in ['return false','return nil','error("cut failure")']:
   l=self.runtime();l.execute('KCD2VR_Gore.Cut=function(e) '+body+' end')
   with self.assertRaisesRegex(LuaError,'not accepted'):l.execute(CHUNK)
   self.assertIsNone(l.eval('KCD2VR_Gore.limb41Leases[victim]'))
 def test_after_acceptance_presentation_fault_preserves_cleanup(self):
  l=self.runtime();l.execute('KCD2VR_Gore.L41=setmetatable({},{__index=function()error("presentation fault")end})')
  l.execute(CHUNK)
  self.assertEqual(l.eval('#KCD2VR_Gore.limb41Leases[victim].made'),2)
  self.assertTrue(any('cleanup retained' in str(x) for x in l.globals().logs.values()))
  l.execute('KCD2VR_Gore.ReleaseAll()');self.assertEqual(len(l.globals().made),4)
 def test_disabled_flying_piece_still_accepts_cleanup(self):
  l=self.runtime();l.execute('KCD2VR_Gore.limbs41=false');l.execute(CHUNK)
  self.assertEqual(l.eval('#KCD2VR_Gore.limb41Leases[victim].made'),2)
 def test_entity_gender_and_identity_reentry_refuse_before_acceptance(self):
  for code in ['victim.human=nil','victim.soul.GetGender=function()return 2 end','victim.soul.GetGender=function()entities[42]={id=42};return 1 end']:
   l=self.runtime();l.execute(code)
   with self.assertRaisesRegex(LuaError,'not accepted'):l.execute(CHUNK)
   self.assertIsNone(l.eval('KCD2VR_Gore.limb41Leases[victim]'))
 def test_duplicate_handover_refuses_without_changing_owner(self):
  l=self.runtime();l.execute(CHUNK);l.execute('old=KCD2VR_Gore.limb41Leases[victim]')
  with self.assertRaisesRegex(LuaError,'not accepted'):l.execute(CHUNK)
  self.assertTrue(l.eval('old==KCD2VR_Gore.limb41Leases[victim]'))
 def test_malformed_copy_name_refuses_before_cleanup(self):
  l=self.runtime()
  with self.assertRaisesRegex(LuaError,'not accepted'):l.execute(CHUNK.replace('kcd2vr_cut_body_arms','unowned_body_arms'))
  self.assertIsNone(l.eval('KCD2VR_Gore.limb41Leases[victim]'))
if __name__=='__main__':unittest.main()
