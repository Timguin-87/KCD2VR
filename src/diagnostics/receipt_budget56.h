#pragma once
#include <atomic>
#include <cstdint>

// C56 review (Claude): per-SITE receipt budget for the F3 combat recorder.
//
// The old Allow(category,perSecond) in native_hit_probe134.inl kept ONE
// per-second counter per category. 31 call sites share category 4 with limits
// from 2 to 30, so a chatty site (HENRY_COMBAT_EDGE 30/s, GEOM_LIVE_HIT 20/s,
// NPC_PHASE 20/s) consumes the budget of a rare refusal receipt
// (GEOM_HIT rejected=... 4/s, VR_HIT_SKIPPED 2/s) in the same second, and a
// refusal that happened is then missing from the log. Codex's C56 combat
// review asked for separate bounded per-site counters; this is that.
//
// Each call site owns one Site (a function-local static created by the Allow
// macro), so sites can never consume each other's budget. The counters are
// receipts themselves: allowed/refused per site, never reset, readable for a
// summary line. No native reads, no formatting, no I/O here.
namespace kcd2vr::receipt56 {
struct Site {
    std::atomic<std::uint64_t> second{0};
    std::atomic<unsigned> used{0};
    std::atomic<std::uint64_t> allowed{0},refused{0};
};
// true = this site may write one line now. perSecond lines per site per
// wall-clock second (nowMs/1000). A refused call is counted, never queued.
inline bool allow(Site& s,unsigned perSecond,std::uint64_t nowMs) noexcept {
    const auto second=nowMs/1000;
    if(s.second.exchange(second,std::memory_order_acq_rel)!=second)s.used.store(0,std::memory_order_relaxed);
    const bool ok=s.used.fetch_add(1,std::memory_order_relaxed)<perSecond;
    (ok?s.allowed:s.refused).fetch_add(1,std::memory_order_relaxed);
    return ok;
}
} // namespace kcd2vr::receipt56
