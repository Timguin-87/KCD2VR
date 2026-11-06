// Real Mod Use/prop producer; only copied XR/native status/pose leaves supplied.
#define main inherited_door_producer_d40
#include "door_producer46_test.cpp"
#undef main

int main() {
    using namespace kcd2vr::physics;
    hands59c7f::rung=0;action_time_owner59c11::grasp=true;
    // F9 OFF and missing committed poses must preserve unknown contextual Use.
    for(bool on:{false,true}) {
        collab_h21::setPhysicsMode(on?2:0);fixture_grab_d40::pose[0]=false;
        Producer p;candidate(1000,0,"BasicEntity",.08f);p.tick(1000,0);
        candidate(1020,0,"BasicEntity",.08f);p.tick(1020,1);
        candidate(1040,0,"BasicEntity",.08f);p.tick(1040,1);
        check(p.touch_bindings_.held&&p.touch_bindings_.risingHeldEdgesH31==1);
    }
    // A live close physical squeeze has one input owner; disabling F9 while
    // held cannot synthesize a native contextual rising edge or release tap.
    for(bool nativePickup:{false,true}) {
        collab_h21::setPhysicsMode(2);fixture_grab_d40::pose[0]=true;
        action_time_owner59c11::grasp=nativePickup;
        Producer p;candidate(2000,0,"BasicEntity",.08f);p.tick(2000,0);
        candidate(2020,0,"BasicEntity",.08f);p.tick(2020,1);
        candidate(2040,0,"BasicEntity",.08f);p.tick(2040,1);
        grab_input60::GripEvidence e{};check(grab_input60::g_evidence[0].read(e)&&e.held&&e.entity==42);
        check(!p.touch_bindings_.held&&!p.touch_bindings_.requests);
        collab_h21::setPhysicsMode(0);p.forceGenericPulseH31=true;
        candidate(2060,0,"BasicEntity",.08f);p.tick(2060,1);
        check(!p.touch_bindings_.held&&!p.touch_bindings_.requests);
        check(grab_input60::g_evidence[0].read(e)&&!e.held&&!e.eligible);
        candidate(2080,0,"BasicEntity",.08f);p.tick(2080,0);
        check(!p.touch_bindings_.held&&!p.touch_bindings_.requests);
        p.forceGenericPulseH31=false;candidate(2100,0,"BasicEntity",.08f);p.tick(2100,1);
        candidate(2120,0,"BasicEntity",.08f);p.tick(2120,1);
        check(p.touch_bindings_.held&&p.touch_bindings_.risingHeldEdgesH31==1);
    }
    // Two already-qualified native constraints continue independently outside
    // a selection ray. This does not invent a second new-selection channel.
    collab_h21::setPhysicsMode(2);action_time_owner59c11::grasp=true;
    Producer p;p.retainOtherGripD40=true;p.xr.hands[1].grip=1;
    fixture_grab_d40::pose[0]=fixture_grab_d40::pose[1]=true;
    for(int side=0;side<2;++side) {
        auto& s=fixture_grab_d40::status[side];s={};s.state=grab_producer60::State::Held;
        s.tick=3000;s.owner.side=side;s.owner.entity=42+side;s.owner.press=2900+side;
        fixture_grab_d40::statusReady[side]=true;
    }
    interaction_alignment::candidate={};p.tick(3000,1);
    for(int side=0;side<2;++side) {
        grab_input60::GripEvidence e{};check(grab_input60::g_evidence[side].read(e));
        check(e.held&&e.eligible&&e.entity==unsigned(42+side)&&e.press==unsigned(2900+side));
    }
    for(auto& s:fixture_grab_d40::status)s.tick=3020;
    p.tick(3020,0);
    grab_input60::GripEvidence r{},l{};
    check(grab_input60::g_evidence[0].read(r)&&!r.held&&r.eligible&&r.press==2900);
    check(grab_input60::g_evidence[1].read(l)&&l.held&&l.entity==43);
    std::printf("D40 actual Use/prop producer: %u checks PASS; both continuation sources, F9 OFF/context fallback and no toggle/release native Use; native game NOT executed\n",checks);
    return 0;
}
