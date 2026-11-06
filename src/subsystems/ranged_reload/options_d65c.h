#pragma once
#include <atomic>
#include <cmath>
#include <cstdint>
namespace kcd2vr::ranged_options_d65c {
inline std::atomic<bool> bowPhysicalAim{true},crossbowReload{true};
inline std::atomic<std::uint64_t> bowAimRevision{1};
inline void SetBowPhysicalAim(bool value) noexcept {
    if(bowPhysicalAim.exchange(value,std::memory_order_acq_rel)!=value)
        bowAimRevision.fetch_add(1,std::memory_order_acq_rel);
}
inline void SetCrossbowReload(bool value) noexcept {crossbowReload.store(value,std::memory_order_release);}
// R96 crossbow reload route (INI VrCrossbowReloadRouteR96, Experiments "Crossbow reload route").
// true = HAND ONLY (default): bow pattern, the game's reload request is consumed only while
// our hand route owns it; idle hands give it back to the game. false = NATIVE-DRIVEN (XR34).
inline std::atomic<bool> crossbowHandOnlyR90{true};
// R111 heavy crossbow string pull (INI VrHeavyCrossbowHandPullR111, Experiments "Heavy crossbow:
// pull string by hand"). true (default) = with the HAND ONLY route the heavy crossbow (class 15)
// is spanned like the light one: grip the string, pull it back, cocked; no cranequin, no game
// action. false = the old route: chest Grip -> the game's cranequin action ridden by XR34.
inline std::atomic<bool> heavyCrossbowHandPullR111{true};
// R117 medium crossbow string pull (INI VrMediumCrossbowHandPullR117, Experiments "Medium crossbow:
// pull string by hand"). true (default) = with the HAND ONLY route the goat's-foot crossbow (class 14)
// is spanned like the light one: grip the string, pull it back, cocked; no spanner, no game action,
// no XR34. R116 log: the spanner was not in the hand (02:49:22) and the next two chest grips fell
// back to the game's lever animation in 22 ms (02:49:48, 02:50:01). false = the old spanner route.
inline std::atomic<bool> mediumCrossbowHandPullR117{true};
}

// Legacy non-instant reload mapping. D72 Instant uses the native string/rail stroke and a qualified RIGHT pouch Grip request.
namespace kcd2vr::ranged_options_d66c {
inline constexpr float kCrankTravelDefault=.08f,kCrankTravelMin=.04f,kCrankTravelMax=.20f;
inline float crankTravelSetting(float v) noexcept {
    if(!std::isfinite(v))return kCrankTravelDefault;
    return v<kCrankTravelMin?kCrankTravelMin:v>kCrankTravelMax?kCrankTravelMax:v;
}
inline std::atomic<float> crankTravel{kCrankTravelDefault};
}
