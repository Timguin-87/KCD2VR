#pragma once
#include "physical_weapon.h"
namespace kcd2vr::belt_layout164 {
// Presentation-only alternative for a real, stored one-hand axe/mace. Native
// equip columns and draw/stow requests are untouched. Keep its native vertical
// height, fore/aft position and orientation; shift the attachment to the right.
inline bool eligible(int cls,bool twoHanded) noexcept {return (cls==3||cls==5)&&!twoHanded;}
inline bool right_belt(const physical::Input& in,const physical::Profile& p,
                       physical::Pose native,physical::Pose& out) noexcept {
    out=native;
    if(!eligible(p.classId,p.twoHanded)||!physical::finite(native))return false;
    const auto axes=physical::body_axes(in);
    const auto center=physical::body_center_r38(in);
    const auto delta=native.p-center;
    const double z=pose_core::dot(delta,axes.up),y=pose_core::dot(delta,axes.forward);
    if(!pose_core::finite(center)||z>-.15||z<-.85||std::abs(y)>.4)return false;
    const double x=pose_core::dot(delta,axes.right);
    if(!std::isfinite(x)||std::abs(x)>.5)return false;
    const double right=std::clamp(std::abs(x),.18,.32);
    out.p=native.p+axes.right*(right-x);
    return physical::finite(out);
}
}
