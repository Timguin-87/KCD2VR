#pragma once
#include "subsystems/alchemy/bridge67.h"
#include <array>
#include <cmath>
#include <limits>

namespace kcd2vr::alchemy_head_name_d75 {
namespace b = subsystems::alchemy::bridge67;
namespace n = subsystems::alchemy::near67;
namespace g = subsystems::alchemy::geometry68;

// Display-only focus. Every box was already observed by the native station
// collector. This never nominates a hand target, grants Use, or calls a second
// selector. A direct visual ray chooses the nearest current box; no broad
// hand cone, previous action type, or synthetic item position is used.
inline n::Ref select(const b::NativeTable& table,
    const std::array<b::NativeOffer,n::Capacity>& offers,
    g::Point eye,g::Point forward,std::uint64_t now,double reach=2.) noexcept {
    n::Ref result{};
    if(!b::current(table,now)||!g::finite(eye)||!g::finite(forward)||
       !std::isfinite(reach)||reach<=0||reach>2.)return result;
    double nearest=std::numeric_limits<double>::infinity();
    for(unsigned i=0;i<n::Capacity;++i){
        const auto& observed=offers[i];const auto& offer=observed.offer;
        if(observed.instance!=table.instance||observed.player!=table.player||
           observed.lifetime!=table.lifetime||!n::valid(table.refs[i])||
           table.refs[i].slot!=int(i)||!n::same(offer.ref,table.refs[i])||
           !offer.valid||!offer.boundsValid||!n::fresh(offer.tick,now))continue;
        const auto hit=g::ray(offer.bounds,eye,forward,reach);
        if(hit.valid&&hit.distance<nearest){nearest=hit.distance;result=offer.ref;}
    }
    return result;
}
}
