#pragma once
#include <array>
#include <cstdint>
#include <cmath>
#include <algorithm>
#include <cstring>
#include "finger_surface.h"

namespace kcd2vr::finger_contact_h31 {
// Presentation only. These are ORIGINAL source clocks, never a refreshed lease.
struct Source {std::uint64_t actor=0,epoch=0,body=0,request=0,sample=0,tick=0;bool enabled=false;};
struct Digit {double gapWorld=0,curl=0;};
struct State {Source source{};std::array<Digit,5> digit{};std::array<std::uint64_t,5> digitSurfaces{};std::array<pose_core::Vec3,7> hardOffsets{};std::uint64_t curve=0,surfaces=0;double scale=0;bool have=false;};
inline bool sameOwner(const Source&a,const Source&b) noexcept {
 return a.actor==b.actor&&a.epoch==b.epoch&&a.body==b.body&&a.request==b.request;
}
inline bool current(const Source&s,std::uint64_t now) noexcept {
 return s.enabled&&s.actor&&s.epoch&&s.body&&s.request&&s.sample&&s.tick&&now>=s.tick&&now-s.tick<=80;
}
inline void hash(std::uint64_t& h,std::uint64_t x) noexcept {h=(h^x)*1099511628211ull;}
inline void scalar(std::uint64_t&h,double x) noexcept {std::uint64_t bits=0;std::memcpy(&bits,&x,sizeof(bits));hash(h,bits);}
inline std::uint64_t curveKey(const pose_core::FingerCurve103 (&curves)[5]) noexcept {
 std::uint64_t h=1469598103934665603ull;
 for(const auto&c:curves){hash(h,c.count);for(unsigned i=0;i<c.count&&i<6;++i){
  for(auto q:{c.open[i],c.closed[i]}){scalar(h,q.x);scalar(h,q.y);scalar(h,q.z);scalar(h,q.w);}
  scalar(h,c.offset[i].x);scalar(h,c.offset[i].y);scalar(h,c.offset[i].z);}
  scalar(h,c.rootClosedShiftRatioH31.x);scalar(h,c.rootClosedShiftRatioH31.y);scalar(h,c.rootClosedShiftRatioH31.z);}
 return h;
}
// Current geometry only, and only surfaces measured for this digit. A
// neighbouring fingertip or palm ray is not evidence about this finger.
template<class Plane> inline double digitClearance(const pose_core::FingerCurve103& curve,
    double curl,pose_core::Quat orientation,pose_core::Vec3 position,
    const Plane* planes,int count,int digit,double skin) noexcept {
    double least=1e6;bool measured=false;
    for(int p=0;p<count;++p){const auto& plane=planes[p];if(plane.feature!=digit+1)continue;
        const auto fit=pose_core::finger_clearance103(curve,curl,orientation,position,
            {{plane.point.x,plane.point.y,plane.point.z},{plane.normal.x,plane.normal.y,plane.normal.z}},skin);
        if(!fit.valid)return -1e6;
        measured=true;least=std::min(least,fit.clearance);
    }
    return measured?least:-1e6;
}
// A bounded extra AIR GAP, never smoothing the mandatory nonpenetration fit.
// Real source dt only; no synthetic integration on repeated eyes/held queries.
inline double advance(double oldGap,double pressureWorld,double dt) noexcept {
 if(!std::isfinite(oldGap)||!std::isfinite(pressureWorld)||!std::isfinite(dt)||dt<=0||dt>.080)return 0;
 const double goal=std::clamp(pressureWorld*.5,0.,.004); //4mm WORLD, independent of model scale
 const double rate=goal>oldGap?16.:8.;
 return std::clamp(oldGap+(goal-oldGap)*(1.-std::exp(-rate*dt)),0.,.004);
}
}
