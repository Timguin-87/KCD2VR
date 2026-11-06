#pragma once
#include <cstdint>
namespace kcd2vr::impact59c6 {
struct Key {
 std::uint64_t tick=0,sample=0,weapon=0,generation=0,epoch=0,request=0,attempt=0;
 std::uint32_t entity=0;std::uintptr_t collider=0;int part=-1;
 bool operator==(const Key& b)const noexcept {return tick==b.tick&&sample==b.sample&&weapon==b.weapon&&generation==b.generation&&epoch==b.epoch&&request==b.request&&attempt==b.attempt&&entity==b.entity&&collider==b.collider&&part==b.part;}
};
template<class R> Key key(const R& r) noexcept {return {r.tick,r.sample,r.key,r.generation,r.epoch,r.request,r.gesture59c3,r.entityId,r.collider,r.part};}
struct Seen {
 Key last{};bool have=false;
 template<class R> bool take(const R& r) noexcept {if(!r.valid||!r.tick||!r.sample)return false;const auto k=key(r);if(have&&k==last)return false;last=k;have=true;return true;}
 void clear()noexcept{last={};have=false;}
};
}
