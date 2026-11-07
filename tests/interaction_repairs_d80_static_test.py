"""Build-hold source contracts, not a substitute for C++ execution/Windows link."""
from pathlib import Path
import hashlib,json,subprocess,sys,unittest
ROOT=Path(__file__).resolve().parents[1]
def text(rel):return (ROOT/rel).read_text(encoding='utf-8')
def function(s,name):
 a=s.index(name);b=s.index('{',a);level=1;e=b+1
 while level:
  level+=(s[e]=='{')-(s[e]=='}');e+=1
 return s[a:e]
class Contracts(unittest.TestCase):
 def test_same_completed_input_publication(self):
  f=text('src/frik_pose_probe.cpp');c=text('src/vrik/coherent_pose.inl')
  self.assertIn('float pickupTriggerD80[2]{}',f)
  self.assertIn('packet.pickupTriggerD80[0]=tx.tracking.right_trigger;packet.pickupTriggerD80[1]=tx.tracking.left_trigger;',c)
 def test_all_three_lifecycle_consumers_and_old_lab(self):
  s=text('src/vrik/staged_pickup596c2.inl');i=text('src/vrik/instant_pickup_native_d72.inl')
  for name in ['static bool BuildStagedPickup596','static void StagedPropTick596','static std::uint32_t SmallPickupAdmissionR24']:
   body=function(s,name);self.assertIn('pickup_input_d80::refusal',body)
   self.assertNotIn('if(FistInputOwned101())',body)
  self.assertIn('pickup_input_d80::refusal(f,c.prop.side,FistInputOwned101())',function(i,'static bool InstantPickupCurrentD72'))
 def test_no_global_combat_policy_change(self):
  old=subprocess.check_output(['git','show','11e3eae:src/vrik/fist_native.inl'],cwd=ROOT).decode()
  self.assertEqual(old.replace('\r\n','\n'),text('src/vrik/fist_native.inl'))
 def test_retained_hand_claim_and_item_admission(self):
  i=text('src/vrik/instant_pickup_native_d72.inl')
  for name in ['static bool InstantPickupUnclaimedD72','static bool InstantPickupOrdinaryD72','static bool InstantPickupTransferD72']:
   old=subprocess.check_output(['git','show','11e3eae:src/vrik/instant_pickup_native_d72.inl'],cwd=ROOT).decode()
   self.assertEqual(function(old,name),function(i,name))
 def test_no_changed_native_alchemy_semantics_or_mortar(self):
  s=text('src/subsystems/alchemy/instant_native_d72.inl')
  old=subprocess.check_output(['git','show','11e3eae:src/subsystems/alchemy/instant_native_d72.inl'],cwd=ROOT).decode()
  for name in ['static bool AlchemyInvokeSemanticD72','static bool AlchemyCompleteD72','static bool AlchemyInstantPrepareD72','static bool InstallInstantAlchemyD72']:
   self.assertEqual(function(old,name),function(s,name))
 def test_async_only_exact_flask_and_current_tracking(self):
  s=text('src/subsystems/alchemy/instant_native_d72.inl');h=function(s,'static bool AlchemyAttachmentPendingCurrentD80')
  for fragment in ['now-s.semanticTickD80>125','r.source!=5||r.side!=1','workstation_sample46::fresh(f,now)','f.poseEpoch==r.poseEpoch&&f.sourceEpoch==r.sourceEpoch','s.input.entity']:
   self.assertIn(fragment,h)
  tick=function(s,'static void TickInstantAlchemyD72');self.assertEqual(tick.count('AlchemyInvokeSemanticD72(s)'),1)
  self.assertLess(tick.index('s.attempted=true'),tick.index('AlchemyInvokeSemanticD72(s)'))
  self.assertLess(tick.index('if(!s.receipt.effect&&AlchemyAttachmentPendingCurrentD80'),tick.index('s.completionAttempted=true'))
 def test_transactional_contact_publication(self):
  c=text('src/vrik/coherent_pose.inl');f=text('src/frik_pose_probe.cpp');w=text('src/subsystems/vrik/pose_final_wrist_d52.inl')
  self.assertIn('AlchemyContactResultD80 alchemyContactD80[2]{}',f)
  self.assertIn('AlchemyContactResultD80 alchemyContactD80[2]{}',c)
  self.assertIn('packet.alchemyContactD80[side]=tx.alchemyContactD80[side]',c)
  self.assertIn('ts.sequence,ts.source_epoch,goals[side],handDesired,tx.alchemyContactD80[side]',w)
 def test_tool_no_fraction_freeze_or_unqualified_normal_swap(self):
  s=text('src/vrik/alchemy_tool_contact_d53.inl')
  self.assertNotIn('ws::hitFraction',s);self.assertNotIn('nativeNormalD63',s)
  for fragment in ['hit.distance<=.002f','Len(Sub(hit.point,expected))>.002f','dot(Sub(from,hit.point),hit.normal)<=.0005f','if(length>.02f)','cached.sample==sample&&cached.tracking==tracking','al::same(cached.binding,current)','prev.doorTrackingEpoch66!=tracking']:
   self.assertIn(fragment,s)
  self.assertEqual(s.count('WorldRayDistance6692('),1)
  self.assertIn('for(int i=0;i<3;++i)',s)
  self.assertIn('if(coverage&&planeCount)',s)
 def test_generated_flask_adapter_is_current(self):
  p=ROOT/'src/subsystems/alchemy/instant_native_d72.inl';receipt=json.loads(text('tests/generated/instant_mortar_d75_provenance.json'))
  self.assertEqual(receipt['sha256'],hashlib.sha256(p.read_bytes()).hexdigest())
  self.assertIn('AlchemyAttachmentPendingCurrentD80',text('tests/generated/instant_mortar_d75.inc'))
 def test_focused_runtime_fixtures_prepared(self):
  for rel,needle in [('tests/pickup_admission_d78_test.cpp','selectedFistsD80=true'),('tests/prop_adapter5916c1_test.cpp','frame.pickupTriggerD80[0]=1'),('tests/instant_pickup_d72_test.cpp','f.frame.pickupTriggerD80[1]=1'),('tests/instant_flask_d75_test.cpp','delayed.attachmentWaitMsD80==20'),('tests/alchemy_tool_contact_d53_test.cpp','new sample unavailable coverage is permissive')]:
   self.assertIn(needle,text(rel))
if __name__=='__main__':unittest.main()
