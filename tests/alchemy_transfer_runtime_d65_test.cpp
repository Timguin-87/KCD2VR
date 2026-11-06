// Includes the complete production owner TU with explicit native leaves.
#include "../src/vrik/static_grab_r89.h" // current pickup producer dependency (same include as production TU)
#define KCD2VR_RESOURCE_D69_RETAIL_GATE
#define KCD2VR_RESOURCE_D60_NO_MAIN
#include "alchemy_resource_receipt_d60_test.cpp"
namespace kcd2vr::action_time_owner59c11 { namespace {
static unsigned checks65=0;static void ck65(bool ok,const char* name){++checks65;if(!ok){std::fprintf(stderr,"FAIL D65 %s\n",name);std::exit(1);}}
static void transferSetup65(unsigned stage=0){
 g_alchemy_transfer_d65={};g_alchemy_transfer_pending_d65=false;g_alchemy_transfer_stop_d65=false;g_alchemy_transfer_lock_d65.clear();
 ResourceFixture46();g_owner={};g_pickup_clock_h23={};g_pickup_clock_live_h23=false;g_alchemy_stage19c4={};
 const auto b=g_base.load();for(const auto& x:kAlchemyTransferStagesD65){put(image.data(),x.vft+0xF8,b+x.start);std::memcpy(image.data()+x.slotPin,x.bytes,4);}
 put(parent46.data(),0,b+kAlchemyTransferStagesD65[stage].vft);put(parent46.data(),0x70,reinterpret_cast<std::uintptr_t>(action.data()));
 put(scope.data(),kScopeLayer,0);put(scope.data(),kScopeNumLayers,1u);put(scope.data(),kScopeSpeedBias,1.f);put(action.data(),kActSpeedBias,1.f);
 put(testSkel,0,b+kSkelVft);put(image.data(),kSkelVft+0xB8,b+kLayerScaleH23);std::memcpy(image.data()+kLayerScaleH23,kLayerScalePinH23,sizeof(kLayerScalePinH23));put(testSkel,0x50,1.f);
 kcd2vr::minigame_hands45::mode=kcd2vr::minigame_hands45::Gesture;
 namespace ab=kcd2vr::subsystems::alchemy::bridge67;ab::Control c{};
 c.instance=reinterpret_cast<std::uintptr_t>(alchemy46.data());c.lifetime=kcd2vr::activity_input163::witness175.lifetime175.load();
 c.parent=reinterpret_cast<std::uintptr_t>(parent46.data());c.tick=hostTickCount64;c.enabled=true;c.side=1;c.modeEpoch=kcd2vr::minigame_hands45::modeEpoch67.load();
 c.poseEpoch=testWorkstationFrame46.poseEpoch;c.sourceEpoch=testWorkstationFrame46.sourceEpoch;ab::control.publish(c);
 g_alchemy_transfer_rate_d65=3;
}
static void tickTransfer65(){OwnerTransaction19c4 tx;AlchemyUpdateScopeR27 a(controller.data());TickAlchemyTransferClipD65(controller.data(),hostTickCount64);}
static bool restoreTransfer65(){return RestoreAlchemyTransferClipD65(controller.data());}
static void pickupSetup65(){
 g_alchemy_transfer_rate_d65=1;g_alchemy_transfer_pending_d65=false;g_alchemy_transfer_d65={};
 reset();ResetOwner();g_consumed_use13=0;g_unowned_pickup_join_d65={};g_unowned_pickup_clock_d65=true;
 kcd2vr::activity_input163::witness175.Clear();
 put(action.data(),0,WhBase()+0x3B13F38);put(action.data(),kActContext,static_cast<void*>(actionContext.data()));
 put(action.data(),kActFragId,int(0));frame(980);UpdateDetour(controller.data(),.02f);frame(1000);NoteUseSent(0,1000);
 put(action.data(),0,WhBase()+0x3A450B0);put(action.data(),kActFragId,int(1));put(action.data(),kActToken,unsigned(56));frame(1020);
 put(testSkel,0,WhBase()+kSkelVft);put(image.data(),kSkelVft+0xB8,WhBase()+kLayerScaleH23);std::memcpy(image.data()+kLayerScaleH23,kLayerScalePinH23,sizeof(kLayerScalePinH23));
 put(scope.data(),kScopeNumLayers,1u);put(scope.data(),kScopeSpeedBias,1.f);put(testSkel,0x50,1.f);
}
static void tickPickup65(){OwnerTransaction19c4 tx;TickUnownedPickupClipD65(controller.data(),hostTickCount64);}
#include "alchemy_resource_root3_runtime_d69.inl"
static int run65(){
 runResourceRoot3D69();
 for(unsigned i=0;i<8;++i){transferSetup65(i);transfer65::Key k{};ck65(ReadAlchemyTransferD65(controller.data(),k),"exact current supported parent joins child");tickTransfer65();
  ck65(g_alchemy_transfer_pending_d65&&Rd<float>(testSkel,0x50,-1)==3,"transfer only clip rate acquired");
  ck65(Rd<float>(action.data(),kActSpeedBias,-1)==1&&Rd<float>(scope.data(),kScopeSpeedBias,-1)==1,"native action and scope speed intact");
  ck65(restoreTransfer65()&&!g_alchemy_transfer_pending_d65&&Rd<float>(testSkel,0x50,-1)==1,"restoration before next native callback");}
 transferSetup65();tickTransfer65();put(controller.data(),kCtrlScopeCount,2u);put(scope.data()+kScopeStride,kScopeLayer,-1);put(scope.data()+kScopeStride,kScopeNumLayers,0u);
 ck65(restoreTransfer65()&&Rd<float>(testSkel,0x50,-1)==1,"inactive zero-layer scope cannot strand current layer cleanup");
 transferSetup65();g_alchemy_transfer_rate_d65=1;tickTransfer65();ck65(!g_alchemy_transfer_pending_d65,"OFF no native write");
 for(int mode=0;mode<2;++mode){transferSetup65();kcd2vr::minigame_hands45::mode=mode;tickTransfer65();ck65(!g_alchemy_transfer_pending_d65,"Native and Semi unchanged");}
 transferSetup65();put(parent46.data(),0,g_base.load()+0x3A5C670);tickTransfer65();ck65(!g_alchemy_transfer_pending_d65,"Use/retort stage not borrowed");
 transferSetup65();put(parent46.data(),0x28,std::uint8_t(0));tickTransfer65();ck65(!g_alchemy_transfer_pending_d65,"not started stage");
 transferSetup65();put(parent46.data(),0x70,std::uintptr_t(0));tickTransfer65();ck65(!g_alchemy_transfer_pending_d65,"missing actual owned child");
 transferSetup65();testWorkstationFrame46.hand[1].valid=false;tickTransfer65();ck65(!g_alchemy_transfer_pending_d65,"tracking loss");
 transferSetup65();put(scope.data(),kScopeNumLayers,3u);g_alchemy_transfer_rate_d65=8;tickTransfer65();ck65(g_alchemy_transfer_pending_d65&&Rd<float>(testSkel,0x50,-1)==8,"multi-layer owner accelerates only single root CAF");ck65(restoreTransfer65(),"8x root rate restored");
 transferSetup65();put(controller.data(),kCtrlScopeCount,2u);put(scope.data()+kScopeStride,kScopeLayer,0);put(scope.data()+kScopeStride,kScopeNumLayers,1u);tickTransfer65();ck65(!g_alchemy_transfer_pending_d65,"overlapping root scope rejected");
 for(int valid=0;valid<3;++valid){transferSetup65();namespace ab=kcd2vr::subsystems::alchemy::bridge67;ab::Control c{};ab::control.read(c);c.passenger=true;c.trackedPresentation71=true;c.gestureTransferD67=valid!=0;if(valid==2)++c.parent;ab::control.publish(c);tickTransfer65();ck65(g_alchemy_transfer_pending_d65==(valid==1),"only exact tracked return passenger qualifies");if(g_alchemy_transfer_pending_d65)ck65(restoreTransfer65(),"return passenger rate restored");}
 transferSetup65();tickTransfer65();put(image.data(),kLayerScaleH23,std::uint8_t(0));
 ck65(!restoreTransfer65()&&g_alchemy_transfer_pending_d65,"unreadable exact setter keeps cleanup");
 // Simulate native continuing while cleanup was unavailable, replacing the
 // action but retaining the layer. Its current native cache owns the give-back.
 put(scope.data(),kScopeAction,static_cast<void*>(nullptr));put(scope.data(),kScopeSpeedBias,2.f);
 std::memcpy(image.data()+kLayerScaleH23,kLayerScalePinH23,sizeof(kLayerScalePinH23));
 ck65(restoreTransfer65()&&Rd<float>(testSkel,0x50,-1)==2&&!g_alchemy_transfer_pending_d65,"replacement gets current cache2 never old saved1 or stranded3");
 transferSetup65();tickTransfer65();put(scope.data(),kScopeAction,static_cast<void*>(nullptr));put(scope.data(),kScopeSpeedBias,3.f);
 ck65(restoreTransfer65()&&Rd<float>(testSkel,0x50,-1)==3,"replacement natively owns3, no clobber");
 transferSetup65();tickTransfer65();put(testSkel,0x50,2.f);ck65(restoreTransfer65()&&Rd<float>(testSkel,0x50,-1)==2,"foreign setter untouched");
 put(testSkel,0x50,1.f);tickTransfer65();ck65(!g_alchemy_transfer_pending_d65,"same child token cannot retake after foreign writer");
 transferSetup65();tickTransfer65();put(controller.data(),kCtrlScopeCount,0u);ck65(!restoreTransfer65()&&g_alchemy_transfer_pending_d65,"unknown current scope owner keeps cleanup");
 put(controller.data(),kCtrlScopeCount,1u);ck65(restoreTransfer65()&&Rd<float>(testSkel,0x50,-1)==1,"cleanup retries when exact scope returns");
 transferSetup65();tickTransfer65();pickupThread19c4=2;ck65(!StopAlchemyTransferClipD65()&&g_alchemy_transfer_pending_d65,"nonowner shutdown defers");pickupThread19c4=1;
 ck65(restoreTransfer65()&&!g_alchemy_transfer_pending_d65,"native owner performs deferred shutdown cleanup");
 pickupSetup65();tickPickup65();ck65(g_owner.state==Idle&&g_pickup_clock_h23.active&&Rd<float>(testSkel,0x50,-1)==8,"current native request allows ownerless clip only");
 ck65(Rd<float>(action.data(),kActSpeedBias,-1)==1&&attaches==0&&transfers==0,"pickup timing never invents Attach/inventory");
 ck65(RestorePickupClipH23()&&Rd<float>(testSkel,0x50,-1)==1,"pickup uses existing restoration boundary");
 pickupSetup65();g_unowned_pickup_clock_d65=false;tickPickup65();ck65(!g_pickup_clock_h23.active,"pickup experiment off");
 pickupSetup65();Use13 retired65{};g_use13.read(retired65);RetireUse55(retired65.serial);tickPickup65();ck65(!g_pickup_clock_h23.active,"retired Use cannot resurrect pickup");
 pickupSetup65();g_use_generation55.fetch_add(1);tickPickup65();ck65(!g_pickup_clock_h23.active,"pickup exact request generation");
 pickupSetup65();tickPickup65();RestorePickupClipH23();put(action.data(),kActToken,unsigned(57));tickPickup65();ck65(!g_pickup_clock_h23.active,"same Use cannot speed later action token");
 pickupSetup65();tickPickup65();put(testSkel,0x50,2.f);ck65(RestorePickupClipH23()&&Rd<float>(testSkel,0x50,-1)==2,"pickup foreign writer retained");
 put(testSkel,0x50,1.f);tickPickup65();ck65(!g_pickup_clock_h23.active,"foreign same request cannot retake after return1");
 pickupSetup65();tickPickup65();put(controller.data(),kCtrlScopeCount,0u);ck65(!RestorePickupClipH23()&&g_pickup_clock_h23.active,"new fallback unknown scope retains cleanup");
 put(controller.data(),kCtrlScopeCount,1u);put(scope.data(),kScopeAction,static_cast<void*>(nullptr));put(scope.data(),kScopeSpeedBias,2.f);
 ck65(RestorePickupClipH23()&&Rd<float>(testSkel,0x50,-1)==2,"pickup replacement restored to new native cache");
 pickupSetup65();tickPickup65();g_pickup_clock_h23.unownedUseD65=0;put(controller.data(),kCtrlScopeCount,0u);
 ck65(RestorePickupClipH23()&&Rd<float>(testSkel,0x50,-1)==1,"zero-tag established H23 UNKNOWN-scope fallback unchanged");
 pickupSetup65();tickPickup65();put(scope.data(),kScopeSpeedBias,8.f);ck65(RestorePickupClipH23()&&Rd<float>(testSkel,0x50,-1)==8,"native pickup8 rate preserved");
 g_unowned_pickup_clock_d65=false;
 g_alchemy_transfer_rate_d65=1;g_alchemy_transfer_stop_d65=false;
 std::printf("D65 actual alchemy transfer owner %u checks PASS; native leaves simulated; game NOT_RUN\n",checks65);return scenarioD60();
}
}}
#ifndef KCD2VR_ALCHEMY_TRANSFER_D65_NO_MAIN
int main(){return kcd2vr::action_time_owner59c11::run65();}
#endif
