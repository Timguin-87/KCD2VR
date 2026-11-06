#pragma once
#include <cstdint>
namespace kcd2vr::ui_pause_page59c19 {
struct Owner {std::uintptr_t menu=0;std::uint64_t generation=0;};
// Actual producer policy used by the pinned ShowPage hook. Native page setup
// runs once for ALL receivers. Only the same still-owned Menu instance emits
// a layout event afterward. No display reads/writes or polling in this path.
template<class Read,class GetOwner,class Native> bool Observe(std::uintptr_t wrapper,
    Read&& read,GetOwner&& owner,Native&& native) {
    const auto before=owner();std::uintptr_t element=0;
    const bool ours=wrapper&&before.menu&&before.generation&&
        read(wrapper+0x48,element)&&element==before.menu;
    native();
    if(!ours)return false;
    const auto after=owner();
    return after.menu==before.menu&&after.generation==before.generation&&
        read(wrapper+0x48,element)&&element==after.menu;
}
}
