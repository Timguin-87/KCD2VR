#pragma once
#include "../../vrik/smith_tool36.h"
#include "../crafting/options_d67.h"
#include "assist_d80.h"
#include "../alchemy/geometry68.h"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>

namespace kcd2vr::subsystems::smithing::native_floor_d56 {
namespace geometry=::kcd2vr::subsystems::alchemy::geometry68;
// A presentation floor for this actual workpiece at THIS native anvil visit.
// The reference is the lower extent of the untouched native idle placement.
// Native damage/heat/StrokeHit and raw gesture tracking are never modified.
struct Identity {
    smith_tool36::Binding hand{};
    std::uintptr_t holder=0,attachment=0,object=0;
    std::uint32_t entity=0;
    std::uint64_t pose=0,source=0,mode=0,station=0;
};
inline bool same(const Identity& a,const Identity& b) noexcept {
    return a.entity&&a.entity==b.entity&&a.holder==b.holder&&a.attachment==b.attachment&&a.object==b.object&&
        a.pose==b.pose&&a.source==b.source&&a.mode==b.mode&&a.station==b.station&&
        smith_tool36::sameGripIdentity67(a.hand,b.hand);
}
inline bool valid(const Identity& a) noexcept {
    return a.hand&&a.hand.side==1&&a.hand.player&&a.hand.character&&a.hand.model&&a.hand.lifetime&&
        a.holder&&a.attachment&&a.object&&a.entity&&a.pose&&a.source&&a.mode&&a.station;
}
// D83R1: phase15 continues the recorded native fall; never acquire a floor
// there, but retain a same-owner floor already learned from phase11.
inline bool anvil(int phase) noexcept {return phase==11||phase==14||phase==15||phase==16||phase==17;}
inline double bottom(const geometry::Bounds& b) noexcept {
    if(!geometry::valid(b))return std::numeric_limits<double>::quiet_NaN();
    double result=std::numeric_limits<double>::infinity();
    for(unsigned i=0;i<8;++i){const geometry::Point p{b.local[(i&1)?3:0],b.local[(i&2)?4:1],b.local[(i&4)?5:2]};
        result=std::min(result,geometry::transform(b,p).z);}
    return result;
}
struct State {
    Identity id{};std::uint64_t sample=0,tick=0;
    double worldBottom=0;bool have=false,semiEngaged=false;
};
struct Result {bool owned=false,acquired=false;double lift=0,worldBottom=0;};
inline Result step(State& state,const Identity& id,std::uint64_t sample,std::uint64_t tick,std::uint64_t now,
    const geometry::Bounds& original,const geometry::Bounds& desired,bool semi=false) noexcept {
    Result out{};
    if(!valid(id)||!anvil(id.hand.phase)||!sample||!tick||now<tick||now-tick>80){state={};return out;}
    const double want=bottom(desired),native=bottom(original);
    if(!std::isfinite(want)||!std::isfinite(native)||std::fabs(want)>1.e6||std::fabs(native)>1.e6){state={};return out;}
    if(state.have&&(!same(state.id,id)||sample<state.sample||tick<state.tick||tick-state.tick>80))state={};
    if(!state.have){
        if(id.hand.phase!=11)return out; // never learn a swinging/transfer/heating pose as the floor
        state.id=id;state.worldBottom=native;state.have=true;out.acquired=true;
    }
    state.sample=sample;state.tick=tick;
    out.owned=true;out.worldBottom=state.worldBottom;out.lift=std::max(0.,state.worldBottom-want);
    // Optional left-only resting support: settle the actual workpiece onto its
    // current anvil plane inside an 8mm approach band. No wrist pose is borrowed
    // from animation, no axis is frozen and no elapsed-time filter is applied.
    // Moving upward beyond 12mm releases immediately; free mode has no snap.
    const double gap=want-state.worldBottom;
    if(!semi||gap>.012)state.semiEngaged=false;
    else if(gap<=.008)state.semiEngaged=true;
    if(state.semiEngaged)out.lift=state.worldBottom-want;
    return out;
}
}
