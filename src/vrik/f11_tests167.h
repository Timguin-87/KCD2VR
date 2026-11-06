#pragma once
#include <cstdint>
// Control-thread preset lease. No native calls and no calibration ownership.
namespace kcd2vr::f11_tests167 {
struct Modes { int combat=0,interaction=0,pose=0,weapon=0; };
inline bool same(Modes a,Modes b){return a.combat==b.combat&&a.interaction==b.interaction&&a.pose==b.pose&&a.weapon==b.weapon;}
inline Modes preset(int rung){return rung==1?Modes{2,0,5,0}:rung==2?Modes{3,0,3,7}:Modes{};}
struct Lease {
    bool owned=false;Modes base{},selected{};
    void begin(Modes current){if(!owned){base=current;owned=true;}}
    bool current(Modes value)const{return owned&&same(selected,value);}
    bool end(Modes value,Modes& restore){const bool ok=current(value);restore=base;*this={};return ok;}
};
// One small cue on contact entry. Sustained pressure cannot vibrate repeatedly;
// loss/held tracking resets the latch and cannot itself count as contact.
struct Touch {
    std::uint64_t epoch=0,sequence=0,lastPulse[2]{};bool touching[2]{};
    bool update(int side,bool valid,bool blocked,std::uint64_t ep,std::uint64_t sample,std::uint64_t tick){
        if(side<0||side>1)return false;
        if(epoch!=ep){*this={};epoch=ep;}
        if(!valid){touching[side]=false;return false;}
        const bool edge=blocked&&!touching[side];touching[side]=blocked;
        if(!edge||!tick||(lastPulse[side]&&(tick<lastPulse[side]||tick-lastPulse[side]<250)))return false;
        sequence=sample;lastPulse[side]=tick;return true;
    }
};
}
