#pragma once
#include <algorithm>
#include <cmath>
#include "dialogue_state59c10r2.h"

namespace kcd2vr::dialogue_presentation19c2 {
// An immersive camera request owns its lens policy even if a particular render
// could not reconstruct Henry's eye. A transient success receipt must never
// switch the submitted picture between full size and a diagnostic half-size.
inline float zoom(bool guard,bool overrideZoom,int mode,float requested) noexcept {
    if(!guard||!overrideZoom||mode==2)return 1.f;
    return std::isfinite(requested)?std::clamp(requested,.05f,2.50f):1.f;
}
struct Request {std::uint64_t tick=0;bool wanted=false;};
inline bool requested(Request r,std::uint64_t now) noexcept {
    return r.wanted&&dialogue_state59c10r2::age(r.tick,now,80);
}
inline bool chooseFirstPerson(Request r,const dialogue_state59c10r2::State& current,std::uint64_t now) noexcept {
    return requested(r,now)&&dialogue_state59c10r2::embodiedOwner(current,now);
}
// Native9E58CC calls dialog-camera manager vfn0 at9E598F. Its selector
// owns pointer changes, activation/deactivation, visibility and event0x26.
template<class Read> bool selectorOwner(std::uintptr_t base,std::uintptr_t self,Read&& read) noexcept {
    std::uintptr_t vf=0,game=0,module=0,manager=0,cameras=0,owner=0,cached=0;
    return base&&self&&read(self,vf)&&vf==base+0x3E63CA8&&
        read(base+0x549D388,game)&&game&&read(game+0x150,module)&&module&&
        read(module,vf)&&vf==base+0x3E64C38&&read(module+0x10,manager)&&manager&&
        read(manager,vf)&&vf==base+0x3E64C90&&read(manager+0x188,owner)&&owner==self&&
        read(game+0x38,cameras)&&cameras&&read(cameras,vf)&&vf==base+0x408E560&&
        read(cameras+0x318,vf)&&vf==base+0x3A5A210&&read(cameras+0x328,cached)&&cached==self;
}
// A failed disarm must retain the trampoline and owner. The surviving detour
// still forwards its native original while visual intent is revoked.
template<class Hook,class Fn> bool releaseHook(Hook& hook,Fn& original) noexcept {
    if(hook&&!hook->disable())return false;
    hook.reset();original=nullptr;return true;
}
}
