#pragma once
#include "hook_cost178r22.h"
#include <array>

namespace kcd2vr::diagnostics::pose_cost_d60 {
// Existing call boundaries only. F2 OFF performs no clock/native read or counter
// write. These inclusive nested costs must not be added to total pose time.
enum Stage : unsigned { ComposeDirty, ComposeFull, ContactSweep, ContactRecovery, Count };
inline constexpr const char* names[Count]{"FK_DIRTY","FK_FULL","HAND_SWEEP","HAND_RECOVERY"};
struct Sample {std::uint64_t session=0,calls=0,us=0,maxUs=0;};
struct Counter {std::atomic_flag busy=ATOMIC_FLAG_INIT;Sample value{};};
inline std::array<Counter,Count> counters{};
inline void record(unsigned stage,std::uint64_t session,std::uint64_t elapsed) noexcept {
    auto& c=counters[stage];if(c.busy.test_and_set(std::memory_order_acquire))return;
    if(c.value.session!=session)c.value={session};
    ++c.value.calls;c.value.us+=elapsed;c.value.maxUs=std::max(c.value.maxUs,elapsed);
    c.busy.clear(std::memory_order_release);
}
struct Scope {
    unsigned stage;hookcost178r22::Lease capture{};
    std::chrono::steady_clock::time_point start{};
    explicit Scope(unsigned s) noexcept:stage(s) {
        if(s>=Count||!(capture168::gate.mask.load(std::memory_order_relaxed)&(1u<<unsigned(capture168::Domain::Bug))))return;
        capture=hookcost178r22::lease();
        if(capture.domain!=capture168::Domain::Bug)capture.serial=0;
        if(capture.serial)start=std::chrono::steady_clock::now();
    }
    ~Scope(){
        if(!capture.serial||!capture.current())return;
        const auto elapsed=std::uint64_t(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now()-start).count());
        record(stage,capture.serial,elapsed);
    }
    Scope(const Scope&)=delete;Scope& operator=(const Scope&)=delete;
};
template<class Writer> inline void drain(std::uint64_t now,Writer&& writer) noexcept {
    const bool enabled=capture168::active(capture168::Domain::Bug,now);
    const auto session=capture168::gate.session[unsigned(capture168::Domain::Bug)].load(std::memory_order_acquire);
    for(unsigned i=0;i<Count;++i){auto& c=counters[i];if(c.busy.test_and_set(std::memory_order_acquire))continue;
        const auto v=c.value;c.value={};c.busy.clear(std::memory_order_release);
        if(!enabled||!v.calls||v.session!=session)continue;
        char line[280]{};std::snprintf(line,sizeof(line),
            "D60 F2_POSE_COST session=%llu stage=%s calls=%llu totalUs=%llu maxUs=%llu timing=INCLUSIVE_WALL overlapsPoseTotal=1 nativeReadsAdded=0",
            (unsigned long long)session,names[i],(unsigned long long)v.calls,(unsigned long long)v.us,(unsigned long long)v.maxUs);
        writer(line);
    }
}
}
