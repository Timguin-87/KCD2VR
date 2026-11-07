// Runs the actual source-extracted native hook entry/fast-return path. Native
// original and ArmDoor are spies; real Arm admission is tested by its IO fixture.
#include <array>
#include <atomic>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "core/snapshot_mailbox.h"
#include "core/door_candidates38.h"
#include "core/door_use46.h"
#include "interaction_alignment.h"
namespace interaction_alignment = kcd2vr::interaction_alignment;
#include "generated_door_request38_types.inc"
namespace diagnostics::capture168 {
enum class Domain { Interaction };
static bool active(Domain, std::uint64_t) noexcept { return false; }
}
namespace kcd2vr::workstation178r20c1 { static std::atomic<std::uint64_t> experimentEpoch36{0}; static bool ownsWrist(std::uint64_t) { return false; } }
namespace diagnostic_trace { static void Write(const char*) { assert(false); } }
static std::uint64_t tick38=1000;
static std::uint64_t GetTickCount64() { return tick38; }
static bool ReloadDiagnostics175() { return false; }
static std::array<unsigned char,0x80> actor38{};
static std::array<unsigned char,8> player38{},otherPlayer38{};
static std::atomic<void*> g_last_player_entity{player38.data()};
static std::atomic<std::uint64_t> g_pose_epoch6689{7};
static kcd2vr::core::SnapshotMailbox<InteractionHandContext158> g_interaction_hand_context158;
static std::atomic<std::uint32_t> g_native_use_target59c10[2];
static kcd2vr::door_candidates38::Records<interaction_alignment::HandCandidate> records38;
static unsigned diagnosticCandidateReads63=0;
namespace kcd2vr::interaction_alignment {
// This fixture has no general hover candidate service. Quiet native entry
// capture must not query it merely because the finite diagnostic code exists.
bool LastHandCandidate(HandCandidate& out) noexcept {++diagnosticCandidateReads63;out={};return false;}
bool DoorSourceCurrent38(std::uint64_t generation) noexcept {return generation==1;}
// Explicit native acquisition-generation leaf, distinct from the request capture under test.
bool DoorAcquisitionCurrent65(std::uint64_t generation) noexcept {return generation==1;}
bool ExactDoorCandidate38(std::uint32_t target,int side,std::uint64_t now,HandCandidate& out) noexcept {
    return records38.find(target,side,now,out);
}
}
#include "generated_door_capture46.inc"
template<class T> static bool Read(const void* p,T& out) { if(!p)return false;std::memcpy(&out,p,sizeof(out));return true; }
static unsigned nativeCalls38=0,armCalls38=0,fingerCalls38=0;
static int grantedSide38=-1;
static DoorRequestWitness38 granted38{};
static std::uintptr_t result38=0x12345;
static bool changeOwner38=false,replaceIntent46=false;
static unsigned delay38=0;
static void NotifyNativeHandUse(int side,unsigned ms) { assert(ms==1800);++fingerCalls38;grantedSide38=side; }
static void ArmDoor19c4(std::uint32_t target,std::uintptr_t action,const char* name,int side,std::uint64_t now,const DoorRequestWitness38* witness=nullptr) {
    assert(target==42&&action==result38&&std::strcmp(name,"door_l_f_o")==0&&now==tick38);
    ++armCalls38;grantedSide38=side;granted38=witness?*witness:DoorRequestWitness38{};
}
static std::uintptr_t Native38(void* actor,const char* name,std::uint32_t target,bool force,float speed) {
    assert(actor==actor38.data()&&std::strcmp(name,"door_l_f_o")==0&&target==42&&force&&speed==.75f);
    ++nativeCalls38;
    InteractionHandContext158 other{};other.side=1;other.valid=true;other.tick=tick38;
    g_interaction_hand_context158.publish(other);records38.clear();
    if(replaceIntent46){kcd2vr::door_use46::Intent other{};other.token=999;other.target=999;other.side=1;(void)kcd2vr::door_use46::intent.publish(other);}
    if(changeOwner38){++g_pose_epoch6689;g_last_player_entity=otherPlayer38.data();}
    tick38+=delay38;return result38;
}
using NativeFn38=std::uintptr_t(*)(void*,const char*,std::uint32_t,bool,float);
static NativeFn38 g_interaction_original6689=Native38;
// Explicit native-entry leaf. C66 motion/root scope is covered by native_door_motion66;
// this fixture executes the actual immutable request snapshot/returned action join.
template<class Fn> static std::uint64_t DoorNativeCall66(Fn fn,void* actor,const char* name,std::uint32_t target,bool force,float speed,const DoorRequestWitness38&,int,std::uint64_t) noexcept {
 return fn?fn(actor,name,target,force,speed):0;
}
static std::uint64_t Hook38(void* actor,const char* name,std::uint32_t targetId,bool force,float speed) noexcept {
#include "generated_door_request38.inc"
    std::abort(); // The diagnostic continuation is not part of this fixture.
}
static void setup38() {
    tick38=1000;g_pose_epoch6689=7;g_last_player_entity=player38.data();
    void* entity=player38.data();std::memcpy(actor38.data()+0x38,&entity,sizeof(entity));
    InteractionHandContext158 hand{};hand.side=0;hand.valid=true;hand.tick=tick38;
    g_interaction_hand_context158.publish(hand);
    interaction_alignment::HandCandidate c{};c.entity=42;c.owner=0;c.object=true;c.tick=tick38;c.distance=.2f;c.sourceGeneration=1;c.sourceEpoch=7;c.sourceTick=tick38;c.sourcePlayer=reinterpret_cast<std::uintptr_t>(player38.data());std::strcpy(c.cls,"AnimDoor");
    records38.clear();records38.record(c);nativeCalls38=armCalls38=fingerCalls38=0;grantedSide38=-1;granted38={};
    result38=0x12345;changeOwner38=replaceIntent46=false;delay38=0;g_interaction_original6689=Native38;
    (void)kcd2vr::door_use46::intent.publish({});
    (void)kcd2vr::door_use46::currentInput62.publish({});
    (void)kcd2vr::door_use46::physicalUse62.publish({});
    (void)kcd2vr::door_use46::acceptedIntent62.publish({});
    kcd2vr::door_hold65::frame.publish({});kcd2vr::door_surface65::witnessMailbox.publish({});
    for(auto& target:g_native_use_target59c10)target=0;
}
static std::uint64_t run38() { return Hook38(actor38.data(),"door_l_f_o",42,true,.75f); }
int main() {
    setup38();assert(run38()==result38&&nativeCalls38==1&&armCalls38==1&&fingerCalls38==1);
    assert(grantedSide38==0&&granted38.candidate.entity==42&&granted38.candidate.owner==0&&granted38.candidate.tick==1000);
    assert(granted38.epoch==7&&granted38.player==reinterpret_cast<std::uintptr_t>(player38.data()));
    assert(granted38.candidate.sourceGeneration==1&&granted38.candidate.sourceEpoch==7&&granted38.candidate.sourceTick==1000&&
        granted38.candidate.sourcePlayer==reinterpret_cast<std::uintptr_t>(player38.data()));
    assert(g_native_use_target59c10[0]==42&&g_native_use_target59c10[1]==0);
    setup38();result38=0;assert(run38()==0&&nativeCalls38==1&&armCalls38==0&&fingerCalls38==0);
    for(int bad=0;bad<3;++bad){setup38();InteractionHandContext158 hand{};hand.side=0;hand.valid=bad!=0;hand.tick=bad==1?919:1001;
        g_interaction_hand_context158.publish(hand);assert(run38()==result38&&nativeCalls38==1&&armCalls38==0&&fingerCalls38==0);}
    setup38();delay38=81;assert(run38()==result38&&nativeCalls38==1&&armCalls38==0&&fingerCalls38==0);
    setup38();records38.clear();assert(run38()==result38&&armCalls38==1&&granted38.candidate.tick==0);
    setup38();changeOwner38=true;assert(run38()==result38&&armCalls38==1);
    assert(granted38.epoch==7&&g_pose_epoch6689==8&&granted38.player==reinterpret_cast<std::uintptr_t>(player38.data()));
    // Native Arm's separate fixture rejects this old epoch/player witness.
    setup38();g_interaction_original6689=nullptr;assert(run38()==0&&nativeCalls38==0&&armCalls38==0&&fingerCalls38==0);
    // C46: the actual entry capture retains the input which issued this exact
    // target. Nested original work cannot relabel it as another target/hand.
    setup38();kcd2vr::door_use46::Intent input{};input.token=7;input.tick=input.poseTick=input.selectedTick=1000;
    input.press=900;input.poseEpoch=7;input.sourceGeneration=1;input.trackingEpoch=3;input.player=reinterpret_cast<std::uintptr_t>(player38.data());
    input.target=42;input.side=0;input.sent=true;(void)kcd2vr::door_use46::intent.publish(input);
    replaceIntent46=true;assert(run38()==result38&&armCalls38==1&&granted38.input46.token==7&&granted38.input46.target==42&&granted38.input46.side==0);
    assert(!kcd2vr::door_use46::current(granted38.input46,tick38)); // Real Arm separately refuses the revoked captured token.
    // C62 physical transport can precede correct native candidate ranking.
    // The actual hook must copy the exact door and immutable source press
    // before original changes the hand/candidate; only its nonzero return arms.
    auto physical62=[](bool withSurface=true){kcd2vr::door_use46::Controller producer;
        kcd2vr::door_use46::Input in{};in.available=in.held=true;in.side=0;in.now=in.poseTick=tick38;
        in.press=tick38-20;in.poseEpoch=7;in.sourceGeneration=1;in.trackingEpoch=3;
        in.player=reinterpret_cast<std::uintptr_t>(player38.data());
        kcd2vr::door_use46::Selection other{};other.target=99;other.side=0;other.tick=tick38;
        assert(!producer.update(in,other).owns);producer.transport62(in,true,false);
        // C66 requires a copied current held hand and independently qualified same-door surface.
        if(withSurface){kcd2vr::door_hold65::Frame f{};auto& h=f.hand[0];h.available=true;h.side=0;h.grip=.8f;
            h.tick=h.poseTick=tick38;h.press=in.press;h.player=in.player;h.poseEpoch=in.poseEpoch;h.trackingEpoch=in.trackingEpoch;
            h.sourceGeneration=in.sourceGeneration;h.settingsEpoch=kcd2vr::door_use46::settingsEpoch.load();h.experimentEpoch=kcd2vr::workstation178r20c1::experimentEpoch36.load();
            assert(kcd2vr::door_hold65::frame.publish(f));kcd2vr::door_surface65::Witness w{};
            w.source=kcd2vr::door_surface65::request(42,h,tick38,{});w.queryTick=w.sample=tick38;w.actualPalm=h.point;w.point={.08f,0,0};w.normal={1,0,0};w.entityId=42;w.opaqueOwner=0x42;w.opaqueCollider=0x4200;w.distance=.08f;w.valid=true;
            assert(kcd2vr::door_surface65::publish(w));}
        return in;};
    setup38();(void)physical62(false);assert(run38()==result38&&armCalls38==1&&!granted38.acceptedUse62.valid);
    setup38();const auto physicalInput62=physical62();replaceIntent46=true;
    assert(run38()==result38&&nativeCalls38==1&&armCalls38==1&&granted38.acceptedUse62.valid&&!granted38.input46.token);
    assert(granted38.acceptedUse62.pulse.input.press==physicalInput62.press&&granted38.acceptedUse62.pulse.input.now==1000&&
        granted38.acceptedUse62.selection.target==42&&granted38.acceptedUse62.selection.side==0&&granted38.candidate.entity==42);
    setup38();(void)physical62();result38=0;
    assert(run38()==0&&nativeCalls38==1&&armCalls38==0&&fingerCalls38==0);
    kcd2vr::door_use46::Intent noAccepted62{};assert(!kcd2vr::door_use46::read(noAccepted62,tick38));
    setup38();(void)physical62();delay38=81;
    assert(run38()==result38&&nativeCalls38==1&&armCalls38==0&&fingerCalls38==0);
    setup38();(void)physical62();records38.clear();assert(run38()==result38&&armCalls38==1&&!granted38.acceptedUse62.valid);
    assert(diagnosticCandidateReads63==0);
    std::puts("door_request38: exact native hook preserves entry target/hand/epoch/player across nested mutation; native arguments, return and rejection preserved PASS");
}
