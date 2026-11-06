#pragma once
#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>
namespace kcd2vr::thrust_tuning_d67 {
struct Values { float speed=1.25f, travel=.15f; };
inline Values defaults(int id) noexcept { return {1.25f,id==4?.1275f:.15f}; }
inline Values bounded(Values v) noexcept {
    return {std::isfinite(v.speed)?std::clamp(v.speed,.65f,3.f):1.25f,
            std::isfinite(v.travel)?std::clamp(v.travel,.06f,.30f):.15f};
}
struct Bank {
    std::array<std::atomic<float>,17> speed{},travel{};
    Bank() noexcept { for(int i=0;i<17;++i){const auto v=defaults(i);speed[i].store(v.speed);travel[i].store(v.travel);} }
};
inline Bank bank;
inline std::atomic<bool> enabled{true};
inline Values read(int id) noexcept { id=std::clamp(id,0,16);return bounded({bank.speed[id].load(),bank.travel[id].load()}); }
inline void set(int id,Values v) noexcept {id=std::clamp(id,0,16);v=bounded(v);bank.speed[id].store(v.speed);bank.travel[id].store(v.travel);}
// Admission only. Never scales damage, native stamina or a counter/master award.
template<class P> inline void apply(int id,P& p) noexcept {
    const auto v=read(id);p.independentThrustD67=enabled.load();
    p.thrustOnsetD67=v.speed;p.thrustTravelD67=v.travel;
}
}
