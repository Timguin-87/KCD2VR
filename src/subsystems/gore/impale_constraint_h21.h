#pragma once
#include "../../native_physics_phase_t4.h"
#include "../physics/runtime/native_weapon_body_t4.h"
namespace kcd2vr::impale_constraint_h21 {
struct Input {
 std::uint64_t serial=0,tick=0,bodyGeneration=0,physicsEpoch=0;
 native_physics_phase_t4::ArticulatedSource source{};
 native_weapon_body_t4::Owner weapon{};
 double grip[3]{},tip[3]{},wound[3]{};
 bool nativeDeadCurrent=false;
};
// Producer already joined native death, current corpse/relinquish/raw part and
// equipped source after callbacks. T4 independently revalidates its typed leases.
bool Submit(const Input&) noexcept;
void Cancel(std::uint64_t serial) noexcept;
bool MotorBlocked() noexcept;
const char* Failure() noexcept;
void Refuse(const char*) noexcept;
}
