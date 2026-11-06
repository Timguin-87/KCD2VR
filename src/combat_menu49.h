#pragma once
// C49 UI (Miki 17 Oct): the first fight-experiments page shows only the main sliders + FREE SWING STARTS AT ONCE.
// Pure layout and value mapping, host tested (tests/combat_menu49_test.cpp). No native API, no state of its own.
namespace kcd2vr::combat_menu49 {
struct Rect { int l, t, r, b; };
constexpr int kSliderLabel = 25;                 // DrawSlider draws its label + value badge at r.t-25 .. r.t-3
constexpr Rect kClip{570, 45, 1070, 672};        // DrawPanel right page clip (IntersectClipRect)
constexpr Rect kHeader{600, 32, 1040, 102};      // title, subtitle, BACK + VIEW/UI/CONTROLS row
// EXPERIMENTS (first fight page): 4 sliders, 1 checkbox, the C45 kill-move row (impale / decapitation), short note, 2 page buttons
constexpr Rect kProximity{620, 150, 1038, 172}, kBlockSpeed{620, 222, 1038, 244}, kWeight{620, 294, 1038, 316}, kSkill{620, 366, 1038, 388};
constexpr Rect kInstantFree{620, 410, 1038, 438};
constexpr Rect kImpaleKill{620, 446, 824, 474}, kDecapKill{834, 446, 1038, 474}; // C45: half width each, shared state with IMMERSION / COUNTER STAB
constexpr Rect kDefaults54{620,482,824,514}, kWindowsH41{834,482,1038,514};
constexpr Rect kShowBlockH24{620,524,1038,550};
constexpr Rect kNote{620, 556, 1038, 586};
constexpr Rect kMore{620, 596, 1038, 626}, kFlow{620, 634, 1038, 664};
// GRAPHICS row 234..256: SHADOW QUALITY LOW / MEDIUM / HIGH + the brightness button (was 2 buttons of 206 px)
constexpr Rect kGfxShadow[3]{{620, 234, 700, 256}, {704, 234, 784, 256}, {788, 234, 868, 256}};
constexpr Rect kGfxEye{874, 234, 1038, 256};

constexpr Rect extent(Rect r, bool slider) noexcept { return {r.l, slider ? r.t - kSliderLabel : r.t, r.r, r.b}; }
constexpr bool overlap(Rect a, Rect b) noexcept { return a.l < b.r && b.l < a.r && a.t < b.b && b.t < a.b; }
constexpr bool inside(Rect a, Rect o) noexcept { return a.l >= o.l && a.r <= o.r && a.t >= o.t && a.b <= o.b; }
struct Item { Rect r; bool slider; };
constexpr Item kPage[]{{kProximity, true}, {kBlockSpeed, true}, {kWeight, true}, {kSkill, true}, {kInstantFree, false},
    {kImpaleKill, false}, {kDecapKill, false}, {kDefaults54, false}, {kWindowsH41, false}, {kShowBlockH24, false}, {kNote, false}, {kMore, false}, {kFlow, false}};
constexpr unsigned kCount = sizeof(kPage) / sizeof(kPage[0]);
// compile-time guard shared by vr_menu.cpp (static_assert) and the host test: no overlap, inside the clip, under the header row
constexpr bool clean() noexcept {
    for (unsigned i = 0; i < kCount; ++i) {
        const Rect e = extent(kPage[i].r, kPage[i].slider);
        if (!(e.l < e.r && e.t < e.b) || !inside(e, kClip) || e.t < kHeader.b) return false;
        for (unsigned j = i + 1; j < kCount; ++j) if (overlap(e, extent(kPage[j].r, kPage[j].slider))) return false;
    }
    return true;
}

inline float clamp01(float t) noexcept { return t == t ? (t < 0.f ? 0.f : (t > 1.f ? 1.f : t)) : 0.f; }
inline float roundTo(float v, float step) noexcept { const float q = v / step; return float(long(q + (q < 0.f ? -.5f : .5f))) * step; }

// 1) proximity distance = VrNpcProximityRadius36 (blade-to-enemy-torso reference radius, 0.20..0.60 m, 1 cm steps)
constexpr float kProxMin = .20f, kProxMax = .60f;
inline float proximityFromT(float t) noexcept { return roundTo(kProxMin + clamp01(t) * (kProxMax - kProxMin), .01f); }
inline float proximityT(float m) noexcept { return clamp01((m - kProxMin) / (kProxMax - kProxMin)); }

// 2) NPC block speed: ONE slider 1.0..3.0 for the C39 VR DIRECT block-raise speed-up. The slider is the master-skill
//    value (VrDirectNpcBlockSpeedHigh); the low-skill value follows at half the speed-up (Low = 1 + (s-1)/2), so the
//    C39 defaults Low 2.0 / High 3.0 are exactly s = 3.0. s = 1.0 = native (VrDirectNpcBlockBySkill off).
struct Block { bool bySkill = true; float lo = 2.f, hi = 3.f; };
constexpr float kBlockMin = 1.f, kBlockMax = 3.f;
inline Block blockFromSlider(float s) noexcept {
    s = s == s ? (s < kBlockMin ? kBlockMin : (s > kBlockMax ? kBlockMax : s)) : kBlockMin;
    s = roundTo(s, .1f);
    if (s <= 1.0001f) return {false, 1.f, 1.f};
    return {true, roundTo(1.f + (s - 1.f) * .5f, .05f), s};
}
inline float blockFromT(float t) noexcept { return roundTo(kBlockMin + clamp01(t) * (kBlockMax - kBlockMin), .1f); }
inline float blockSlider(bool bySkill, float hi) noexcept { return bySkill ? (hi < kBlockMin ? kBlockMin : (hi > kBlockMax ? kBlockMax : hi)) : kBlockMin; }
inline float blockT(bool bySkill, float hi) noexcept { return (blockSlider(bySkill, hi) - kBlockMin) / (kBlockMax - kBlockMin); }

// 3) weapon weight and drag = VrWeaponWeightFeel (arm authority 0.20..2.00, lower = heavier + more drag), same as WEAPON FEEL
inline float weightFromT(float t) noexcept { return roundTo(.2f + clamp01(t) * 1.8f, .05f); }
inline float weightT(float w) noexcept { return clamp01((w - .2f) / 1.8f); }

// 4) skill simulation = VrSimulateHenry: 0 OFF (real stats), 1 LOW, 2 MEDIUM, 3 HIGH - 4 snapping stops
inline int skillFromT(float t) noexcept { return int(clamp01(t) * 3.f + .5f); }
inline float skillT(int s) noexcept { return s <= 0 ? 0.f : (s >= 3 ? 1.f : float(s) / 3.f); }
// 5) C45 IMPALE ON KILL STAB: ON/OFF over VrImpaleHang (0 off, 1 light, 2 normal, 3 heavy). OFF keeps the detailed choice (lastOn,
//    remembered by the mod while non-zero); ON restores it, 2 (normal) when nothing was ever chosen. The 1/2/3 pick stays on IMMERSION > EXPERIMENTS.
constexpr int impaleToggle(int hang, int lastOn) noexcept { return hang > 0 ? 0 : (lastOn >= 1 && lastOn <= 3 ? lastOn : 2); }
// 6) C45/C46 DECAPITATION kill move: the IMMERSION cycle OFF -> HIDE HEAD -> LOOSE NECK -> STUMP+HEAD (VrDecapitation + VrDecapitationStyle 0/1/2)
//    as one value 0 OFF, 1 HIDE HEAD, 2 LOOSE NECK, 3 STUMP+HEAD; all pages share the state, so the cycle is the same function.
constexpr int decapState(bool on, int style) noexcept { return !on ? 0 : (style == 2 ? 3 : style == 1 ? 2 : 1); }
constexpr int decapNext(int state) noexcept { return state == 0 ? 1 : (state == 1 ? 2 : (state == 2 ? 3 : 0)); }
}

namespace kcd2vr::combat_menu49 {
template<class State> inline void defaults54(State& s) noexcept {
 const State d{};
 s.exp_npc_proximity_radius36=d.exp_npc_proximity_radius36;
 s.exp_direct_block_timing39=d.exp_direct_block_timing39;s.exp_direct_block_lo39=d.exp_direct_block_lo39;s.exp_direct_block_hi39=d.exp_direct_block_hi39;
 s.exp_weight_feel=d.exp_weight_feel;s.stat_sim26=0;
 s.exp_telegraph_windup=false;s.exp_windup_hold=false;s.exp_telegraph_strike=true;
 s.exp_guard_strict=false;s.exp_guard_enter_speed=2.5f;s.exp_guard_enter_dwell_ms=0;
 s.exp_guard_exit_speed=3.5f;s.exp_guard_auto_windup=false;s.exp_guard_counter_motion=true;
}
constexpr int dodgeMode54(int mode) noexcept {return mode==2?1:mode==3?0:mode==1?2:3;}
}
