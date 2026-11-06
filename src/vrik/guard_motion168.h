#pragma once
#include "pose_math.h"
#include <cstdint>
namespace kcd2vr::guard_motion168 {
using V=pose_core::Vec3;
// Gesture INPUT only. Never interprets a movement as an accepted parry.
// Coordinates are the original XR tracking-space hand/head translations: game
// locomotion and character rebasing cannot become a weapon motion here.
struct Sample {std::uint64_t tick=0,sequence=0,epoch=0,key=0;V hand{},head{};bool eligible=false;};
struct State {
    Sample previous{};V anchorHand{},anchorRelative{};
    std::uint64_t settledSince=0,lastEdge=0;bool armed=false;
    void reset() noexcept {*this={};}
    bool update(Sample s) noexcept {
        if(!s.eligible||!s.tick||!s.sequence||!s.key||!pose_core::finite(s.hand)||!pose_core::finite(s.head)) {reset();return false;}
        if(!previous.sequence||previous.epoch!=s.epoch||previous.key!=s.key||
           s.sequence<previous.sequence||s.tick<previous.tick||s.tick-previous.tick>80){
            reset();previous=s;anchorHand=s.hand;anchorRelative=s.hand-s.head;settledSince=s.tick;return false;
        }
        if(s.sequence==previous.sequence||s.tick==previous.tick)return false;
        const double dt=double(s.tick-previous.tick)*.001;
        const V relative=s.hand-s.head;
        const double rawSpeed=pose_core::length(s.hand-previous.hand)/dt;
        const double relativeSpeed=pose_core::length(relative-(previous.hand-previous.head))/dt;
        previous=s;
        if(!armed){
            if(rawSpeed<.25&&relativeSpeed<.25){
                if(!settledSince)settledSince=s.tick;
                if(s.tick-settledSince>=120&&(!lastEdge||s.tick-lastEdge>=300)){
                    armed=true;anchorHand=s.hand;anchorRelative=relative;
                }
            }else settledSince=0;
            return false;
        }
        // Require both actual controller travel and head-relative travel.
        // Head translation alone, a rotating HMD, or a repeated sample cannot fire.
        const bool edge=rawSpeed>=.90&&relativeSpeed>=.90&&rawSpeed<=8.&&relativeSpeed<=8.&&
            pose_core::length(s.hand-anchorHand)>=.08&&pose_core::length(relative-anchorRelative)>=.08;
        if(edge){armed=false;settledSince=0;lastEdge=s.tick;return true;}
        // Slow repositioning changes the neutral anchor, rather than charging up
        // a future edge from metres of old travel.
        if(rawSpeed<.25&&relativeSpeed<.25){anchorHand=s.hand;anchorRelative=relative;}
        return false;
    }
};
}
