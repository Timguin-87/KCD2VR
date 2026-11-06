#pragma once
#include "capture168.h"
#include <atomic>
#include <cstdint>

namespace kcd2vr::diagnostics::trace164 {
// Diagnostic provenance only. Never used for native admission, hit eligibility,
// damage or pose writes. A temporal neighbour is deliberately not a join.
inline std::atomic<std::uint64_t> until{0},nextId{0};
struct Context {
    std::uint64_t prediction=0,sweep=0,contact=0,sample=0,key=0,generation=0;
    std::uintptr_t collisions=0;
    bool redirected=false;
};
inline thread_local Context current{};
struct Scope {
    Context prior;
    explicit Scope(Context c={}) noexcept:prior(current){current=c;}
    ~Scope(){current=prior;}
    Scope(const Scope&)=delete;Scope& operator=(const Scope&)=delete;
};
inline bool active(std::uint64_t now,int f12) noexcept {return f12==7&&now&&now<=until.load(std::memory_order_acquire)&&capture168::active(capture168::Domain::Combat,now);}
inline std::uint64_t serial() noexcept {return nextId.fetch_add(1,std::memory_order_relaxed)+1;}
inline Context for_owner(std::uintptr_t collisions) noexcept {
    return collisions&&current.collisions==collisions?current:Context{};
}
}
