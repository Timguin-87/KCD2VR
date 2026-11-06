"""Execute the actual shipped F9 script under Lua5.1; native leaves mocked."""
import unittest
import os
from pathlib import Path
from lupa.lua51 import LuaRuntime
ROOT=Path(__file__).resolve().parents[1]
SOURCE=Path(os.environ.get('KCD2VR_CORPSE_LUA_SOURCE',ROOT/'companion/source/zzkcd2vr_corpse_d72.lua'))
class Corpse(unittest.TestCase):
 def setUp(self):
  self.lua=LuaRuntime(unpack_returned_tuples=True)
  self.lua.execute('''
   calls=0;scanCalls=0;boneCalls=0;entities={};player={id=1};g_localActor=player
   System={GetEntity=function(id)return entities[id]end,GetEntitiesInSphere=function(p,r)scanCalls=scanCalls+1;assert(r==1.6);return entities end}
   function corpse(id,x,y,z,isdead)
    local e={id=id,pos={x=x,y=y,z=z},actor={dead=isdead},velocity=nil}
    function e.actor:IsDead()return self.dead end
    function e:GetBonePos(name) boneCalls=boneCalls+1;if name~=(self.boneName or 'RHand') then return nil end return self.pos end
    function e:SetVelocity(v) calls=calls+1;self.velocity=v;if self.throw then error('native refused')end;return self.answer end
    entities[id]=e;return e
   end
   a=corpse(42,0,0,0,true);b=corpse(43,2,0,0,true);living=corpse(44,0.01,0,0,false)
  ''')
  self.lua.execute(SOURCE.read_text())
 def call(self,op,side=0,id=0,token=0,x=0,y=0,z=0,session='1_1_1'):
  return self.lua.globals().kcd2vr_corpse_d72(f'D72|{op}|{session}|{side}|1|{id}|{token}|{x}|{y}|{z}').split('|')
 def acquire(self,side=0,x=0):
  q=self.call('Q',side,x=x);self.assertEqual(q[-1],'CANDIDATE');a=self.call('A',side,int(q[2]),int(q[3]),x=x);self.assertEqual(a[-1],'ACQUIRED');return int(q[2]),int(q[3])
 def test_loaded_save_corpse_hand_target_no_death_receipt(self):
  id,t=self.acquire(x=.1);self.assertEqual(id,42);self.assertEqual(self.lua.globals().calls,0)
  a=self.call('D',id=id,token=t,x=.5);self.assertEqual(a[-1],'VELOCITY_RETURNED');self.assertAlmostEqual(self.lua.globals().a.velocity.x,1.5)
  self.assertEqual(self.lua.globals().scanCalls,1) # retaining/moving does not rescan
 def test_either_hand_and_no_two_motors_on_one_corpse(self):
  self.acquire();q=self.call('Q',1);self.assertEqual(q[-1],'NO_NEAR_DEAD_BONE')
  id,t=self.acquire(1,x=2);self.assertEqual(id,43)
  self.call('D',1,id,t,x=1);self.assertAlmostEqual(self.lua.globals().b.velocity.x,-2.5)
 def test_release_no_final_zero_or_continuing_timer(self):
  id,t=self.acquire();self.call('D',id=id,token=t,x=.5);n=self.lua.globals().calls
  self.assertEqual(self.call('R')[-1],'RELEASED');self.assertEqual(self.call('D',id=id,token=t)[-1],'OWNER_CHANGED');self.assertEqual(self.lua.globals().calls,n)
 def test_current_dead_answer_not_stale_living_or_revived(self):
  id,t=self.acquire();self.lua.execute('a.actor.dead=false')
  self.assertEqual(self.call('D',id=id,token=t)[-1],'OWNER_CHANGED');self.assertEqual(self.lua.globals().calls,0)
  self.assertEqual(self.call('Q')[-1],'NO_NEAR_DEAD_BONE')
 def test_reused_id_or_actor_component_invalidates(self):
  id,t=self.acquire();self.lua.execute('a=corpse(42,0,0,0,true)');self.assertEqual(self.call('D',id=id,token=t)[-1],'OWNER_CHANGED')
  id,t=self.acquire();self.lua.execute('a.actor={IsDead=function()return true end}')
  self.assertEqual(self.call('D',id=id,token=t)[-1],'OWNER_CHANGED');self.assertEqual(self.lua.globals().calls,0)
 def test_far_bone_or_moved_candidate_never_grabs(self):
  q=self.call('Q',x=.3);self.assertEqual(q[-1],'NEAR')
  self.assertEqual(self.call('A',id=int(q[2]),token=int(q[3]),x=.3)[-1],'ACQUIRE_REFUSED')
  self.assertEqual(self.call('Q',x=.5)[-1],'NO_NEAR_DEAD_BONE')
  q=self.call('Q');self.lua.execute('a.pos.x=1');self.assertEqual(self.call('A',id=int(q[2]),token=int(q[3]))[-1],'ACQUIRE_REFUSED')
 def test_near_nomination_promotes_same_token_without_motion(self):
  q=self.call('Q',x=.4);self.assertEqual(q[-1],'NEAR');id,t=int(q[2]),int(q[3])
  self.assertEqual(self.call('V',id=id,token=t,x=.3)[-1],'NEAR')
  v=self.call('V',id=id,token=t,x=.2);self.assertEqual(v[-1],'CANDIDATE');self.assertEqual(int(v[3]),t)
  self.assertEqual(self.call('A',id=id,token=t,x=.2)[-1],'ACQUIRED');self.assertEqual(self.lua.globals().calls,0)
 def test_false_or_exception_retires(self):
  id,t=self.acquire();self.lua.execute('a.answer=false');self.assertEqual(self.call('D',id=id,token=t)[-1],'VELOCITY_REFUSED')
  self.assertEqual(self.call('D',id=id,token=t)[-1],'OWNER_CHANGED')
  self.lua.execute('a.answer=nil;a.throw=true');id,t=self.acquire();self.assertEqual(self.call('D',id=id,token=t)[-1],'VELOCITY_REFUSED')
 def test_epoch_load_and_leash_retire_without_mutation(self):
  id,t=self.acquire();self.assertEqual(self.call('D',id=id,token=t,session='2_1_1')[-1],'OWNER_CHANGED')
  id,t=self.acquire();self.assertEqual(self.call('D',id=id,token=t,x=2)[-1],'LEASH');self.assertEqual(self.lua.globals().calls,0)
 def test_scan_bounded_and_malformed_packets(self):
  self.lua.execute('entities={};for i=1,100 do corpse(i+50,10,0,0,false)end')
  self.assertEqual(self.call('Q')[-1],'NO_NEAR_DEAD_BONE');self.assertEqual(self.call('Q',x='nan')[-1],'BAD_PACKET')
 def test_documented_kcd2_limb_names(self):
  # Actual project skeleton names, with no legacy RHand/Bip01 alias in the leaf.
  for bone in ('RightHand','LeftHand','RightForeArm','LeftForeArm','RightFoot','LeftFoot','RightLeg','LeftLeg'):
   with self.subTest(bone=bone):
    self.lua.globals().a.boneName=bone
    q=self.call('Q');self.assertEqual(q[-1],'CANDIDATE');self.assertEqual(int(q[2]),42)
    self.assertEqual(self.lua.globals().calls,0)
 def test_clutter_does_not_spend_corpse_budget(self):
  self.lua.execute('''
   list={};for i=1,100 do list[#list+1]=corpse(i+100,0,0,0,false) end
   list[#list+1]=a
   System.GetEntitiesInSphere=function()scanCalls=scanCalls+1;return list end
  ''')
  q=self.call('Q');self.assertEqual(q[-1],'CANDIDATE');self.assertEqual(int(q[2]),42)
  self.assertLessEqual(self.lua.globals().boneCalls,31) # only the one eligible corpse
 def test_sorted_tie_break_is_independent_of_enumeration_order(self):
  self.lua.execute('''
   b.pos={x=0,y=0,z=0};reverse=false
   System.GetEntitiesInSphere=function()reverse=not reverse;if reverse then return {b,a} end;return {a,b} end
  ''')
  for _ in range(4):
   q=self.call('Q');self.assertEqual(q[-1],'CANDIDATE');self.assertEqual(int(q[2]),42)
 def test_eligible_pages_rotate_without_unbounded_bone_queries(self):
  self.lua.execute('''
   entities={};list={};for i=1,40 do list[i]=corpse(i+100,10,0,0,true) end
   list[40].pos={x=0,y=0,z=0}
   System.GetEntitiesInSphere=function()return list end
  ''')
  self.assertEqual(self.call('Q')[-1],'NO_NEAR_DEAD_BONE')
  first=self.lua.globals().boneCalls;self.assertLessEqual(first,32*31)
  q=self.call('Q');self.assertEqual(q[-1],'CANDIDATE');self.assertEqual(int(q[2]),140)
  self.assertLessEqual(self.lua.globals().boneCalls-first,32*31)
 def test_duplicate_entries_do_not_spend_pages(self):
  self.lua.execute('''
   list={};for i=1,100 do list[i]=b end;list[101]=a
   System.GetEntitiesInSphere=function()return list end
  ''')
  q=self.call('Q');self.assertEqual(q[-1],'CANDIDATE');self.assertEqual(int(q[2]),42)
  self.assertLessEqual(self.lua.globals().boneCalls,2*31)
 def test_dense_query_refuses_whole_prefix_without_physics(self):
  self.lua.execute('list={};for i=1,4097 do list[i]=a end;System.GetEntitiesInSphere=function()return list end')
  self.assertEqual(self.call('Q')[-1],'QUERY_TOO_DENSE');self.assertEqual(self.lua.globals().calls,0)
if __name__=='__main__':unittest.main(verbosity=2)
