#pragma once
#include <array>
#include <atomic>
#include <algorithm>
#include <cmath>
namespace kcd2vr::melee_tuning_c54 {
inline constexpr unsigned classCount=17;
struct Values {float cut=1.f,stab=1.f,weight=.6f,fatigue=.6f,stats=1.f,intentH29=1.f,sensitivityH29=1.f,travelR8=1.f;};
inline float bound(float v,float lo,float hi,float fallback) noexcept {return std::isfinite(v)?std::clamp(v,lo,hi):fallback;}
inline constexpr float kWeaponSensitivityMinD65=.25f;
inline Values bounded(Values v) noexcept {return {bound(v.cut,.5f,2.f,1.f),bound(v.stab,.5f,2.f,1.f),bound(v.weight,.2f,2.f,.6f),bound(v.fatigue,0.f,1.f,.6f),bound(v.stats,0.f,1.5f,1.f),bound(v.intentH29,.5f,2.f,1.f),bound(v.sensitivityH29,kWeaponSensitivityMinD65,2.f,1.f),bound(v.travelR8,.4f,2.f,1.f)};}
struct Entry {std::atomic<float> cut{1.f},stab{1.f},weight{.6f},fatigue{.6f},stats{1.f},intentH29{1.f},sensitivityH29{1.f},travelR8{1.f};};
inline std::array<Entry,classCount> entries{};
inline std::atomic<bool> bypassExtraAttackVetoesH41{false};
inline std::atomic<float> punchSensitivity{1.f},punchTravel{1.f},guardHalfWidth{.50f},guardHalfHeight{.20f},guardEntrySpeed{3.f},guardReleaseSpeed{4.f};
inline std::atomic<float> blockSensitivityH29{1.f};
// D56: one body-distance policy for the shared weapon/shield guard predicate.
// D61 pending: face-level box front-to-back extent; .50 preserves the D60 envelope.
inline std::atomic<float> guardBoxDepthD61{.50f};
inline std::atomic<float> guardBodyDistanceD56{.65f};
inline double guardDistanceD56(bool held) noexcept {
    return double(bound(guardBodyDistanceD56.load(std::memory_order_relaxed),.35f,1.05f,.65f))+(held?.05:0.);
}
// Global blade-axis classification, shared by forward thrusts and reverse pommel strokes.
inline std::atomic<float> bladeAxisCosineH41{.80f};
inline float bladeAxisSettingH41(float v) noexcept {return std::isfinite(v)&&v>=.60f&&v<=.95f?v:.80f;}
inline double bladeAxisH41() noexcept {const float v=bladeAxisSettingH41(bladeAxisCosineH41.load(std::memory_order_relaxed));return v==.80f?.80:double(v);}
inline float blockH29() noexcept {return bound(blockSensitivityH29.load(std::memory_order_relaxed),.5f,1.5f,1.f);}
inline bool valid(int id) noexcept {return id>=0&&id<int(classCount);}
inline bool weapon(int id) noexcept {return (id>=0&&id<=7)||id==16;}
inline Values read(int id) noexcept {if(!valid(id))return {};auto& e=entries[unsigned(id)];return bounded({e.cut.load(std::memory_order_relaxed),e.stab.load(std::memory_order_relaxed),e.weight.load(std::memory_order_relaxed),e.fatigue.load(std::memory_order_relaxed),e.stats.load(std::memory_order_relaxed),e.intentH29.load(std::memory_order_relaxed),e.sensitivityH29.load(std::memory_order_relaxed),e.travelR8.load(std::memory_order_relaxed)});}
inline void set(int id,Values v) noexcept {if(!valid(id))return;v=bounded(v);auto& e=entries[unsigned(id)];e.cut.store(v.cut,std::memory_order_relaxed);e.stab.store(v.stab,std::memory_order_relaxed);e.weight.store(v.weight,std::memory_order_relaxed);e.fatigue.store(v.fatigue,std::memory_order_relaxed);e.stats.store(v.stats,std::memory_order_relaxed);e.intentH29.store(v.intentH29,std::memory_order_relaxed);e.sensitivityH29.store(v.sensitivityH29,std::memory_order_relaxed);e.travelR8.store(v.travelR8,std::memory_order_relaxed);}
inline float punch() noexcept {return bound(punchSensitivity.load(std::memory_order_relaxed),.5f,2.f,1.f);}
}
