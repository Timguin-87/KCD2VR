#pragma once

namespace kcd2vr::stereo_upscaler_d81r2 {
// D82 selection/save policy. Runtime additionally requires captured per-eye
// vendor contexts and successful matching L/R receipts before publication.
inline constexpr bool stereoPolicy(int productionMode) noexcept {return productionMode==3;}
// Historical compatibility for external fixtures; this means policy applies.
inline constexpr bool nativeRequired(int productionMode) noexcept {return stereoPolicy(productionMode);}
inline constexpr bool canSelect(int productionMode,int technology) noexcept {
    // D85 UPSCALERS: XeSS (4) uses the same per-eye bridge contract as DLSS/FSR.
    return !stereoPolicy(productionMode)||technology==0||technology==1||technology==2||technology==4;
}
inline constexpr bool canKeep(int productionMode,bool known,int actualTechnology) noexcept {
    return !stereoPolicy(productionMode)||(known&&canSelect(productionMode,actualTechnology));
}
}
