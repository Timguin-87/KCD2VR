#pragma once
#include "capture168.h"

namespace kcd2vr::diagnostics::contact_capture_d84 {
namespace cap=capture168;
struct Ticket {cap::Domain domain=cap::Domain::Physics;std::uint64_t session=0;unsigned activeMask=0;};
inline Ticket select(std::uint64_t now) noexcept {
    Ticket out{};
    // One copied contact/ray for combined keys; no added world query.
    for(const auto d:{cap::Domain::Physics,cap::Domain::Interaction,cap::Domain::Vrik,cap::Domain::Combat}){
        if(!cap::active(d,now))continue;
        const auto serial=cap::gate.session[unsigned(d)].load(std::memory_order_acquire);
        if(!serial||!cap::active(d,now)||serial!=cap::gate.session[unsigned(d)].load(std::memory_order_acquire))continue;
        out.activeMask|=1u<<unsigned(d);
        if(!out.session){out.domain=d;out.session=serial;}
    }
    return out;
}
inline bool current(const Ticket& ticket,std::uint64_t now) noexcept {
    return ticket.session&&cap::active(ticket.domain,now)&&
        cap::gate.session[unsigned(ticket.domain)].load(std::memory_order_acquire)==ticket.session;
}
inline bool receipt(const Ticket& ticket,std::uint64_t now) noexcept {
    return ticket.session&&cap::gate.receipt(ticket.domain,ticket.session,now);
}
inline bool same(const Ticket&a,const Ticket&b) noexcept {
    return a.domain==b.domain&&a.session==b.session&&a.activeMask==b.activeMask;
}
}
