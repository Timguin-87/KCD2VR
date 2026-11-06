#pragma once
#include "capture168.h"
#include <atomic>
#include <cstdint>
namespace kcd2vr::diagnostics::door_receipt63 {
// Diagnostic scope only. The real producer refreshes this only in F5=2/3;
// it conveys no door/input/animation authority and expires after80 ms.
inline std::atomic<std::uint64_t> sourceTick{0};
inline void scope(bool f5two,std::uint64_t now) noexcept {
    sourceTick.store(f5two&&capture168::active(capture168::Domain::Interaction,now)?now:0,std::memory_order_release);
}
inline bool active(std::uint64_t now) noexcept {
    const auto t=sourceTick.load(std::memory_order_acquire);
    return t&&now>=t&&now-t<=80&&capture168::active(capture168::Domain::Interaction,now);
}
struct Gate {std::uint64_t session=0,last=0;unsigned count=0;};
inline bool allow(Gate& g,std::uint64_t now) noexcept {
    if(!active(now))return false;
    const auto s=capture168::gate.session[unsigned(capture168::Domain::Interaction)].load(std::memory_order_acquire);
    if(g.session!=s){g={};g.session=s;}
    if(g.count>=128||(g.last&&(now<g.last||now-g.last<250)))return false;
    g.last=now;++g.count;return true;
}
inline const char* join(bool heldSource,bool otherCandidate,bool exactCandidate,bool physical,bool physicalCurrent) noexcept {
    if(!heldSource)return "NO_MATCHING_HELD_SOURCE";
    if(!exactCandidate&&otherCandidate)return "ANOTHER_PALM_CANDIDATE";
    if(!physical)return "NO_PHYSICAL_USE_TRANSPORT";
    if(!physicalCurrent)return "PHYSICAL_USE_EXPIRED_OR_REVOKED";
    if(!exactCandidate)return "NO_EXACT_DOOR_PALM_WITNESS";
    return "HELD_SOURCE_AND_PHYSICAL_TRANSPORT_PRESENT";
}
}
