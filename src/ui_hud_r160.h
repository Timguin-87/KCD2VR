#pragma once
// R160 UI (Miki 8 Nov): "UI for the others (non-dev) should have only a unified slider for icon size (rescaling all
// icons except minigames) and a separate slider for minigames icon size" + "a tutorials position slider would be nice -
// only for dev build".
//
// 1. VrUiUnifiedScaleR160 (default 1.0 = every HUD element exactly as big as before, range 0.50..1.50). one factor on
//    top of the stored per-element values (the dev pages keep their own numbers, nothing is rewritten):
//    - the readable HUD parents (tl tc tr cl cr cc bl bc br popUpVisual tutorialInDialog) get it through their parent
//      icon scale (VrAutoHudIconScale x factor, same .50..6 clamp as ComputeAdjustableReadable49). every child of them
//      (compass, health, quick slots, hints, control/tutorial prompts, subtitles, dialogue, quest text, status icons,
//      popups) grows with its parent, so their own per-element sizes are NOT multiplied a second time (that would be
//      factor squared).
//    - the clips outside those parents (cccursor = world-projected cursor family) get it on their own scale:
//      combat crosshair (VrCombatCrosshairScaleR112, clamp .25..1.50), the A grab / open item cursor (CursorOutline /
//      CursorCross, control prompt size, clamp .25..1.50), the yellow InfoText line (hints size, clamp .25..1.00).
//    - crafting / minigame prompts (VrMinigamePromptScale) are children of the readable parents too, so the station
//      size sent to the movie is divided by the factor: the crafting icons keep exactly their own slider size.
// 2. VrUiTutorialYR160 (default 0 = native place, stage px of the readable box, + = down, -540..540): moves the two
//    parents that hold the tutorial box: tl (tl.tutorial = TutorialMessage, the gameplay tutorial / control prompt
//    box; HUD.xml declares nothing else under tl) and tutorialInDialog (TutorialInDialog). same readable owner and
//    transaction as the R160 dialogue offset (ui_native_canvas Fit.tutorialY160).
#include <algorithm>
#include <atomic>
#include <cmath>

namespace kcd2vr::ui_hud_r160 {
inline constexpr float kUnifiedMin=.50f,kUnifiedMax=1.50f,kUnifiedDefault=1.f;
inline constexpr float kTutorialYMin=-540.f,kTutorialYMax=540.f,kTutorialYDefault=0.f;
inline std::atomic<float> unifiedScale{kUnifiedDefault}; // VrUiUnifiedScaleR160
inline std::atomic<float> tutorialY{kTutorialYDefault};   // VrUiTutorialYR160

inline float ClampUnified(float u) noexcept {return std::isfinite(u)?std::clamp(u,kUnifiedMin,kUnifiedMax):kUnifiedDefault;}
inline float ClampTutorialY(float y) noexcept {return std::isfinite(y)?std::clamp(y,kTutorialYMin,kTutorialYMax):kTutorialYDefault;}
inline float Unified() noexcept {return ClampUnified(unifiedScale.load(std::memory_order_acquire));}
inline float TutorialY() noexcept {return ClampTutorialY(tutorialY.load(std::memory_order_acquire));}

// readable parent icon scale (VrAutoHudIconScale x factor). a non-finite stored value is passed on unchanged: the
// consumer (ComputeAdjustableReadable49) refuses it exactly as before.
inline float ParentIconScale(float stored,float u) noexcept {
    if(!std::isfinite(stored))return stored;
    return std::clamp(stored*ClampUnified(u),.50f,6.f);
}
// a clip outside the readable parents: its own size x factor inside the consumer's own clamp
inline float OwnScale(float stored,float u,float lo,float hi) noexcept {
    if(!std::isfinite(stored))return stored;
    return std::clamp(stored*ClampUnified(u),lo,hi);
}
// crafting / minigame station size sent to the movie: divided by the factor (the parents already carry it), same
// .25..1 clamp as prompt_policy_d80::State. factor 1 = the slider value unchanged.
inline float StationScale(float minigame,float u) noexcept {
    if(!std::isfinite(minigame))return minigame;
    return std::clamp(minigame/ClampUnified(u),.25f,1.f);
}
}
