#pragma once
#include "physical_weapon.h"
namespace kcd2vr::native_transform166 {
namespace m=pose_core;
// Retail A7CEE0, A7CF9E..A7D12A stores conjugate(q), then
// -t + 2*cross(conjugate(q).xyz, cross(conjugate(q).xyz,-t)+q.w*(-t)).
// A q*v*q-conjugate round trip introduces norm^2 scaling that retail does not.
// Keep this boundary-specific: do not replace the global pose math subsystem.
inline m::Vec3 Rotate(m::Quat q,m::Vec3 v){
    const m::Vec3 xyz{q.x,q.y,q.z};return v+m::cross(xyz,m::cross(xyz,v)+v*q.w)*2;
}
struct Check {bool valid=false;double translationError=-1,angleError=-1,normError=-1,legacyClosure=-1;};
inline Check InversePair(physical::Pose world,physical::Pose inverse){
    Check r{};if(!physical::finite(world)||!physical::finite(inverse))return r;
    const auto q=m::conjugate(world.q);
    const auto norm=[](m::Quat v){return v.x*v.x+v.y*v.y+v.z*v.z+v.w*v.w;};
    r.normError=std::max(std::abs(norm(world.q)-1.0),std::abs(norm(inverse.q)-1.0));
    r.translationError=m::length(Rotate(q,world.p*-1)-inverse.p);
    r.angleError=m::angle(q,inverse.q);
    r.legacyClosure=m::length(physical::compose(world,inverse).p);
    r.valid=r.normError<=.0001&&r.translationError<=.002&&r.angleError<=.002;
    return r;
}
}
