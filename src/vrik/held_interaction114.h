#pragma once
#include "physical_interaction.h"
#include <cmath>
namespace kcd2vr::held_interaction114 {
struct State {int owner=-1;bool neutral[2]{false,false};std::uint64_t tick=0,pressed=0;};
struct Output {int owner=-1;bool ray=false,hold=false,simultaneous=false;};
inline Output update(State& s,const physical_interaction106::Input& in) noexcept {
    Output o{};
    const bool continuous=!s.tick||(in.now_ms>=s.tick&&in.now_ms-s.tick<=80);
    if(!in.enabled||in.menu||!in.now_ms||!continuous){s={};return o;}
    s.tick=in.now_ms;
    bool edge[2]{};
    for(int side=0;side<2;++side){const auto& h=in.hand[side];
        const bool eligible=h.tracked&&h.empty&&(h.neutral||s.owner==side)&&std::isfinite(h.grip);
        if(!eligible){s.neutral[side]=false;if(s.owner==side)s.owner=-1;continue;}
        if(h.grip<.30f){s.neutral[side]=true;if(s.owner==side)s.owner=-1;}
        if(h.grip>.58f&&s.neutral[side]){edge[side]=true;s.neutral[side]=false;}
    }
    if(edge[0]&&edge[1]){o.simultaneous=true;s.owner=-1;}
    else if(s.owner<0&&(edge[0]||edge[1])){s.owner=edge[0]?0:1;s.pressed=in.now_ms;}
    if(s.owner>=0){o.owner=s.owner;o.ray=true;o.hold=in.now_ms-s.pressed>=20;}
    return o;
}
}
