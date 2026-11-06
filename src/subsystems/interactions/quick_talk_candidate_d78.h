#pragma once
#include "../../core/snapshot_mailbox.h"
#include <atomic>
#include <cstdint>

namespace kcd2vr::quick_talk_target_d78 {
// Copied head/chat-policy nominee only. Native descriptor eligibility is a
// separate original-first script result; neither record authorizes an action.
struct Candidate {std::uintptr_t player=0;std::uint64_t tick=0,generation=0;std::uint32_t entity=0;};
inline core::SnapshotMailbox<Candidate> candidate;
inline std::atomic<std::uint64_t> wantedUntil{0};
inline bool Fresh(std::uint64_t tick,std::uint64_t now) noexcept {return tick&&now>=tick&&now-tick<=80;}
inline bool Observing(std::uint64_t now) noexcept {
 const auto until=wantedUntil.load(std::memory_order_acquire);return until&&now<=until&&until-now<=80;
}
inline bool Same(const Candidate& a,const Candidate& b) noexcept {
 return a.player&&a.entity&&a.generation&&a.player==b.player&&a.entity==b.entity&&a.generation==b.generation;
}
}
