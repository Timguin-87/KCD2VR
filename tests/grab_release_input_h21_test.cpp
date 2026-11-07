// Executes the ACTUAL game-thread release producer, with only its external
// grab status/entity service substituted. Native retirement is tested in the
// actual backend/native-phase fixture, not claimed from this mailbox test.
#include "subsystems/physics/native_grab_input60.h"
#include <cassert>
#include <cstdio>
namespace gp=kcd2vr::physics::grab_producer60;
namespace gi=kcd2vr::physics::grab_input60;
namespace th=kcd2vr::throw60;
namespace {gp::Status status{};int cancelled=0;void* entity(std::uint32_t) noexcept{return reinterpret_cast<void*>(0x456);}}
namespace kcd2vr::physics::grab_producer60 {
bool ReadStatus(int,Status& out) noexcept{out=status;return true;}
void Cancel(int) noexcept{++cancelled;}
bool PublishGrip(const Intent&) noexcept{return false;}
}
int main(){
    auto input=[](){
        kcd2vr::collab_h21::setPhysicsMode(2);kcd2vr::collab_h21::setThrowing(true);
        gi::g_base=1;gi::resolveEntity=&entity;gi::g_lastEntity[0]=73;gi::g_lastBody[0]=0x123;gi::g_lastPress[0]=9;
        gi::g_lastPlayerD40[0]=0x111;gi::g_lastPoseEpochD40[0]=3;gi::g_lastTrackingEpochD40[0]=4;
        gi::g_wasHeld[0]=true;gi::g_prevPressH21[0]=9;gi::g_prevTick[0]=990;
        gi::g_releaseVelocityH21[0][0]=2;gi::g_releaseVelocityH21[0][1]=gi::g_releaseVelocityH21[0][2]=0;
        gi::g_motionThrowEpochH21[0]=kcd2vr::collab_h21::throwEpoch.load();gi::g_motionPhysicsEpochH21[0]=kcd2vr::collab_h21::epoch.load();
        status={};status.tick=990;status.state=gp::State::Held;status.owner.side=0;status.owner.player=0x111;status.owner.press=9;status.owner.entity=73;
        status.owner.world=0x789;status.owner.worldGeneration=5;status.owner.poseEpoch=3;status.owner.trackingEpoch=4;
        (void)th::mailbox[0].publish({});
        gi::GripEvidence e{};e.side=0;e.entity=73;e.press=9;e.tick=1000;e.object=e.eligible=true;
        return e;
    };
    auto run=[](const gi::GripEvidence& e){assert(gi::PublishGripEvidence(e));gi::GameThreadTick(1000);th::ThrowRequest out{};assert(th::read(0,out));return out;};
    auto e=input();auto r=run(e);assert(r.press==9&&r.body==0x123&&r.entityPointer==0x456&&r.world==0x789&&r.v[0]==2&&r.tick==990);
    e=input();e.eligible=false;assert(!run(e).press); // menu/tracking loss is never a throw
    e=input();e.press=10;assert(!run(e).press);
    e=input();gi::g_prevTick[0]=919;assert(!run(e).press);
    e=input();kcd2vr::collab_h21::setThrowing(false);assert(!run(e).press);
    e=input();kcd2vr::collab_h21::setPhysicsMode(0);assert(!run(e).press);
    e=input();e.object=false;assert(!run(e).press);
    e=input();gi::g_releaseVelocityH21[0][0]=.2f;assert(!run(e).press);
    e=input();kcd2vr::collab_h21::setThrowing(false);kcd2vr::collab_h21::setThrowing(true);assert(!run(e).press);
    e=input();kcd2vr::collab_h21::setPhysicsMode(0);kcd2vr::collab_h21::setPhysicsMode(2);assert(!run(e).press);
    assert(cancelled==10);
    std::puts("PASS actual release producer: zero release palm never creates velocity; exact held source, F4/F9, tracking and age gates");
}
