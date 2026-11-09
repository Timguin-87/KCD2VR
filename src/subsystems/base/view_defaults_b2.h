#pragma once
#include <string_view>
namespace kcd2vr::view_defaults_b2 {
// User-approved graphics/view defaults, captured2026-10-19. Saved INI wins.
struct Value {std::string_view key;double value;};
inline constexpr Value Values[]={
 {"ApplyHeadPosition",1},
 {"ApplyHeadRotation",1},
 {"DiagnosticRendering",0},
 {"EnableVrBalancedGraphics",1},
 {"EnableVrOutputBridge",1},
 {"HeadPoseScale",1},
 {"VrActivityFovLock",1},
 {"VrAutoCenterXTrim",0},
 {"VrAutoCenterYTrim",0.08},
 {"VrAutoFovTrimDegrees",0},
 {"VrBrightness",-0.01},
 {"VrCameraBackOffsetMeters",0},
 {"VrCameraHeightOffsetMeters",0.035},
 {"VrCleanPresentationMode",0},
 {"VrContrast",1},
 {"VrCorrectedCenterScaleFovDegrees",95},
 {"VrCullFrustumScale",1},
 {"VrDepthStereoStrength",0.3},
 {"VrDialogueFovDegrees",95},
 {"VrDialogueFovOverride",0},
 {"VrDialogueZoomCompensation",0.5},
 {"VrDialogueZoomOverride",1},
 {"VrDisplayMode",0},
 {"VrEngineClarity",0.18},
 {"VrEyeAdaptHeadSteady",1},
 {"VrFlatScreenScale",1.05},
 {"VrFlatUiIpdMm",0},
 {"VrFpsCap",90},
 {"VrFpsCapEnabled",0},
 {"VrGameFov",95},
 {"VrGamma",1}, // R156: 1 = the game's picture (0.65 compensated the double sRGB encode of the eye blit)
 {"VrHorizontalFov",95},
 {"VrImmersiveFov",0},
 {"VrLeftOffsetX",0},
 {"VrLeftOffsetY",0},
 {"VrMergedMeshDistance",120},
 {"VrPassiveFullFrameScale",1.56}, // R160 UI: his INI (was 1.2)
 {"VrPoseSourceMode",0},
 {"VrPresentationMode",3},
 {"VrPresentationZoom",1},
 {"VrRenderScale",1},
 {"VrRightOffsetX",0},
 {"VrRightOffsetY",0},
 {"VrShadowQuality",0},
 {"VrShadowTweaks",0},
 {"VrSourceHeight",1800},
 {"VrSourceVerticalFovDegrees",95},
 {"VrSourceWidth",1920},
 {"VrStereoExperiment",3},
};
template<class T> constexpr T Get(std::string_view key,T fallback) noexcept {
 for(const auto& v:Values)if(v.key==key)return static_cast<T>(v.value);
 return fallback;
}
}
