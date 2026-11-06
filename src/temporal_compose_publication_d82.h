#pragma once
#include <atomic>
#include <cstdint>

// Production queue/recording policy, kept independent of D3D12 so the actual
// decision can be exercised by the portable regression. Callers serialize a
// Recording with the existing command-list registry mutex. Scope is TLS.
namespace kcd2vr::temporal_compose_d82 {
struct Stamp {
    std::uintptr_t list{},output{};
    std::uint64_t generation{},epoch{};
    std::uint32_t frame{};
    bool Valid() const noexcept { return list&&output&&generation&&epoch&&frame; }
};
struct Scope {
    std::uint64_t epoch{};
    std::uint32_t frame{};
    std::uintptr_t output{};
    bool Active() const noexcept { return epoch&&frame&&output; }
};
struct Recording {
    std::uint64_t generation{1};
    std::uintptr_t boundOutput{};
    Stamp composed{};

    // Reset discards a recording even when its COM pointer is reused; submit
    // consumes its proof as well. Neither operation can certify another draw.
    void Discard() noexcept {
        ++generation;if(!generation)++generation;
        boundOutput=0;composed={};
    }
    void BindMain(std::uintptr_t output) noexcept { boundOutput=output; }
    // ClearState erases bindings, not commands already recorded in this list.
    void ClearBindings() noexcept { boundOutput=0; }
    bool RecordDraw(std::uintptr_t list,const Scope& scope,
                    std::uint32_t primitives,std::uint32_t instances) noexcept {
        if(!list||!scope.Active()||!primitives||!instances||
           !generation||boundOutput!=scope.output)return false;
        composed={list,scope.output,generation,scope.epoch,scope.frame};
        return true;
    }
    bool ObserveUnscopedDraw(std::uintptr_t list,std::uint32_t primitives,
                             std::uint32_t instances) noexcept {
        if(!primitives||!instances||!composed.Valid()||composed.list!=list||
           composed.generation!=generation||boundOutput!=composed.output)return false;
        composed={};return true;
    }
    Stamp SubmittedCandidate(std::uintptr_t list,std::uint32_t frame) const noexcept {
        if(!composed.Valid()||composed.list!=list||composed.generation!=generation||
           composed.frame!=frame)return {};
        return composed;
    }
};

// The SDK receipt must be for this stamp's exact epoch/frame. The caller checks
// that with PairReady(stamp.epoch,stamp.frame), after the game submits the list.
// Crucially, an early barrier or an incomplete receipt never spends the one-shot
// RIGHT copy. Native0 retains its existing first-main-writer decision.
inline bool ClaimRightCopy(std::atomic<bool>& consumed,bool temporal,
                          bool nativeRightMain,const Stamp& composition,
                          std::uint32_t frame,std::uintptr_t currentOutput,
                          bool samePairSdkReady) noexcept {
    if(temporal) {
        if(!composition.Valid()||composition.frame!=frame||
           composition.output!=currentOutput||!samePairSdkReady)return false;
    } else if(!nativeRightMain) return false;
    return !consumed.exchange(true,std::memory_order_acq_rel);
}
}
