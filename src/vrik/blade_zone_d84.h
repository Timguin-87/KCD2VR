#pragma once
// D84 blade zone: the native attack direction follows the BLADE stroke, not the cocked hand position.
// INI VrBladeZoneD84 + Experiments > Combat page 2 "Attack direction". ON = blade, OFF = old hand zone.
#include <atomic>
#include <cstdint>
#include <cmath>
namespace kcd2vr::blade_zone_d84 {
inline std::atomic<bool> enabled{false};
inline std::atomic<std::uint64_t> reannounced{0}; // held preparations corrected to the blade zone
inline std::atomic<bool> upCutLow{false};    // INI VrUpCutLowD84: a cut moving UP = the native low attack (5), for combos
inline std::atomic<bool> stabStart{false};   // INI VrStabStartZoneD84: stab side = where the hand starts (above 0, left 1, right 2, chest/below 5)
inline std::atomic<std::int64_t> last{-1};        // (tick<<8)|(hand<<4)|blade, for the F3 stats line
}
inline bool bladeZoneD84Enabled() noexcept {return kcd2vr::blade_zone_d84::enabled.load(std::memory_order_acquire);}
inline void bladeZoneNoteD84(std::uint64_t tick,int hand,int blade) noexcept {
    kcd2vr::blade_zone_d84::reannounced.fetch_add(1,std::memory_order_relaxed);
    kcd2vr::blade_zone_d84::last.store(std::int64_t((tick<<8)|(std::uint64_t(hand&15)<<4)|std::uint64_t(blade&15)),std::memory_order_relaxed);}
inline bool upCutLowD84Enabled() noexcept {return kcd2vr::blade_zone_d84::upCutLow.load(std::memory_order_acquire);}
// D84 stab start: chest-relative start of the stab (x right, z up, metres). above the chest = overhead 0,
// clearly left/right = 1/2, a forward thrust from the chest or from below = low 5.
template<class V> inline int stabStartZoneD84(const V& o,int old) noexcept {
    if(!kcd2vr::blade_zone_d84::stabStart.load(std::memory_order_acquire))return old;
    if(!std::isfinite(o.x)||!std::isfinite(o.z))return old;
    if(o.z>.20)return 0;
    if(std::abs(o.x)>=.15&&std::abs(o.x)>=std::abs(o.z))return o.x<0?1:2;
    return 5;}

namespace kcd2vr::blade_zone_d84 {
// Only a still-owned accepted refinement may retag the queued release.
template<class Owner,class Command> bool ownsRefinedRelease(const Owner& n,const Command& c) noexcept {
 return c.held57&&n.attackPrepared&&n.attackKey==c.key&&n.attackGeneration117==c.generation&&
   n.epoch==c.bodyEpoch&&n.request==c.epoch&&n.prepareGestureC10R18==c.gesture59c3&&
   c.gesture59c3&&n.bladeZoneGestureD84==c.gesture59c3&&c.zone117==n.bladeZoneOriginalD84;
}
inline bool cancelRefusedZone(bool optional,bool sameOwner) noexcept {return sameOwner&&!optional;}
}
