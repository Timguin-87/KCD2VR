#pragma once
// R112 HUD (Miki R111 report, 5 Nov):
//  1. the big directional combat crosshair (Scaleform clip CombatCursorCross, child of cccursor.actioncursor) was
//     drawn at native size on the full-FOV HUD quad = "so big it is not funny". It gets its own size slider
//     (VR HUD > ELEMENT CONTROLS "Combat crosshair size", INI VrCombatCrosshairScaleR112, default 50%).
//     Only the clip's own x/y scale is written (flags 0x18); the game keeps moving it (position, rotation, alpha,
//     visibility, the lock/perfect-block children), so the attack directions still point where the game says.
//  2. the quick weapon/potion selectors (QAMWeapon/QAMFood) never faded: the mod holds wh_ui_EnableAdaptiveHud=0
//     for the palm HUD (mod.cpp R57C1 lease, always wanted), and the native adaptive HUD is what faded them.
//     The mod now fades them itself (alpha only, never _visible): shown while the quick-access D-pad (left = weapon,
//     right = food) is used and for 3 s after, while the right palm shows the vitals, and when the game itself
//     re-shows one; then they fade out in 0.7 s. INI VrHudQuickSlotFadeR112 / Experiments BODY page 2 switch.
#include "ui_prompt_scale120r3.h"
#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>
#include <cstdint>

namespace kcd2vr::ui_hud_r112 {

// ---- 1. combat crosshair size ----
inline std::atomic<float> combatCrosshairScale{0.50f};
inline constexpr float kCombatCrosshairMin=0.25f,kCombatCrosshairMax=1.50f,kCombatCrosshairDefault=0.50f;
inline float CombatCrosshairScale() noexcept {
    const float s=combatCrosshairScale.load(std::memory_order_relaxed);
    return std::isfinite(s)?std::clamp(s,kCombatCrosshairMin,kCombatCrosshairMax):kCombatCrosshairDefault;
}
// Scale-only owner of the one clip (same NamedTransaction as the D69 action cursor: native position follows,
// a native scale reset is answered on the next child revision / HUD event, exit restores the native scale).
class CombatCrosshair : public ui_prompt_scale120r3::NamedTransaction<1> {
public:
    inline static constexpr std::array<const char*,1> Names{{"CombatCursorCross"}};
    CombatCrosshair() noexcept : NamedTransaction(Names,true,true) {}
    template<class Backend> bool Run(Backend& io,std::uint64_t generation,bool enabled,float scale) noexcept {
        return NamedTransaction::Run(io,generation,enabled,{std::clamp(scale,kCombatCrosshairMin,kCombatCrosshairMax)});
    }
};

// ---- 2. quick weapon/potion selector fade ----
inline std::atomic<bool> quickSlotFade{true};
// Last time (GetTickCount64 ms) the D-pad left/right quick-access state was sent to the game; 0 = never.
inline std::atomic<std::uint64_t> weaponQuickInputMs{0},foodQuickInputMs{0};
inline constexpr std::uint64_t kQuickHoldMs=3000,kQuickFadeMs=700;
struct QuickInput {
    bool enabled=false;     // VrHudQuickSlotFadeR112
    bool reveal=false;      // right palm shows the vitals: selectors visible too
    std::array<std::uint64_t,2> input{}; // weapon, food: last quick-access input ms
    std::uint64_t now=0,hudLifetime=0;
};
// Pure timing. Target alpha per selector: 100 shown, 0 faded, in between while fading; <0 = not owned (native).
class QuickFade {
    std::array<std::uint64_t,2> change_{};std::uint64_t lifetime_=~std::uint64_t{0};bool have_=false;
public:
    // A native re-show (the game made the selector visible / raised its alpha) counts as a change.
    void Changed(unsigned i,std::uint64_t now) noexcept {if(i<2&&now>change_[i])change_[i]=now;}
    std::uint64_t LastChange(unsigned i) const noexcept {return i<2?change_[i]:0;}
    std::array<float,2> Targets(const QuickInput& in) noexcept {
        std::array<float,2> out{-1.f,-1.f};
        // R115 (Miki 6 Nov: "turn them off - they turn on only by thumbrest + joystick left or right for selection and
        // then turn themselves off after 3 seconds"): ON = hidden; ONLY your quick-access selection shows the selector
        // (3 s, then 0.7 s fade). no show at a new HUD/load, no palm show, a native re-show does not count (the caller
        // ignores it). before R115 the R112 fade also showed them on load/palm/native re-show.
        if(!have_||in.hudLifetime!=lifetime_){have_=true;lifetime_=in.hudLifetime;change_={0,0};}
        if(!in.enabled)return out;
        for(unsigned i=0;i<2;++i){
            if(in.input[i]&&in.input[i]<=in.now&&in.input[i]>change_[i])change_[i]=in.input[i];
            if(!change_[i]){out[i]=0.f;continue;}
            const std::uint64_t age=in.now>=change_[i]?in.now-change_[i]:0;
            out[i]=age<=kQuickHoldMs?100.f:age>=kQuickHoldMs+kQuickFadeMs?0.f:
                100.f*(1.f-float(age-kQuickHoldMs)/float(kQuickFadeMs));
        }
        return out;
    }
};

} // namespace kcd2vr::ui_hud_r112
