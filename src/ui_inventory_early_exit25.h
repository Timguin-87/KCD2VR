#pragma once
// C25 native modal-close hint, C26C1 qualified by the actual InventoryList root.
// A child dialog ending on B does not establish inventory closing. Require the
// previously visible primary root to be KNOWN closed; other APSE children may
// still be draining. Unknown observations never end Theater. Native Map/Shop
// owners are excluded by the caller; real owner release remains authoritative.
#include <cstdint>

namespace kcd2vr::ui_inventory_early_exit25 {

enum class Edge : unsigned { None, Begin, CancelModalReturned, CancelTimeout, Complete, CancelPrimaryReturned };

struct Input {
    bool enabled = true;          // INI VrInventoryEarlyExit
    bool inventoryOwner = false;  // router owner == InventoryMap
    bool modalVisible = false;    // ApseModalDialog visible
    bool modalKnownClosed = false;// ApseModalDialog known AND not visible (an observer loss is not a close)
    bool rootsVisible = false;    // any APSE root still visible (inventory not yet gone)
    bool primaryRootVisible = false; // exact native InventoryList root, not a modal
    bool primaryRootKnownClosed = false;
    bool otherPrimaryRootVisible = false, uiUncertain = false;
    bool closePressRecent = false;// close press <=1 s ago, or inventory_close_pending
    std::uint64_t nowMs = 0;
};

inline constexpr std::uint64_t kWindowMs = 1000;

class Gate {
    bool prevModal_ = false, active_ = false, primarySeen_ = false;
    std::uint64_t sinceMs_ = 0;
public:
    bool Active() const noexcept { return active_; }
    std::uint64_t SinceMs() const noexcept { return sinceMs_; }
    // Returns the edge for this tick (edge-only logging); Active() is the override.
    Edge Update(const Input& in) noexcept {
        const bool drop = prevModal_ && !in.modalVisible && in.modalKnownClosed;
        prevModal_ = in.modalVisible;
        if (!in.inventoryOwner || !in.enabled) {
            const bool was = active_; active_ = false; primarySeen_ = false;
            return was && !in.inventoryOwner ? Edge::Complete : (was ? Edge::CancelTimeout : Edge::None);
        }
        if(in.otherPrimaryRootVisible||in.uiUncertain){
            const bool was=active_;active_=false;primarySeen_=in.primaryRootVisible;
            return was?Edge::CancelModalReturned:Edge::None;
        }
        const bool primaryWasSeen=primarySeen_;
        const bool primaryDrop=primaryWasSeen&&in.primaryRootKnownClosed;
        if(in.primaryRootVisible)primarySeen_=true;
        else if(in.primaryRootKnownClosed)primarySeen_=false;
        if (active_) {
            if (in.modalVisible) { active_ = false; return Edge::CancelModalReturned; }
            if (in.primaryRootVisible) { active_ = false; return Edge::CancelPrimaryReturned; }
            if (in.nowMs - sinceMs_ > kWindowMs) { active_ = false; return Edge::CancelTimeout; }
            return Edge::None;
        }
        // A B press may close only a child confirmation. Never leave Theater
        // while the actual previously visible InventoryList is still up/unknown.
        if ((drop||primaryDrop) && primaryWasSeen && !in.primaryRootVisible && in.primaryRootKnownClosed &&
            in.modalKnownClosed && in.rootsVisible && in.closePressRecent) {
            active_ = true; sinceMs_ = in.nowMs; return Edge::Begin;
        }
        return Edge::None;
    }
};

inline constexpr const char* Name(Edge e) noexcept {
    switch (e) {
    case Edge::Begin: return "NATIVE_CLOSE_BEGIN";
    case Edge::CancelModalReturned: return "MODAL_RETURNED";
    case Edge::CancelPrimaryReturned: return "PRIMARY_INVENTORY_RETURNED";
    case Edge::CancelTimeout: return "TIMEOUT_OR_DISABLED";
    case Edge::Complete: return "OWNER_RELEASED";
    default: return "NONE";
    }
}

} // namespace kcd2vr::ui_inventory_early_exit25
