#pragma once
#include <cstdint>
namespace kcd2vr::geometry40c1 {
// Exact call-local provenance, never a process-wide permission to deliver hits.
struct Scope {
    const void* actor=nullptr;const void* details=nullptr;const void* target=nullptr;
    Scope* previous=nullptr;
    int reactionZone=-1; // NR1: native zone of the blade's real hit spot for the victim reaction, -1 = native
    static inline thread_local Scope* current=nullptr;
    Scope(const void* a,const void* d,const void* t) noexcept:actor(a),details(d),target(t),previous(current){current=this;}
    ~Scope(){current=previous;}
    Scope(const Scope&)=delete;Scope& operator=(const Scope&)=delete;
    static bool matches(const void* a,const void* d,const void* t) noexcept {
        const auto* s=current;return s&&a&&d&&t&&a==s->actor&&d==s->details&&t==s->target;
    }
};
// Denial ownership must not depend on the ticket that grants a tracked hit.
// A stale/missing ticket in contact-only mode means no hit, never a native sweep.
inline bool rejectAuthored51c3(std::uintptr_t caller,int type,bool mode,bool exactHenry,bool injected) noexcept {
    return mode&&exactHenry&&!injected&&caller==0xC52534ull&&(type==0||type==1);
}
inline bool ownsOrdinary(std::uintptr_t caller,int type,bool qualified,bool sameActor,bool ticket) noexcept {
    return caller==0xC52534ull&&(type==0||type==1)&&qualified&&sameActor&&ticket;
}
}
