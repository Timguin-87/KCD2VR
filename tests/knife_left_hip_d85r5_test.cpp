#include "vrik/physical_weapon.h"
#include <cassert>
#include <cstdio>
namespace p=kcd2vr::physical;namespace m=kcd2vr::pose_core;
int main(){for(double yaw:{0.,.7,1.6,-2.4}){p::Input in{};
auto right=m::Vec3{std::cos(yaw),std::sin(yaw),0};auto root=m::Vec3{4,2,1.7};
in.shoulders[0]=root+right*.2;in.shoulders[1]=root-right*.2;
in.playerShouldersKnownD85=true;in.playerShouldersD85[0]=in.shoulders[0];in.playerShouldersD85[1]=in.shoulders[1];
auto hip=p::knife_left_hip_d85r5(in);auto a=p::body_axes(in);
assert(p::knife_left_hip_holds_d85r5(in,hip));assert(p::knife_left_hip_holds_d85r5(in,hip+a.right*.15));
assert(!p::knife_left_hip_holds_d85r5(in,hip+a.right*.17));
assert(!p::knife_left_hip_holds_d85r5(in,hip+a.right*.46));
assert(!p::knife_left_hip_holds_d85r5(in,p::chest_knife_pouch_k32(in)));
in.shoulders[1].z+=2;assert(m::length(p::knife_left_hip_d85r5(in)-hip)<1e-8); // native arm height cannot move the player's anchor
in.shoulders[1].z-=2;
// R109: the chest pouch (where the VrChestDagger visual sits) is a second draw zone, switchable; the hip always stays
auto chest=p::chest_knife_pouch_k32(in);
kcd2vr::knife_pouch_d66::setChestZone(true);
assert(p::knife_pouch_holds_r109(in,hip)&&p::knife_pouch_holds_r109(in,chest));
assert(p::knife_pouch_holds_r109(in,chest+a.right*.15)&&!p::knife_pouch_holds_r109(in,chest+a.right*.17));
assert(m::length(p::knife_pouch_centre_r109(in,chest+a.up*.05)-chest)<1e-8&&m::length(p::knife_pouch_centre_r109(in,hip+a.up*.05)-hip)<1e-8);
kcd2vr::knife_pouch_d66::setChestZone(false);
assert(p::knife_pouch_holds_r109(in,hip)&&!p::knife_pouch_holds_r109(in,chest));
assert(m::length(p::knife_pouch_centre_r109(in,chest)-hip)<1e-8);
kcd2vr::knife_pouch_d66::setChestZone(true);
}std::puts("PASS left hip zone: yaw, radius, chest/right exclusion, player anchor; R109 chest pouch second zone + switch");}
