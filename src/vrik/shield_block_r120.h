#pragma once
#include <array>
#include <atomic>
#include <algorithm>
#include <cmath>
#include <cstdint>
// R120 SHIELD BLOCK BOX (Miki, 6 Nov: "whenever I am holding the shield, the game shows me block - so we need also a
// block box for the shield, that I can change, where and when - the shield should have also the speed modification").
// Before R120 a shield used the broad R40C1 rule in guard_parry176::position: grip anywhere 60 cm left/right of the
// shoulder centre, 0..min(1 m, chest distance) in front, 42 cm below to 58 cm above the shoulder line. A shield held
// at the belly or hanging a bit forward was inside it, so the native block stayed pressed all the time and every
// prepared right-hand attack was cancelled by it (native_combat104.inl PhysCancelAttack117 for side 1).
// ON (default): the shield (left hand grip) presses block only inside THIS box, entered below the entry speed after
// the dwell, released when it leaves the box or moves faster than the release speed, and not entered while the
// right-hand weapon prepares/strikes or for afterAttackMs after its attack. OFF: the old broad shield rule.
// Frame: body right / forward from the shoulders, centre = middle between the shoulders (the same chest point as
// the weapon guard). The weapon guard box (VrGuardPlane*, VrGuardBoxDepthD61) is not touched.
namespace kcd2vr::shield_block_r120 {
enum Slot : unsigned {Height,HalfHeight,Side,HalfWidth,Forward,Depth,EnterSpeed,DwellMs,ExitSpeed,AfterAttackMs,Count};
struct Spec {const char* key;const wchar_t* label;float lo,hi,def,step,shown;const wchar_t* format;};
inline constexpr std::array<Spec,Count> kSpecs{{
    {"VrShieldBoxHeightR120",L"Box centre height (0 = shoulders, minus = lower)",-.40f,.30f,-.08f,.01f,100.f,L"%+.0f cm"},
    {"VrShieldBoxHalfHeightR120",L"Shield box height",.08f,.45f,.24f,.01f,200.f,L"%.0f cm"},
    {"VrShieldBoxSideR120",L"Box centre: left (minus) / right (plus)",-.40f,.40f,-.05f,.01f,100.f,L"%+.0f cm"},
    {"VrShieldBoxHalfWidthR120",L"Shield box width",.10f,.60f,.32f,.01f,200.f,L"%.0f cm"},
    {"VrShieldBoxForwardR120",L"Box starts this far in front of the chest",0.f,.50f,.15f,.01f,100.f,L"%.0f cm"},
    {"VrShieldBoxDepthR120",L"Shield box depth (front to back)",.10f,.80f,.45f,.01f,100.f,L"%.0f cm"},
    {"VrShieldEnterMaxSpeedR120",L"Block ON: maximum shield speed (0 = any)",0.f,5.f,1.5f,.1f,1.f,L"%.1f m/s"},
    {"VrShieldEnterDwellMsR120",L"Block ON: shield in the box this long",0.f,500.f,100.f,10.f,1.f,L"%.0f ms"},
    {"VrShieldExitSpeedR120",L"Block release: shield speed (0 = never)",0.f,6.f,3.f,.1f,1.f,L"%.1f m/s"},
    {"VrShieldAfterAttackMsR120",L"No shield block after your attack for",0.f,1000.f,350.f,10.f,1.f,L"%.0f ms"}
}};
inline constexpr const char* kEnabledKey="VrShieldBlockBoxR120";
inline std::atomic<bool> enabled{true};
inline std::array<std::atomic<float>,Count> values{{kSpecs[0].def,kSpecs[1].def,kSpecs[2].def,kSpecs[3].def,kSpecs[4].def,
    kSpecs[5].def,kSpecs[6].def,kSpecs[7].def,kSpecs[8].def,kSpecs[9].def}};
// The block indicator says SHIELD BLOCK while the shield side owns the pressed native block.
inline std::atomic<bool> indicatorShield{false};
constexpr std::array<float,Count> defaults() noexcept {std::array<float,Count> d{};for(unsigned i=0;i<Count;++i)d[i]=kSpecs[i].def;return d;}
inline float clampValue(unsigned i,float v) noexcept {
    if(i>=Count)return 0.f;
    const auto& s=kSpecs[i];
    if(!std::isfinite(v))return s.def;
    v=std::clamp(v,s.lo,s.hi);return std::round(v/s.step)*s.step;
}
inline float get(unsigned i) noexcept {return i<Count?clampValue(i,values[i].load(std::memory_order_relaxed)):0.f;}
inline void set(unsigned i,float v) noexcept {if(i<Count)values[i].store(clampValue(i,v),std::memory_order_relaxed);}
inline bool on() noexcept {return enabled.load(std::memory_order_relaxed);}
inline unsigned dwellMs() noexcept {return unsigned(get(DwellMs)+.5f);}
inline std::uint64_t afterAttackMs() noexcept {return std::uint64_t(get(AfterAttackMs)+.5f);}
// x = body right, y = body forward, z = up, all metres from the shoulder centre. A held block keeps 4 cm of
// hysteresis on every face so a shaking arm does not flicker the native block.
inline bool inside(double x,double y,double z,bool held,double maxForward) noexcept {
    if(!std::isfinite(x)||!std::isfinite(y)||!std::isfinite(z))return false;
    const double m=held?.04:0.;
    const double cx=get(Side),hw=get(HalfWidth),cz=get(Height),hh=get(HalfHeight),f0=get(Forward),depth=get(Depth);
    const double yHi=std::min(double(f0)+depth+m,std::isfinite(maxForward)?maxForward:1e9);
    return x>=cx-hw-m&&x<=cx+hw+m&&z>=cz-hh-m&&z<=cz+hh+m&&y>=f0-m&&y<=yHi;
}
}
