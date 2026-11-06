#pragma once
#include "capture168.h"

namespace kcd2vr::diagnostics::npc_dialogue_d52 {
// Native callbacks never wait for a recorder. This lock protects only the
// small diagnostic budget; it cannot grant or revoke gameplay/gaze ownership.
struct Budget {
    std::atomic_flag busy=ATOMIC_FLAG_INIT;
    std::uint64_t session=0,last=0;unsigned count=0;
    bool allow(capture168::Domain domain,std::uint64_t now,std::uint64_t interval,unsigned limit) noexcept {
        using namespace capture168;
        if(!active(domain,now)||busy.test_and_set(std::memory_order_acquire))return false;
        const auto serial=gate.session[unsigned(canonical(domain))].load(std::memory_order_acquire);
        if(serial!=session){session=serial;last=0;count=0;}
        const bool ok=serial&&count<limit&&(!last||now<last||now-last>=interval)&&active(domain,now);
        if(ok){last=now;++count;}busy.clear(std::memory_order_release);return ok;
    }
};
}
