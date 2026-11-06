#pragma once
// R96 (Miki: "don't add any more blockers which cancel functions forever, even after loading the save, until
// restart"): every fault/refusal latch that used to stay on for the whole session is re-armed after a save/level load.
// R97 merge: ONE load signal for the whole mod. This no longer keeps its own counter bumped from mod.cpp; it reads the
// same positively observed loading episode (inventory_lifecycle_t8 via observed_load_recovery_r96) that the Codex
// load-scoped Fault class (subsystems/physics/load_fault_recovery_r96.h) uses. A latch is either a Fault (clears itself
// when a load completes after the fault) or a plain flag re-armed here once per completed load - never both.
// Each owner thread calls Seen::take() on its own copy and re-arms its latches there, so nothing races a native call in
// flight. Real native-code mismatches (pins, retail build, hook prologues) are NOT reset here; they stay off and say so.
#include "observed_load_recovery_r96.h"
#include <atomic>
#include <cstdint>
namespace kcd2vr::load_rearm_r96 {
// true once per COMPLETED observed load for this owner. The first call only takes the baseline (the episode the
// owner was born in), so nothing fires at startup. A fault during the load screen is cleared when that load completes.
struct Seen {
    std::uint64_t last=0;
    bool take() noexcept {
        const auto w=observed_load_recovery_r96::observe();
        if(!w.admitted||!w.episode)return false;
        if(!last){last=w.episode;return false;}
        if(w.episode==last)return false;
        last=w.episode;return true;
    }
};
// clears a latched flag; returns true when it was set (so the caller can log one line)
inline bool rearm(std::atomic<bool>& flag) noexcept {return flag.exchange(false,std::memory_order_acq_rel);}
inline bool rearm(bool& flag) noexcept {const bool was=flag;flag=false;return was;}
}
