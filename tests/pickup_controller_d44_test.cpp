// Complete production action-owner TU using the established memory/native
// leaves. These cases reproduce migrating engine Update callbacks; they do not
// simulate successful native calls from proximity or invent an Attach event.
#include <windows.h>
#if defined(_MSC_VER)
#undef __try
#undef __except
#endif
#define main inherited_native_grasp_owner_d44
#include "native_grasp_owner13_test.cpp"
#undef main

namespace kcd2vr::action_time_owner59c11 { namespace {
static unsigned checksD44=0;
static void requireD44(bool value,const char* reason){
 ++checksD44;if(!value){std::fprintf(stderr,"FAIL D44 %s\n",reason);std::exit(1);}
}
static void (*afterAttachD44)()=nullptr;
static void NativeAttachInsideUpdateD44(void* ctrl,float dt){
 if(ctrl!=controller.data()){
  requireD44(g_owner_transaction_depth19c4==0&&!OnOwnerThread13(),
             "nested foreign controller receives no pickup owner transaction");
  ++updateOriginalCalls;return;
 }
 requireD44(g_owner_transaction_depth19c4==0&&!OnOwnerThread13(),
            "current callback releases its transaction before original native Update");
 requireD44(Rd<float>(testSkel,0x50,-1.f)==1.f,
            "current migrated callback restores the previous clip lease before native Update");
 UpdateActionLeaf(ctrl,dt);
 static std::array<unsigned char,0x1100> foreignController{};
 UpdateDetour(foreignController.data(),dt);
 std::thread nativeJob([]{pickupThread19c4=24777;attach(false);});nativeJob.join();
 requireD44(g_owner.state==PendingAttach&&g_pick13.controllerPausePending19c4&&
            !g_pick13.attached&&finishes==0&&transfers==0,
            "real native Attach has occurred but its job grants no controller pause or placement");
 if(afterAttachD44)afterAttachD44();
}
static void MigratedUseD44(std::uint32_t expected=72){
 reset();ResetOwner();g_consumed_use13=0;afterAttachD44=nullptr;
 testAttachManager=manager.data();
 put(action.data(),0,WhBase()+0x3B13F38);put(action.data(),kActFragId,int(0));
 pickupThread19c4=19728;frame(980);UpdateDetour(controller.data(),.02f);
 RootObservation55 before{};
 requireD44(g_root_before_use55.read(before)&&before.valid&&!before.picking,
            "verified current controller Update observes the pre-Use root on a migrated worker");
 frame(1000);NotePhysicalPickupUse63(0,1000,expected);
 put(action.data(),0,WhBase()+0x3A450B0);put(action.data(),kActFragId,int(1));
 put(action.data(),kActToken,unsigned(56));
 put(testSkel,0,WhBase()+kSkelVft);put(image.data(),kSkelVft+0xB8,WhBase()+kLayerScaleH23);
 std::memcpy(image.data()+kLayerScaleH23,kLayerScalePinH23,sizeof(kLayerScalePinH23));
 put(scope.data(),kScopeNumLayers,unsigned(3));put(scope.data(),kScopeSpeedBias,1.f);put(testSkel,0x50,1.f);
 g_update_original=&UpdateActionLeaf;frame(1020);UpdateDetour(controller.data(),.02f);
 requireD44(g_owner_thread13==1&&!OnOwnerThread13()&&g_owner.state==Playing&&
            g_owner.expectedEntityD40==expected&&attaches==0&&transfers==0&&finishes==0,
            "new exact Picking root is armed before Attach without changing the global service thread owner");
 ng13::Acquisition54 visual{};
 requireD44(ReadAcquisition54(visual)&&visual.active&&g_pickup_clock_h23.active&&
            Rd<float>(testSkel,0x50,-1.f)==8.f&&Rd<float>(action.data(),kActSpeedBias,-1.f)==1.f,
            "current callback publishes controller presentation and existing clip-only acceleration, never IAction speed");
}
static void ScenarioD44(){
 MigratedUseD44();g_update_original=&NativeAttachInsideUpdateD44;
 pickupThread19c4=27852;frame(1040);UpdateDetour(controller.data(),.02f);
 requireD44(attaches==1&&notices==1&&transfers==0&&g_owner.state==Held&&g_pick13.attached&&
            !g_pick13.controllerPausePending19c4&&g_pick13.attachedAt==1040&&
            Rd<float>(action.data(),kActSpeedBias,-1.f)==0.f,
            "same-return adoption follows real Attach on another current Update worker without a cached-thread delay");
 // Render/socket proof is independent and must exist before native Finish.
 requireD44(finishes==0&&!g_pick13.bind.gripReady55,"real attached item waits for its actual canonical socket proof");
 poseProof55();g_update_original=&UpdateActionLeaf;pickupThread19c4=31808;
 frame(1060);UpdateDetour(controller.data(),.02f);
 requireD44(finishes==1&&g_pick13.bind.gripReady55&&g_pick13.cleanupSeen&&g_owner.state==Held&&
            attaches==1&&transfers==0,"next current callback captures the original grip and finishes Picking exactly once");
 frame(1080);UpdateDetour(controller.data(),.02f);
 requireD44(finishes==1&&attaches==1&&transfers==0&&g_owner_thread13==1,
            "held updates cannot replay Attach/Finish or grant unrelated services migrated-thread authority");
 frame(1100,0);UpdateDetour(controller.data(),.02f);
 requireD44(drops==1&&transfers==0&&g_owner.state==Idle,"fresh release uses the same current exact hand and native drop chain");

 // A replacement root must see its original native layer speed even when the
 // cached worker never visits this controller again.
 MigratedUseD44();put(action.data(),0,WhBase()+0x3B13F38);put(action.data(),kActFragId,int(0));
 g_update_original=+[](void*,float){requireD44(Rd<float>(testSkel,0x50,-1.f)==1.f&&!g_pickup_clock_h23.active,
                         "replacement native action never inherits pickup clip acceleration");};
 pickupThread19c4=27852;frame(1040);UpdateDetour(controller.data(),.02f);
 requireD44(g_owner.state==Idle&&attaches==0&&finishes==0&&transfers==0,"replacement retires approach without native transfer or Finish");

 for(const DWORD currentWorker:{DWORD(1),DWORD(27852)}){
  MigratedUseD44();pickupThread19c4=currentWorker;g_owner_transaction19c4.test_and_set();
  put(action.data(),0,WhBase()+0x3B13F38);put(action.data(),kActFragId,int(0));
  g_update_original=+[](void*,float){requireD44(Rd<float>(testSkel,0x50,-1.f)==1.f&&!g_pickup_clock_h23.active,
                          "independent clip restoration on current/cached worker precedes a replacement even while the general owner transaction is busy");};
  frame(1040);UpdateDetour(controller.data(),.02f);g_owner_transaction19c4.clear();
  requireD44(attaches==0&&transfers==0&&finishes==0,"busy owner cannot invent native pickup outcomes");
 }

 MigratedUseD44(73);g_update_original=+[](void* ctrl,float dt){
  UpdateActionLeaf(ctrl,dt);std::thread nativeJob([]{pickupThread19c4=24777;attach(false);});nativeJob.join();
 };
 pickupThread19c4=27852;frame(1040);UpdateDetour(controller.data(),.02f);
 ng13::Binding rejected{};
 const bool rejectedBinding=ReadGraspBinding(rejected);
 if(transfers!=1||attaches!=1||notices!=1||finishes||rejectedBinding)std::fprintf(stderr,
  "mismatch evidence: transfers=%d attaches=%d notices=%d finishes=%d binding=%d state=%d expected=%u\n",
  transfers,attaches,notices,finishes,int(rejectedBinding),int(g_owner.state),g_owner.expectedEntityD40);
 // The original TransferLeaf keeps its native Attach/notice semantics. The
 // absence of our binding/held state proves refusal, not a zero native Attach.
 requireD44(transfers==1&&attaches==1&&notices==1&&finishes==0&&!rejectedBinding&&
            !g_pick13.attachStarted&&g_owner.state==Idle&&Container13(item.data())==&inventory,
            "new migrated owner cannot substitute native target72 for transported expected target73");

 const auto refuse=[](void (*change)(),const char* reason){
  MigratedUseD44();afterAttachD44=change;g_update_original=&NativeAttachInsideUpdateD44;
  pickupThread19c4=27852;frame(1040);UpdateDetour(controller.data(),.02f);
  requireD44(attaches==1&&notices==1&&transfers==0&&finishes==0&&!g_pick13.attached&&
             g_owner.state!=Held&&Rd<float>(action.data(),kActSpeedBias,-1.f)==1.f,reason);
 };
 refuse(+[]{put(action.data(),kActToken,unsigned(57));},"root token replacement after original Update refuses adoption");
 refuse(+[]{put(action.data(),0,WhBase()+0x3B13F38);},"root class replacement after original Update refuses adoption");
 refuse(+[]{g_henry_ctrl=nullptr;},"retired controller cannot inherit callback authority");
 refuse(+[]{put(player.data(),0x30,unsigned(43));},"changed Henry actor cannot adopt the prior attachment");
 refuse(+[]{put(attachmentObject.data(),8,unsigned(73));},"changed native entity attachment cannot adopt the old target");
 refuse(+[]{SetContextAvailable(false);},"lost interaction context cannot adopt a pending native attachment");
 refuse(+[]{frame(1040,0);},"released hand cannot adopt the old request");
 refuse(+[]{g_foreign_cancel13=true;},"native foreign cancellation wins over post-Update adoption");

 // A callback which cannot acquire the one transaction remains native, and
 // direct calls from a random worker do not obtain the scoped owner.
 reset();testAttachManager=manager.data();pickupThread19c4=19728;attach(false);
 requireD44(g_owner.state==PendingAttach&&!g_pick13.attached,"independent native job remains pending");
 frame(1020);TickGrasp13(1020);
 requireD44(!g_pick13.attached&&finishes==0,"arbitrary foreign call outside verified Update cannot mutate the pickup");
 g_owner_transaction19c4.test_and_set();g_update_original=&UpdateActionLeaf;
 UpdateDetour(controller.data(),.02f);g_owner_transaction19c4.clear();
 requireD44(!g_pick13.attached&&finishes==0&&transfers==0,"contended owner transaction grants no pickup mutation");
 frame(1040);UpdateDetour(controller.data(),.02f);
 requireD44(g_pick13.attached&&g_owner.state==Held,"next uncontended verified Update adopts the original pending attachment");
 std::printf("pickup_controller_d44: %u checks PASS; production owner/Update/native-attachment chain; no retail/headset execution\n",checksD44);
}
}}
int main(){kcd2vr::action_time_owner59c11::ScenarioD44();}
