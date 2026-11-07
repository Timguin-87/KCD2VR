// Actual production start, C5235C wrapper and ProcessHit detour together.
// Only original retail bodies/OS are fixture leaves; no retail code executes.
#define KCD2VR_TEST_HOOK_SPY115
#define KCD2VR_RENDER_BOUNDS_TEST63 // supplies an explicit failed-disable native-hook leaf
#include "../src/frik_pose_probe.cpp"
#include "../src/combat_diag_probe.cpp"
#include <cassert>
#include <cstdio>
namespace kcd2vr::diagnostic_trace {void Write(const char*) noexcept {}}
namespace kcd2vr::animation_filter {void MarkContactForPlayback(std::uint64_t) noexcept {}}
namespace kcd2vr::retail_build112 {extern bool hostRetailVerified112;}
using namespace kcd2vr::frik_pose_probe;
namespace n=kcd2vr::combat_diag_probe::nhp134;
namespace h=kcd2vr::fist_action_h12;
template<class T>static void putH(unsigned char* p,T v){std::memcpy(p,&v,sizeof(v));}
static unsigned startsH=0,receiversH=0,processesH=0;
static bool startResultH=true,nestedH=false,throwReceiverH=false;
static void *rpgH=nullptr,*requestH=nullptr,*npcReceiverH=nullptr,*npcDetailsH=nullptr;
static std::array<unsigned char,0xA0> hitH{},resultH{};
static bool __fastcall processH(void*,void*,const void*,void*) {++processesH;return true;}
static bool __fastcall startH(void*,void*) {++startsH;return startResultH;}
static void __fastcall receiverH(const void* receiver,void*) {
    ++receiversH;
    if(throwReceiverH)throw 12;
    if(nestedH&&receiver!=npcReceiverH){
        nestedH=false;
        n::FistReceiverDetourH12(npcReceiverH,npcDetailsH);
        // The outer exact receiver is restored after a nested NPC callback.
        assert(h::Scope::live&&h::Scope::live->qualified);
    }
    (void)n::ProcessHitDetour(rpgH,hitH.data(),requestH,resultH.data());
}
#ifdef KCD2VR_FIST_PREDICTION_TEST_H31
#include "fist_prediction_h31_fixture.h"
#endif
int main(){
    std::vector<unsigned char> image(0x5400000);hostModuleHandleW=image.data();g_wh_pinned104=0;
    const auto base=WhBase();hostTickCount64=10000;kcd2vr::retail_build112::hostRetailVerified112=true;
    std::array<unsigned char,0xE00> player{},state{};std::array<unsigned char,0x500> actor{};
    std::array<unsigned char,0x100> itemManager{},entity{},helper{},rpg{},combatManager{};
    std::array<unsigned char,0x300> hands{},row{};std::array<unsigned char,0x100> action{},type{};
    std::array<unsigned char,0x280> collisions{};std::array<unsigned char,0x40> receiver{},foreignReceiver{};
    std::array<unsigned char,0x40> request{};std::array<float,8> contact{};
    std::array<std::uintptr_t,2> actionSlots{};std::array<std::uintptr_t,4> callbacks{};
    putH(player.data(),base+0x3AA4B98);putH(player.data()+0x38,entity.data());putH(entity.data(),base+0x4072C78);
    putH(player.data()+0x278,actor.data());putH(actor.data(),base+0x3A80610);putH(actor.data()+0x1F8,player.data());
    putH(actor.data()+0x210,state.data());putH(state.data()+0xCE0,actor.data());g_last_player_entity=entity.data();
    putH(player.data()+0x9D0,itemManager.data());putH(itemManager.data(),base+0x3EC9958);putH(itemManager.data()+0x38,player.data());
    putH(player.data()+0x990,hands.data());g_henry_item_attach_manager=itemManager.data();
    putH(action.data(),base+0x3A61C58);putH(action.data()+0x70,actor.data());putH(action.data()+0x58,row.data());
    putH(action.data()+0x88,base+0x3A61440);putH(action.data()+0xA0,helper.data());
    putH(helper.data(),base+0x3A61040);putH(helper.data()+8,actor.data());putH(helper.data()+0x10,base+0x3A61028);
    putH(helper.data()+0x18,receiver.data());putH(helper.data()+0x20,action.data()+0x88);
    putH(row.data()+0x148,type.data());putH(type.data()+8,5);putH(row.data()+0x224,1); // native LEFT, VR RIGHT
    putH(actor.data()+0x388,rpg.data());putH(rpg.data()+8,actor.data());
    putH(actor.data()+0x200,combatManager.data());putH(combatManager.data()+0x38,actionSlots.data());actionSlots[0]=reinterpret_cast<std::uintptr_t>(action.data());
    putH(action.data()+0x40,combatManager.data());putH(action.data()+0x28,std::uint8_t(1));
    putH(receiver.data(),base+0x3A60FF0);putH(receiver.data()+8,actor.data());putH(receiver.data()+0x10,helper.data()+0x10);putH(receiver.data()+0x18,std::uint8_t(1));
    putH(actor.data()+0x218,collisions.data());putH(collisions.data()+8,actor.data());
    callbacks[1]=std::uintptr_t(1)<<32|1;callbacks[2]=reinterpret_cast<std::uintptr_t>(receiver.data());callbacks[3]=base+0xC5235C;
    putH(collisions.data()+0x28,callbacks.data()+2);
    putH(state.data()+0x218,5);putH(state.data()+0x1B0,77);putH(state.data()+0x1B8,1);putH(state.data()+0x38,8);
    putH(request.data()+0x18,contact.data());rpgH=rpg.data();requestH=request.data();
    n::g_base=base;n::g_f4_contract176=true;n::g_process_original=&processH;
    n::g_fist_receiver_base_h12=base;n::g_fist_receiver_original_h12=&receiverH;
    g_native_contract104=base;g_attack_timing_contract22=base;g_attack_start22=&startH;
    // This fixture isolates the legacy receiver seam. The separate H31
    // predictor fixture below selects the actual public F4 HYBRID contract.
    g_census_mode=5;g_weapon_control_mode=0;
    g_gameplay_allowed=g_head_valid=g_left_valid=g_right_valid=true;
    g_pose_epoch6689=2;g_fist_epoch101=7;kcd2vr::animation_filter::nativeBodyOwned112=false;
    assert(FistSelected101());
    TrackingSnapshot t{};t.tick=t.head_good_tick=t.left_good_tick=t.right_good_tick=hostTickCount64;
    t.allowed=t.head_valid=t.right_valid=t.left_valid=true;t.right_grip=t.right_trigger=t.left_grip=t.left_trigger=1;
    kcd2vr::tracking59c6::enabled=false;g_tracking_frames6695.publish(t);
    FistContactFrameH11 palms{};palms.player=player.data();palms.request=7;palms.epoch=2;palms.tick=hostTickCount64;
    palms.sample=9000;palms.character=0x1234;palms.valid[0]=palms.valid[1]=true;palms.completedH31=true;
    palms.palm[0]=palms.previous[0]={2,3,4};palms.palm[1]=palms.previous[1]={-2,3,4};g_fist_contact_h11.publish(palms);
    CommittedFrame6695 committed{};committed.stamp.epoch=2;committed.stamp.sample=9000;committed.stamp.actor=0x1234;
    committed.valid[0]=committed.valid[1]=committed.palmValid59c9[0]=committed.palmValid59c9[1]=true;
    committed.world[3]=committed.world[7]=1;committed.palm59c9[0]={2,3,4};committed.palm59c9[1]={-2,3,4};
    g_committed_frame6695.publish(committed);
    FistOwner101 ownerH31{};assert(FistReadOwner101(ownerH31)&&ownerH31.empty);g_fist_owner101.publish(ownerH31);
    const bool blockedH31[2]{};t.sequence=9000;t.hq={0,0,0,1};
    FistPublish101(t,committed,blockedH31);assert(g_fist_contact_h11.read(palms)&&palms.completedH31&&palms.valid[0]&&palms.valid[1]);
    hostReturnAddressValue=reinterpret_cast<void*>(base+0xC52534);
    const auto start=[&](){NativeTimingFist178r22c2(player.data(),123,hostTickCount64,0,2.5);return NativeAttackStart22(action.data(),nullptr);};
    const auto hit=[&](float x){contact[0]=x;contact[1]=3;contact[2]=4;const auto before=processesH;
        n::FistReceiverDetourH12(receiver.data(),contact.data());assert(!h::Scope::live);return processesH-before;};
    assert(hit(2)==0&&h::receiptH31.failure==h::FailureH31::NoCurrentAction); // no admitted fist
    assert(start());assert(hit(2)==1);assert(hit(-2)==0); // native rowhand1 must use the causally admitted VRside0
    assert(startsH==1&&receiversH==3&&processesH==1);
    // I(state,0x1B0) reads the property's VALUE at +8. Raw descriptor +1B0
    // deliberately differs from raw hand +1B8; a field swap must not pass.
    assert(n::I(state.data(),0x1B0)==1&&h::receiptH31.failure==h::FailureH31::OutsidePalm);
    // A newer whole committed packet can be visible before its matching palm
    // publication. Keep the original exact <=60ms completed palm segment.
    ++committed.stamp.sample;g_committed_frame6695.publish(committed);assert(hit(2)==1);
    assert(h::receiptH31.palmSample==9000&&h::receiptH31.committedSample==9001);
    palms.completedH31=false;g_fist_contact_h11.publish(palms);assert(hit(2)==0&&h::receiptH31.failure==h::FailureH31::UncommittedPalm);
    palms.completedH31=true;g_fist_contact_h11.publish(palms);
    --committed.stamp.sample;g_committed_frame6695.publish(committed);
    t.right_trigger=t.right_grip=.61f;g_tracking_frames6695.publish(t);FistPublish101(t,committed,blockedH31);
    assert(g_fist_contact_h11.read(palms)&&palms.valid[0]);assert(hit(2)==1);
    t.right_trigger=.60f;g_tracking_frames6695.publish(t);assert(hit(2)==0&&h::receiptH31.failure==h::FailureH31::Squeeze);
    t.right_trigger=.61f;t.right_grip=.60f;g_tracking_frames6695.publish(t);assert(hit(2)==0&&h::receiptH31.failure==h::FailureH31::Squeeze);
    FistPublish101(t,committed,blockedH31);assert(g_fist_contact_h11.read(palms)&&!palms.valid[0]);
    t.right_grip=t.right_trigger=1;g_tracking_frames6695.publish(t);
    FistPublish101(t,committed,blockedH31);assert(g_fist_contact_h11.read(palms)&&palms.valid[0]);
    // Proposing the other fist does not overwrite an already-admitted action.
    NativeTimingFist178r22c2(player.data(),124,hostTickCount64,1,2.5);assert(hit(2)==1&&hit(-2)==0);
    t.right_held=true;g_tracking_frames6695.publish(t);assert(hit(2)==0);t.right_held=false;g_tracking_frames6695.publish(t);
    t.right_trigger=0;g_tracking_frames6695.publish(t);assert(hit(2)==0);t.right_trigger=1;g_tracking_frames6695.publish(t);
    actionSlots[0]=reinterpret_cast<std::uintptr_t>(foreignReceiver.data());assert(hit(2)==0&&h::receiptH31.failure==h::FailureH31::ManagerMembership);actionSlots[0]=reinterpret_cast<std::uintptr_t>(action.data());
    putH(helper.data()+0x18,foreignReceiver.data());assert(hit(2)==0&&h::receiptH31.failure==h::FailureH31::HelperOwner);putH(helper.data()+0x18,receiver.data());
    ++callbacks[3];assert(hit(2)==0&&h::receiptH31.failure==h::FailureH31::ReceiverRegistration);--callbacks[3];
    std::array<float,8> unrelatedDetails{};putH(request.data()+0x18,unrelatedDetails.data());assert(hit(2)==0&&h::receiptH31.failure==h::FailureH31::DetailsMismatch);putH(request.data()+0x18,contact.data());
    ++hostTickCount64;NativeTimingCancel178r22c2();assert(hit(2)==0);assert(start());
    // Nested unrelated/NPC callback masks authority, then restores the outer one.
    npcReceiverH=foreignReceiver.data();npcDetailsH=contact.data();nestedH=true;assert(hit(2)==1);
    startResultH=false;assert(!start());assert(hit(2)==0);startResultH=true;assert(start());
    // Same allocation restarted without a fresh VR request cannot inherit it.
    assert(NativeAttackStart22(action.data(),nullptr));assert(hit(2)==0);assert(start());
    h::recycled(reinterpret_cast<std::uintptr_t>(action.data()));assert(hit(2)==0);assert(start());
    hostTickCount64+=61;IntegratedFistSelection178r22c2();assert(hit(2)==0&&h::receiptH31.failure==h::FailureH31::PalmAge);hostTickCount64-=61;IntegratedFistSelection178r22c2();
    ++g_pose_epoch6689;assert(hit(2)==0);--g_pose_epoch6689;
    ++g_fist_epoch101;assert(hit(2)==0);--g_fist_epoch101;
    for(int punch:{6,7}){putH(type.data()+8,punch);putH(state.data()+0x218,punch);assert(start());assert(hit(2)==1);}
    putH(type.data()+8,5);putH(state.data()+0x218,5);assert(start());
#ifdef KCD2VR_FIST_PREDICTION_TEST_H31
#include "fist_prediction_h31_cases.inl"
#endif
    // Nested native exceptions cannot leave a dangling borrowed TLS scope.
    try{h::run(reinterpret_cast<std::uintptr_t>(receiver.data()),reinterpret_cast<std::uintptr_t>(contact.data()),base,
        hostTickCount64,true,ClockRead50,[](){throw 12;});assert(false);}catch(int code){assert(code==12&&!h::Scope::live);}
    // The native SEH filter uses this same restoration leaf because /EHsc may
    // bypass the normal destructor. Unknown partial work retires the binding.
    h::run(reinterpret_cast<std::uintptr_t>(receiver.data()),reinterpret_cast<std::uintptr_t>(contact.data()),base,
        hostTickCount64,true,ClockRead50,[](){assert(h::Scope::live&&h::Scope::live->qualified);
            h::exceptionRestore(h::Scope::live->prior);assert(!h::Scope::live);});
    assert(hit(2)==0);assert(start());
    // Native reference, kicks and paired/native-body ownership are not changed.
    kcd2vr::diagnostics::lab::Set(kcd2vr::diagnostics::lab::Key::F3,1);assert(hit(2)==1);
    kcd2vr::diagnostics::lab::Set(kcd2vr::diagnostics::lab::Key::F3,0);
    putH(state.data()+0x218,4);assert(hit(2)==1);putH(state.data()+0x218,55);assert(hit(2)==1);putH(state.data()+0x218,5);
    putH(hands.data()+0x260,std::uint64_t(77));assert(hit(2)==0);putH(hands.data()+0x260,std::uint64_t(0));
    // Existing live HYBRID calls accept only our owned patch and trampoline;
    // an unrelated patch must not be mistaken for the newly installed owner.
    n::g_fist_receiver_original_h12=nullptr;n::g_fist_receiver_base_h12=0;
    std::memcpy(image.data()+n::kFistReceiverH12,n::kFistReceiverEntryH12,sizeof(n::kFistReceiverEntryH12));
    assert(n::FistReceiverCallableH12()==reinterpret_cast<n::FistReceiverFnH12>(base+n::kFistReceiverH12));
    ++image[n::kFistReceiverH12+1];assert(!n::FistReceiverCallableH12());--image[n::kFistReceiverH12+1];
    DetourModKit::hook::hostOriginal115=+[](const DetourModKit::hook::InlineRequest&)->std::uintptr_t{return reinterpret_cast<std::uintptr_t>(&receiverH);};
    assert(n::ArmFistReceiverH12(base));
    assert(n::FistReceiverOriginalH12()==&receiverH&&n::FistReceiverCallableH12()==&n::FistReceiverDetourH12);
    const auto beforeLive=receiversH;assert(n::geom27::CallLiveHandler35(receiver.data(),contact.data()));assert(receiversH==beforeLive+1);
    const auto beforeThrow=receiversH,beforeThrowProcess=processesH;throwReceiverH=true;
    assert(!n::geom27::CallLiveHandler35(receiver.data(),contact.data()));
    assert(receiversH==beforeThrow+1&&processesH==beforeThrowProcess&&!h::Scope::live);throwReceiverH=false;
    ++image[n::kFistReceiverH12+1];assert(!n::FistReceiverOriginalH12()&&!n::FistReceiverCallableH12());--image[n::kFistReceiverH12+1];
    DetourModKit::hook::hostFailDisableBounds63=true;
    assert(!n::ReleaseFistReceiverH12()&&n::g_fist_receiver_hook_h12&&n::g_fist_receiver_original_h12==&receiverH);
    assert(n::g_fist_receiver_uncertain_h12&&!n::FistReceiverCallableH12());
    const auto retainedForwards=receiversH;n::FistReceiverDetourH12(receiver.data(),contact.data());assert(receiversH==retainedForwards+1);
    DetourModKit::hook::hostFailDisableBounds63=false;assert(n::ReleaseFistReceiverH12());
    assert(n::FistReceiverCallableH12()==reinterpret_cast<n::FistReceiverFnH12>(base+n::kFistReceiverH12));
    assert(!h::Scope::live);std::puts("H31 receiver seam passed; types5/6/7, exact >60%, complete palm transaction, original once; public F4 predictor covered separately; native damage body is fixture leaf");
}
