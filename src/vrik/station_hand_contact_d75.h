#pragma once
#include <cstdint>
#include <cmath>
// Minigame free-hand contact: measured palm-only surface queries, close positive
// clearance, no per-digit curl owner and no previous-publication wrist hold.
// This does not own held smith/alchemy tools or create a clearance witness.
namespace kcd2vr::station_hand_contact_d75 {
constexpr bool selected(bool nativeStationOwner,bool nativeReference,bool contactMode) noexcept {
    return nativeStationOwner&&!nativeReference&&contactMode;
}
constexpr bool skipWorldRays(bool performance,bool station) noexcept {return performance&&!station;}
constexpr float skin(bool station) noexcept {return station?.002f:.008f;}
inline bool admitCorrection(double modelDistance,double worldScale) noexcept {
    return std::isfinite(modelDistance)&&std::isfinite(worldScale)&&modelDistance>=0&&worldScale>0&&
        modelDistance*worldScale<=.020001; // 20 mm real-world cap plus float comparison tolerance
}
// Algebraic bound, not a temporal hold. The old all-or-nothing test jumped the
// whole20mm correction away when penetration crossed the limit by one ULP.
// The caller keeps the measured outside point separately; this visible result
// must not become a new clearance witness or the next native sweep's origin.
template<class V> inline bool presentationD81(V raw,V measured,double scale,V& out,bool& limited) noexcept {
    limited=false;out=raw;
    const double dx=double(measured.x)-raw.x,dy=double(measured.y)-raw.y,dz=double(measured.z)-raw.z;
    const double distance=std::sqrt(dx*dx+dy*dy+dz*dz);
    if(!std::isfinite(scale)||scale<=0||!std::isfinite(distance))return false;
    const double cap=.020/scale;
    if(distance<=cap){out=measured;return true;}
    const double k=cap/distance;limited=true;
    out={float(raw.x+dx*k),float(raw.y+dy*k),float(raw.z+dz*k)};
    return true;
}
constexpr bool samePublication(std::uint64_t recordedSample,std::uint64_t recordedTracking,
        std::uint64_t sample,std::uint64_t tracking) noexcept {
    return recordedSample&&recordedTracking&&recordedSample==sample&&recordedTracking==tracking;
}
}
