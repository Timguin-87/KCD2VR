#pragma once
#include <cstdint>
namespace kcd2vr::gore_eligibility_d56 {
enum class Result {Pending,Refused,MaleHuman};
struct Owner {std::uint32_t target=0;std::uintptr_t entity=0,character=0;std::uint64_t epoch=0;};
inline bool same(const Owner& a,const Owner& b) noexcept {
    return a.target&&a.entity&&a.character&&a.epoch&&a.target==b.target&&a.entity==b.entity&&a.character==b.character&&a.epoch==b.epoch;
}
struct Request {Owner owner{};std::uint64_t serial=0,tick=0;};
struct Receipt {Request request{};std::uint64_t tick=0,vm=0;Result result=Result::Refused;};
inline bool fresh(const Receipt& r,const Owner& o,std::uint64_t now) noexcept {
    return same(r.request.owner,o)&&r.request.serial&&r.vm&&r.tick&&now>=r.tick&&now-r.tick<=80;
}
}
