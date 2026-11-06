#pragma once
#include <atomic>
#include <cstdint>
namespace kcd2vr::finisherdiag_d42 {
inline std::atomic<std::uint64_t> seen{0},written{0},suppressed{0};
// Producer-local budget. Global counters retain suppressed rows for the F3
// session summary; the finite capture owner still gates every call.
struct Budget {
    std::uint64_t session=0,window=0;unsigned lines=0;
    bool take(std::uint64_t currentSession,std::uint64_t now) noexcept {
        if(!currentSession||!now)return false;
        if(session!=currentSession||now<window||now-window>=1000){session=currentSession;window=now;lines=0;}
        seen.fetch_add(1,std::memory_order_relaxed);
        if(lines>=8){suppressed.fetch_add(1,std::memory_order_relaxed);return false;}
        ++lines;written.fetch_add(1,std::memory_order_relaxed);return true;
    }
};
}
