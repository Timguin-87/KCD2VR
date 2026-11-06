#pragma once
// D62C: two copied signals between translation units, nothing else.
//  - VR geometry contact on an entity (GeometryHitSubmit178r27; feature is not certified here)
//  - native hit APPLIED on an entity (SubmitHitDetour D568B0, ok && applyNative only; evaluations never count)
// Eight observational entity/tick slots. They do not carry a weapon/gesture,
// native pair occurrence or damage-event identity and cannot grant timing.
#include "../../diagnostics/capture168.h"
#include <array>
#include <atomic>
#include <cstdint>
namespace kcd2vr::paired_signals_d62c {
inline std::atomic<bool> enabled{false};     // D63: explicit request only; no timing writes without a verified native pair join
inline std::atomic<float> speed{4.f};        // INI VrPairedHitSpeedD62C, 1..8: preliminary paired motion runs this much faster
inline std::atomic<bool> live{false};        // a Henry paired attack row is live: producers only write while this is true
struct Slot {std::atomic<std::uint32_t> entity{0},source{0};std::atomic<std::uint64_t> tick{0};};
struct Ring {
    std::array<Slot,8> s{};std::atomic<unsigned> next{0};
    void note(std::uint32_t entity,std::uint64_t tick,std::uint32_t source=0) noexcept {
        if(!entity)return;auto& x=s[next.fetch_add(1,std::memory_order_relaxed)&7u];
        x.tick.store(0,std::memory_order_relaxed);x.entity.store(entity,std::memory_order_relaxed);x.source.store(source,std::memory_order_relaxed);
        x.tick.store(tick,std::memory_order_release);
    }
    // newest tick for this entity in [from, to] (and from this source when source != 0), 0 if none
    std::uint64_t latest(std::uint32_t entity,std::uint64_t from,std::uint64_t to,std::uint32_t source=0) const noexcept {
        std::uint64_t best=0;if(!entity)return 0;
        for(const auto& x:s){const auto t=x.tick.load(std::memory_order_acquire);
            if(t>=from&&t<=to&&x.entity.load(std::memory_order_relaxed)==entity&&(!source||x.source.load(std::memory_order_relaxed)==source)&&
               x.tick.load(std::memory_order_acquire)==t&&t>best)best=t;}
        return best;
    }
};
inline Ring contacts{},applied{};
inline void NoteVrContact(std::uint32_t entity,std::uint64_t tick) noexcept {
 if(diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,tick)&&live.load(std::memory_order_acquire))contacts.note(entity,tick);
}
// attacker = hitInfo+0x00, target = hitInfo+0x28 (the fields V66.134 HitInfoText prints)
inline void NoteAppliedHit(std::uint32_t attacker,std::uint32_t target,std::uint64_t tick) noexcept {
 if(diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,tick)&&live.load(std::memory_order_acquire))applied.note(target,tick,attacker);
}
} // namespace kcd2vr::paired_signals_d62c
