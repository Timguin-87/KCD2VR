// Runs the same complete production detour fixture on MSVC. No native ABI emulation.
#define _ReturnAddress() testCaller168
#include "quiet_hit_feedback168_test.cpp"
// MSVC resolves otherwise-unused complete-TU leaves before dead stripping.
// These are absent services outside the exercised SubmitHitDetour, not replacements for its production checks.
namespace kcd2vr::frik_pose_probe {
bool GeometryAdmittedRelease51(const contact176::Ticket&,const void*) noexcept {return false;}
int F4ContactGateMode178r26() noexcept {return 0;}
bool PhysicalCombat177Enabled() noexcept {return false;}
bool GeometryAutoLock178r29() noexcept {return false;}
bool GuardOwnedByUs59c7e(const void*) noexcept {return false;}
int WeaponControlMode() noexcept {return 0;}
void* WeaponControlEntity(int) noexcept {return nullptr;}
void ObserveCollisionAnimationCharacter(void*,unsigned,std::uint64_t) noexcept {}
}
namespace kcd2vr::animation_filter {
void MarkContactForPlayback(std::uint64_t) noexcept {}
PlaybackStamp DiagnosticPlaybackStamp() noexcept {return {};}
}
namespace kcd2vr::retail_build112 {
bool Verify(std::uintptr_t) noexcept {return false;}
bool Ready(std::uintptr_t) noexcept {return false;}
}
