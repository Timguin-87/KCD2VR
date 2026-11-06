// Connected real Held policy -> Use/prop producer -> game-thread input
// consumer. Only XR/committed-pose/body and the physics publication leaves are
// substituted. The separate grab_context_d41 fixture executes that consumer.
#define main inherited_door_producer_rearm_d41
#include "door_producer46_test.cpp"
#undef main
namespace gi=kcd2vr::physics::grab_input60;
namespace gp=kcd2vr::physics::grab_producer60;
static unsigned publicationsD41=0,cancellationsD41=0;
static gp::Intent publishedD41{};
namespace kcd2vr::physics::grab_producer60 {
bool PublishGrip(const Intent& in) noexcept {++publicationsD41;publishedD41=in;return true;}
void Cancel(int) noexcept {++cancellationsD41;}
}
static bool resolve(const gi::GripEvidence& e,void*& body,int& id) noexcept {
    body=reinterpret_cast<void*>(std::uintptr_t(0x3000+e.side));id=50+e.side;return true;
}
static gi::GripEvidence step(Producer& p,std::uint64_t tick,float grip) {
    candidate(tick,0,"BasicEntity",.08f);p.tick(tick,grip);
    gi::GripEvidence e{};check(gi::g_evidence[0].read(e));
    gi::ConsumeEvidenceD40(e,tick,&resolve);return e;
}
int main() {
    for(unsigned cause=0;cause<8;++cause) {
        gi::CancelSideD40(0);gi::CancelSideD40(1);
        gi::g_retiredPressD40[0]=gi::g_retiredPressD40[1]=0;
        fixture_grab_d40::statusReady[0]=fixture_grab_d40::statusReady[1]=false;
        fixture_grab_d40::pose[0]=true;fixture_grab_d40::pose[1]=false;
        fixture_grab_d40::poseEpochD41[0]=7;fixture_grab_d40::trackingEpochD41[0]=3;
        hands59c7f::rung=0;action_time_owner59c11::grasp=true;kcd2vr::collab_h21::setPhysicsMode(2);
        Producer p;std::uint64_t t=1000+cause*1000;
        step(p,t,0);step(p,t+=20,1);auto e=step(p,t+=20,1);
        const auto press=e.press;check(e.held&&e.eligible&&press&&publishedD41.press==press);
        const auto count=publicationsD41,cancels=cancellationsD41;
        if(cause==0)p.ui=true;
        if(cause==1)p.tracked=false;
        if(cause==2)t+=100; // discontinuous input resets owner and both neutral flags
        if(cause==3)p.sourceAge=81;
        if(cause==4){++p.poseEpoch;++fixture_grab_d40::poseEpochD41[0];}
        if(cause==5){++p.trackingEpoch;++fixture_grab_d40::trackingEpochD41[0];}
        if(cause>=6)kcd2vr::collab_h21::setPhysicsMode(0);
        if(cause==7)kcd2vr::collab_h21::setPhysicsMode(2); // OFF/ON between input visits
        e=step(p,t+=20,1);
        if(cause==7) {
            // The downstream actual producer retires this profile epoch (see
            // grab_context_d41). This upstream owner retains the SAME press.
            check(e.press==press&&publishedD41.press==press);
        } else check(publicationsD41==count&&cancellationsD41>cancels);
        p.ui=false;p.tracked=true;p.sourceAge=0;kcd2vr::collab_h21::setPhysicsMode(2);
        for(unsigned n=0;n<3;++n){e=step(p,t+=20,1);
            if(cause<3)check(!e.held&&p.heldUse114.owner<0);
            else check(e.press==press); // epochs/stale input do not mint a new squeeze
            if(cause!=7)check(publicationsD41==count);
        }
        const auto resumed=publicationsD41;
        step(p,t+=20,0);step(p,t+=20,1);e=step(p,t+=20,1);
        check(e.held&&e.eligible&&e.press!=press&&publicationsD41>resumed&&publishedD41.press==e.press);
    }
    std::printf("D41 connected input rearm: %u checks PASS; menu/tracking/gap/stale input/pose/tracking/profile epochs preserve open-regrip\n",checks);
}
