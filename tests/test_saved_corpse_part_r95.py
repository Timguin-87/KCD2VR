"""Execute the actual R95 native-dead nomination protocol under Lua 5.1."""
from test_saved_corpse_drag_d72 import Corpse
import unittest
class NativePart(Corpse):
 def prepare(self):
  self.lua.execute('''
   function a:GetWorldBBox()return {x=-.2,y=-.2,z=-.2},{x=.2,y=.2,z=.2}end
   function b:GetWorldBBox()return {x=1.8,y=-.2,z=-.2},{x=2.2,y=.2,z=.2}end
   a.GetBonePos=nil;a.SetVelocity=nil;b.GetBonePos=nil;b.SetVelocity=nil
  ''')
 def nominate(self,side=0,x=0):
  q=self.call('N',side,x=x);self.assertIn(q[-1],('CANDIDATE','NEAR'));return int(q[2]),int(q[3])
 def test_native_nomination_without_bone_or_entity_velocity_api(self):
  self.prepare();i,t=self.nominate();self.assertEqual(i,42)
  self.assertEqual(self.call('A',id=i,token=t)[-1],'ACQUIRED')
  self.assertEqual(self.call('V',id=i,token=t)[-1],'HELD')
  self.assertEqual(self.call('D',id=i,token=t)[-1],'NATIVE_PART_ONLY')
  self.assertEqual(self.lua.globals().boneCalls,0);self.assertEqual(self.lua.globals().calls,0)
 def test_native_nomination_revived_or_replaced_owner_retires(self):
  self.prepare();i,t=self.nominate();self.lua.execute('a.actor.dead=false')
  self.assertEqual(self.call('V',id=i,token=t)[-1],'OWNER_CHANGED')
  self.lua.execute('a.actor.dead=true');i,t=self.nominate()
  self.lua.execute('a.actor={IsDead=function()return true end}')
  self.assertEqual(self.call('A',id=i,token=t)[-1],'OWNER_CHANGED')
 def test_native_nomination_either_hand_release_and_reload(self):
  self.prepare()
  for side in (0,1):
   i,t=self.nominate(side);self.assertEqual(self.call('A',side,i,t)[-1],'ACQUIRED')
   self.assertEqual(self.call('R',side)[-1],'RELEASED')
   i,t=self.nominate(side);self.assertEqual(self.call('V',side,i,t,session='9_9_9')[-1],'OWNER_CHANGED')
  self.assertEqual(self.lua.globals().calls,0)
 def test_native_query_bounded_without_bone_reads(self):
  self.prepare();q=self.call('n');self.assertIn('F9',q);self.assertEqual(self.lua.globals().boneCalls,0)
  self.assertEqual(self.lua.globals().scanCalls,1);self.assertEqual(self.lua.globals().calls,0)
if __name__=='__main__':unittest.main(verbosity=2)
