#pragma once
#include <atomic>
#include <cstdint>

namespace kcd2vr::npc_difficulty {
// 0: current native values, including native perk/table changes.
// 1: the ten earlier C8 scalar targets at automatic-combat formula inputs.
// This does not replay the old PAK or change Henry's manual timing windows.
inline std::atomic<float> difficulty{0.f};
bool Install(std::uintptr_t module) noexcept;
void Shutdown() noexcept;
bool Ready() noexcept;
}

namespace kcd2vr::npc_unarmed_spacing {
// Positive scale of the current NPC unarmed movement ring's native base.
// 1 is exactly native; smaller values ask the native movement goal to be
// closer. Native tactical/target adjustments, attack reach and Henry remain
// unchanged. This is not an automatic punch, hit or movement acceptance.
// New boxing preference at the existing native movement boundary. This does
// not change attack reach or guarantee an absolute standing distance.
inline constexpr float defaultPercent=60.f;
inline constexpr int settingsVersion=1;
inline std::atomic<float> scale{defaultPercent*.01f};
bool Install(std::uintptr_t module) noexcept;
void Shutdown() noexcept;
bool Ready() noexcept;
const char* Failure() noexcept;
}
