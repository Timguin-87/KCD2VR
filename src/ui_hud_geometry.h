#pragma once
#include <array>
#include <cstdint>
#include <cstring>
#include <algorithm>
#include <cmath>
namespace kcd2vr::ui_hud_geometry {
// Manual borders move semantic top/bottom anchors independently. Centered
// prompts keep the measured surface center, not the asymmetric rectangle midpoint.
struct ManualVertical30c1 {float top,bottom,center;};
inline ManualVertical30c1 ManualBorders30c1(float top,float bottom,float upper,float lower) noexcept {
    const float center=(top+bottom)*.5f,half=(bottom-top)*.5f;
    upper=std::isfinite(upper)?std::clamp(upper,.55f,3.f):1.f;
    lower=std::isfinite(lower)?std::clamp(lower,.55f,3.f):1.f;
    return {std::clamp(center-half*upper,-1080.f,540.f),std::clamp(center+half*lower,540.f,2160.f),center};
}
// Validated retail SFlashDisplayInfo layout. ONLY X/Y/XScale/YScale are writable.
// No alpha, visibility, rotation, perspective, or native animation state flags.
inline std::array<unsigned char,0x38> Pack(float x,float y,float xs,float ys) noexcept {
    std::array<unsigned char,0x38> info{};
    std::memcpy(info.data(),&x,4);std::memcpy(info.data()+4,&y,4);
    std::memcpy(info.data()+0x0C,&xs,4);std::memcpy(info.data()+0x10,&ys,4);
    const std::uint16_t flags=0x1B;std::memcpy(info.data()+0x34,&flags,2);return info;
}
inline std::array<unsigned char,0x38> Restore(const std::array<unsigned char,0x38>& baseline) noexcept {
    float x,y,xs,ys;std::memcpy(&x,baseline.data(),4);std::memcpy(&y,baseline.data()+4,4);
    std::memcpy(&xs,baseline.data()+0x0C,4);std::memcpy(&ys,baseline.data()+0x10,4);return Pack(x,y,xs,ys);
}
} // namespace
