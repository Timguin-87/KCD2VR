#pragma once
#include <cstddef>
#include <cstdint>

namespace kcd2vr::diagnostics::optional175 {
// Keep related request/return callbacks together.  Failure in one optional
// native group cannot remove independent recorder channels.
struct Group { std::size_t first,last; const char* name; };
inline constexpr Group lifecycle[] {
    {0,4,"PICKUP"}, {4,5,"HAND_CHANGE"}, {5,14,"ALCHEMY"},
    {14,16,"REMOVAL"}, {16,18,"CONSTRAINT"}, {18,23,"PHYSIC_PARAMS"}
};
template<class Validate,class Install,class Remove>
unsigned arm(Validate validate,Install install,Remove remove) noexcept {
    unsigned ready=0;
    for(unsigned g=0;g<sizeof(lifecycle)/sizeof(lifecycle[0]);++g) {
        const auto group=lifecycle[g];
        if(!validate(group))continue;
        bool ok=true;
        for(std::size_t i=group.first;i<group.last;++i) {
            if(!install(i)){ok=false;break;}
        }
        if(ok)ready|=1u<<g;
        else for(std::size_t i=group.first;i<group.last;++i)remove(i);
    }
    return ready;
}
}
