#pragma once
#include "../combat_menu49.h"
#include "minigame_hands45.h"
// One UI contract for the same existing saved/native modes. UI names do not
// change native tool capability or grant an unproved physics/animation owner.
namespace kcd2vr::crafting_menu_h18 {
using Kind=minigame_hands45::Witness;
using Rect=combat_menu49::Rect;
inline constexpr int kModeCount=4;
inline constexpr Rect kChoices[2][kModeCount]{
    {{620,158,716,190},{726,158,824,190},{834,158,932,190},{942,158,1038,190}},
    {{620,334,716,366},{726,334,824,366},{834,334,932,366},{942,334,1038,366}}
};
constexpr const wchar_t* label(int mode) noexcept {
    return mode==minigame_hands45::SixDofVrik?L"6DOF VRIK":mode==minigame_hands45::Gesture?L"FREE VRIK":mode==minigame_hands45::SemiPassenger?L"SEMI":L"NATIVE";
}
template<class State> int selected(const State& state,Kind kind) noexcept {
    return minigame_hands45::clampMode(kind==minigame_hands45::Alchemy?state.alchemy_mode67:state.smithing_mode67);
}
template<class State> void select(State& state,Kind kind,int value) noexcept {
    const int mode=minigame_hands45::clampMode(value);
    if(kind==minigame_hands45::Alchemy){state.alchemy_mode67=mode;if(mode)state.activity_hands59c7f[0]=true;}
    else if(kind==minigame_hands45::Smithing){state.smithing_mode67=mode;if(mode){state.activity_hands59c7f[1]=true;state.smith_auto_hold36=true;}}
}
template<class State> void next(State& state,Kind kind) noexcept {select(state,kind,(selected(state,kind)+1)%kModeCount);}
}
