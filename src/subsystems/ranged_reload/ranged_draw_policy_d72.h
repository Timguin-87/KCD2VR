#pragma once
#include <cstdint>
namespace kcd2vr::ranged_draw_d72 {
struct Identity {
    std::uintptr_t entity=0,self=0,attachment=0;
    std::uint64_t key=0,generation=0,body=0,request=0;
    std::uint32_t id=0;int side=-1;
};
inline bool same(const Identity& a,const Identity& b) noexcept {
    return a.entity&&a.self&&a.attachment&&a.key&&a.generation&&a.id&&a.side>=0&&a.side<2&&
        a.entity==b.entity&&a.self==b.self&&a.attachment==b.attachment&&a.key==b.key&&
        a.generation==b.generation&&a.body==b.body&&a.request==b.request&&a.id==b.id&&a.side==b.side;
}
struct Verification {bool read=false,periodic=false;};
inline Verification verification(bool first,bool pending,std::uint64_t now,std::uint64_t last) noexcept {
    const bool periodic=first||now<last||now-last>=500;
    return {pending||periodic,periodic};
}
// A skipped periodic read is never a first-placement acknowledgment.
inline bool mayReveal(bool read,bool succeeded) noexcept {return read&&succeeded;}
}
