#pragma once
#include "pose_math.h"

namespace kcd2vr::body_contact164 {
using namespace pose_core;
struct Capsule { Vec3 a{},b{}; double radius=0; };
struct Result { Vec3 wrist{}; double correction=0; unsigned contacts=0; bool valid=false; };
inline Vec3 nearest(Vec3 p,const Capsule& c) noexcept {
    const auto d=c.b-c.a;const double d2=dot(d,d);
    return c.a+d*(d2>1e-12?std::clamp(dot(p-c.a,d)/d2,0.0,1.0):0.0);
}
// Bounded visual proxy, not a rigid body, mesh collision or native hit producer.
// The caller runs its existing world sweep AFTER this projection. No skeleton
// offsets, controller orientation or finger endpoints are changed here.
inline Result project(Vec3 wrist,const Vec3* offsets,unsigned count,const Capsule* obstacles,
                      unsigned n,Vec3 fallback,double skin=.008) noexcept {
    Result r{};r.wrist=wrist;
    if(!finite(wrist)||!offsets||!count||count>6||!obstacles||n>2||!unit(fallback)||!std::isfinite(skin)||skin<0)return r;
    for(unsigned i=0;i<count;++i)if(!finite(offsets[i]))return r;
    for(unsigned j=0;j<n;++j)if(!finite(obstacles[j].a)||!finite(obstacles[j].b)||
        !std::isfinite(obstacles[j].radius)||obstacles[j].radius<=0||obstacles[j].radius>.3)return r;
    for(unsigned pass=0;pass<4;++pass)for(unsigned j=0;j<n;++j)for(unsigned i=0;i<count;++i){
        const auto p=r.wrist+offsets[i];const auto delta=p-nearest(p,obstacles[j]);
        const double penetration=obstacles[j].radius+skin-length(delta);
        if(penetration>0){const auto normal=unit(delta).value_or(*unit(fallback));
            r.wrist=r.wrist+normal*penetration;r.contacts|=1u<<j;}
    }
    r.correction=length(r.wrist-wrist);
    // A badly calibrated body must not throw the hands. Report an unresolved
    // proxy instead of inventing a distant endpoint or stretching an arm.
    if(!finite(r.wrist)||r.correction>.20){r.wrist=wrist;return r;}
    for(unsigned j=0;j<n;++j)for(unsigned i=0;i<count;++i)
        if(length(r.wrist+offsets[i]-nearest(r.wrist+offsets[i],obstacles[j]))+1e-5<obstacles[j].radius+skin){r.wrist=wrist;return r;}
    r.valid=true;return r;
}
}
