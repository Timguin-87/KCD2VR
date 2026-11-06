#pragma once
#include "contact_handoff59c3.h"
#include <array>
#include <atomic>
#include <cstdint>
namespace kcd2vr::recovery65h {
enum class Site:unsigned {SubmitPrune,DrainPrune,Waiting,DrainEntry,BeforeLive,Count};
struct Stats {
 std::atomic<std::uint64_t> edges{0},retired{0},recoveryOnly{0},expiredOnly{0},both{0},hadRelease{0};
 std::array<std::atomic<std::uint64_t>,unsigned(Site::Count)> sites{};
 void reset() noexcept {edges=0;retired=0;recoveryOnly=0;expiredOnly=0;both=0;hadRelease=0;for(auto& n:sites)n=0;}
};
inline Stats stats;
// Edge observation is NOT a contact disposition. No queue/actor/native read,
// application mutex, formatting, terminal-gesture ledger or damage authority.
inline void edge(bool capture) noexcept {if(capture)stats.edges.fetch_add(1,std::memory_order_relaxed);}
// Called only where the original consumer actually refuses/removes this exact
// contact. Same-new-epoch fresh contacts and an already-started native hit never
// become a drop merely because an edge occurred. Counters describe CONTACTS,
// not unique swings; a diagnostic cannot suppress a later native outcome.
template<class P> inline void retired(const P& p,std::uint64_t current,std::uint64_t now,Site site,bool capture) noexcept {
 if(!capture||unsigned(site)>=unsigned(Site::Count))return;
 const bool recovery=p.recovery55c1!=current;
 const bool expired=!contact176::fresh(p.tick,now,contact59c3::maximumContactAge);
 if(!recovery&&!expired)return;
 stats.retired.fetch_add(1,std::memory_order_relaxed);
 (recovery&&expired?stats.both:recovery?stats.recoveryOnly:stats.expiredOnly).fetch_add(1,std::memory_order_relaxed);
 stats.sites[unsigned(site)].fetch_add(1,std::memory_order_relaxed);
 if(p.release51.serial)stats.hadRelease.fetch_add(1,std::memory_order_relaxed);
}
}
