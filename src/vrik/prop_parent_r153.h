#pragma once
// R153 (Miki 8 Nov, R152 run: "instant pickups - the item disappears"). the run had ONE instant world hold
// (11:18:47.037 INSTANT_NATIVE_ACCEPTED_WORLD_HOLD, item 524663). F5_OBJECT_WORLD read the item at
// (2994.34,676.38,52.51) with an exact identity rotation at 11:18:47.050, then the R96 kinematic carry placed it
// (11:18:47.289 KINEMATIC_CARRY_R96) and from 11:18:47.556 on it sat at (5988.68,1352.78,105.08) = two times the
// world position, ~3 km away, for good (28 equal reads, it never fell). CEntity::SetPosRotScale (+509810) sets the
// LOCAL transform. after the native immediate pickup the item hangs on a parent that stands where the item was, so
// "local = the wanted world pose" doubled it. same thing explains R94R1/R96 "the body never moves" (deltaPosition=0).
// fix: read the world pose back after the set. if it is not where we asked, learn the parent from that readback
// (parent = world * inverse(local)), set local = inverse(parent) * wanted, check again. an entity without a parent
// reads back right the first time = nothing changes for it. pure math here (host test prop_parent_r153_test.cpp).
#include "pose_math.h"
namespace kcd2vr::prop_parent_r153 {
namespace pc=kcd2vr::pose_core;
inline constexpr double kPositionM=.02;   // readback within 2 cm = placed where asked
inline constexpr double kRotationDot=.9986; // and within ~6 deg (|q1.q2| = cos(angle/2))
template<class P> inline P Compose(const P& a,const P& b){P r=a;r.q=pc::multiply(a.q,b.q);r.p=a.p+pc::rotate(a.q,b.p);return r;}
template<class P> inline P Inverse(const P& a){P r=a;r.q=pc::conjugate(a.q);r.p=pc::rotate(r.q,a.p*-1.);return r;}
template<class P> inline bool Matches(const P& world,const P& wanted){
    const double d=std::fabs(world.q.x*wanted.q.x+world.q.y*wanted.q.y+world.q.z*wanted.q.z+world.q.w*wanted.q.w);
    return std::isfinite(d)&&pc::finite(world.p)&&pc::length(world.p-wanted.p)<=kPositionM&&d>=kRotationDot;
}
// what was set (local) and what the world reads back -> the parent the entity hangs on
template<class P> inline P Parent(const P& world,const P& local){return Compose(world,Inverse(local));}
// the local transform that puts the entity at the wanted world pose under that parent
template<class P> inline P LocalFor(const P& parent,const P& wanted){return Compose(Inverse(parent),wanted);}
}
