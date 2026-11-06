#pragma once
#include <cstdint>

namespace kcd2vr::carry59c7g {
// Diagnostic phases derived from the three retail C_ActorCarryCorpse predicates.
// This is not permission to force a phase or to create a ragdoll constraint.
inline const char* phase(int state) noexcept {
    if(state==0)return "IDLE";
    if(state>=1&&state<=4)return "PICKUP_PHASE";
    if(state==5)return "CARRYING";
    if(state>=6&&state<=8)return "PUTDOWN_PHASE";
    return "UNKNOWN";
}
struct Budget {
    std::uint64_t session=0,tick=0;std::uintptr_t controller=0;
    std::uint32_t target=0;int state=-1;unsigned emitted=0;
    bool accept(std::uint64_t capture,std::uint64_t now,std::uintptr_t instance,
                std::uint32_t id,int value) noexcept {
        if(!capture||!now||!instance||value<0||value>8)return false;
        if(capture!=session){*this={};session=capture;}
        if(emitted>=64)return false;
        const bool change=instance!=controller||id!=target||value!=state;
        if(!change&&(now<tick||now-tick<500))return false;
        controller=instance;target=id;state=value;tick=now;++emitted;return true;
    }
};
}
