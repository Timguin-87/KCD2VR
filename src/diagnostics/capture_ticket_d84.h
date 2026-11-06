#pragma once
#include "capture168.h"

namespace kcd2vr::diagnostics::capture_ticket_d84 {
namespace cap=capture168;
struct Ticket {
    cap::Domain domain=cap::Domain::Bug;
    std::uint64_t session=0,now=0;
    explicit operator bool() const noexcept {return session!=0;}
};
// Select one original recorder for a shared, copied-data observation. A cold
// recorder does not call the supplied clock. Reopening a recorder cannot make
// an observation from its previous session eligible for the new session.
template<cap::Domain... Domains,class Clock>
inline Ticket select(Clock&& clock) noexcept {
    constexpr unsigned mask=(0u|...|(1u<<unsigned(cap::canonical(Domains))));
    if(!(cap::gate.mask.load(std::memory_order_relaxed)&mask))return {};
    const auto now=std::uint64_t(clock());
    for(const auto d:{Domains...}){
        const auto i=unsigned(cap::canonical(d));
        const auto serial=cap::gate.session[i].load(std::memory_order_acquire);
        if(serial&&cap::active(d,now)&&
           cap::gate.session[i].load(std::memory_order_acquire)==serial)
            return {cap::canonical(d),serial,now};
    }
    return {};
}
}
