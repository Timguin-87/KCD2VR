#pragma once
// W5 contact expiry glide (Claude, 25 Oct 2026). Codex join 10:32:11.639: the free-hand contact hold expired (prior
// complete query 78 -> 94 ms old) and the wrist jumped 75 mm while the controller moved 0.8 mm. the query had starved:
// budgetSkips 7351 -> 12154 over the session, maxQueryUs 13-20 ms (one >8 ms query backs ALL contact rays off for
// 0.5-4 s, world_query_contract.h). this is the "precise contacts flicker": hold 80 ms, then drop in one frame.
// glide: an expired hold walks back to the current request at <= stepM per call, then the old reset runs.
// no plane, no surface and no deadline is kept or extended; it never pushes AWAY from the request.
#include <algorithm>
#include <cmath>
#include <atomic>
namespace kcd2vr::contact_glide_w5 {
inline constexpr float kStepM=.012f,kDoneM=.002f,kMaxM=.5f;
// R8b (Codex review): time based, not per call. 1.0 m/s, dt clamped to 50 ms; a repeated pose callback inside the same
// contact pass (dtMs==0) keeps the current position. so the release takes the same time at 45, 90 or 180 calls/s.
inline constexpr float kSpeedMps=1.0f,kMaxDtMs=50.f;
inline float stepFor(float dtMs) noexcept {if(!std::isfinite(dtMs)||dtMs<=0)return 0.f;return kSpeedMps*std::min(dtMs,kMaxDtMs)*.001f;}
// V needs x,y,z floats. scale = model world scale. returns true when the glide applies (moved, or held this pass).
template<class V> inline bool step(V& allowed,const V& goal,float scale,float dtMs=1000.f/90.f) noexcept {
    if(!std::isfinite(scale)||scale<=0)return false;
    const float dx=goal.x-allowed.x,dy=goal.y-allowed.y,dz=goal.z-allowed.z;
    const float l=std::sqrt(dx*dx+dy*dy+dz*dz);
    if(!std::isfinite(l)||l<=kDoneM*scale||l>=kMaxM*scale)return false;
    const float s=std::min(l,stepFor(dtMs)*scale)/l;
    allowed.x+=dx*s;allowed.y+=dy*s;allowed.z+=dz*s;return true;
}
// J26 (F2 rung 2/3): R8d2 F7 - free hands with NO live plane still sat 1-40 cm off the request (left 31% of free
// frames) and came back in exact 16 mm steps (1 m/s per 16 ms contact pass). when nothing holds the hand (no plane,
// no hit) the offset is not contact, it is a leftover. it now decays with tau 25 ms by real time: ~95% gone in 75 ms.
// a live plane or a ray hit keeps the old 80 ms hold; once its glide has dropped the plane the rest goes this way too.
// never pushes away from the request, never adds a plane.
inline constexpr float kFastTauMs=25.f;
inline std::atomic<unsigned long long> fastReleases{0};
inline constexpr bool fastRelease(bool enabled,int planeCount,bool blocked) noexcept {return enabled&&planeCount<=0&&!blocked;}
template<class V> inline bool stepFast(V& allowed,const V& goal,float scale,float dtMs) noexcept {
    if(!std::isfinite(scale)||scale<=0)return false;
    const float dx=goal.x-allowed.x,dy=goal.y-allowed.y,dz=goal.z-allowed.z;
    const float l=std::sqrt(dx*dx+dy*dy+dz*dz);
    if(!std::isfinite(l)||l<=kDoneM*scale||l>=kMaxM*scale)return false;
    if(!std::isfinite(dtMs)||dtMs<=0)return true; // same contact pass: keep the current point
    const float k=1.f-std::exp(-std::min(dtMs,kMaxDtMs)/kFastTauMs);
    allowed.x+=dx*k;allowed.y+=dy*k;allowed.z+=dz*k;fastReleases.fetch_add(1,std::memory_order_relaxed);return true;
}
}
