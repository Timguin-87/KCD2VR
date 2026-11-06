#include "../src/vrik/body_contact164.h"
#include "../src/vrik/belt_layout164.h"
#include "../src/diagnostics/combat_trace164.h"
#include <cassert>
#include <limits>
#include <cstdio>
using namespace kcd2vr;
int main(){
    using namespace pose_core;
    const Vec3 points[2]{{0,0,0},{0,0,.06}};
    body_contact164::Capsule torso{{0,0,.8},{0,0,1.3},.12};
    auto r=body_contact164::project({.08,0,1},points,2,&torso,1,{1,0,0});
    assert(r.valid&&r.contacts==1&&r.wrist.x>=.12799&&r.correction<.05);
    const auto stable=body_contact164::project(r.wrist,points,2,&torso,1,{1,0,0});
    assert(stable.valid&&length(stable.wrist-r.wrist)<1e-8);
    r=body_contact164::project({.4,0,1},points,2,&torso,1,{1,0,0});
    assert(r.valid&&r.contacts==0&&r.correction==0);
    r=body_contact164::project({0,0,1},points,2,&torso,1,{-1,0,0});
    assert(r.valid&&r.wrist.x<-.1279); // deterministic centre/left fallback
    torso.radius=-1;assert(!body_contact164::project({},points,2,&torso,1,{1,0,0}).valid);
    torso.radius=.12;
    auto invalid=Vec3{std::numeric_limits<double>::quiet_NaN(),0,0};
    assert(!body_contact164::project(invalid,points,2,&torso,1,{1,0,0}).valid);
    body_contact164::Capsule palm{{.4,0,1},{.4,0,1},.035};
    r=body_contact164::project({.42,0,1},points,2,&palm,1,{1,0,0});
    assert(r.valid&&r.wrist.x>=.44299);
    // Caller can reject unresolved opposite constraints; no distant hand launch.
    body_contact164::Capsule jam[2]{{{-.02,0,0},{-.02,0,0},.3},{{.02,0,0},{.02,0,0},.3}};
    r=body_contact164::project({},points,1,jam,2,{1,0,0});
    assert(!r.valid&&length(r.wrist)==0);

    physical::Input in{};in.shoulders[0]={.2,0,1.4};in.shoulders[1]={-.2,0,1.4};
    physical::Profile p{};p.classId=3;physical::Pose original{{},{-.23,.05,.9}},moved{};
    assert(belt_layout164::right_belt(in,p,original,moved));
    assert(std::abs(moved.p.x-.23)<1e-8&&moved.p.y==original.p.y&&moved.p.z==original.p.z&&angle(moved.q,original.q)==0);
    auto right=moved;assert(belt_layout164::right_belt(in,p,right,moved)&&length(moved.p-right.p)<1e-8);
    p.classId=4;assert(!belt_layout164::right_belt(in,p,original,moved));
    p.classId=5;p.twoHanded=true;assert(!belt_layout164::right_belt(in,p,original,moved));
    p.twoHanded=false;original.p.z=1.4;assert(!belt_layout164::right_belt(in,p,original,moved));
    p.legacyHolsters147=true;p.classId=9;
    assert(physical::holster_zone126(p)==physical::HolsterZone126::RightShoulderBowR12&&physical::holster_hand126(p)==1);
    p.classId=8;assert(physical::holster_zone126(p)==physical::HolsterZone126::LeftShoulder&&physical::holster_hand126(p)==1); // R14B: shield LEFT shoulder, left hand

    namespace t=diagnostics::trace164;
    // R168R1: causal observation also requires an explicit recording lease;
    // stopping recording must not require changing physical weapon rung 7.
    namespace cap=diagnostics::capture168;
    cap::gate.select(cap::Domain::Combat,false,100);
    t::until=500;assert(!t::active(400,7));
    cap::gate.select(cap::Domain::Combat,true,100);
    assert(t::active(400,7)&&!t::active(501,7)&&!t::active(400,6));
    cap::gate.select(cap::Domain::Combat,false,401);
    assert(!t::active(402,7));
    t::Context a{};a.prediction=12;a.collisions=100;a.redirected=true;
    {t::Scope outer(a);assert(t::for_owner(100).prediction==12&&t::for_owner(101).prediction==0);
      {t::Scope foreign;assert(t::for_owner(100).prediction==0);}
      assert(t::for_owner(100).prediction==12);}
    assert(!t::current.prediction);
    std::puts("PASS contact164: bounded proxies, belt geometry, causal scope isolation");
}
