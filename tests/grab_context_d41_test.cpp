// Actual producer and constraint Service. Native callbacks are the only leaves
// substituted. This file also builds against D40R1 for behavioral red controls.
#include "../src/subsystems/physics/runtime/native_grab_producer60.h"
#include "../src/subsystems/physics/runtime/native_grab_producer60.inl"
#include <cstdio>

namespace g=kcd2vr::physics::grab_producer60;
namespace cs=kcd2vr::constraint60;
static int failures=0,checks=0;
static const char* scenario="startup";
#define CK(x) do{++checks;if(!(x)){std::printf("FAIL %s line %d: %s\n",scenario,__LINE__,#x);++failures;}}while(0)
static bool context[2]{true,true};
static bool sourceCurrent(const g::Intent& in) noexcept {
    return in.side>=0&&in.side<2&&context[in.side];
}
struct Hand {
    g::Owner owner{};
    unsigned prepares=0,adds=0,reads=0,drives=0,refreshes=0,withdrawals=0,removes=0,retires=0,refs=0;
    std::uint64_t addSerial=0,removeSerial=0;
    bool proxyTarget=false,pendingPrepare=false,pendingRetire=false,holdAbsent=false;
    bool revokePrepare=false,revokeRead=false,cancelPrepare=false,cancelRead=false,refreshRefused=false,driveUncertain=false;
};
struct Native {
    g::Stamp clock{0xABCD,5,1};std::uint64_t now=1000;
    Hand hand[2]{};
};
static bool stamp(void* p,g::Stamp& out) noexcept {out=static_cast<Native*>(p)->clock;return true;}
static g::Progress prepare(void* p,const g::Intent& in,std::uint64_t,g::Owner& out) noexcept {
    auto& n=*static_cast<Native*>(p);auto& h=n.hand[in.side];++h.prepares;
    h.owner=out;h.owner.world=n.clock.world;h.owner.worldGeneration=n.clock.generation;
    h.owner.proxyGeneration=100+in.side;h.refs=2;h.proxyTarget=true;
    if(h.revokePrepare)context[in.side]=false;
    if(h.cancelPrepare)g::Cancel(in.side);
    if(h.pendingPrepare)return g::Progress::Pending;
    out=h.owner;return g::Progress::Complete;
}
static g::Progress drive(void* p,const g::Owner& o,const g::Intent&) noexcept {
    auto& h=static_cast<Native*>(p)->hand[o.side];++h.drives;h.proxyTarget=true;
    return h.driveUncertain?g::Progress::Uncertain:g::Progress::Complete;
}
static g::Result add(void* p,const g::Owner& o,std::uint64_t token) noexcept {
    auto& n=*static_cast<Native*>(p);auto& h=n.hand[o.side];++h.adds;
    h.addSerial=n.clock.serial;h.removeSerial=0;
    return {o,token,n.clock.serial,std::uint32_t(70+o.side),g::Progress::Complete};
}
static g::Sample read(void* p,const g::Owner& o,std::uint64_t token,std::uint32_t id) noexcept {
    auto& n=*static_cast<Native*>(p);auto& h=n.hand[o.side];++h.reads;
    g::Sample s{o,token,n.clock.serial,id,cs::Status::Kind::Unknown,true};
    if(h.removeSerial&&n.clock.serial>h.removeSerial&&!h.holdAbsent)s.kind=cs::Status::Kind::Absent;
    else if(h.addSerial&&n.clock.serial>h.addSerial)s.kind=cs::Status::Kind::Present;
    if(h.revokeRead)context[o.side]=false;
    if(h.cancelRead)g::Cancel(o.side);
    return s;
}
static g::Result remove(void* p,const g::Owner& o,std::uint64_t token,std::uint32_t id) noexcept {
    auto& n=*static_cast<Native*>(p);auto& h=n.hand[o.side];++h.removes;h.removeSerial=n.clock.serial;
    return {o,token,n.clock.serial,id,g::Progress::Complete};
}
static g::Progress retire(void* p,const g::Owner& o) noexcept {
    auto& h=static_cast<Native*>(p)->hand[o.side];++h.retires;h.proxyTarget=false;
    if(h.pendingRetire)return g::Progress::Pending;
    h.refs=0;return g::Progress::Complete;
}
static bool refresh(void* p,const g::Owner& o,const g::Intent&) noexcept {
    auto& h=static_cast<Native*>(p)->hand[o.side];++h.refreshes;if(h.refreshRefused)return false;h.proxyTarget=true;return true;
}
static bool withdraw(void* p,const g::Owner& o) noexcept {
    auto& h=static_cast<Native*>(p)->hand[o.side];++h.withdrawals;h.proxyTarget=false;return true;
}
static g::Backend backend() {
    g::Backend b{stamp,prepare,drive,add,read,remove,retire,nullptr,refresh};
#ifdef KCD2VR_GRAB_CONTEXT_D41
    b.withdraw=&withdraw;
#endif
    return b;
}
static void reset(const char* name) {
    scenario=name;
    for(int s=0;s<2;++s){g::g_session[s]={};g::Cancel(s);context[s]=true;}
    g::SetEnabled(true);
}
static g::Intent intent(int side=0,std::uint64_t press=1) {
    g::Intent i{};i.player=0x100;i.expectedBody=0x200+side;i.entity=300+side;
    i.press=press;i.poseEpoch=2;i.trackingEpoch=3;i.sourceGeneration=press;
    i.side=side;i.physicsId=400+side;i.allowed=i.held=true;i.p[0]=2.f;return i;
}
static void advance(Native& n) {n.now+=10;++n.clock.serial;}
static void publish(Native& n,g::Intent& i) {i.tick=i.poseTick=n.now;CK(g::PublishGrip(i));}
static void frame(Native& n,g::Intent& i) {advance(n);publish(n,i);g::PhysicsRefresh(&n,n.now);g::PhysicsStep(&n,n.now);}
static g::Status status(int side=0) {g::Status s{};CK(g::ReadStatus(side,s));return s;}
static void held(Native& n,g::Intent& i) {
    frame(n,i);frame(n,i);CK(status(i.side).state==g::State::Held&&n.hand[i.side].refs==2);
}
static void finish(Native& n) {advance(n);g::PhysicsRefresh(&n,n.now);g::PhysicsStep(&n,n.now);}
int main() {
    kcd2vr::collab_h21::setPhysicsMode(2);
    CK(g::InstallBackend(backend()));
    // No live owner callback is not affirmative permission to drive native bodies.
    {reset("missing_context_provider");Native n{};auto i=intent();frame(n,i);CK(n.hand[0].prepares==0);}
#ifdef KCD2VR_GRAB_CONTEXT_D41
    CK(g::SetSourcePredicateD41(&sourceCurrent));
    CK(g::SetSourcePredicateD41(&sourceCurrent));
    CK(!g::SetSourcePredicateD41(nullptr));
#endif
    // A fresh copied input remains <=80ms old when a cutscene takes ownership.
    // Refresh executes BEFORE the real proxy motor; therefore it must withdraw
    // that retained target now, without destroying either constraint resource.
    {reset("cutscene_withdraw_before_motor");Native n{};auto i=intent();held(n,i);
     auto& h=n.hand[0];const auto driven=h.drives,refreshed=h.refreshes;
     context[0]=false;advance(n);g::PhysicsRefresh(&n,n.now);
     CK(!h.proxyTarget&&h.withdrawals==1&&h.refreshes==refreshed);
     CK(h.refs==2&&h.removes==0&&h.retires==0);
     g::PhysicsStep(&n,n.now);CK(h.removes==1&&h.drives==driven&&h.refs==2);
     h.holdAbsent=true;finish(n);CK(h.removes==1&&h.refs==2&&h.retires==0&&!h.proxyTarget);
     h.holdAbsent=false;finish(n);CK(status().state==g::State::Idle&&h.refs==0);
     context[0]=true;frame(n,i);CK(h.adds==1); // held old squeeze cannot resume
     ++i.poseEpoch;++i.trackingEpoch;++i.sourceGeneration;frame(n,i);
     CK(h.adds==1&&h.refs==0); // new tracking/context evidence is not a new squeeze
     ++i.press;++i.sourceGeneration;frame(n,i);frame(n,i);CK(h.adds==2&&status().state==g::State::Held);}
    // An OFF/ON context transition can complete between two physics callbacks.
    // A newly published packet of the same squeeze must not erase its Cancel.
    {reset("cancel_then_same_press_republish");Native n{};auto i=intent();held(n,i);
     auto& h=n.hand[0];const auto driven=h.drives,refreshed=h.refreshes;
     g::Cancel(0);advance(n);publish(n,i);g::PhysicsRefresh(&n,n.now);
     CK(!h.proxyTarget&&h.refreshes==refreshed);
     g::PhysicsStep(&n,n.now);CK(h.removes==1&&h.drives==driven);
     finish(n);frame(n,i);CK(h.adds==1&&h.refs==0);}
    // Direct cancellation also survives republish when called without the
    // runtime refresh phase (e.g. original teardown/shutdown path).
    {reset("cancel_identity_owned_by_session");Native n{};auto i=intent();held(n,i);
     g::Cancel(0);advance(n);publish(n,i);g::PhysicsStep(&n,n.now);CK(n.hand[0].removes==1);}
    // Recheck after prepare/read native leaves: initial permission may have
    // changed during a callback. No new Add or Drive may use that old verdict.
    {reset("context_retires_during_prepare");Native n{};auto i=intent();n.hand[0].revokePrepare=true;
     frame(n,i);CK(n.hand[0].adds==0&&n.hand[0].refs==0&&status().state==g::State::Idle);}
    {reset("context_retires_during_read");Native n{};auto i=intent();held(n,i);auto& h=n.hand[0];
     const auto driven=h.drives;h.revokeRead=true;frame(n,i);
     CK(h.drives==driven&&!h.proxyTarget&&status().state==g::State::Releasing);
     finish(n);CK(h.removes==1&&h.refs==2);finish(n);CK(h.refs==0);}
    // The UI may toggle OFF/ON between input frames. PublishGrip joins the new
    // profile epoch, but that does not manufacture another physical squeeze.
    {reset("profile_epoch_same_squeeze");Native n{};auto i=intent();held(n,i);auto& h=n.hand[0];
     kcd2vr::collab_h21::setPhysicsMode(0);kcd2vr::collab_h21::setPhysicsMode(2);
     frame(n,i);CK(h.removes==1);frame(n,i);CK(h.refs==0);
     frame(n,i);CK(h.adds==1&&h.refs==0);++i.press;++i.sourceGeneration;
     frame(n,i);frame(n,i);CK(h.adds==2&&status().state==g::State::Held);}
    // Native callback cancellation must survive even if its context returns to
    // allowed before the callback ends, with no extra producer publication.
    {reset("cancel_during_prepare");Native n{};auto i=intent();n.hand[0].cancelPrepare=true;
     frame(n,i);CK(n.hand[0].adds==0&&n.hand[0].refs==0);}
    {reset("cancel_during_read");Native n{};auto i=intent();held(n,i);auto& h=n.hand[0];
     const auto driven=h.drives;h.cancelRead=true;frame(n,i);
     CK(h.drives==driven&&!h.proxyTarget&&status().state==g::State::Releasing);}
    // Backend refresh can itself refuse the second context/owner check. Leaving
    // its last input untouched would still drive the following proxy phase.
    {reset("refused_refresh_withdraw");Native n{};auto i=intent();held(n,i);auto& h=n.hand[0];
     h.refreshRefused=true;advance(n);publish(n,i);g::PhysicsRefresh(&n,n.now);
     CK(!h.proxyTarget&&h.withdrawals==1);g::PhysicsStep(&n,n.now);CK(h.removes==1);}
    // Quarantined native resources retain references, never their old drive.
    {reset("quarantine_withdraw");Native n{};auto i=intent();held(n,i);auto& h=n.hand[0];
     h.driveUncertain=true;frame(n,i);CK(status().state==g::State::Quarantined&&h.refs==2);
     advance(n);publish(n,i);g::PhysicsRefresh(&n,n.now);CK(!h.proxyTarget);
     g::PhysicsStep(&n,n.now);CK(h.removes==1&&h.refs==2);finish(n);CK(h.refs==0);}
    // F9 OFF still withdraws a target and services Remove/Absent/retirement.
    {reset("disabled_still_retires");Native n{};auto i=intent();held(n,i);auto& h=n.hand[0];
     g::SetEnabled(false);advance(n);g::PhysicsRefresh(&n,n.now);CK(!h.proxyTarget);
     g::PhysicsStep(&n,n.now);CK(h.removes==1&&h.refs==2);
     h.pendingRetire=true;finish(n);CK(h.refs==2&&status().state==g::State::Retiring);
     h.pendingRetire=false;finish(n);CK(h.refs==0&&status().state==g::State::Idle);}
    // A proxy issued during incomplete prepare has no published generation yet.
    // Its target still needs withdrawing; no native constraint exists to remove.
    {reset("partial_prepare_withdraw");Native n{};auto i=intent();auto& h=n.hand[0];h.pendingPrepare=true;
     frame(n,i);CK(h.proxyTarget&&h.refs==2&&status().owner.proxyGeneration==0);
     context[0]=false;advance(n);g::PhysicsRefresh(&n,n.now);CK(!h.proxyTarget&&h.withdrawals==1);
     g::PhysicsStep(&n,n.now);CK(h.refs==0&&h.adds==0&&h.removes==0);}
    // Losing one hand's ownership must not revoke the other source.
    {reset("hand_local_ownership");Native n{};auto a=intent(0),b=intent(1);
     for(int k=0;k<2;++k){advance(n);publish(n,a);publish(n,b);g::PhysicsRefresh(&n,n.now);g::PhysicsStep(&n,n.now);}
     CK(status(0).state==g::State::Held&&status(1).state==g::State::Held);
     context[0]=false;advance(n);publish(n,a);publish(n,b);g::PhysicsRefresh(&n,n.now);
     CK(!n.hand[0].proxyTarget&&n.hand[1].proxyTarget&&n.hand[1].withdrawals==0);
     g::PhysicsStep(&n,n.now);CK(n.hand[0].removes==1&&n.hand[1].removes==0&&status(1).state==g::State::Held);}
    std::printf("grab_context_d41: %d checks, %d failures; production lifecycle, native leaves substituted\n",checks,failures);
    return failures?1:0;
}
