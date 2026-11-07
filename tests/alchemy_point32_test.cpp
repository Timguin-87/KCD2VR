// Actual Mod ray eligibility -> point owner -> native-selected PalmLock chain.
#include "../src/activity_input163.h"
#include "../src/vrik/alchemy_palm_lock27.h"
#include "../src/vrik/hand_point123r1.h"
#include "../src/vrik/held_interaction114.h"
#include "../src/tracking_sample59c6.h"
#include "../src/vrik/workstation178r20c1.h"
#include "../src/vrik/palm_selection59c9.h"
#include "../src/core/door_use46.h"
#include <cassert>
#include <cstdio>
using namespace kcd2vr;
namespace kcd2vr::interaction_alignment {bool DoorSourceCurrent38(std::uint64_t) noexcept{return true;}std::uint64_t DoorSourceGeneration38() noexcept{return 1;}}
namespace kcd2vr::interaction_alignment {bool DoorAcquisitionCurrent65(std::uint64_t) noexcept{return true;}std::uint64_t DoorAcquisitionGeneration65() noexcept{return 1;}}
namespace kcd2vr::diagnostics::lab {static bool NativeReference() noexcept{return false;}}
int main(){
    auto& w=activity_input163::witness175;w.ready=true;
    w.Begin(activity_input163::Kind175::Alchemy,123,1000);
    workstation178r20c1::setExperiment36(true);
    struct Controller {bool active=true;float grip=0,trigger=0;};
    struct XR {Controller hands[2];std::uint64_t tick=1000;const Controller& RightController(){return hands[0];}
        const Controller& LeftController(){return hands[1];}
        bool CopyInputSampleStamp59c6(tracking59c6::Stamp& out){out.tick=tick;out.epoch=1;return true;}} xr;
    auto* xr_manager_=&xr;
    bool alchemyPointer59c2=true,pointerEnabled32=true;
    unsigned f5Mode=2,reserved114=0,gripMask116=0;int itemState116[2]{0,0};
    std::uint64_t ladder_now_ms=1000;hand_point123r1::State point123;
    held_interaction114::State heldUse114{};
    physical_interaction106::Input pi{};bool comparison114=false;
    const auto GetTickCount64=[&](){return ladder_now_ms;}; // deterministic native-clock leaf
    alchemy_input19c4::PalmLock27 alchemyPalmLock27;
    struct {bool ray_active=false;int ray_owner=-1;} po;
    struct {bool object=true;int owner=1;std::uint32_t entity=777;std::uint64_t tick=1000;} candidate59c7f;
    bool transitionInBuild32=false;
    auto buildPalm59c9=[&](int side,interaction112::Ray& ray){if(side==1&&transitionInBuild32)w.transitionDepth175=1;
        ray={};ray.owner=side;ray.hand=true;
        ray.tick=ladder_now_ms;ray.limit=2;ray.radius=.1f;ray.z=1;return true;};
    auto run=[&](float grip,float trigger,std::uint32_t native=42){
        ++ladder_now_ms;xr.hands[1].grip=grip;xr.hands[1].trigger=trigger;
        xr.tick=ladder_now_ms;physical_interaction106::Input heldInput64{};heldInput64.enabled=true;heldInput64.now_ms=ladder_now_ms;
        heldInput64.hand[0]={true,true,true,0,false};heldInput64.hand[1]={true,true,true,grip,false};
        pi=heldInput64;const auto held114=held_interaction114::update(heldUse114,heldInput64);
        candidate59c7f.tick=ladder_now_ms;
        alchemy_input19c4::selected.publish({123,w.lifetime175.load(),ladder_now_ms,native});
#include "../src/vrik/interaction_point32.inl"
        return palms59c9;
    };
    auto acquired=[&](){run(0,0);const auto p=run(1,0);assert(p.locked==1&&p.lockedEntity==42&&p.eligible[1]);};
    acquired();
    auto p=run(1,1,43);assert(p.locked==1&&p.lockedEntity==42&&p.eligible[1]);
    assert(palm59c9::accepts(p,42,1)&&!palm59c9::accepts(p,43,1));
    p=run(1,0,43);assert(p.locked==1&&p.lockedEntity==42);
    p=run(0,0,43);assert(p.locked==-1&&p.lockedEntity==0);
    // Trigger cannot acquire a new owner, even in Alchemy.
    p=run(1,1);assert(p.locked==-1&&!p.eligible[1]);
    acquired();
    // Same-address native restart retires trigger permission and held selection.
    w.End(123);w.Begin(activity_input163::Kind175::Alchemy,123,ladder_now_ms);
    p=run(1,1);assert(p.locked==-1&&!p.eligible[1]&&p.lockedEntity==0);
    p=run(1,0);assert(p.locked==-1&&p.lockedEntity==0);acquired();
    w.transitionDepth175=1;p=run(1,1);assert(p.locked==-1&&!p.eligible[1]);
    w.transitionDepth175=0;p=run(1,0);assert(p.lockedEntity==0);acquired();
    workstation178r20c1::setExperiment36(false);p=run(1,1);assert(p.locked==-1&&!p.eligible[1]);
    acquired();f5Mode=1;p=run(1,1);assert(p.locked==-1&&!p.eligible[1]&&p.lockedEntity==0);
    f5Mode=2;acquired();pointerEnabled32=false;p=run(1,1);assert(p.locked==-1&&!p.eligible[1]);
    pointerEnabled32=true;acquired();xr.hands[1].active=false;p=run(1,1);assert(p.locked==-1&&!p.eligible[1]);
    xr.hands[1].active=true;acquired();
    // A native transition during actual ray construction cannot publish the
    // earlier copied permission after the final PalmLock witness check.
    transitionInBuild32=true;p=run(1,1);assert(p.locked==-1&&!p.eligible[1]&&p.lockedEntity==0);
    transitionInBuild32=false;w.transitionDepth175=0;acquired();
    // Existing ordinary free-hand/fist/combat trigger ownership is unchanged.
    alchemyPointer59c2=false;run(0,0);p=run(1,0);assert(p.locked==1);
    p=run(1,1);assert(p.locked==-1&&!p.eligible[1]);
    // Even an accidentally passed maintenance capability never acquires.
    hand_point123r1::State s{};hand_point123r1::Input in{};in.enabled=true;in.now=1;
    in.hand[1]={true,0,0,true};hand_point123r1::Update(s,in);
    in.now=2;in.hand[1]={true,1,1,true};assert(hand_point123r1::Update(s,in).owner==-1);
    // H18: execute the actual Mod selection policy. Native/Semi keep gaze;
    // Free/F5 nonNative may nominate one close G1 hand, never a legacy palm ray.
    bool interaction_alignment_installed=true,safe_ui_fit=false,f2_ui_menu_active=false,matrix_active=false;
    struct {bool nativeControls=true;} activity163;
    struct {bool blocked=false;bool BlocksWorldInteraction()const{return blocked;}bool NativeContextControls()const{return !blocked;}} ui89_policy;
    const auto selectionPolicyH18=[&](){
#include "generated_guided_pointer55.inc"
        return std::array<bool,3>{alchemyPointer59c2,alchemyHeadSelection63,alchemyGesture67};
    };
    workstation178r20c1::alchemyPassenger42=true;workstation178r20c1::alchemyHandRay59c2=true;
    activity_input163::trackedHands59c7f[0]=true;
    minigame_hands45::publish(minigame_hands45::Alchemy,ladder_now_ms);
    unsigned selectionChecksH18=0;
    for(int mode=0;mode<3;++mode)for(unsigned rung=0;rung<4;++rung)for(bool armed:{false,true}){
        minigame_hands45::setMode(minigame_hands45::Alchemy,mode);
        combat_rules178r10::f5Rung=rung;workstation178r20c1::setExperiment36(armed);
        const auto policy=selectionPolicyH18();const bool bridge=mode!=minigame_hands45::Native&&
            (mode==minigame_hands45::Gesture||(rung==2&&armed));
        assert(!policy[0]&&policy[1]&&policy[2]==bridge&&subsystems::alchemy::bridge67::enabled.load()==bridge);++selectionChecksH18;
    }
    ui89_policy.blocked=true;auto unavailable=selectionPolicyH18();assert(!unavailable[0]&&!unavailable[1]&&!unavailable[2]);++selectionChecksH18;
    ui89_policy.blocked=false;activity_input163::trackedHands59c7f[0]=false;unavailable=selectionPolicyH18();
    assert(!unavailable[0]&&!unavailable[1]&&!unavailable[2]);++selectionChecksH18;activity_input163::trackedHands59c7f[0]=true;
    w.transitionDepth175=1;unavailable=selectionPolicyH18();assert(!unavailable[0]&&!unavailable[1]&&!unavailable[2]);++selectionChecksH18;w.transitionDepth175=0;
    // The retired hand source cannot retain a previous squeeze in minigames.
    alchemyPointer59c2=false;pointerEnabled32=false;p=run(1,1,43);
    assert(p.locked==-1&&!p.lockedEntity&&!p.eligible[1]);
    std::printf("PASS actual Mod point/palm producer + H18 selector policy: %u Native/Semi/Free/F5/blocked checks\n",selectionChecksH18);
}
