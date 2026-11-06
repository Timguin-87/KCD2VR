#pragma once
#include <algorithm>
#include <cmath>
#include <cstdint>

namespace kcd2vr::dialogue_fingers_d78 {
// Copied with the ordinary tracking packet. These values have no gameplay,
// menu/chooser, Use, contact-query or attack ownership.
struct VisualInput {
    float trigger[2]{}; // right, left (the pose consumer's side convention)
    unsigned tracked=0;
    bool dialogue=false;
};
inline VisualInput capture(bool dialogue,bool leftTracked,bool rightTracked,
                           float leftRaw,float rightRaw) noexcept {
    VisualInput out{};
    if(!dialogue)return out;
    out.dialogue=true;
    const float raw[]{rightRaw,leftRaw};
    const bool tracked[]{rightTracked,leftTracked};
    for(int side=0;side<2;++side)if(tracked[side]&&std::isfinite(raw[side])){
        out.trigger[side]=std::clamp(raw[side],0.f,1.f);out.tracked|=1u<<side;
    }
    return out;
}
template<class Snapshot> inline float presentationTrigger(const Snapshot& s,int side,
        bool currentDialogue,bool currentDialogueBody,bool gameplay,
        std::uint64_t now,std::uint64_t trackingEpoch) noexcept {
    if(side<0||side>1)return 0.f;
    const float gameplayTrigger=side?s.left_trigger:s.right_trigger;
    if(!s.dialogueFingersD78.dialogue||!currentDialogue||!currentDialogueBody||gameplay)
        return gameplayTrigger;
    // Pose-family refresh/grace cannot refresh the age of this original input.
    const bool tracked=side?s.left_valid:s.right_valid,held=side?s.left_held:s.right_held;
    if(!s.allowed||!s.head_valid||s.head_held||!tracked||held||
       !(s.dialogueFingersD78.tracked&(1u<<side))||!s.input_tick||
       now<s.input_tick||now-s.input_tick>80||!s.source_epoch||s.source_epoch!=trackingEpoch)
        return 0.f;
    return s.dialogueFingersD78.trigger[side];
}
}
