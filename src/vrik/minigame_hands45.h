#pragma once
// IMMERSION / VrMinigameHands: Native uses authored hands and native buttons;
// SemiPassenger lets small tracked gestures pace verified native working actions.
// Henry retains authored reach/return and held-tool alignment. The pose gate here
// joins the current alchemy/smithing witness. semi_workstations46 publishes input,
// and exact child-action hooks own work playback; no controller-wide time scaling.
// These permissions are scoped to the current minigame, independent of F5/F10.
#include <atomic>
#include <cstdint>
namespace kcd2vr::minigame_hands45 {
enum Mode : int { Native = 0, SemiPassenger = 1, Gesture = 2, SixDofVrik = 3 };
// Saved numeric modes0/1/2 keep their existing meaning. Mode3 is the separate
// 6DOF presentation owner; both free modes retain tracked work/input support.
constexpr bool freeTrackedMode(int m) noexcept {return m==Gesture||m==SixDofVrik;}
constexpr bool sixDofMode(int m) noexcept {return m==SixDofVrik;}
enum Witness : unsigned { None = 0, Alchemy = 1, Smithing = 2 };
constexpr std::uint64_t kFreshMs = 80;
inline std::atomic<int> mode{SemiPassenger};
// G1: independent saved owners. -1 inherits the legacy shared setting until
// an explicit per-activity choice is loaded; existing settings are preserved.
inline std::atomic<int> alchemyMode67{-1},smithingMode67{-1};
inline std::atomic<std::uint64_t> modeEpoch67{1};
inline std::atomic<std::uint64_t> witness{0}; // (tick << 2) | Witness; 0 = no live minigame with native controls
constexpr int clampMode(int m) noexcept { return m >= Native && m <= SixDofVrik ? m : Native; }
constexpr std::uint64_t pack(unsigned kind, std::uint64_t tick) noexcept { return kind && tick ? (tick << 2) | (kind & 3u) : 0; }
constexpr unsigned kindOf(std::uint64_t w) noexcept { return unsigned(w & 3u); }
constexpr std::uint64_t tickOf(std::uint64_t w) noexcept { return w >> 2; }
inline int modeFor(unsigned kind) noexcept {
    const int selected=kind==Alchemy?alchemyMode67.load(std::memory_order_acquire):
        kind==Smithing?smithingMode67.load(std::memory_order_acquire):-1;
    return clampMode(selected<0?mode.load(std::memory_order_acquire):selected);
}
inline int currentMode() noexcept {return modeFor(kindOf(witness.load(std::memory_order_acquire)));}
inline void setMode(unsigned kind,int value) noexcept {
    auto& setting=kind==Alchemy?alchemyMode67:smithingMode67;
    if(setting.exchange(clampMode(value),std::memory_order_acq_rel)!=clampMode(value))
        modeEpoch67.fetch_add(1,std::memory_order_acq_rel);
}
inline bool witnessCurrent67(unsigned kind,std::uint64_t now) noexcept {
    const auto w=witness.load(std::memory_order_acquire);const auto tick=tickOf(w);
    return (kind==Alchemy||kind==Smithing)&&kindOf(w)==kind&&tick&&now>=tick&&now-tick<=kFreshMs;
}
inline bool gestureNow(unsigned kind,std::uint64_t now) noexcept {
    return freeTrackedMode(modeFor(kind))&&witnessCurrent67(kind,now);
}
// nullptr = the setting admits the tracked workstation wrist/body now; otherwise the literal refusal (diagnostic key).
constexpr const char* reason(int m, bool workstationEnabled, std::uint64_t w, std::uint64_t now) noexcept {
    if (clampMode(m) == Native) return "MINIGAME_HANDS_NATIVE";
    if (!workstationEnabled) return "WORKSTATION_INI_DISABLED";
    const unsigned kind = kindOf(w); const std::uint64_t tick = tickOf(w);
    if (kind != Alchemy && kind != Smithing) return "NO_MINIGAME_WITNESS";
    if (!tick || !now || now < tick || now - tick > kFreshMs) return "MINIGAME_WITNESS_STALE";
    return nullptr;
}
constexpr const char* label(int m) noexcept { return clampMode(m) == SixDofVrik ? "6DOF_VRIK" : clampMode(m) == Gesture ? "GESTURE" : clampMode(m) == SemiPassenger ? "SEMI_PASSENGER" : "NATIVE"; }
constexpr const char* witnessName(unsigned k) noexcept { return k == Alchemy ? "ALCHEMY" : k == Smithing ? "SMITHING" : "NONE"; }
inline void publish(unsigned kind, std::uint64_t now) noexcept { witness.store(pack(kind, now), std::memory_order_release); }
inline const char* reasonNow(bool workstationEnabled, std::uint64_t now) noexcept {
    const auto w=witness.load(std::memory_order_acquire);
    return reason(modeFor(kindOf(w)), workstationEnabled, w, now);
}
}
