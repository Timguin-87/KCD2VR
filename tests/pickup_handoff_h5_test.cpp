// Complete production owner and real Grip producer; only existing native/OS
// leaves are substituted. This tests ordering, not retail action completion.
#define main inherited_native_grasp_owner_h5
#include "../src/subsystems/doors/hand_only_h40.h"
#include "native_grasp_owner13_test.cpp"
#undef main
#include <crtdbg.h>
#include <cstdlib>

namespace kcd2vr::action_time_owner59c11 { namespace {
static unsigned handoffChecksH5=0,handoffFailuresH5=0;
static void CheckH5(bool value,const char* message){
 ++handoffChecksH5;if(!value){++handoffFailuresH5;std::printf("FAIL %s\n",message);}
}
static bool attachInOriginalH5=false;
static void (*afterAttachH5)()=nullptr;
static std::uint64_t originalTokenH5=0,originalAttachTickH5=0;
static void AttachInsideOriginalH5(void* ctrl,float dt){
 UpdateActionLeaf(ctrl,dt);
 if(!attachInOriginalH5)return;attachInOriginalH5=false;
 std::thread job([]{pickupThread19c4=19728;attach(false);});job.join();
 originalTokenH5=g_pick13.bind.token;originalAttachTickH5=g_pick13.attachedAt;
 CheckH5(g_pick13.controllerPausePending19c4&&g_owner.state==PendingAttach,
         "actual native job publishes pending adoption while original Update is running");
 CheckH5(Rd<float>(action.data(),kActSpeedBias,-1)==1.f&&finishes==0,
         "native job never writes controller action speed or finishes the action");
 if(afterAttachH5)afterAttachH5();
}
static void ProducerRequestH5(){
 reset();ResetOwner();g_consumed_use13=0;
 // This retained owner/lifetime fixture uses the unchanged distant Assisted
 // transport timing; Direct pickup latency has its own production fixture.
 pickup_use19c3::setNearOnlyD40(false);
 rawSampleScaleH5=1000000;
 SetPickupHold62(false);SetPickupExperiment62(false);
 PickupProducer19 producer;producer.f5Mode=0;producer.comparison114=false;
 const auto selected=[](std::uint64_t now){auto& c=interaction_alignment::testCandidate;
  c={};c.entity=72;c.owner=0;c.object=true;c.distance=.15f;c.tick=now;
  std::strcpy(c.cls,"PickableItem");};
 selected(1000);producer.tick(1000,0);
 for(std::uint64_t now=1020;now<=1180;now+=20){selected(now);producer.tick(now,1);}
 CheckH5(producer.touch_bindings_.pulses==1&&!producer.touch_bindings_.held,
         "ordinary F5 OFF producer sends one native short Use while Grip stays physically held");
 Use13 use{};CheckH5(g_use13.read(use)&&use.physicalPickup63&&NativeGraspEnabled(),
         "actual transported per-request pickup enables hold without global pickup preference");
 frame(1180);QueueDetour(controller.data(),action.data(),0);TickOwner(controller.data(),1180);
 CheckH5(g_owner.state==Playing&&g_owner.side==0,"same native Picking root owns the transported request");
 ng13::HandFrame committed{};
 CheckH5(g_hands13.read(committed)&&committed.sample==1180&&g_owner.useInput55.sample==1180000000,
         "raw input sourceNs and committed hand sequence remain distinct production domains");
 testAttachManager=manager.data();g_update_original=&AttachInsideOriginalH5;
 updateOriginalCalls=0;attachInOriginalH5=true;originalTokenH5=originalAttachTickH5=0;
 afterAttachH5=nullptr;
}
int HandoffScenarioH5(){
 ProducerRequestH5();frame(1190);UpdateDetour(controller.data(),.02f);
 CheckH5(updateOriginalCalls==1&&attaches==1&&notices==1&&transfers==0,
         "original controller Update and original native attach each run once");
 CheckH5(g_owner.state==Held&&g_pick13.attached&&!g_pick13.controllerPausePending19c4&&
         Rd<float>(action.data(),kActSpeedBias,-1)==0.f,
         "post-return exact pending native attachment is adopted before the next Update");
 CheckH5(g_pick13.bind.token==originalTokenH5&&g_pick13.attachedAt==originalAttachTickH5&&
         originalAttachTickH5==1190&&g_owner.useInput55.tick==1180,
         "post-return adoption preserves original request and attach occurrence clocks");
 poseProof55();frame(1200);UpdateDetour(controller.data(),.02f);
 CheckH5(g_owner.state==Held&&g_pick13.cleanupSeen&&finishes==1&&transfers==0,
         "original socket proof precedes once-only native finish and current held requalification");
 frame(1220);UpdateDetour(controller.data(),.02f);
 CheckH5(attaches==1&&notices==1&&finishes==1&&transfers==0&&drops==0,
         "later held frames cannot repeat attach, native finish, inventory storage or drop");
 frame(1240,1,.20f);UpdateDetour(controller.data(),.02f);
 frame(1460,1,.20f);UpdateDetour(controller.data(),.02f);
 CheckH5(transfers==1&&g_owner.state==Idle&&notices==1,
         "fresh deliberate chest stow retains exactly one native transfer and notice");
 // Every refusal below occurs AFTER a real native job Attach inside Update.
 // It may not become a speed/attachment owner through the new return route.
 const auto refuse=[](void (*change)(),const char* name){
  ProducerRequestH5();afterAttachH5=change;frame(1190);UpdateDetour(controller.data(),.02f);
  CheckH5(updateOriginalCalls==1&&attaches==1&&notices==1&&transfers==0,name);
  CheckH5(Rd<float>(action.data(),kActSpeedBias,-1)==1.f&&!g_pick13.attached&&
          g_owner.state!=Held&&finishes==0&&drops==0,
          "hard loss after original Update cannot grant a new pause, hold, finish or drop");
 };
 refuse(+[]{SetContextAvailable(false);},"context loss preserves original native callback parity");
 refuse(+[]{SetContextAvailable(false);SetContextAvailable(true);},"context generation cannot be revived in the same return");
 refuse(+[]{g_native_ready13=false;},"unavailable native backend cannot adopt a pending attach");
 refuse(+[]{g_foreign_cancel13=true;},"foreign cancellation cannot adopt a pending attach");
 refuse(+[]{g_owner_thread13=99;},"different controller thread cannot adopt a pending attach");
 refuse(+[]{g_henry_ctrl=nullptr;},"controller retirement cannot adopt a pending attach");
 refuse(+[]{put(action.data(),kActToken,unsigned(56));},"changed native root token cannot adopt a pending attach");
 refuse(+[]{put(action.data(),0,WhBase()+0x3B13F38);},"replacement native action class cannot adopt a pending attach");
 refuse(+[]{put(scope.data(),kScopeAction,static_cast<void*>(nullptr));},"ended native root cannot adopt a pending attach");
 refuse(+[]{put(player.data(),0x30,unsigned(43));},"changed actor cannot adopt a pending attach");
 refuse(+[]{put(slots.data(),0x260,key+1);},"replacement hand item cannot adopt the old pending attach");
 refuse(+[]{put(item.data(),0x30,key+1);},"replaced item identity cannot adopt the old pending attach");
 refuse(+[]{put(attachmentObject.data(),8,unsigned(73));},"replacement native entity attachment cannot adopt the old item");
 refuse(+[]{g_pick13.bind.token++;},"different grasp token cannot borrow the old outer callback lifetime");
 refuse(+[]{ng13::AcquisitionInput55 in{};g_acquisition_input55.read(in);in.tracked[0]=false;PublishAcquisitionInput55(in);},
        "raw tracking loss irreversibly retires the transported request");
 refuse(+[]{ng13::AcquisitionInput55 in{};g_acquisition_input55.read(in);in.epoch++;PublishAcquisitionInput55(in);},
        "changed body epoch cannot adopt a pending attach");
 refuse(+[]{ng13::AcquisitionInput55 in{};g_acquisition_input55.read(in);in.trackingEpoch++;PublishAcquisitionInput55(in);},
        "changed tracking epoch cannot adopt a pending attach");
 refuse(+[]{ng13::AcquisitionInput55 in{};g_acquisition_input55.read(in);in.sample=1179;PublishAcquisitionInput55(in);},
        "fresh raw sample behind the original transported source cannot adopt a pending attach");
 refuse(+[]{ng13::AcquisitionInput55 in{};g_acquisition_input55.read(in);in.tick=1179;PublishAcquisitionInput55(in);},
        "fresh raw tick behind the original transported source cannot adopt a pending attach");
 refuse(+[]{ng13::HandFrame in{};g_hands13.read(in);in.tracked[0]=false;PublishHands(in);},
        "held or invalid committed hand contributes no new pickup owner");
 refuse(+[]{ng13::HandFrame in{};g_hands13.read(in);in.grip[0]=0;PublishHands(in);},
        "physical Grip release cannot adopt a pending attach");
 refuse(+[]{ng13::HandFrame in{};g_hands13.read(in);in.tick=1179;PublishHands(in);},
        "fresh committed hand predating the original Use cannot adopt a pending attach");
 refuse(+[]{worldReadChangeH5=+[]{ng13::HandFrame in{};g_hands13.read(in);in.tracked[0]=false;PublishHands(in);};},
        "native getter invalidating committed hand cannot create a post-return pause");
 refuse(+[]{worldReadChangeH5=+[]{ng13::HandFrame in{};g_hands13.read(in);in.epoch++;PublishHands(in);};},
        "native getter replacing committed body cannot create a post-return pause");
 refuse(+[]{worldReadChangeH5=+[]{SetContextAvailable(false);};},
        "native getter changing gameplay context cannot create a post-return pause");
 refuse(+[]{hostTickCount64=1189;},"future original source clock cannot adopt a pending attach");
 refuse(+[]{hostTickCount64=1251;},"head source61ms old cannot adopt a pending attach");
 refuse(+[]{rawFrame55(1271);},"committed hand81ms old cannot adopt a pending attach");
 refuse(+[]{frame(2191);},"original pending attach1001ms old cannot be revived by fresh hands");
 refuse(+[]{PickupEndDetour19c4(logic.data(),0,0);},
        "completed native cleanup inside original Update stays authoritative");
 ProducerRequestH5();afterAttachH5=+[]{frame(2190);};frame(1190);UpdateDetour(controller.data(),.02f);
 CheckH5(g_owner.state==Held&&g_pick13.attached&&g_pick13.attachedAt==1190,
         "original pending1000ms boundary may adopt current fresh attachment without clock renewal");
 ProducerRequestH5();afterAttachH5=+[]{hostTickCount64=1250;};frame(1190);UpdateDetour(controller.data(),.02f);
 CheckH5(g_owner.state==Held&&g_pick13.attached&&g_pick13.attachedAt==1190,
         "head source60ms boundary remains the existing exact grace, not a new deadline");
 ProducerRequestH5();afterAttachH5=+[]{rawFrame55(1270);};frame(1190);UpdateDetour(controller.data(),.02f);
 CheckH5(g_owner.state==Held&&g_pick13.attached&&g_pick13.attachedAt==1190,
         "committed hand80ms boundary retains original clock with current raw input");
 // A job arriving AFTER return is outside this correction. It retains the
 // existing next-controller-tick path; no replay or worker speed write.
 ProducerRequestH5();attachInOriginalH5=false;frame(1190);UpdateDetour(controller.data(),.02f);
 std::thread lateJob([]{pickupThread19c4=19728;attach(false);});lateJob.join();
 CheckH5(g_owner.state==PendingAttach&&Rd<float>(action.data(),kActSpeedBias,-1)==1.f,
         "job arriving after original return stays pending and cannot pause from its job thread");
 frame(1200);UpdateDetour(controller.data(),.02f);
 CheckH5(g_owner.state==Held&&attaches==1&&notices==1&&transfers==0,
         "late callback keeps the existing exact next-owner-tick adoption without replay");
 // A transported request already admitted by native Picking remains the same
 // operation while the physical hand briefly opens before its Attach.
 ProducerRequestH5();Use13 approach{};g_use13.read(approach);attachInOriginalH5=false;
 frame(1190,0);UpdateDetour(controller.data(),.02f);
 CheckH5(UseLive55(approach)&&g_owner.state==Playing&&NativeGraspEnabled()&&!CurrentOwnerInput55(1190),
         "open hand preserves exact admitted approach interest without granting current Grip ownership");
 CheckH5(attaches==0&&transfers==0&&notices==0&&finishes==0,
         "opening during the approach neither finishes native Picking nor invents a held item");
 frame(1200);attachInOriginalH5=true;UpdateDetour(controller.data(),.02f);
 CheckH5(g_owner.state==Held&&g_owner.useSerial54==approach.serial&&g_owner.useInput55.tick==1180&&
         g_pick13.attachedAt==1200&&attaches==1&&notices==1&&transfers==0,
         "re-squeeze before native Attach adopts the original request and original item once");
 poseProof55();frame(1210);UpdateDetour(controller.data(),.02f);
 rawFrame55(1220,0);UpdateDetour(controller.data(),.02f);frame(1240);UpdateDetour(controller.data(),.02f);
 CheckH5(drops==1&&transfers==0&&attaches==1&&notices==1&&g_owner.state==Idle&&!UseLive55(approach),
         "post-Attach raw release drops once despite the older completed hand still showing Grip and cannot replay old Use");
 // Playing is not proof that native Attach has not happened. The existing
 // binding lifetime wins even if its state label still says Playing.
 ProducerRequestH5();frame(1190);UpdateDetour(controller.data(),.02f);poseProof55();
 g_owner.state=Playing;Publish();const auto heldToken=g_pick13.bind.token;
 rawFrame55(1200,0);UpdateDetour(controller.data(),.02f);frame(1220);UpdateDetour(controller.data(),.02f);
 CheckH5(heldToken&&drops==1&&transfers==0&&attaches==1&&notices==1&&g_owner.state==Idle,
         "Playing plus actual Attach cannot retain approach interest over raw release or delay the original single drop");
 // Reset of owner A retires A only, never a newer transported request B.
 ProducerRequestH5();Use13 oldUse{},newUse{};g_use13.read(oldUse);
 NotePhysicalPickupUse63(0,1180,72);g_use13.read(newUse);ResetOwner();
 CheckH5(newUse.serial>oldUse.serial&&!UseLive55(oldUse)&&UseLive55(newUse),
         "retiring original owner A preserves independently transported newer request B");
 rawFrame55(1190,0);rawFrame55(1200);const auto nativeBefore=eventCalls;
 put(action.data(),kActToken,Rd<std::uint32_t>(action.data(),kActToken,0)+1);attach(false);
 CheckH5(!UseLive55(newUse)&&g_owner.state==Idle&&eventCalls==nativeBefore+1&&
         !g_pick13.attachStarted&&!g_pick13.bind.token&&transfers==1,
         "before-admission release leaves the later native event and its original transfer once, with no old-request VR takeover");
 ProducerRequestH5();attachInOriginalH5=false;frame(1190,0);UpdateDetour(controller.data(),.02f);
 grasp13::AcquisitionInput55 changed{};g_acquisition_input55.read(changed);++changed.trackingEpoch;
 changed.tick=hostTickCount64=1200;changed.sample+=10000000;changed.grip[0]=1;PublishAcquisitionInput55(changed);
 CheckH5(!NativeGraspEnabled()&&!CurrentOwnerInput55(1200),
         "new tracking lifetime still retires the old approach; re-squeeze never deletes its epoch identity");
 std::printf("H5 native pickup post-Update handoff checks=%u failures=%u\n",handoffChecksH5,handoffFailuresH5);
 return handoffFailuresH5?1:0;
}
}}
int main(){_set_error_mode(_OUT_TO_STDERR);_CrtSetReportMode(_CRT_ASSERT,_CRTDBG_MODE_FILE);
 _CrtSetReportFile(_CRT_ASSERT,_CRTDBG_FILE_STDERR);
 _set_abort_behavior(0,_WRITE_ABORT_MSG|_CALL_REPORTFAULT);
 return kcd2vr::action_time_owner59c11::HandoffScenarioH5();}
