#pragma once
#include <algorithm>
#include <cmath>
#include <cstdint>
namespace kcd2vr::contact_release_d45 {
// Release only a measured contact correction, never filter ordinary tracking.
// This candidate is selected BEFORE the existing mandatory geometry queries;
// it is not an extra visual result applied after collision. A withdrawing
// controller cancels the corresponding correction immediately. Tangential
// movement remains current; only spring-back from a removed surface is bounded.
inline constexpr float kReleaseMps=1.0f, kDoneM=.002f, kMaxCorrectionM=.5f;
template<class V> inline V candidate(V raw,V previousRaw,V correction,float scale,
                                    std::uint64_t tick,std::uint64_t previousTick,
                                     std::uint64_t timeNs=0,std::uint64_t previousNs=0) noexcept {
    if(!std::isfinite(scale)||scale<=0||!previousTick||tick<previousTick||tick-previousTick>80)return raw;
    const float length=std::sqrt(correction.x*correction.x+correction.y*correction.y+correction.z*correction.z);
    if(!std::isfinite(length)||length<=kDoneM*scale||length>=kMaxCorrectionM*scale)return raw;
    const float withdrawal=std::max(0.f,((raw.x-previousRaw.x)*correction.x+
        (raw.y-previousRaw.y)*correction.y+(raw.z-previousRaw.z)*correction.z)/length);
    // Only the existing short contact lifetime may carry a release offset.
    // After 80 ms the caller must query the current request without it.
    // High-resolution sample time removes the0/16ms staircase caused by
    // GetTickCount64. This clock affects ONLY a measured contact return.
    if(timeNs&&previousNs&&(timeNs<previousNs||timeNs-previousNs>80000000ull))return raw;
    const float seconds=timeNs&&previousNs?
        float(std::min<double>(double(timeNs-previousNs)*1.e-9,.020)):
        std::min<float>(float(tick-previousTick),20.f)*.001f;
    // Current feature queries now include their positive clearance. A
    // minimum16mm release step is no longer necessary to rediscover the wall.
    const float springTravel=kReleaseMps*scale*seconds;
    const float remaining=std::max(0.f,length-withdrawal-springTravel);
    const float factor=remaining/length;
    return {raw.x+correction.x*factor,raw.y+correction.y*factor,raw.z+correction.z*factor};
}
}
