#pragma once
// R120 DRAW GUARD (Miki 6 Nov: "the unsheathe again showed the 1 frame of the native position of crossbow or other
// weapons"). F2 log 10:54:34 (D77 draw=3): the game binds the drawn sword to the hand socket at ms 313343, our first
// VR placement is at ms 313406 (63 ms = ~5 frames at the native hand pose) and NO CEntity::Hide(true) from the D75
// first-placement hide was recorded in any of the 6 draws (D75 refuses before its equipment snapshot/graph proofs
// catch up, and never adopts an entity that is already hidden, e.g. the crossbow we hid on the back).
// This guard: the native item attach (draw into a hand) arms it for that item entity; every Henry
// CEntityAttachment callback of that entity keeps it hidden until a VR placement was written
// (PlaceBothConsumers6689 marks it), then shows it in the same callback. Max 150 ms after the first callback
// (1000 ms if no callback came), then it is shown whatever happened. Per draw, no latch.
// R121 (Miki: "is there any better solution?" - 150 ms can look like the weapon vanished): the long wait is only kept
// while our grip for this item is still being learned (placement cannot be written yet). Once the grip is ready the
// placement comes on the next hand callback (log: cached sword draws 4/7/8/9 = first callback in the bind frame,
// placement on the second), so the hide is capped at kReadyFrames native frames after the first hand callback.
// Crossbow draws now reuse their settled grip (physical_native.inl PhysCaptureGrip6698 R121), so they take this
// short path too.
#include <atomic>
#include <cstdint>
#include <mutex>

namespace kcd2vr::draw_guard_r120 {

inline constexpr std::uint64_t kAfterCallbackMs=150;   // first hand callback -> forced show
inline constexpr std::uint64_t kNoCallbackMs=1000;     // attach op without any hand callback -> forced show
inline constexpr unsigned kReadyFrames=2;              // R121: grip ready -> at most 2 native frames hidden

struct Guard {
    std::atomic<std::uint32_t> id{0};      // entity id, 0 = free
    std::atomic<std::uint64_t> key{0};     // R121: inventory key of the drawn item (grip readiness lookup)
    std::atomic<unsigned> frames{0};       // R121: native frames since the first hand callback
    std::atomic<std::uint64_t> armed{0};   // tick of the native attach op
    std::atomic<std::uint64_t> first{0};   // tick of the first hand callback
    std::atomic<bool> placed{false};       // a VR placement was written for this entity
    std::atomic<bool> ours{false};         // we own a hidden write that must be shown again
};
inline Guard guards[2]{};                  // one per hand side; side 2 (class hand) uses the first free one
inline std::mutex lock;                    // arm/service/show transitions (never held across native code that re-enters)

// Called from PlaceBothConsumers6689 after a successful write. Lock-free.
inline void markPlaced(std::uint32_t id) noexcept {
    if(!id)return;
    for(auto& g:guards)if(g.id.load(std::memory_order_acquire)==id)g.placed.store(true,std::memory_order_release);
}
inline bool armedFor(std::uint32_t id) noexcept {
    if(!id)return false;
    for(auto& g:guards)if(g.id.load(std::memory_order_acquire)==id)return true;
    return false;
}
// pure decision for one guard at `now`
enum class Step : unsigned char { Keep, Show, Hide };
inline Step decide(bool placed,std::uint64_t armed,std::uint64_t first,std::uint64_t now,bool visible,
                   unsigned frames=0,bool gripReady=false) noexcept {
    if(placed)return Step::Show;
    if(first&&gripReady&&frames>=kReadyFrames)return Step::Show; // R121: placement possible but late -> short fallback
    if(first){if(now>=first&&now-first>kAfterCallbackMs)return Step::Show;}
    else if(now>=armed&&now-armed>kNoCallbackMs)return Step::Show;
    return visible?Step::Hide:Step::Keep;
}

} // namespace kcd2vr::draw_guard_r120
