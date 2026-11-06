#pragma once
#include <cmath>
#include <cstdint>
namespace kcd2vr::immersive_fov59c7g {
struct Policy {std::uint64_t tick=0;float radians=0;bool immersive=false;};
inline float select(const Policy& p,std::uint64_t now,float native,bool exactView,bool customLens) noexcept {
 if(!p.immersive||!exactView||customLens||!p.tick||now<p.tick||now-p.tick>80||
    !std::isfinite(native)||native<=.1f||native>=3.1f||!std::isfinite(p.radians)||
    p.radians<1.0f||p.radians>2.45f)return native;
 return p.radians;
}
bool Install() noexcept;
void Uninstall() noexcept;
void Update(bool immersive,float gameFovDegrees,std::uint64_t now) noexcept;
}
