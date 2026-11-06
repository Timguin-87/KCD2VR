#pragma once
#include "native_physics_phase_t4.h"
#include <cmath>
#include <cstdint>

namespace kcd2vr::native_weapon_clearance_t4 {
enum class Result : unsigned {Unknown,Clear,Occupied,CallerUnavailableH5};
struct Capsule {native_physics_phase_t4::V3 center{},axis{0,1,0};float radius=0,halfHeight=0;};
static_assert(sizeof(Capsule)==32);
inline bool Valid(const Capsule& c) noexcept {
    const auto& a=c.axis;const double n=double(a.x)*a.x+double(a.y)*a.y+double(a.z)*a.z;
    return std::isfinite(c.center.x)&&std::isfinite(c.center.y)&&std::isfinite(c.center.z)&&
        std::isfinite(n)&&std::abs(n-1)<.01&&std::isfinite(c.radius)&&c.radius>=.005f&&c.radius<=.25f&&
        std::isfinite(c.halfHeight)&&c.halfHeight>=.05f&&c.halfHeight<=1.25f;
}
inline Result Classify(float result) noexcept {
    return !std::isfinite(result)||result<0?Result::Unknown:result==0?Result::Clear:Result::Occupied;
}
// Own primitive copy only; never retains native contact/geometry pointers.
// Step already owns query slot4. A default/unsupported native caller cannot
// recursively acquire that exclusive slot; it is refused before the query.
Result Query(native_physics_phase_t4::Context&,Capsule,void* ownBody) noexcept;
bool Install() noexcept;
}
