#pragma once
#include "vrik/pose_math.h"
#include "held_map_pose_m1.h"
#include "map_geometry_d64.h"
namespace kcd2vr::paper_grip_d64 {
namespace m=pose_core;
struct Support {bool held=false;};
// Visible paper alone follows its edge grip. Henry's wrist/fingers are never
// repositioned to fit the utility, and secondary support never stretches it.
inline bool place(const float* right,const map_geometry_d64::Sheet& sheet,float scale,
                  const float* left,float grip,bool supportAllowed,Support& state,float* out) noexcept {
    if(!std::isfinite(scale)||scale<.40f||scale>1.5f||!held_map_pose_m1::Place(right,out))return false;
    m::Quat q{out[0],out[1],out[2],out[3]};
    const m::Vec3 rightPoint{right[4],right[5],right[6]};
    const m::Vec3 anchor{(sheet.width*.5-.012)*scale,0,(-sheet.height*.5+.022)*scale};
    const double span=(sheet.width-.024)*scale;
    m::Vec3 other{};bool validLeft=left&&supportAllowed&&std::isfinite(grip);
    if(validLeft){other={left[4],left[5],left[6]};validLeft=m::finite(other);}
    if(!validLeft||grip<.30f)state.held=false;
    if(validLeft){
        const auto expected=rightPoint+m::rotate(q,{-span,0,0});
        const auto distance=m::length(other-expected);
        if(!state.held&&grip>=.55f&&distance<.12)state.held=true;
        if(state.held){
            const auto a=m::unit(m::rotate(q,{-1,0,0})),b=m::unit(other-rightPoint);
            const auto reach=m::length(other-rightPoint);
            if(!a||!b||reach<.10||std::fabs(reach-span)>.23||m::dot(*a,*b)<-.7)state.held=false;
            else {
                const m::Vec3 c{a->y*b->z-a->z*b->y,a->z*b->x-a->x*b->z,a->x*b->y-a->y*b->x};
                const auto turn=m::unit(m::Quat{c.x,c.y,c.z,1+m::dot(*a,*b)});
                if(turn)q=m::multiply(*turn,q);
            }
        }
    }
    const auto p=rightPoint-m::rotate(q,anchor);
    out[0]=float(q.x);out[1]=float(q.y);out[2]=float(q.z);out[3]=float(q.w);
    out[4]=float(p.x);out[5]=float(p.y);out[6]=float(p.z);out[7]=scale;
    return m::finite(p);
}
}
