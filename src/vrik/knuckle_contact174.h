#pragma once
#include "finger_surface.h"
namespace kcd2vr::knuckle174 {
// One extra measured knuckle-bar point. The four proximal phalange endpoints
// come from the same captured finger curves used for visible fingers. This
// covers the forward knuckles of a closed fist, which fingertip-only rays miss.
// It remains a sparse proxy, not a claim of full convex/rigid finger collision.
inline std::optional<pose_core::Vec3> offset(const pose_core::FingerCurve103* curves,
                                           const float* curl,pose_core::Quat wrist) noexcept {
    namespace m=pose_core;const auto w=m::unit(wrist);if(!curves||!curl||!w)return {};
    m::Vec3 center{};
    for(int f=1;f<5;++f){const auto& c=curves[f];
        if(c.count<3||c.count>6||!std::isfinite(curl[f]))return {};
        auto q=*w;m::Vec3 p{};
        for(unsigned k=0;k<2;++k){
            const auto offset=m::finger_offset103(c,k,curl[f]);if(!m::finite(offset))return {};
            p=p+m::rotate(q,offset);
            const auto joint=m::slerp(c.open[k],c.closed[k],std::clamp(double(curl[f]),0.0,1.0));
            if(!joint)return {};q=m::multiply(q,*joint);
        }
        center=center+p*.25;
    }
    return m::finite(center)&&m::length(center)<.35?std::optional<m::Vec3>(center):std::nullopt;
}
}
