#pragma once
#include "capture168.h"
#include <type_traits>

namespace kcd2vr::diagnostics::reload_capture_d84 {
namespace cap=capture168;
struct Ticket {
    cap::Domain domain=cap::Domain::Reload;
    std::uint64_t session=0;
    unsigned activeMask=0;
};
inline constexpr unsigned mask=(1u<<unsigned(cap::Domain::Interaction))|
    (1u<<unsigned(cap::Domain::Vrik))|(1u<<unsigned(cap::Domain::Reload));
inline bool requested() noexcept {return (cap::gate.mask.load(std::memory_order_relaxed)&mask)!=0;}
inline Ticket select(std::uint64_t now) noexcept {
    Ticket ticket{};
    if(!requested())return ticket;
    // One copied record per event even with all keys on. Prefer the requested
    // interaction/pose recorders; legacy F11 keeps its own independent lease.
    for(const auto d:{cap::Domain::Interaction,cap::Domain::Vrik,cap::Domain::Reload}){
        if(!cap::active(d,now))continue;
        const auto session=cap::gate.session[unsigned(d)].load(std::memory_order_acquire);
        if(!session||!cap::active(d,now)||cap::gate.session[unsigned(d)].load(std::memory_order_acquire)!=session)continue;
        ticket.activeMask|=1u<<unsigned(d);
        if(!ticket.session){ticket.domain=d;ticket.session=session;}
    }
    return ticket;
}
inline bool active(std::uint64_t now) noexcept {return select(now).session!=0;}
inline bool current(const Ticket& ticket,std::uint64_t now) noexcept {
    return ticket.session&&cap::active(ticket.domain,now)&&
        cap::gate.session[unsigned(ticket.domain)].load(std::memory_order_acquire)==ticket.session;
}
inline bool receipt(const Ticket& ticket,std::uint64_t now) noexcept {
    return ticket.session&&cap::gate.receipt(ticket.domain,ticket.session,now);
}
// Production supplies (domain, session, row) to the explicit receipt sink.
// Existing host fixtures may still collect strings or (domain, row) directly.
template<class Sink> inline void emit(Sink& sink,const Ticket& ticket,const char* row) noexcept {
    if constexpr(std::is_invocable_v<Sink,cap::Domain,std::uint64_t,const char*>)sink(ticket.domain,ticket.session,row);
    else if constexpr(std::is_invocable_v<Sink,cap::Domain,const char*>)sink(ticket.domain,row);
    else sink(row);
}
}
