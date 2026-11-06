#pragma once
#include <cmath>
#include <cstdint>
namespace kcd2vr::reading_pages42 {
// Native reading_base binds read_prev/read_next to LB/RB. Input pulses only;
// never calls a book action, changes pages directly or owns the camera.
struct Output { bool left=false,right=false; };
inline void apply(bool reading,Output page,bool& lb,bool& rb,float& lx,float& rx) noexcept {
    if(!reading)return;
    lb=page.left;rb=page.right;lx=rx=0; // held physical Grip cannot mask the native page-button edge
}
struct State {
    std::uint64_t lifetime=0,last=0,until=0;
    int direction=0;bool armed=false;
    Output step(bool reading,std::uint64_t owner,std::uint64_t now,float x) noexcept {
        if(!reading||!owner||!now||!std::isfinite(x)){*this={};return {};}
        if(lifetime!=owner||now<last){*this={};lifetime=owner;}
        last=now;
        if(std::abs(x)<.25f)armed=true;
        if(armed&&std::abs(x)>.60f){direction=x<0?-1:1;until=now+80;armed=false;}
        return {now<until&&direction<0,now<until&&direction>0};
    }
};
}
