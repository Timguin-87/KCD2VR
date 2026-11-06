#pragma once
// R120 (Miki 6 Nov: "instant pickups are still not working at all"): the 6 Nov R118 log had "Instant interactions" ON
// (its 5 D72 hooks are installed only when the switch is ON, 10:50:19) and 4 VR pickups of the same item
// (10:51:45..48, C19C3 F5_PICKUP_USE queued=1) - none of them reached the item: no PickableItem.Use (no Lua prepare row),
// no native pickup request (no F8 PICKUP_REQUEST row), so the instant route never even started. The VR Use pulse goes to
// the GAME interactor, and the interactor did not run Use for the item under the hand (the same happened with the switch
// OFF at 10:52:23/24). Fix: when "Instant interactions" is ON and the game did not take the gripped item within
// kWaitMs, the mod runs the item's own script Use for Henry once (the same PickableItem:OnUsed the interactor would call),
// so the normal instant route (TryPreparePickupR24 -> D72 forceImmediate) gets its pickup. One try per squeeze, never for
// owned (steal) or dead-body items, never while the game already picks it (native Picking / held prop / item gone).
// No latch: every new squeeze is a new attempt.
#include <atomic>
#include <cstdint>
namespace kcd2vr::pickup_use_fallback_r120 {
inline constexpr std::uint64_t kWaitMs=600,kMaxMs=1500;
inline std::atomic<std::uint64_t> serial{0},tick{0},handled{0},fallbacks{0};
inline std::atomic<std::uint32_t> target{0};
inline std::atomic<int> side{-1};
struct Pending {std::uint64_t serial=0,tick=0;std::uint32_t target=0;int side=-1;};
// producer: the VR pickup pulse that was really sent to the game (mod.cpp pickup_use_producer19c3.inl)
inline void note(std::uint32_t id,int hand,std::uint64_t now) noexcept {
    if(!id||hand<0||hand>1)return;
    target.store(id,std::memory_order_relaxed);side.store(hand,std::memory_order_relaxed);tick.store(now?now:1,std::memory_order_relaxed);
    serial.fetch_add(1,std::memory_order_release);
}
inline Pending read() noexcept {
    Pending p{};p.serial=serial.load(std::memory_order_acquire);p.tick=tick.load(std::memory_order_relaxed);
    p.target=target.load(std::memory_order_relaxed);p.side=side.load(std::memory_order_relaxed);
    if(serial.load(std::memory_order_acquire)!=p.serial)p.serial=0; // torn: next frame
    return p;
}
enum class Step : unsigned char {Wait,Act,Drop};
// gameTook = native Picking running / a held instant prop / the world entity is gone.
inline Step decide(const Pending& p,std::uint64_t handledSerial,std::uint64_t now,bool switchOn,bool context,
    bool gameTook,bool gripHeld) noexcept {
    if(!p.serial||p.serial==handledSerial||!p.target)return Step::Wait;
    if(!switchOn||gameTook)return Step::Drop;
    if(now<p.tick||now-p.tick<kWaitMs)return Step::Wait;
    if(now-p.tick>kMaxMs||!gripHeld||!context)return Step::Drop;
    return Step::Act;
}
}
