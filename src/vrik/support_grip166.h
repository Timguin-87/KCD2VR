#pragma once
#include "pose_math.h"
#include <algorithm>
#include <optional>
namespace kcd2vr::support_grip166 {
namespace m=pose_core;
// Longsword only: the off hand belongs behind the primary grip, not on the
// blade. This is a geometric admission rule, not a fabricated native socket.
inline bool OnHilt(m::Vec3 primary,m::Vec3 tip,m::Vec3 candidate){
    const auto axis=m::unit(tip-primary);if(!axis||!m::finite(candidate))return false;
    const auto d=candidate-primary;const double along=m::dot(d,*axis);
    return along<=-.025&&along>=-.22&&m::length(d-(*axis)*along)<=.10;
}
inline std::optional<m::Vec3> Fallback(m::Vec3 primary,m::Vec3 tip,const double* lo,const double* hi,double desired){
    const auto axis=m::unit(tip-primary);if(!axis)return {};
    const double p[]{primary.x,primary.y,primary.z},d[]{-axis->x,-axis->y,-axis->z};
    double available=std::clamp(desired,.06,.60); // V66.178R9: INI-driven second-slot distance
    for(int i=0;i<3;++i){
        if(!std::isfinite(lo[i])||!std::isfinite(hi[i])||lo[i]>hi[i]||p[i]<lo[i]-.005||p[i]>hi[i]+.005)return {};
        if(d[i]>1e-8)available=std::min(available,(hi[i]-p[i])/d[i]-.005);
        else if(d[i]<-1e-8)available=std::min(available,(lo[i]-p[i])/d[i]-.005);
    }
    // The clearance belongs to the model boundary, not to the requested
    // spacing. Subtracting it unconditionally made the 6 cm slider unusable.
    if(available<.06)return {};
    return primary-(*axis)*available;
}
}
