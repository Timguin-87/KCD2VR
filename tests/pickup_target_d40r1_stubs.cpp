// Explicit external-module leaves missing from the retained owner fixture.
// No target/Use/Queue/Attach/transfer/presentation function is substituted here.
#include "frik_pose_probe.h"
#include "native_script_t2.h"
#include "subsystems/physics/runtime/native_weapon_body_t4.h"
#include "subsystems/alchemy/finish_observer_cb1.h"
namespace kcd2vr::native_script_t2 {bool Context::Current() const noexcept {return false;}}
namespace kcd2vr::frik_pose_probe {
bool HoverHaptic178r14(int,std::uint64_t) noexcept {return false;}
int CombatCensusMode() noexcept {return 0;}
int WeaponControlMode() noexcept {return 0;}
std::uint64_t PoseEpochH24() noexcept {return 9;}
bool NativeReloadHandRemoveEntryQualifiedT30(std::uintptr_t) noexcept {return false;}
ThrowAuthorityT22 ThrowNativeAuthorityT22(int,std::uintptr_t,std::uint64_t,std::uint64_t,std::uint64_t,std::uint64_t) noexcept {return ThrowAuthorityT22::Unknown;}
ThrowAuthorityT22 ThrowHandAdmissionT22(int,std::uintptr_t,std::uintptr_t,std::uint64_t,std::uint64_t) noexcept {return ThrowAuthorityT22::Unknown;}
ThrowAuthorityT22 ThrowEquipmentAdmissionT25(int,std::uintptr_t,std::uintptr_t,std::uintptr_t,std::uint64_t,std::uint64_t) noexcept {return ThrowAuthorityT22::Unknown;}
bool ReadThrowModelGripT18(void*,float (&)[7]) noexcept {return false;}
bool HandClaimBlocksT24(int) noexcept {return false;} // fixture has no competing hand/equipment owner
}
namespace kcd2vr::native_weapon_body_t4 {
bool EquipmentAdmissionSuspendedH36() noexcept {return false;}
bool HasUnretiredCollider() noexcept {return false;}
bool ThrowAdmissionSuspendedT23() noexcept {return false;}
std::uint64_t AdmissionSerial() noexcept {return 0;}
}
namespace kcd2vr::alchemy_finish_cb1 {bool Last(Result& out) noexcept {out={};return false;}}
