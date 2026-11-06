#pragma once
#include "finger_surface.h"
#include <array>

namespace kcd2vr::hand_shape59c7f {
using namespace pose_core;
struct Segment {Vec3 a{},b{};double radius=0;};
struct Shape {std::array<Segment,26> part{};unsigned count=0;bool valid=false;};
// Geometry follows the captured native finger chains at their current curl.
// Thickness is a bounded visual approximation; this is not a native rigid body.
inline Shape make(const FingerCurve103 (&curves)[5],const float (&curl)[5],Quat orientation) noexcept {
    Shape s{};const auto q=unit(orientation);if(!q)return s;
    Vec3 bases[5]{};
    for(unsigned f=0;f<5;++f){
        const auto& c=curves[f];if(c.count<2||c.count>6||!std::isfinite(curl[f]))return {};
        auto rotation=*q;Vec3 p{};
        for(unsigned k=0;k<c.count;++k){
            const auto offset=finger_offset103(c,k,curl[f]);if(!finite(offset))return {};
            const auto next=p+rotate(rotation,offset);
            const auto joint=slerp(c.open[k],c.closed[k],std::clamp(double(curl[f]),0.,1.));
            if(!joint||length(next)>.35)return {};
            if(k){const auto len=length(next-p);if(len>1e-5)s.part[s.count++]={p,next,std::clamp(len*.22,.004,.009)};}
            else bases[f]=next;
            p=next;rotation=multiply(rotation,*joint);
        }
    }
    // One transverse palm capsule at the metacarpals, not a 7 cm sphere.
    s.part[s.count++]={bases[1],bases[4],.014};s.valid=true;return s;
}
struct Closest {Vec3 a{},b{};};
inline Closest closest(Vec3 p,Vec3 q,Vec3 r,Vec3 s) noexcept {
    const auto d1=q-p,d2=s-r,delta=p-r;
    const double a=dot(d1,d1),e=dot(d2,d2),f=dot(d2,delta);
    double u=0,v=0;
    if(a<=1e-12&&e<=1e-12)return {p,r};
    if(a<=1e-12)v=std::clamp(f/e,0.,1.);
    else {
        const double c=dot(d1,delta);
        if(e<=1e-12)u=std::clamp(-c/a,0.,1.);
        else {const double b=dot(d1,d2),den=a*e-b*b;
            if(den>1e-12)u=std::clamp((b*f-c*e)/den,0.,1.);
            v=(b*u+f)/e;
            if(v<0){v=0;u=std::clamp(-c/a,0.,1.);}
            else if(v>1){v=1;u=std::clamp((b-c)/a,0.,1.);}
        }
    }
    return {p+d1*u,r+d2*v};
}
struct Result {Vec3 correction[2]{};bool contact=false,valid=false;};
inline Result separate(const Shape& a,const Shape& b,Vec3 right,Vec3 left,
                       std::optional<Vec3> contactNormal=std::nullopt) noexcept {
    Result out{};
    if(!a.valid||!b.valid||!a.count||!b.count||a.count>26||b.count>26||!finite(right)||!finite(left))return out;
    if(length(right-left)>.70){out.valid=true;return out;}
    // Symmetric Jacobi steps: neither hand becomes a fixed obstacle or gains
    // priority from loop order. Two passes, at most 1352 pair checks; no rays.
    const auto fallback=unit(right-left).value_or(Vec3{1,0,0});
    if(contactNormal)contactNormal=unit(*contactNormal);
    for(unsigned pass=0;pass<2;++pass){Vec3 correction{};double deepest=0;
        for(unsigned i=0;i<a.count;++i)for(unsigned j=0;j<b.count;++j){
            const auto& x=a.part[i];const auto& y=b.part[j];
            const auto c=closest(right+out.correction[0]+x.a,right+out.correction[0]+x.b,
                                 left+out.correction[1]+y.a,left+out.correction[1]+y.b);
            const auto delta=c.a-c.b;const double radius=x.radius+y.radius+.002;
            const double overlap=radius-length(delta);
            if(overlap>deepest){
                auto normal=unit(delta).value_or(contactNormal.value_or(fallback));
                // D39: a current contact episode keeps its separating side.
                // Near a capsule intersection, the closest pair/normal can
                // reverse under sub-millimetre motion. Signed separation along
                // the admitted hemisphere avoids ejecting the wrists to the
                // other side; the exact overlap still selects the manifold.
                if(contactNormal&&dot(normal,*contactNormal)<0)normal=normal*-1.;
                deepest=overlap;
                correction=normal*((contactNormal?radius-dot(delta,normal):overlap)*.5);
            }
        }
        if(deepest<=0)break;
        out.contact=true;out.correction[0]=out.correction[0]+correction;out.correction[1]=out.correction[1]-correction;
        // Only a caller with a qualified temporal witness requests continuity.
        // Other callers retain the original stateless function exactly.
    }
    out.valid=finite(out.correction[0])&&length(out.correction[0])<=.06;
    if(!out.valid)out.correction[0]=out.correction[1]={};
    return out;
}
}
