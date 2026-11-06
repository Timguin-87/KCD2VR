#pragma once
#include <cstdint>

namespace kcd2vr::crossbow_hand_r91 {
// Identity of the whole gesture, including the part before an ammo entity exists.
struct Owner {
    std::uintptr_t player=0,equipment=0;
    std::uint64_t key=0,generation=0,epoch=0,request=0,ammo=0;
    int side=-1;
    bool valid() const noexcept {return player&&equipment&&key&&epoch&&request&&side==1;}
};
inline bool same(const Owner& a,const Owner& b) noexcept {
    return a.player==b.player&&a.equipment==b.equipment&&a.key==b.key&&
        a.generation==b.generation&&a.epoch==b.epoch&&a.request==b.request&&a.ammo==b.ammo&&a.side==b.side;
}
struct Admission {Owner owner{};std::uint64_t tick=0;bool runnable=false;};
inline bool current(const Admission& a,std::uint64_t now,std::uintptr_t equipment,
                    std::uint64_t epoch,std::uint64_t request) noexcept {
    return a.runnable&&a.owner.valid()&&a.owner.equipment==equipment&&a.owner.epoch==epoch&&
        a.owner.request==request&&a.tick&&now>=a.tick&&now-a.tick<=80;
}
// A partially changed native transaction must retire before another route runs.
// This is distinct from ordinary admission: disabling a mode is not retirement.
inline bool intercept(const Admission& a,std::uint64_t now,std::uintptr_t equipment,
                      std::uint64_t epoch,std::uint64_t request,bool enabled,bool fault,
                      unsigned char requestA,unsigned char requestB,std::uintptr_t retained) noexcept {
    if(!equipment||requestB!=0||requestA>1)return false;
    if(retained==equipment)return true;
    return enabled&&!fault&&current(a,now,equipment,epoch,request);
}
}
