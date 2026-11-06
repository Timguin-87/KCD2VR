#pragma once
#include <cmath>
namespace kcd2vr::defaults_migration111 {
inline bool Near(float a,float b,float eps=0.0005f) noexcept { return std::fabs(a-b)<=eps; }
inline bool LegacyCameraHeight(float h) noexcept { return Near(h,0.0f); }
inline bool LegacyMergedCanvas(bool enabled,float size,float spreadX,float spreadY) noexcept {
    return enabled&&Near(size,1.00f,0.005f)&&Near(spreadX,0.78f,0.005f)&&Near(spreadY,0.78f,0.005f);
}
}
