#pragma once
#include "../vrik/pose_math.h"
#include <array>
#include <cmath>

namespace kcd2vr::door_fingers66 {
namespace m=pose_core;
inline constexpr unsigned capacity=32;
// Slots are homologous (finger, joint depth), not raw skeleton bone IDs. The
// runtime adapter must build each side's layout from its validated finger map.
struct Layout {std::array<unsigned,5> joints{};};
inline unsigned count(const Layout& l) noexcept {
    unsigned n=0;for(const auto chain:l.joints){if(!chain||chain>6)return 0;n+=chain;}
    return n<=capacity?n:0;
}
inline bool same(const Layout& a,const Layout& b) noexcept {return a.joints==b.joints&&count(a);}
inline bool rotation(m::Quat q) noexcept {
    const double n=q.x*q.x+q.y*q.y+q.z*q.z+q.w*q.w;
    return std::isfinite(n)&&n>=.98&&n<=1.02;
}
// Inert anatomical/native data only. None of these records grants a Door,
// tracking, wrist, action or animation-clock owner. Live permission stays with
// the exact accepted Door lease and qualified moving-handle witness.
struct References {
    Layout layout{};std::array<m::Quat,capacity> open{},closed{};
    std::array<m::Vec3,capacity> rest{};bool ready=false;
};
struct NativeShape {Layout layout{};std::array<m::Quat,capacity> local{};int side=-1;};
struct Retargeted {std::array<m::LocalPose,capacity> local{};unsigned count=0;bool exact=false;};

inline std::optional<double> curl(m::Quat open,m::Quat closed,m::Quat native) noexcept {
    if(!rotation(open)||!rotation(closed)||!rotation(native))return std::nullopt;
    const auto o=m::unit(open),c=m::unit(closed),n=m::unit(native);
    if(!o||!c||!n)return std::nullopt;
    const auto axis=m::rotation_vector(m::multiply(*c,m::conjugate(*o)));
    const auto value=m::rotation_vector(m::multiply(*n,m::conjugate(*o)));
    const double denominator=m::dot(axis,axis);
    if(!m::finite(axis)||!m::finite(value)||!std::isfinite(denominator))return std::nullopt;
    // A constant authored joint has no measurable curl coordinate. It can
    // represent OPEN, but may not hide an unrelated native rotation.
    if(denominator<1e-10)return m::dot(value,value)<=1e-8?std::optional<double>{0.}:std::nullopt;
    const double projected=m::dot(value,axis)/denominator;
    if(!std::isfinite(projected))return std::nullopt;
    return std::clamp(projected,0.,1.);
}

inline bool retarget(const NativeShape& source,int destinationSide,const References& from,
                     const References& to,Retargeted& result) noexcept {
    result={};const unsigned n=count(source.layout);
    if(source.side<0||source.side>1||destinationSide<0||destinationSide>1||!n||!to.ready||
       !same(source.layout,to.layout))return false;
    const bool exact=source.side==destinationSide;
    if(!exact&&(!from.ready||!same(source.layout,from.layout)))return false;
    Retargeted pending{};pending.count=n;pending.exact=exact;
    for(unsigned i=0;i<n;++i){
        if(!rotation(source.local[i])||!m::finite(to.rest[i]))return false;
        m::Quat q=source.local[i];
        if(!exact){
            const auto coordinate=curl(from.open[i],from.closed[i],source.local[i]);
            if(!coordinate||!rotation(to.open[i])||!rotation(to.closed[i]))return false;
            const auto mapped=m::slerp(to.open[i],to.closed[i],*coordinate);if(!mapped)return false;q=*mapped;
        }
        pending.local[i]={q,to.rest[i]}; // Never copy source lengths or mirror axes.
    }
    result=pending;return true;
}
}
