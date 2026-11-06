// Frozen D78 reference, not runtime. Original SHA256 f8841c4783ae016bcd1892ade85e040ece2ba058f00780707ddc5be96a5feeba
#pragma once
#include <algorithm>
#include <cmath>
#include <cstdint>
namespace kcd2vr::contact_release_d78_reference {
// Release only a measured contact correction, never filter ordinary tracking.
// This candidate is selected BEFORE the existing mandatory geometry queries;
// it is not an extra visual result applied after collision. A withdrawing
// controller cancels the corresponding correction immediately. Tangential
// movement remains current; only spring-back from a removed surface is bounded.
inline constexpr float kReleaseMps=1.0f, kDoneM=.002f, kMaxCorrectionM=.5f;
template<class V> inline V candidate(V raw,V previousRaw,V correction,float scale,
                                    std::uint64_t tick,std::uint64_t previousTick) noexcept {
    if(!std::isfinite(scale)||scale<=0||!previousTick||tick<previousTick||tick-previousTick>80)return raw;
    const float length=std::sqrt(correction.x*correction.x+correction.y*correction.y+correction.z*correction.z);
    if(!std::isfinite(length)||length<=kDoneM*scale||length>=kMaxCorrectionM*scale)return raw;
    const float withdrawal=std::max(0.f,((raw.x-previousRaw.x)*correction.x+
        (raw.y-previousRaw.y)*correction.y+(raw.z-previousRaw.z)*correction.z)/length);
    // Only the existing short contact lifetime may carry a release offset.
    // After 80 ms the caller must query the current request without it.
    const float seconds=std::min<float>(float(tick-previousTick),20.f)*.001f;
    // The checked endpoint must cross the existing 8 mm contact skin.
    // Ending a candidate exactly at that skin would falsely clear a live wall
    // when native's intersection excludes an endpoint on the surface.
    // D63: distinct XR samples can share Windows' coarse millisecond tick.
    // No elapsed time keeps the measured correction; it must not release the
    // whole offset or spend another minimum spring step. Raw withdrawal still
    // cancels the corresponding correction below.
    const float springTravel=tick==previousTick?0.f:std::max(kReleaseMps*scale*seconds,.016f*scale);
    const float remaining=std::max(0.f,length-withdrawal-springTravel);
    const float factor=remaining/length;
    return {raw.x+correction.x*factor,raw.y+correction.y*factor,raw.z+correction.z*factor};
}
}
