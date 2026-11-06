// Unrelated external services absent from the retained full native-owner TU.
// These do not replace any tested alchemy queue/update/input/return method.
#include "frik_pose_probe.h"
#include "subsystems/alchemy/finish_observer_cb1.h"
namespace kcd2vr::frik_pose_probe {
bool HandClaimBlocksT24(int) noexcept {return false;}
ThrowAuthorityT22 ThrowEquipmentAdmissionT25(int,std::uintptr_t,std::uintptr_t,std::uintptr_t,std::uint64_t,std::uint64_t) noexcept {return ThrowAuthorityT22::Unknown;}
std::uint64_t PoseEpochH24() noexcept {return 0;}
int CombatCensusMode() noexcept {return 0;}
int WeaponControlMode() noexcept {return 0;}
}
namespace kcd2vr::alchemy_finish_cb1 {bool Last(Result& r) noexcept {r={};return false;}}
