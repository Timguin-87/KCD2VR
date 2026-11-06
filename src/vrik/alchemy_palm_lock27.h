#pragma once
#include "alchemy_input19c4.h"
namespace kcd2vr::alchemy_input19c4 {
// Worker consumes copied native selection only. No native lookup or Use call.
// One closed Grip cannot carry a target across activity restart/experiment reset.
struct PalmLock27 {
    std::uintptr_t instance=0;std::uint64_t lifetime=0,epoch=0;
    int owner=-1;std::uint32_t entity=0;bool armed=false;
    bool owns32(std::uintptr_t current,std::uint64_t life,std::uint64_t request,int side) const noexcept {
        return current&&life&&side>=0&&side<2&&instance==current&&lifetime==life&&epoch==request&&owner==side&&entity;
    }
    std::uint32_t update(bool allowed,std::uintptr_t current,std::uint64_t life,std::uint64_t request,
                         int side,const Selection& selection) noexcept {
        const bool same=allowed&&current&&life&&instance==current&&lifetime==life&&epoch==request;
        if(!same){instance=allowed?current:0;lifetime=allowed?life:0;epoch=request;
            owner=side;entity=0;armed=allowed&&side<0;return 0;}
        if(side<0){owner=-1;entity=0;armed=true;return 0;}
        if(side>1){owner=side;entity=0;armed=false;return 0;}
        if(owner!=side){owner=side;entity=armed&&selection.instance==current&&selection.lifetime==life?selection.entity:0;armed=false;}
        return entity;
    }
};
}
