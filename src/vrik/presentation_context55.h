#pragma once
#include <cstdint>

namespace kcd2vr::presentation_context55 {
// Decode the existing single-word context publication. Ordinary Picking uses
// the same real-UI/age refusal as the already-qualified workstation reader;
// this function neither publishes a context nor grants an activity owner.
inline const char* refusal(std::uint64_t context,std::uint64_t now,bool requireFresh) noexcept {
    const auto tick=context>>8;const unsigned flags=unsigned(context&255);
    if(requireFresh&&(!tick||now<tick||now-tick>80))return "WORKSTATION_CONTEXT_STALE";
    if(flags&2)return "WORKSTATION_DIALOGUE_OWNS_POSE";
    if(flags&4)return "WORKSTATION_CUTSCENE_OWNS_POSE";
    if((flags&16)||((flags&1)&&!(flags&8)))return "WORKSTATION_REAL_UI_OWNS_POSE";
    return nullptr;
}
}
