#pragma once
// R11 lane K (Claude): what a free hand does when this frame's contact query is late, busy or starved.
// 25 Oct R8d2 log: a free hand with NO contact froze at its last queried world point for up to 80 ms while the
// controller moved (16:37:34.603-.618 R and L finalGoal identical to sample 60954, contactFresh=0), then an R8 W5
// glide started from the fresh NATIVE animated-hand seed: 25 cm drop, then a 1 m/s float back (16:37:34.660/.925).
// C7E (13 Oct, the smooth build) did neither: a not-ready query returned "no contact" and the hand followed the
// controller. H18 added the 80 ms world hold, R8 the glide; both apply to clear scenes too. now:
//  - last complete query was clear and no glide runs -> follow the request (C7E). the next complete sweep still
//    starts from the last verified point, so a wall entered meanwhile is caught there.
//  - a real contact (plane/hit) keeps the bounded 80 ms hold and then the glide, as before.
//  - nothing measured yet (seed from the animated hand, lastComplete 0) -> reset, never hold/glide from it.
#include <cstdint>
namespace kcd2vr::contact_hold_r11 {
enum class Hold:int {Reset=0,FollowRequest=1,HoldWorld=2,Glide=3};
inline constexpr std::uint64_t kHoldMs=80;
inline Hold decide(bool have,int planeCount,bool blocked,std::uint64_t glideTick,std::uint64_t lastComplete,std::uint64_t now) noexcept {
    if(!have||!lastComplete)return Hold::Reset;
    if(planeCount<=0&&!blocked&&!glideTick)return Hold::FollowRequest;
    if(now>=lastComplete&&now-lastComplete<=kHoldMs)return Hold::HoldWorld;
    return Hold::Glide;
}
}
