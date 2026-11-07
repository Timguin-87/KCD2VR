// Entire production action-owner TU, exact native memory fixtures and explicit
// engine leaves. On a non-Windows host ONLY unrelated SEH syntax is adapted;
// this test makes no Windows exception-cleanup or retail execution claim.
#ifndef _MSC_VER
#define __finally catch(...) {} if(true)
#define __declspec(x)
#endif
#define KCD2VR_RESOURCE_D69_RETAIL_GATE
#define KCD2VR_RESOURCE_D60_NO_MAIN
#include "alchemy_resource_receipt_d60_test.cpp"

namespace kcd2vr::action_time_owner59c11 {namespace {
static unsigned checks81=0;
static float originalDt81=0,unscaledHeat81=0;
static void ck81(bool v,const char* why){++checks81;if(!v){std::fprintf(stderr,"D81 clock check%u: %s\n",checks81,why);std::abort();}}
static void nativeClock81(void* ctrl,float dt){originalDt81=dt;unscaledHeat81+=dt;NativeUpdate46(ctrl,dt);}
static void resetClocks81(){
 g_alchemy_presentation_d81={};g_alchemy_presentation_previous_d81={};g_alchemy_presentation_foreign_d81={};g_alchemy_presentation_pending_d81=false;
 g_smith_fall_d81={};g_smith_fall_abandoned_d81={};g_smith_fall_pending_d81=false;
 g_alchemy_transfer_d65={};g_alchemy_transfer_pending_d65=false;g_alchemy_transfer_stop_d65=false;g_alchemy_transfer_lock_d65.clear();
 g_alchemy_stage19c4={};g_alchemy_pending19c4=false;g_semi_smith46={};g_semi_smith_pending46=false;
 smith46::clockInterested=false;smith46::clockReady=false;smith46::clockControl.publish({});
 originalDt81=unscaledHeat81=0;refusedWrite19=nullptr;
}
static void alchemy81(ap81::Kind kind=ap81::Kind::Transfer){
 resetClocks81();Prepare46(kind==ap81::Kind::Book?semi46::Book:semi46::Resource);
 g_mode=Off;g_owner={};g_alchemy_transfer_rate_d65=8;g_update_original=nativeClock81;
 kcd2vr::minigame_hands45::setMode(kcd2vr::minigame_hands45::Alchemy,3);
 NativeQueue46(controller.data(),action.data(),0);InstallQueued46();
 put(testSkel,0,g_base.load()+kSkelVft);put(image.data(),kSkelVft+0xB8,g_base.load()+kLayerScaleH23);
 std::memcpy(image.data()+kLayerScaleH23,kLayerScalePinH23,sizeof(kLayerScalePinH23));
 if(kind==ap81::Kind::Transfer){
  const auto b=g_base.load();for(const auto& stage:kAlchemyTransferStagesD65){
   put(image.data(),stage.vft+0xF8,b+stage.start);std::memcpy(image.data()+stage.slotPin,stage.bytes,4);}
  put(parent46.data(),0,b+kAlchemyTransferStagesD65[6].vft); // real MovePot stage
  put(parent46.data(),0x70,reinterpret_cast<std::uintptr_t>(action.data()));
 }
 g_semi_resource46={};g_semi_alchemy46={};g_semi_pending46=false;semi46::hooksReady=true;
}
static void alchemyCases81(){
 for(auto kind:{ap81::Kind::Transfer,ap81::Kind::Book,ap81::Kind::Resource}){
  alchemy81(kind);UpdateDetour(controller.data(),.01f);
  ck81(g_alchemy_presentation_pending_d81&&std::fabs(Rd<float>(action.data(),kActActiveTime,-1)-.08f)<1e-6f,"native clock advances before original Update for transfer/book/resource intro");
  ck81(originalDt81==.01f&&unscaledHeat81==.01f,"original dt and separate heat remain unscaled");
  ck81(RestoreAlchemyPresentationBoundaryD81(controller.data())&&Rd<float>(action.data(),kActSpeedBias,-1)==1.f,"exact child rate restores");
 }
 alchemy81();UpdateDetour(controller.data(),.033f);
 ck81(std::fabs(Rd<float>(action.data(),kActActiveTime,-1)-.08f)<1e-6f,"long frame bounded80ms");
 RestoreAlchemyPresentationBoundaryD81(controller.data());
 for(int invalid=0;invalid<7;++invalid){alchemy81();
  switch(invalid){case 0:g_alchemy_transfer_rate_d65=1;break;case 1:testWorkstationFrame46.allowed=false;break;
   case 2:testWorkstationFrame46.tick=900;break;case 3:kcd2vr::minigame_hands45::setMode(kcd2vr::minigame_hands45::Alchemy,0);break;
   case 4:put(action.data(),kActSpeedBias,.5f);break;case 5:put(parent46.data(),0x70,std::uintptr_t(0));break;
   case 6:testRetailReadyD69=false;break;}
  UpdateDetour(controller.data(),.01f);ck81(!g_alchemy_presentation_pending_d81,"off/stale/foreign/unverified has no presentation lease");testRetailReadyD69=true;
 }
 alchemy81(ap81::Kind::Resource);semi46::WorkWitness work{};
 {OwnerTransaction19c4 tx;AlchemyUpdateScopeR27 scope81(controller.data());ck81(SemiAlchemyParent46(controller.data(),work),"resource owner joined");}
 g_semi_resource46.owner=work;g_semi_resource46.phase=1;UpdateDetour(controller.data(),.01f);
 ck81(!g_alchemy_presentation_pending_d81&&std::fabs(Rd<float>(action.data(),kActActiveTime,-1)-.01f)<1e-6f,"physical resource work is never sped by presentation slider");
 alchemy81();UpdateDetour(controller.data(),.01f);put(action.data(),kActSpeedBias,.6f);
 ck81(RestoreAlchemyPresentationBoundaryD81(controller.data())&&Rd<float>(action.data(),kActSpeedBias,-1)==.6f,"foreign speed writer wins");
 put(action.data(),kActSpeedBias,1.f);UpdateDetour(controller.data(),.01f);ck81(!g_alchemy_presentation_pending_d81,"foreign-owned same token cannot be reacquired");
 alchemy81();UpdateDetour(controller.data(),.01f);put(controller.data(),kCtrlScopeCount,25u);
 ck81(!RestoreAlchemyPresentationBoundaryD81(controller.data())&&g_alchemy_presentation_pending_d81,"unknown membership retains cleanup");
 put(controller.data(),kCtrlScopeCount,1u);refusedWrite19=action.data()+kActSpeedBias;
 ck81(!RestoreAlchemyPresentationBoundaryD81(controller.data())&&g_alchemy_presentation_pending_d81,"failed restore retains cleanup");
 refusedWrite19=nullptr;ck81(RestoreAlchemyPresentationBoundaryD81(controller.data()),"same lease retries restoration only");
 alchemy81();UpdateDetour(controller.data(),.01f);put(scope.data(),kScopeAction,static_cast<void*>(nullptr));
 forbidRetiredRead19c4=true;ck81(RestoreAlchemyPresentationBoundaryD81(controller.data())&&retiredReads19c4==0,"retired child is not dereferenced");forbidRetiredRead19c4=false;
 alchemy81();UpdateDetour(controller.data(),.01f);put(action.data(),kActToken,56u);put(action.data(),kActSpeedBias,1.f);
 ck81(RestoreAlchemyPresentationBoundaryD81(controller.data())&&Rd<float>(action.data(),kActSpeedBias,-1)==1.f,"replacement token is not written");
 alchemy81();UpdateDetour(controller.data(),.01f);pickupThread19c4=2;
 ck81(RestoreAlchemyPresentationBoundaryD81(controller.data())&&!g_alchemy_presentation_pending_d81,"exact migrated native callback may restore");pickupThread19c4=1;
}
#include "alchemy_resource_root3_runtime_d69.inl"
static void resourceInput81(){
 namespace ab=kcd2vr::subsystems::alchemy::bridge67;ab::Control c{};
 c.instance=reinterpret_cast<std::uintptr_t>(alchemy46.data());c.lifetime=kcd2vr::activity_input163::witness175.lifetime175.load();
 c.parent=reinterpret_cast<std::uintptr_t>(parent46.data());c.tick=1000;c.enabled=true;c.poseEpoch=8;c.sourceEpoch=9;
 c.modeEpoch=kcd2vr::minigame_hands45::modeEpoch67.load();c.side=1;c.type=13;c.entity=77;c.held[1]=true;c.admissionTickR22=990;
 put(alchemy46.data(),0x150+4*13,77u);ab::control.publish(c);
}
static void resourceHandoff81(){
 resetClocks81();resourceRoot3SetupD69();g_alchemy_transfer_rate_d65=8;
 kcd2vr::minigame_hands45::setMode(kcd2vr::minigame_hands45::Alchemy,3);
 resourceInput81();
 {OwnerTransaction19c4 tx;AlchemyUpdateScopeR27 a(controller.data());TickAlchemyPresentationD81(controller.data(),.01f,1000);}
 ck81(g_alchemy_presentation_pending_d81,"resource intro has presentation lease");
 resourceNativeAdvanceD69(.01f);resourceAttachD69();
 ck81(nativeAttach46==1&&nativeUse46==0&&g_semi_alchemy46.written,"native Attach hands intro to physical work once");
 ck81(!g_alchemy_presentation_pending_d81&&g_semi_alchemy46.saved==1.f&&Rd<float>(action.data(),kActSpeedBias,-1)==0.f,"physical work saves native1x and clears intro lease");
 ck81(resourceRestoreD69()&&Rd<float>(action.data(),kActSpeedBias,-1)==1.f,"resource work restores native1x, never accelerated8x");
 // Same root3 scope evidence remains valid if the worker's Attach is consumed
 // after the ordinary Update boundary restored the child but before native
 // refreshed its cached layer/scope rates.
 resetClocks81();resourceRoot3SetupD69();g_alchemy_transfer_rate_d65=8;
 kcd2vr::minigame_hands45::setMode(kcd2vr::minigame_hands45::Alchemy,3);
 resourceInput81();
 {OwnerTransaction19c4 tx;AlchemyUpdateScopeR27 a(controller.data());TickAlchemyPresentationD81(controller.data(),.01f,1000);}
 resourceNativeAdvanceD69(.01f);ck81(RestoreAlchemyPresentationBoundaryD81(controller.data()),"pre-callback intro restored");
 resourceAttachD69();ck81(g_semi_alchemy46.written&&g_semi_alchemy46.saved==1.f,"delayed Attach accepts only our exact cached prior rate");
 resourceRestoreD69();
}
static std::array<unsigned char,0x700> smithInstance81{},smithParent81{},smithModel81{};
static void smith81(int code=11){
 resetClocks81();reset();ResetOwner();g_mode=Off;g_semi_stop46=false;g_semi_pending46=false;g_semi_alchemy46={};semi46::hooksReady=false;
 g_semi_smith_stop46=false;g_update_original=nativeClock81;put(controller.data(),kCtrlTimeScale,1.f);
 smithInstance81.fill(0);smithParent81.fill(0);smithModel81.fill(0);
 const auto b=g_base.load(),a=reinterpret_cast<std::uintptr_t>(smithInstance81.data()),p=reinterpret_cast<std::uintptr_t>(smithParent81.data()),m=reinterpret_cast<std::uintptr_t>(smithModel81.data());
 for(const auto& pin:smith46::pins::all)std::memcpy(image.data()+pin.rva,pin.bytes,pin.size);
 for(const auto& slot:smith46::pins::slots)put(image.data(),slot.rva,b+slot.target);
 put(smithInstance81.data(),0,b+smithNative46::kInstanceVft);put(smithInstance81.data(),0x2C,std::uint8_t(1));put(smithInstance81.data(),0x18,reinterpret_cast<std::uintptr_t>(player.data()));
 put(smithInstance81.data(),0x70,b+smithNative46::kListenerVft);put(smithInstance81.data(),0x98,m);put(smithInstance81.data(),0xC8,p);
 put(smithModel81.data(),0,b+smithNative46::kModelVft);put(smithModel81.data(),0x88,std::uint8_t(code==10?14:16));
 put(smithParent81.data(),0,b+smithNative46::kParentVft);put(smithParent81.data(),0x10,b+smithNative46::kParentActionVft);put(smithParent81.data(),0x28,std::uint8_t(1));
 put(smithParent81.data(),0x50,std::uint8_t(code));put(smithParent81.data(),0x78,reinterpret_cast<std::uintptr_t>(player.data()));put(smithParent81.data(),0x80,a+0x70);put(smithParent81.data(),0x88,reinterpret_cast<std::uintptr_t>(action.data()));
 auto& w=kcd2vr::activity_input163::witness175;w.Clear();w.ready=true;w.Begin(kcd2vr::activity_input163::Kind175::Smithing,a);w.transitionDepth175=0;
 kcd2vr::activity_input163::trackedHands59c7f[1]=true;kcd2vr::minigame_hands45::setMode(kcd2vr::minigame_hands45::Smithing,3);
 kcd2vr::combat_rules178r10::workstationEnabled=true;kcd2vr::workstation178r20c1::smithAutoHold36=true;
 kcd2vr::minigame_hands45::publish(kcd2vr::minigame_hands45::Smithing,1000);
 const kcd2vr::workstation_sample46::Frame emptyFrame81{};testWorkstationFrame46=emptyFrame81;
 testWorkstationFrame46.allowed=testWorkstationFrame46.headValid=true;
 testWorkstationFrame46.tick=1000;testWorkstationFrame46.timeNs=1000000000;testWorkstationFrame46.sequence=8;
 testWorkstationFrame46.poseEpoch=8;testWorkstationFrame46.sourceEpoch=9;testWorkstationFrame46.hand[0].valid=true;
 smith46::ClockControl c{};auto& r=c.fallD81;r.instance=a;r.lifetime=w.lifetime175.load();r.request=1;r.poseEpoch=8;r.sourceEpoch=9;
 r.modeEpoch=kcd2vr::minigame_hands45::modeEpoch67.load();r.sequence=8;r.tick=1000;r.released=990;r.phase=16;
 smith46::clockReady=smith46::clockInterested=true;smith46::clockControl.publish(c);
}
static void smithCases81(){
 smith81();UpdateDetour(controller.data(),.01f);
 ck81(g_smith_fall_pending_d81&&std::fabs(Rd<float>(action.data(),kActActiveTime,-1)-.04f)<1e-6f,"exact code11 uses4x before native Update");
 ck81(originalDt81==.01f&&unscaledHeat81==.01f,"smith heat/global dt unchanged");
 ck81(RestoreSmithFallBoundaryD81(controller.data())&&Rd<float>(action.data(),kActSpeedBias,-1)==1.f,"fall speed restores before next callback");
 smith81(10);UpdateDetour(controller.data(),.01f);
 ck81(!g_smith_fall_pending_d81&&std::fabs(Rd<float>(action.data(),kActActiveTime,-1)-.01f)<1e-6f,"code10 charge remains native1x");
 for(int invalid=0;invalid<8;++invalid){smith81();smith46::ClockControl c{};smith46::clockControl.read(c);
  switch(invalid){case 0:c.fallD81={};break;case 1:c.fallD81.tick=900;break;case 2:c.fallD81.released=200;break;
   case 3:++c.fallD81.lifetime;break;case 4:++c.fallD81.poseEpoch;break;case 5:++c.fallD81.modeEpoch;break;
   case 6:testWorkstationFrame46.hand[0].valid=false;break;case 7:c.enabled=true;c.request=1;break;}
  smith46::clockControl.publish(c);UpdateDetour(controller.data(),.01f);ck81(!g_smith_fall_pending_d81,"unearned/stale/foreign/legacy work cannot speed native fall");
 }
 smith81();UpdateDetour(controller.data(),.01f);put(action.data(),kActSpeedBias,.6f);
 ck81(RestoreSmithFallBoundaryD81(controller.data())&&Rd<float>(action.data(),kActSpeedBias,-1)==.6f,"smith foreign speed writer wins");
 put(action.data(),kActSpeedBias,1.f);UpdateDetour(controller.data(),.01f);ck81(!g_smith_fall_pending_d81,"foreign same stroke cannot be reacquired");
 smith81();UpdateDetour(controller.data(),.01f);put(controller.data(),kCtrlScopeCount,25u);
 ck81(!RestoreSmithFallBoundaryD81(controller.data())&&g_smith_fall_pending_d81,"smith unknown membership retains cleanup");
 put(controller.data(),kCtrlScopeCount,1u);ck81(RestoreSmithFallBoundaryD81(controller.data()),"smith current membership restores");
 smith81();UpdateDetour(controller.data(),.01f);put(scope.data(),kScopeAction,static_cast<void*>(nullptr));forbidRetiredRead19c4=true;
 ck81(RestoreSmithFallBoundaryD81(controller.data())&&retiredReads19c4==0,"smith retired child never dereferenced");forbidRetiredRead19c4=false;
}
static int run81(){alchemyCases81();resourceHandoff81();smithCases81();std::printf("D81 complete production action-owner crafting clock: %u checks PASS; engine/OS leaves mocked, SEH and game NOT_RUN\n",checks81);return 0;}
}}
#ifndef KCD2VR_CRAFTING_CLOCK_D81_NO_MAIN
int main(){return kcd2vr::action_time_owner59c11::run81();}
#endif
