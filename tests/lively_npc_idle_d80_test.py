"""Actual Lua 5.1 scheduler, with explicit engine/link/timer leaves; no game proof."""
from pathlib import Path
import hashlib
import json
import sys
import unittest
import xml.etree.ElementTree as ET
from lupa.lua51 import LuaRuntime

ROOT=Path(__file__).resolve().parents[1]
SCRIPT=ROOT/'companion/source/Scripts/Startup/zzkcd2vr_life_d73.lua'
XML=ROOT/'companion/source/AI/npc/basic/switch/runDialog.xml'

LEAVES=r"""
t=10; timers={}; schedules=0; adds=0; removes=0; scans=0; bones=0; actorReads=0; clocks=0; finds=0; entities=0
failRemove=false; scanFault=false
local function entity(id)
 return {id=id,this={id=id+100},human=true,
 soul={IsInCombatDanger=function()actorReads=actorReads+1;return false end},
 actor={IsDead=function()actorReads=actorReads+1;return false end,
 IsUnconscious=function()actorReads=actorReads+1;return false end},
 GetBonePos=function()bones=bones+1;return {x=0,y=2,z=0} end}
end
player=entity(1); npc=entity(2); registry={[1]=player,[2]=npc}; graph={}
Framework={WUIDToMsg=function(id)return tostring(id)end}
System={GetCurrTime=function()clocks=clocks+1;return t end,
 GetEntity=function(id)entities=entities+1;return registry[id]end,
 GetEntitiesInSphere=function()scans=scans+1;if scanFault then error('native scan leaf failure')end;return {npc}end,
 LogAlways=function()end}
Script={SetTimer=function(delay,callback)assert(delay==250);schedules=schedules+1;timers[#timers+1]=callback end}
XGenAIModule={FindLinks=function(from,tag)finds=finds+1;return graph[tostring(from)..tag] or {} end,
 AddLink=function(from,to,tag)adds=adds+1;graph[tostring(from)..tag]={to};return true end,
 RemoveLink=function(from,to,tag)removes=removes+1;if failRemove then return false end
 assert(graph[tostring(from)..tag][1]==to);graph[tostring(from)..tag]=nil;return true end}
function packet(on)
 local reply=kcd2vr_life_d73('D76|1|'..(on and '5' or '4')..'|0|'..(on and '1' or '0')..'|0|0|0|0|0|1|0|'..(on and '500' or '0')..'|0|102')
 if on then KCD2VR_LifeD73.Dialogue(npc) end -- explicit native subtree witness
 return reply
end
function fire()
 assert(#timers==1,'must have exactly one pending timer');t=t+.25
 local callback=table.remove(timers,1);callback()
end
function owned()local n=0;for _ in pairs(KCD2VR_LifeD73.active)do n=n+1 end;return n end
function acquire()
 for i=1,5 do packet(true);fire() end
 assert(owned()==1,'fixture must acquire through actual dialogue participant path')
end
"""

class Idle(unittest.TestCase):
 def setUp(self):
  self.lua=LuaRuntime(unpack_returned_tuples=True)
  self.lua.execute(LEAVES)
  self.reload()
 def reload(self):self.lua.execute(SCRIPT.read_text())
 def runLua(self,text):self.lua.execute(text)
 def test_never_enabled_and_reload_no_timer_or_native_reads(self):
  self.reload()
  self.runLua('''assert(schedules==0 and #timers==0)
for i=1,100 do assert(not KCD2VR_LifeD73.Enabled());KCD2VR_LifeD73.Dialogue(npc);assert(not KCD2VR_LifeD73.CanAnimate(npc,true)) end
assert(clocks==0 and scans==0 and bones==0 and actorReads==0 and finds==0 and entities==0)''')
 def test_off_packet_and_invalid_packet_do_not_wake(self):
  self.runLua('''assert(packet(false)=='D76|0');assert(kcd2vr_life_d73('bad')=='D76|BAD_PACKET')
assert(not KCD2VR_LifeD73.Enabled() and schedules==0 and #timers==0)
assert(scans==0 and bones==0 and actorReads==0 and finds==0)''')
 def test_enable_and_reload_preserve_single_timer_and_current_tick(self):
  self.runLua('packet(true);packet(true);assert(#timers==1 and schedules==1)')
  self.reload()
  self.runLua('''assert(#timers==1 and schedules==1);fire();assert(scans==0 and bones==0 and owned()==1 and #timers==1)
local old=KCD2VR_LifeD73.Tick;calls=0;KCD2VR_LifeD73.Tick=function()calls=calls+1;old()end
packet(true);fire();assert(calls==1 and #timers==1)''')
 def test_off_drains_one_pending_callback_without_engine_work(self):
  self.runLua('''packet(true);assert(packet(false)=='D76|0');local c,a,b,s,f,e=clocks,actorReads,bones,scans,finds,entities
fire();assert(#timers==0 and not KCD2VR_LifeD73.timerPending and schedules==1)
assert(clocks==c and actorReads==a and bones==b and scans==s and finds==f and entities==e)''')
 def test_lease_expiry_stops_and_clears_gaze_without_actor_reads(self):
  self.runLua('''packet(true);assert(next(KCD2VR_LifeD73.dialogs)~=nil)
t=t+1;local a,b,s=actorReads,bones,scans;fire()
assert(#timers==0 and next(KCD2VR_LifeD73.gaze)==nil and next(KCD2VR_LifeD73.dialogs)==nil)
assert(actorReads==a and bones==b and scans==s)''')
 def test_expired_owned_link_retires_then_stops(self):
  self.runLua('''acquire();t=t+1;local a,b,s=actorReads,bones,scans;fire()
assert(owned()==0 and #timers==0 and actorReads==a and bones==b and scans==s)''')
 def test_failed_cleanup_retries_no_npc_scan_or_bone_reads_then_stops(self):
  self.runLua('''acquire();failRemove=true;assert(packet(false)=='D76|1')
local a,b,s=actorReads,bones,scans
for i=1,3 do fire();assert(owned()==1 and #timers==1) end
assert(actorReads==a and bones==b and scans==s)
failRemove=false;fire();assert(owned()==0 and #timers==0 and not KCD2VR_LifeD73.timerPending)''')
 def test_reload_with_pending_cleanup_does_not_duplicate(self):
  self.runLua('acquire();failRemove=true;packet(false);oldSchedules=schedules')
  self.reload()
  self.runLua('''assert(schedules==oldSchedules and #timers==1 and owned()==1)
fire();assert(owned()==1 and #timers==1);failRemove=false;fire();assert(owned()==0 and #timers==0)''')
 def test_explicit_enable_wakes_after_full_retirement(self):
  self.runLua('''acquire();packet(false);fire();assert(#timers==0 and owned()==0)
packet(true);assert(#timers==1);for i=1,5 do packet(true);fire() end
assert(owned()==1 and #timers==1)''')
 def test_tick_exception_no_idle_timer_or_new_scan_loop(self):
  self.runLua('''packet(true);KCD2VR_LifeD73.Tick=function()error('explicit tick fault')end;fire()
assert(#timers==0 and owned()==0 and KCD2VR_LifeD73.untilTime==0)''')
 def test_exception_with_owned_debt_retries_cleanup_only(self):
  self.runLua('''acquire();failRemove=true;local real=KCD2VR_LifeD73.Tick;KCD2VR_LifeD73.Tick=function()error('explicit tick fault')end;packet(true);fire();KCD2VR_LifeD73.Tick=real
assert(owned()==1 and #timers==1);local a,b,s=actorReads,bones,scans
fire();assert(actorReads==a and bones==b and scans==s and owned()==1)
failRemove=false;fire();assert(owned()==0 and #timers==0)''')
 def test_absent_identity_keeps_cleanup_debt_not_other_actor(self):
  self.runLua('''acquire();registry[2]={id=2,this={id=999}};packet(false)
local r=removes;fire();assert(owned()==1 and removes==r and #timers==1)
graph['102kcd2vr_lifeLookD73']=nil;fire();assert(owned()==0 and #timers==0)''')
 def test_legacy_outdoor_packet_never_wakes_discovers_or_animates(self):
  self.runLua("""for i=1,20 do
assert(kcd2vr_life_d73('D76|1|5|1|0|0|0|0|0|0|1|0|0|0|102')=='D76|0')
assert(not KCD2VR_LifeD73.Enabled());KCD2VR_LifeD73.Dialogue(npc)
assert(not KCD2VR_LifeD73.CanAnimate(npc,false))
end
assert(schedules==0 and scans==0 and bones==0 and actorReads==0 and finds==0 and owned()==0)""")
 def test_dialogue_to_gameplay_retires_and_stops_even_legacy_gameplay_true(self):
  self.runLua("""acquire();assert(KCD2VR_LifeD73.CanAnimate(npc,false))
failRemove=true
assert(kcd2vr_life_d73('D76|1|5|1|0|0|0|0|0|0|1|0|0|0|102')=='D76|1')
local a=actorReads;fire();assert(owned()==1 and #timers==1 and actorReads==a)
failRemove=false;fire();assert(owned()==0 and #timers==0 and scans==0 and bones==0)
packet(true);fire();assert(owned()==1 and #timers==1)""")
 def test_dialogue_without_native_participant_cannot_acquire(self):
  self.runLua("""kcd2vr_life_d73('D76|1|5|1|1|1|0|0|0|0|1|0|500|1|102')
fire();assert(owned()==0 and not KCD2VR_LifeD73.CanAnimate(npc,false))
packet(true);fire();assert(owned()==1 and scans==0 and bones==0)""")
 def test_cpp_dialogue_scope_contract(self):
  header=(ROOT/'src/subsystems/dialogues/lively_npc_d73.h').read_text()
  gaze=(ROOT/'src/subsystems/dialogues/runtime/npc_lookat_hmd.cpp').read_text()
  runtime=(ROOT/'src/subsystems/dialogues/runtime/lively_npc_d73.inl').read_text()
  self.assertIn('f.hmd&&f.dialogue&&f.scene&&',header)
  self.assertNotIn('life.gameplay||life.dialogue',gaze)
  self.assertNotIn('lifeCurrent.gameplay||lifeCurrent.dialogue',gaze)
  self.assertIn('&&life.dialogue&&life.scene;',gaze)
  self.assertIn('&&lifeCurrent.dialogue&&lifeCurrent.scene&&',gaze)
  self.assertIn('(Enabled()&&dialogueInterest.load(std::memory_order_acquire))||cleanup.load',runtime)
  publish=runtime[runtime.index('void Publish('):]
  self.assertNotIn('f.gameplay=gameplay',publish)
  self.assertLess(publish.index('if(f.dialogue){'),publish.index('CameraHook::GetLatestVisualPositionWorld'))
  self.assertLess(publish.index('if(frames.publish(f))'),publish.index('dialogueInterest.store'))
  self.assertLess(publish.index('cleanup.store(true'),publish.index('dialogueInterest.store'))
  self.assertIn('revision!=dialogueRevision.load',runtime)
  self.assertNotIn('GetEntitiesInSphere',SCRIPT.read_text())
  self.assertNotIn('GetBonePos',SCRIPT.read_text())
 def receiptStep(self):
  # Interpret the actual receipt branch. Native DoDialog and animation tasks are
  # explicit leaves; LuaGate, permanent waits and the 250ms cadence use XML.
  if not hasattr(self,'receipt'):
   outer=ET.parse(XML).find('.//Root/Behavior/FuseBox/Child/Parallel')
   optional=next(n for n in outer if n.tag!='DoDialog')
   if optional.tag=='LuaGate':
    branch=optional.find('Then' if self.lua.execute(optional.attrib['code']) else 'Else')
    optional=branch[0]
   self.receipt=optional.find('./Selector/Loop/Sequence') if optional.tag=='Parallel' else None
   self.receiptBlocked=self.receipt is None
  if self.receiptBlocked:return
  self.lua.execute('entity=npc')
  gate=self.receipt.find('LuaGate')
  branch=gate.find('Then' if self.lua.execute(gate.attrib['code']) else 'Else')
  for node in branch:
   if node.tag=='ExecuteLua':self.lua.execute(node.attrib['code'])
   elif node.tag=='Wait' and node.attrib['duration']=="'-1'":self.receiptBlocked=True
   elif node.tag!='Success':raise AssertionError('unhandled receipt node '+node.tag)
  self.assertEqual(self.receipt.find('Wait').attrib['duration'],"'250ms'")
 def test_delayed_first_packet_reaches_same_native_dialogue(self):
  self.receiptStep() # DoDialog begins before the DLL/VM packet exists.
  self.runLua('assert(next(KCD2VR_LifeD73.dialogs)==nil and schedules==0 and actorReads==0)')
  self.runLua("kcd2vr_life_d73('D76|1|7|0|1|1|0|0|0|0|1|0|500|1|102')")
  self.receiptStep() # The same subtree must re-evaluate, not wait for re-entry.
  self.runLua('fire();assert(owned()==2 and KCD2VR_LifeD73.CanGesture(npc) and scans==0 and bones==0)')
 def test_packet_expiry_then_reenable_does_not_latch_receipt_off(self):
  self.receiptStep()
  self.runLua("kcd2vr_life_d73('D76|1|7|0|1|1|0|0|0|0|1|0|500|1|102')")
  self.receiptStep();self.runLua('fire();assert(owned()==2);t=t+1;fire();assert(owned()==0)')
  self.receiptStep()
  self.runLua("kcd2vr_life_d73('D76|1|7|0|1|1|0|0|0|0|1|0|500|1|102')")
  self.receiptStep();self.runLua('fire();assert(owned()==2 and scans==0 and bones==0)')
 def test_current_npc_subtitle_retires_listening_without_losing_gaze(self):
  self.runLua("kcd2vr_life_d73('D76|1|7|0|1|1|0|0|0|0|1|0|500|1|102')")
  self.receiptStep();self.runLua('fire();assert(owned()==2 and KCD2VR_LifeD73.CanGesture(npc))')
  self.runLua("kcd2vr_life_d73('D76|1|7|0|1|0|0|0|0|0|1|0|500|0|102');assert(owned()==1 and not KCD2VR_LifeD73.CanAnimate(npc,true))")
  self.runLua("kcd2vr_life_d73('D76|1|7|0|1|1|0|0|0|0|1|0|500|1|102')")
  self.receiptStep();self.runLua('fire();assert(owned()==2 and KCD2VR_LifeD73.CanGesture(npc))')
 def test_xml_native_lifetime_and_finite_receipt_retry(self):
  tree=ET.parse(XML);fuse=tree.find('.//Root/Behavior/FuseBox')
  outer=fuse.find('./Child/Parallel');self.assertIsNotNone(outer)
  self.assertEqual(outer.attrib,{'successMode':'Any','failureMode':'Any'})
  self.assertEqual(len(tree.findall('.//DoDialog')),1)
  self.assertEqual(outer.find('DoDialog').attrib,{'DialogId':'$dialogRequest.reqId'})
  self.assertIsNone(outer.find('LuaGate')) # no one-time OFF decision around loops
  parallel=outer.find('Parallel');self.assertEqual(parallel.attrib,{'successMode':'Any','failureMode':'Any'})
  receipt=parallel.find('./Selector/Loop/Sequence/LuaGate');self.assertIsNotNone(receipt)
  self.assertEqual(receipt.attrib['code'],'return KCD2VR_LifeD73 and KCD2VR_LifeD73.Enabled()')
  self.assertEqual([e.tag for e in receipt.find('Then')],['ExecuteLua'])
  self.assertEqual([e.tag for e in receipt.find('Else')],['Success'])
  self.assertEqual(len(receipt.find('Else').findall('.//ExecuteLua')),0)
  self.assertEqual(fuse.find('./OnFail/ClearInbox').attrib['inbox'],"'dialogMailbox'")

if __name__=='__main__':
 result=unittest.TextTestRunner(verbosity=2).run(unittest.defaultTestLoader.loadTestsFromTestCase(Idle))
 report={'passed':result.wasSuccessful(),'tests':result.testsRun,'runtime':'Lua 5.1 via lupa.lua51',
  'scope':'Actual script with explicit timer/link/entity leaves; receipt XML gates/waits interpreted by a small host runner. Native behavior-tree scheduler and headset NOT_RUN.',
  'limitations':['One already queued callback may drain after OFF, without NPC queries.',
   'Native DoDialog remains outside optional work. Host XML runner models only receipt gates, waits and Lua; native scheduling not validated.',
   'A legacy D79 timer closure already running in the same VM is not cancelled; new delivery requires a fresh game/script VM.'],
  'sources':{str(p.relative_to(ROOT)):hashlib.sha256(p.read_bytes()).hexdigest() for p in (SCRIPT,XML,Path(__file__))}}
 if len(sys.argv)>1:
  out=Path(sys.argv[1]);out.parent.mkdir(parents=True,exist_ok=True);out.write_text(json.dumps(report,indent=2)+'\n')
 sys.exit(0 if result.wasSuccessful() else 1)
