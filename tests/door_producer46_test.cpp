// Complete Mod native-Use producer, with explicit copied XR/selection/transport
// leaves. Native Arm, actual private channel and Lua are separate joined tests.
#include "../src/vrik/pickup_use19c3.h"
#include "../src/subsystems/doors/hand_only_h40.h"
#include "../src/vrik/held_interaction114.h"
#include "../src/vrik/hands_mode59c7f.h"
#include "../src/interaction_alignment.h"
#include "../src/tracking_sample59c6.h"
#include <cassert>
#include <cstdio>
#include <string>
#include <vector>
using namespace kcd2vr;
static std::uint64_t clock54=0;
static std::uint64_t GetTickCount64() noexcept{return clock54;}
namespace kcd2vr::interaction_alignment {
static HandCandidate candidate{};static bool exact=true;static std::uint64_t generation=1;
static HandCandidate direct54r1{};
static palm59c9::Pair published{};
bool LastHandCandidate(HandCandidate& c) noexcept {c=candidate;return c.tick!=0;}
bool NativeDirectDoorCandidate54r1(int side,std::uint64_t now,HandCandidate& c) noexcept {
    c=direct54r1;return c.entity&&c.owner==side&&door_use46::fresh(c.tick,now)&&door_use46::fresh(c.sourceTick,now);
}
bool ExactDoorCandidate38(std::uint32_t target,int side,std::uint64_t now,HandCandidate& c) noexcept {
    if(NativeDirectDoorCandidate54r1(side,now,c)&&c.entity==target)return exact;
    c=candidate;return exact&&c.entity==target&&c.owner==side&&door_use46::fresh(c.tick,now);
}
std::uint64_t DoorSourceGeneration38() noexcept{return generation;}
bool DoorSourceCurrent38(std::uint64_t g) noexcept{return g&&g==generation;}
std::uint64_t DoorAcquisitionGeneration65() noexcept{return generation;}
bool DoorAcquisitionCurrent65(std::uint64_t g) noexcept{return g&&g==generation;}
void SetPalmRays59c9(const palm59c9::Pair& p) noexcept{published=p;}
void SetHandRayWorld(bool,float,float,float,float,float,float,float,float,int) noexcept{}
}
namespace kcd2vr::action_time_owner59c11 {
static bool grasp=false;
bool NativeGraspEnabled() noexcept{return grasp;}
bool NativeGraspAvailable63() noexcept {return grasp;}
static unsigned physicalPickups63=0;
void PreparePhysicalPickup63(bool,int,std::uint64_t) noexcept {}
void NotePhysicalPickupUse63(int,std::uint64_t,std::uint32_t) noexcept {++physicalPickups63;}
void NoteUseInput(bool,bool,int,std::uint64_t) noexcept{}
}
namespace kcd2vr::frik_pose_probe {
void NoteVrPickupUse43(int,std::uint32_t,bool,std::uint64_t) noexcept{}
void SetInteractionHandContext158(int,bool,float,float,float,float) noexcept{}
}
namespace kcd2vr::diagnostic_trace {static std::vector<std::string> lines;void Write(const char* s){lines.emplace_back(s);}}
#include "door_producer_fixture46.h"

static unsigned checks=0;static void checkAt(bool v,int line){++checks;if(!v)std::fprintf(stderr,"door producer check %u failed at line %d\n",checks,line);assert(v);}
#define check(x) checkAt((x),__LINE__)
static void candidate(std::uint64_t tick,int side=0,const char* cls="AnimDoor",float distance=1.4f){
    auto& c=interaction_alignment::candidate;c={};c.tick=c.sourceTick=tick;c.sourceEpoch=7;c.sourceGeneration=1;c.sourcePlayer=0x1234;
    c.entity=42;c.owner=side;c.object=true;c.distance=distance;c.z=distance;std::snprintf(c.cls,sizeof(c.cls),"%s",cls);
}
static void start(Producer& p,int side=0){
    candidate(1000,side);p.tick(1000,0,side);candidate(1020,side);p.tick(1020,1,side);candidate(1040,side);p.tick(1040,1,side);
}
int main(){
    // F10 near-grab arbitration and Picking are deliberately unavailable: an
    // ordinary native door still needs its short release while Grip is held.
    hands59c7f::rung=2;hands59c7f::admitted=true;hands59c7f::pickupReady=false;
    action_time_owner59c11::grasp=false;
    for(int side=0;side<2;++side){Producer p;start(p,side);
        check(p.touch_bindings_.requests==1&&!p.touch_bindings_.held&&p.touch_bindings_.value());
        door_use46::Intent original{};check(door_use46::read(original,1040)&&original.target==42&&original.side==side);
        check(interaction_alignment::published.lockedEntity==42&&interaction_alignment::published.locked==side);
        for(unsigned t=1060;t<=1200;t+=20){candidate(t,side,"PickableItem",.1f);interaction_alignment::candidate.entity=77;p.tick(t,1,side);}
        check(p.touch_bindings_.requests==1&&!p.touch_bindings_.held&&!p.touch_bindings_.value());
        check(door_use46::current(original,1200)); // The native release now occurs while physical Grip stays down.
        check(interaction_alignment::published.lockedEntity==42);
        p.rayPublished116=false;p.tick(1220,1,side);check(!door_use46::current(original,1220));
        p.rayPublished116=true;candidate(1240,side);p.tick(1240,1,side);check(p.touch_bindings_.requests==1&&!p.touch_bindings_.held);
        p.tick(1260,0,side);candidate(1280,side);p.tick(1280,1,side);candidate(1300,side);p.tick(1300,1,side);
        check(p.touch_bindings_.requests==2);
    }
    for(int bad=0;bad<6;++bad){Producer p;candidate(1000);p.tick(1000,0);candidate(1020);p.tick(1020,1);candidate(1040);
        if(bad==0)interaction_alignment::exact=false;if(bad==1)p.surfaceDistance65=.201f;
        if(bad==2)p.poseAge=81;if(bad==3)p.sourceAge=81;if(bad==4)p.trackingEpoch=0;if(bad==5)p.touch_bindings_.allow=false;
        p.tick(1040,1);check(p.touch_bindings_.requests==(bad==5?1u:0u));
        check(!p.touch_bindings_.held);
        interaction_alignment::exact=true;p.poseAge=p.sourceAge=0;p.trackingEpoch=3;p.touch_bindings_.allow=true;
        for(unsigned t=1060;t<=1160;t+=20){candidate(t);p.tick(t,1);}
        // C65 permits a first exact eligible near Door to arrive later during
        // the same fresh unsent squeeze. A real tracking/queue failure remains
        // retired; the initially absent exact witness alone is not transport.
        check(p.touch_bindings_.requests==((bad==0||bad==5)?1u:0u));
    }
    // The native selector can lag the newly locked grip by several frames.
    // Neither the other palm's item nor an absent candidate gets to classify
    // this squeeze before its own native door offer has arrived.
    for(int side=0;side<2;++side){Producer p;
        candidate(1000,1-side,"PickableItem",.1f);p.tick(1000,0,side);
        for(unsigned t=1020;t<=1100;t+=20){candidate(t,1-side,"PickableItem",.1f);p.tick(t,1,side);
            check(p.touch_bindings_.requests==0&&!p.touch_bindings_.held);}
        candidate(1120,side);p.tick(1120,1,side);check(p.touch_bindings_.requests==1&&!p.touch_bindings_.held);
        candidate(1140,side);p.tick(1140,1,side);check(p.touch_bindings_.requests==1);
        door_use46::Intent own{};check(door_use46::read(own,1140)&&own.side==side&&own.target==42);
    }
    {Producer p;candidate(1000,1);p.tick(1000,0);
     for(unsigned t=1020;t<=1300;t+=20){candidate(t,1);p.tick(t,1);}
     candidate(1320,0);p.tick(1320,1);check(p.touch_bindings_.requests==1&&!p.touch_bindings_.held);
     p.tick(1340,0);candidate(1360);p.tick(1360,1);candidate(1380);p.tick(1380,1);check(p.touch_bindings_.requests==2);
    }
    // A confirmed non-door keeps ordinary native held input and cannot become
    // a second door action merely by moving the hand during the same squeeze.
    {hands59c7f::rung=0;Producer p;
     candidate(1000,0,"PickableItem",.4f);p.tick(1000,0);
     candidate(1020,0,"PickableItem",.4f);p.tick(1020,1);
     candidate(1040,0,"PickableItem",.4f);p.tick(1040,1);check(p.touch_bindings_.held&&p.touch_bindings_.requests==0);
     candidate(1060);p.tick(1060,1);check(p.touch_bindings_.held&&p.touch_bindings_.requests==0);
    }
    // Execute the actual generic Held fallback across the setting change. An
    // OFF transaction must not become a new native A/E hold for the same Grip.
    for(int transition=0;transition<2;++transition){Producer p;door_use46::SetEnabled(true);start(p);
        check(p.touch_bindings_.requests==1&&!p.touch_bindings_.held);
        door_use46::SetEnabled(false);if(transition)door_use46::SetEnabled(true); // same input-frame toggle
        for(unsigned t=1060;t<=1200;t+=20){candidate(t);p.tick(t,1);}
        check(p.touch_bindings_.requests==1&&!p.touch_bindings_.held&&!p.touch_bindings_.value());
        door_use46::Intent retired{};check(!door_use46::read(retired,1200));
        if(!transition){ // A genuinely new OFF interaction keeps native controls.
            p.tick(1220,0);candidate(1240);p.tick(1240,1);candidate(1260);p.tick(1260,1);
            check(p.touch_bindings_.held&&p.touch_bindings_.requests==1);
            p.tick(1280,0);
        }
        door_use46::SetEnabled(true);candidate(1300);p.tick(1300,transition?1:0);
        if(transition)check(p.touch_bindings_.requests==1&&!p.touch_bindings_.held);
        p.tick(1320,0);candidate(1340);p.tick(1340,1);candidate(1360);p.tick(1360,1);
        check(p.touch_bindings_.requests==2&&!p.touch_bindings_.held);
    }
    // A new XR publication can be later than the loop-start time by a complete
    // xrWaitFrame. Execute the ACTUAL producer and retain the 80-ms age cap.
    for(unsigned wait: {1u,16u,32u}){Producer p;p.frameWait54=wait;
        candidate(1000+wait);p.tick(1000,0);candidate(1020+wait);p.tick(1020,1);
        candidate(1040+wait);p.tick(1040,1);
        check(p.touch_bindings_.requests==1&&!p.touch_bindings_.held);
        door_use46::Intent own{};check(door_use46::read(own,1040+wait)&&own.tick==1040+wait);
        p.sourceAge=81;candidate(1060+wait);p.tick(1060,1);
        check(!door_use46::current(own,1060+wait)&&!p.touch_bindings_.held);
    }
    // An unclassified squeeze which failed freshness once remains suppressed
    // on BOTH unavailable and available frames. No native held-use chatter.
    {Producer p;candidate(1000,1);p.tick(1000,0);
        p.sourceAge=81;candidate(1020,1);p.tick(1020,1);
        for(unsigned t=1040;t<=1200;t+=20){p.sourceAge=(t/20)%2?81:0;
            candidate(t,1);p.tick(t,1);check(!p.touch_bindings_.held&&p.touch_bindings_.requests==0);}
        p.sourceAge=0;p.tick(1220,0);candidate(1240);p.tick(1240,1);candidate(1260);p.tick(1260,1);
        check(p.touch_bindings_.requests==1&&!p.touch_bindings_.held);
    }
    // Retail doors can be selected by the direct-ray path without ever
    // publishing a projection candidate. A secondary nearby item (including
    // the SAME hand) must not turn that actual door offer into held native A.
    for(int side=0;side<2;++side){Producer p;
        auto direct=[&](std::uint64_t t){candidate(t,side);interaction_alignment::direct54r1=interaction_alignment::candidate;
            candidate(t,side,"PickableItem",.12f);interaction_alignment::candidate.entity=263342;};
        direct(1000);p.tick(1000,0,side);direct(1020);p.tick(1020,1,side);direct(1040);p.tick(1040,1,side);
        check(p.touch_bindings_.requests==1&&!p.touch_bindings_.held);
        door_use46::Intent own{};check(door_use46::read(own,1040)&&own.target==42&&own.side==side);
        direct(1060);p.tick(1060,1,side);check(p.touch_bindings_.requests==1&&door_use46::current(own,1060));
        p.tick(1080,0,side);check(!door_use46::current(own,1080));interaction_alignment::direct54r1={};
    }
    // Real independent body/clock tests handle native admission. This producer
    // never guesses it from a pulse and never grants a reward or hinge owner.
    // Retained distant Assisted route: clicks do nothing, a held Pickable
    // Grip owns one native short Use after150ms. Direct has its own D40 test.
    const bool savedReachD40=pickup_use19c3::nearOnlyD40.load();pickup_use19c3::setNearOnlyD40(false);
    hands59c7f::rung=0;action_time_owner59c11::grasp=true;
    for(unsigned mode:{0u,2u}){
        Producer p;p.f5Mode=mode;p.comparison114=mode==2;
        candidate(1000,0,"PickableItem",.15f);p.tick(1000,0);
        for(unsigned t=1020;t<=1120;t+=20){candidate(t,0,"PickableItem",.15f);p.tick(t,1);check(!p.touch_bindings_.held&&p.touch_bindings_.requests==0);}
        candidate(1140,0,"PickableItem",.15f);p.tick(1140,0);check(p.touch_bindings_.requests==0);
        for(unsigned t=1160;t<=1260;t+=20){candidate(t,0,"PickableItem",.15f);p.tick(t,0);check(p.touch_bindings_.requests==0);}
        const auto before=action_time_owner59c11::physicalPickups63;
        for(unsigned t=1280;t<=1540;t+=20){candidate(t,0,"PickableItem",.15f);p.tick(t,1);check(!p.touch_bindings_.held);}
        check(p.touch_bindings_.requests==1&&action_time_owner59c11::physicalPickups63==before+1);
        // After the actual Use is sent, moving to another object does not
        // revoke the already owned held item or generate another pickup.
        for(unsigned t=1560;t<=1700;t+=20){candidate(t,0,"PickableItem",.15f);interaction_alignment::candidate.entity=77;p.tick(t,1);}
        check(p.touch_bindings_.requests==1&&action_time_owner59c11::physicalPickups63==before+1);
    }
    {Producer p;candidate(1000,0,"PickableItem",.15f);p.tick(1000,0);
        for(unsigned t=1020;t<=1100;t+=20){candidate(t,0,"PickableItem",.15f);p.tick(t,1);}
        candidate(1120,0,"PickableItem",.15f);
        p.rayPublished116=false;
        p.tick(1120,1);p.rayPublished116=true;
        for(unsigned t=1140;t<=1300;t+=20){candidate(t,0,"PickableItem",.15f);p.tick(t,1);}
        check(p.touch_bindings_.requests==0&&!p.touch_bindings_.held);
    }
    // H12 actual production caller: Grip can precede its first native target.
    // Neither no candidate nor the other palm's item may classify the squeeze
    // permanently or leak a generic native held Use. Qualify at arrival, not
    // at the older squeeze timestamp; test ordinary play and F5 comparison.
    for(unsigned mode:{0u,2u})for(int side=0;side<2;++side){Producer p;
        p.f5Mode=mode;p.comparison114=mode==2;interaction_alignment::candidate={};
        p.tick(2000,0,side);
        for(unsigned t=2020;t<=2200;t+=20){interaction_alignment::candidate={};p.tick(t,1,side);
            check(p.touch_bindings_.requests==0&&!p.touch_bindings_.held);}
        for(unsigned t=2220;t<=2300;t+=20){candidate(t,1-side,"PickableItem",.12f);p.tick(t,1,side);
            check(p.touch_bindings_.requests==0&&!p.touch_bindings_.held);}
        for(unsigned t=2320;t<2400;t+=20){candidate(t,side,"PickableItem",.12f);p.tick(t,1,side);
            check(p.touch_bindings_.requests==0&&!p.touch_bindings_.held);}
        candidate(2400,side,"PickableItem",.12f);p.tick(2400,1,side);
        check(p.touch_bindings_.requests==1&&!p.touch_bindings_.held&&p.pickupUse19c3.target63==42);
        // Only real transport freezes the target. Later candidates cannot
        // produce another request or retarget the attached-item acquisition.
        for(unsigned t=2420;t<=2540;t+=20){candidate(t,side,"PickableItem",.12f);interaction_alignment::candidate.entity=77;p.tick(t,1,side);}
        check(p.touch_bindings_.requests==1&&p.pickupUse19c3.target63==42);
        p.tick(2560,0,side);candidate(2580,side,"PickableItem",.12f);p.tick(2580,1,side);
        for(unsigned t=2600;t<=2760;t+=20){candidate(t,side,"PickableItem",.12f);p.tick(t,1,side);}
        check(p.touch_bindings_.requests==2);
    }
    // Before a native request exists, a changed/stale candidate restarts only
    // its qualification. No native input is replayed, no old target is frozen.
    for(unsigned mode:{0u,2u}){Producer p;p.f5Mode=mode;p.comparison114=mode==2;
        candidate(3000,0,"PickableItem",.12f);p.tick(3000,0);
        for(unsigned t=3020;t<=3140;t+=20){candidate(t,0,"PickableItem",.12f);p.tick(t,1);}
        candidate(3160,0,"PickableItem",.12f);interaction_alignment::candidate.entity=77;p.tick(3160,1);
        for(unsigned t=3180;t<3240;t+=20){candidate(t,0,"PickableItem",.12f);interaction_alignment::candidate.entity=77;p.tick(t,1);
            check(p.touch_bindings_.requests==0&&!p.touch_bindings_.held);}
        candidate(3240,0,"PickableItem",.12f);interaction_alignment::candidate.entity=77;p.tick(3240,1);
        check(p.touch_bindings_.requests==1&&p.pickupUse19c3.target63==77);
        p.tick(3260,0);
        candidate(3280,0,"PickableItem",.12f);p.tick(3280,1);
        candidate(3300,0,"PickableItem",.12f);interaction_alignment::candidate.tick=3100;p.tick(3300,1);
        for(unsigned t=3320;t<=3480;t+=20){candidate(t,0,"PickableItem",.12f);p.tick(t,1);}
        check(p.touch_bindings_.requests==2&&p.pickupUse19c3.target63==42);
    }
    // An actually fresh non-pickable retains native contextual held controls.
    // Releasing before any target remains inert; a new squeeze rearms normally.
    {Producer p;interaction_alignment::candidate={};p.tick(4000,0);
        for(unsigned t=4020;t<=4200;t+=20){interaction_alignment::candidate={};p.tick(t,1);check(!p.touch_bindings_.held&&!p.touch_bindings_.requests);}
        p.tick(4220,0);candidate(4240,0,"BasicEntity",.6f);p.tick(4240,1);
        candidate(4260,0,"BasicEntity",.6f);p.tick(4260,1);
        check(p.touch_bindings_.held&&!p.touch_bindings_.requests);
    }
    // Default mode and explicit spatial test share actual Use dispatch,
    // without manufacturing comparison admission in ordinary gameplay.
    for(unsigned mode:{0u,2u,3u})for(int side=0;side<2;++side){
        Producer p;p.f5Mode=mode;p.comparison114=mode!=0;
        candidate(5000,side);p.tick(5000,0,side);
        candidate(5020,side);p.tick(5020,1,side);candidate(5040,side);p.tick(5040,1,side);
        door_use46::Intent own{};check(p.touch_bindings_.requests==1&&!p.touch_bindings_.held&&door_use46::read(own,5040));
        check(own.target==42&&own.side==side);
        p.f5Mode=mode==0?2:0;candidate(5060,side);p.tick(5060,1,side);
        check(!door_use46::current(own,5060));
    }
    // H31: execute the real Use producer across a reachable raw-current /
    // missing point seam. Neither a pending direct-door pulse nor contextual
    // held input may turn reacquisition into another A/E press or generic tap.
    for(unsigned mode:{0u,2u})for(int side=0;side<2;++side)for(bool knownDoor:{false,true}){
        (void)door_use46::intent.publish({});(void)door_use46::acceptedIntent62.publish({});
        palm59c9::ordinaryTransportLock64={};interaction_alignment::direct54r1={};
        hands59c7f::rung=0;Producer p;p.f5Mode=mode;p.comparison114=mode!=0;
        const unsigned base=10000+mode*10000+side*2000+(knownDoor?1000:0);
        const char* cls=knownDoor?"AnimDoor":"BasicEntity";
        candidate(base,side,cls,.1f);p.tick(base,0,side);
        candidate(base+20,side,cls,.1f);p.tick(base+20,1,side);
        candidate(base+40,side,cls,.1f);p.tick(base+40,1,side);
        const auto serial=p.doorUse46.transportedSerial62;check(serial!=0);
        check(p.touch_bindings_.requests==(knownDoor?1u:0u));
        check(p.touch_bindings_.risingHeldEdgesH31==(knownDoor?0u:1u));
        door_use46::PhysicalUse62 original{};check(door_use46::physicalUse62.read(original)&&original.serial==serial);
        door_use46::Intent prior{};if(knownDoor)check(door_use46::read(prior,base+40));
        p.rayPublished116=false;candidate(base+60,side,cls,.1f);p.tick(base+60,1,side);
        check(p.doorUse46.pointGapPendingH31&&!p.doorUse46.cancelled&&door_use46::retiredPhysical62.load()<serial);
        check(!p.touch_bindings_.held&&p.touch_bindings_.requests==(knownDoor?1u:0u));
        door_use46::Input input{};check(door_use46::currentInput62.read(input)&&!input.available);
        if(knownDoor)check(!door_use46::read(prior,base+60)&&!door_use46::consume(p.doorUse46.chosen,base+60));
        // Deliberately hostile upstream tap on reacquisition. Plan ownership
        // must suppress it AND prevent the previous held input's rising edge.
        p.rayPublished116=true;p.forceGenericPulseH31=true;
        candidate(base+80,side,cls,.1f);p.tick(base+80,1,side);
        check(!p.touch_bindings_.held&&p.touch_bindings_.requests==(knownDoor?1u:0u));
        check(p.touch_bindings_.risingHeldEdgesH31==(knownDoor?0u:1u));
        check(door_use46::currentInput62.read(input)&&door_use46::inputCurrent62(input,base+80));
        door_use46::PhysicalUse62 retained{};check(door_use46::physicalUse62.read(retained)&&retained.serial==serial&&
            retained.input.now==original.input.now&&retained.input.poseTick==original.input.poseTick);
        if(!knownDoor){ // supplied native accepted-selection leaf, still checked against actual transport
            door_use46::Selection accepted{true,42,side,input.now,input.poseTick,input.poseEpoch,input.sourceGeneration,input.player,{0,.08f,0},.08f};
            door_use46::AcceptedUse62 receipt{};door_use46::Intent joined{};
            check(door_use46::captureAccepted62(accepted,base+80,receipt));
            check(door_use46::publishAccepted62(receipt,base+80,joined));
        }else check(door_use46::read(prior,base+80)&&prior.token==p.doorUse46.chosen.token);
        candidate(base+100,side,cls,.1f);p.tick(base+100,1,side);
        check(p.doorUse46.door&&!p.touch_bindings_.held&&p.touch_bindings_.requests==(knownDoor?1u:0u));
        check(p.touch_bindings_.risingHeldEdgesH31==(knownDoor?0u:1u));
        p.forceGenericPulseH31=false;p.tick(base+120,0,side);
    }
    check(door_use46::inputMode(1)&&door_use46::inputAvailable(1,true)&&!door_use46::inputAvailable(2,false));
    std::printf("door_producer46: %u checks PASS; complete production Use dispatch with explicit native/XR leaves\n",checks);
    pickup_use19c3::setNearOnlyD40(savedReachD40);
    return 0;
}
