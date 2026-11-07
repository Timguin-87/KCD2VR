// MSVC resolves references before discarding unused production sections.
// Fail-closed external services only; no tested native leaf is replaced here.
#include "native_held_backend_t4_stub.h"
#include "subsystems/physics/runtime/native_grab_producer60.h"
#include "subsystems/alchemy/finish_observer_cb1.h"
#include "subsystems/throw_drop/grip_stow_cb1.h"
// These modules are absent in retained full-frik fixtures. Their own H41
// provider/observer fixtures exercise registration; never grant a hand here.
namespace kcd2vr::alchemy_finish_cb1 {void SetCurrentReader(CurrentReader) noexcept {}}
namespace kcd2vr::grip_stow_cb1 {void SetCurrentReader(CurrentReader) noexcept {}}
namespace kcd2vr::physics::grab_producer60 {
bool PublishGrip(const Intent&) noexcept{return false;}
void Cancel(int) noexcept{}
bool ReadStatus(int,Status&) noexcept{return false;}
}
#include "subsystems/gore/impale_constraint_h21.h"
namespace kcd2vr::impale_constraint_h21 {
bool Submit(const Input&) noexcept{return false;}
void Cancel(std::uint64_t) noexcept{}
bool MotorBlocked() noexcept{return false;}
const char* Failure() noexcept{return "NO_CURRENT_ACHIEVED_RIGID_BUDDY_PHYSICAL_IMPALE_ENTRY_NOT_CONNECTED";}
void Refuse(const char*) noexcept{}
}
namespace kcd2vr::native_weapon_body_t4 {
void SetEnabled(bool v) noexcept {fixture::enabled=v;}
bool Enabled() noexcept {return fixture::enabled;}
void SetEquipmentAdmissionSuspendedH36(bool value) noexcept {fixture::equipmentSuspendedH36=value;}
void SetThrowAdmissionSuspendedT23(bool value) noexcept {fixture::throwAdmissionSuspendedT23=value;}
bool ThrowAdmissionSuspendedT23() noexcept {return fixture::throwAdmissionSuspendedT23;}
bool Install() noexcept {return false;}
bool SetSourcePredicate(SourcePredicate p) noexcept {fixture::predicate=p;return true;}
void Submit(const Input& p) noexcept {fixture::input=p;++fixture::submits;}
void Retire(const Owner&) noexcept {++fixture::retires;}
void Cancel() noexcept {++fixture::cancels;}
bool ReadContactOwnerH24(ContactOwnerH24& out) noexcept {if(!fixture::contactReadH28)return false;out=fixture::contactH28;return true;}
bool Read(Achieved& a) noexcept {if(!fixture::read)return false;a=fixture::achieved;return true;}
bool HasUnretiredCollider() noexcept {return fixture::pending;}
std::uint64_t AdmissionSerial() noexcept {return fixture::admissions;}
bool ReadRetainedOwner(Owner& o) noexcept {if(!fixture::ownerRead)return false;o=fixture::achieved.owner;return true;}
const char* ReasonName(Reason) noexcept {return "ABSENT_BACKEND";}
}
#include "animation_filter.h"
namespace kcd2vr::held_map_m1 {void NativeFrameTick() noexcept {}} // native scene leaf absent in host bridges
#include "door_signal19c4r4.h"
namespace kcd2vr::door_signal19c4r4 {
bool SetEnabled(bool) noexcept {return false;}
bool Ready() noexcept {return false;}
bool Publish(const door_script19c4r4::Command&,std::uintptr_t) noexcept {return false;}
bool ReadCleanupIdentity(door_script19c4r4::CleanupIdentity&) noexcept {return false;}
bool RetireKnownAbsent(const door_script19c4r4::CleanupIdentity&,std::uintptr_t) noexcept {return false;}
bool ReadAck(door_script19c4r4::Ack&) noexcept {return false;}
bool HasCleanupLease() noexcept {return false;}
// C66 curve service is absent in these complete-frik fixtures. Registration
// must not invoke/retain a production callback or grant a synthetic door lease.
void SetCurveHandler66(door_curve_signal66::Handler) noexcept {}
void Stop() noexcept {}
bool Shutdown() noexcept {return true;}
}
#include "combat_diag_probe.h"
#include "action_time_owner59c11.h"
// Native grasp is outside these complete-frik bridge fixtures. Preserve the
// absent-service contract explicitly; the separate native_grasp_owner13 target
// includes the real owner and exercises acquisition/hold/drop/stow.
namespace kcd2vr::action_time_owner59c11 {
// C46R1 door service needs the same explicit absent-owner leaf in this host fixture.
bool Installed() noexcept {return false;}
bool CrossbowNativeReadyXR34() noexcept {return false;} // No native reload executor is installed in this host bridge.
bool pickupHoldRequested62=false,pickupExperimentRequested62=false;
void SetPickupHold62(bool on) noexcept {pickupHoldRequested62=on;}
void SetPickupExperiment62(bool on) noexcept {pickupExperimentRequested62=on;}
void PublishHands(const grasp13::HandFrame&) noexcept {}
void PublishAcquisitionInput55(const grasp13::AcquisitionInput55&) noexcept {}
bool ReadGraspBinding(grasp13::Binding&) noexcept {return false;}
bool ReadPendingUse55(grasp13::PendingUse55&) noexcept {return false;}
bool ReadAcquisition54(grasp13::Acquisition54&) noexcept {return false;}
bool NativeGraspEnabled() noexcept {return false;}
bool NativeGraspAvailable63() noexcept {return false;}
void PreparePhysicalPickup63(bool,int,std::uint64_t) noexcept {}
void NotePhysicalPickupUse63(int,std::uint64_t,std::uint32_t) noexcept {}
void PublishGraspPlacement(const grasp13::Placement&) noexcept {}
void PublishGraspSocket28(const grasp13::SocketProof28&) noexcept {}
bool WristOwnedByController(int) noexcept {return false;}
weapon_release_t1::Result DropWeaponT1(const weapon_release_t1::Request&) noexcept {return {};}
void TickWeaponDropT1(bool,std::uintptr_t,std::uint64_t,std::uint64_t) noexcept {}
bool ReadCommittedDropT2(weapon_release_t1::CommittedT2&) noexcept {return false;}
void SetThrowingSupplyT2(bool value) noexcept {supply_t2::enabled.store(value);}
bool ThrowingSupplyT2() noexcept {return supply_t2::enabled.load();}
// Existing XR34 action-owner counter is idle in ordinary full-frik fixtures.
bool CrossbowNativeAdmissionBusyXR34() noexcept {return false;}
supply_t2::Input lastSupplyInputT9{};
void PublishThrowingSupplyT2(const supply_t2::Input& input) noexcept {lastSupplyInputT9=input;}
bool ReadThrowingSupplyT2(supply_t2::Lease&) noexcept {return false;}
std::uintptr_t hostOwnedDrawEntry62=0;
bool NativeDrawEntryQualified62(std::uintptr_t base) noexcept {return base&&base==hostOwnedDrawEntry62;}
}
#include "vrik/motion_owned59c4.h"
#ifndef BRIDGE_HAS_COMBAT_DIAG_IMPL
namespace kcd2vr::combat_diag_probe {
bool NativeCharacterEntryQualified63(std::uintptr_t) noexcept {return false;} // absent passive hook in full-frik host bridges
bool ArmF4ContactGate176() noexcept {return false;} // absent native contact contract cannot enable F4
bool F4ContactGateReady176() noexcept {return false;}
bool RegisterContactObserver115(ContactObserver115) noexcept {return false;}
void ClearContactObserver115(ContactObserver115) noexcept {}
void SetGeometryHits178r27(bool) noexcept {} // V66.178R27: no native funnel on the host
bool hostGeometry59c3=false;
bool GeometryHitsEnabled178r27() noexcept {return hostGeometry59c3;}
static bool hostSync59c10r9=false;void SetSynchronized59c10r9(bool on) noexcept {hostSync59c10r9=on;} bool Synchronized59c10r9() noexcept {return hostSync59c10r9;} // R59.16C10R9
static bool hostIntent59c16=false;void SetIntent59c16(bool on) noexcept {hostIntent59c16=on;} bool Intent59c16() noexcept {return hostIntent59c16;} // R59.16C10R16
void SetRung4Switches59c16(bool,bool) noexcept {} // R59.16C10R16
void SetCombatFlow59c17(bool,unsigned,unsigned) noexcept {} // R59.16C10R17
void CombatFlowReceipts59c17(char* out,std::size_t cap) noexcept {if(out&&cap)out[0]=0;}
bool ContactImpulse596() noexcept {return false;} // R59.6 host stub: impulses are native-only
void SetContactImpulseDiagnostics19c4r4(bool) noexcept {}
bool VrStateConsequence596() noexcept {return false;}
void SetSwingStamina5916(bool) noexcept {}
void SetTelegraphStrike59c10(bool) noexcept {}
void SelectVrBackendRung596(bool) noexcept {}
void (*hostVrStateEventObserver19c4r2)()=nullptr;
bool VrStateEvent5912(const kcd2vr::motion59c4::Key&,std::uint64_t,int,int,int) noexcept {if(hostVrStateEventObserver19c4r2)hostVrStateEventObserver19c4r2();return true;}
bool hostMotionGuardOwned59c10=false;
bool hostMotionGuardUnknown59c10=false;
kcd2vr::motion59c4::Key hostGuardQuery59c10{};
kcd2vr::motion59c4::AttackOwnership VrStateAttackOwnership59c10(const kcd2vr::motion59c4::Key& context,std::uint64_t) noexcept {
    hostGuardQuery59c10=context;
    return hostMotionGuardUnknown59c10?kcd2vr::motion59c4::AttackOwnership::Unknown:
        hostMotionGuardOwned59c10?kcd2vr::motion59c4::AttackOwnership::Owned:kcd2vr::motion59c4::AttackOwnership::None;
}
void ContactImpulseRequest596(std::uint32_t,int,int,const float*,const float*,float,std::uint64_t,std::uintptr_t,std::uint64_t,std::uint64_t,std::uint64_t,std::uint64_t) noexcept {}
void SetGeometryAttackTypes178r27(int,int) noexcept {}
void NoteImpactBodyMaterial59c7e(std::uint64_t,int) noexcept {} // R59.16C7E
void NoteImpactPower59c8(std::uint64_t,float,bool) noexcept {} // R59.16C8
bool GeometryHitSubmit178r27(std::uint32_t,const float*,float,int,int,int,std::uint64_t,std::uint64_t,std::uint64_t,std::uint64_t,std::uint64_t,std::uint64_t,std::uint64_t,const kcd2vr::contact59c7g::Metadata*) noexcept {return false;}
bool GeometryWeaponContactSubmit178r43(std::uint32_t,const float*,float,int,int,int,std::uint64_t,std::uint64_t,std::uint64_t,std::uint64_t,std::uint64_t,std::uint64_t,std::uint64_t) noexcept {return false;}
void SetGeometryApply178r30(bool) noexcept {}
int HenryInCombat178r31(std::uint64_t) noexcept {return -1;}
void SetGeometryEngageNative178r31(bool) noexcept {}
bool GeometryEngageNative178r31() noexcept {return false;}
void SetGeometryOuter178r33(bool) noexcept {}
void SetGeometryTelegraph178r34(bool) noexcept {}
static bool liveRoute55c1=false;
void SetGeometryLiveAction178r35(bool on) noexcept {liveRoute55c1=on;}
void SetGeometryDirectFallback178r37(bool) noexcept {}
void SetGeometryHold178r39(unsigned,int) noexcept {}
void SetGeometryRejectNativeSweep178r40(bool) noexcept {}
bool GeometryLiveAction178r35() noexcept {return liveRoute55c1;}
void GeometryTelegraph178r34(int,int,std::uint64_t) noexcept {}
void NativeOwnerLatch178r33(std::uint64_t,std::uint64_t) noexcept {}
bool NativeOwned178r33(std::uint64_t) noexcept {return false;}
void GeometryHitSummary178r27() noexcept {}
}
#endif
namespace kcd2vr::animation_filter {
// Ordinary full-frik host bridges install no native XR34 action owner.
// Count this explicit read-only leaf so specialized fixtures can assert non-use.
unsigned reloadPassengerEligibilityCallsXR34=0;
bool ReloadPassengerOnlyXR34(void*,std::uintptr_t,int) noexcept {++reloadPassengerEligibilityCallsXR34;return false;}
bool NativeBodyRequired112(void*) noexcept {return false;}
bool PlayerNativeBodyRequired112() noexcept {return nativeBodyOwned112.load();}
bool PlayerNativeBodyRequiredForMotion122() noexcept {return nativeBodyOwned112.load();}
#ifndef BRIDGE_HAS_MODE_STUBS
void SetMode(SuppressionMode) noexcept {}
bool Install() noexcept {return false;}
bool Ready() noexcept {return false;}
#endif
#ifndef BRIDGE_HAS_TARGET_STUB
void SetInteractionAnimationTarget(void*,std::uint32_t,std::uint64_t) noexcept {}
#endif
bool PlayerAnimationObjectValidated() noexcept {return false;}
bool ResolveCurveReference(void*,const char*,CurveReferenceInfo&) noexcept {return false;}
int SampleCurveReference(void*,int,std::uint32_t,float,float*) noexcept {return -1;}
PlaybackStamp DiagnosticPlaybackStamp() noexcept {return {};}
#ifndef BRIDGE_HAS_KICK_OUTPUT23
bool ReadKickPlaybackOutput23(void*,KickPlaybackOutput23& out) noexcept {out={};return false;}
#endif
bool ReadHandPlaybackOutput28(void*,HandPlaybackOutput28& out) noexcept {out={};return false;}
bool ReloadDiagnosticWindow136R3(std::uint64_t,int& weapon_code) noexcept {weapon_code=0;return false;}
const char* ReloadDiagnosticWeaponName136R3(int) noexcept {return "UNKNOWN";}
void ObserveDiagnosticSkinning(void*,bool) noexcept {}
void ObservePlayerSkinning(void*,void*,void*) noexcept {}
void RegisterContactAnimation(void*,std::uint32_t,std::uint64_t) noexcept {}
void ServiceAliasCatalog(bool) noexcept {}
void SetStartDiagnosticsLevel(int) noexcept {}
}
#include "retail_build112.h"
namespace kcd2vr::retail_build112 {
// Explicit synthetic fixture state; this is NOT the production hash verifier.
bool hostRetailVerified112=false;
bool Verify(std::uintptr_t module) noexcept {
#if defined(BRIDGE_VERIFIED_ACTIVATION_H24)
    // Explicit fixture-only activation leaf; never reads/executes retail DLL.
    return module&&hostRetailVerified112;
#else
    return false;
#endif
}
bool Ready(std::uintptr_t module) noexcept {return module&&hostRetailVerified112;}
}
#ifndef BRIDGE_HAS_MODE_STUBS
namespace kcd2vr::native_stereo_outer_lab {bool EnsurePhysicalFramePump6698() noexcept {return false;}}
#endif

#include "interaction_alignment.h"
namespace kcd2vr::interaction_alignment {bool LastHandCandidate(HandCandidate&) noexcept {return false;}} // absent native selector in complete-frik bridges

// C40: new C37I1 native selector services are absent in complete-frik host fixtures.
// Their real implementations have separate connected alchemy/door fixture targets.
namespace kcd2vr::activity_input163 {void AlchemySelectionNativeFrameEnd38() noexcept {}}
namespace kcd2vr::interaction_alignment {
 bool ExactDoorCandidate38(std::uint32_t,int,std::uint64_t,HandCandidate&) noexcept {return false;}
 bool DoorSourceCurrent38(std::uint64_t) noexcept {return false;}
 // C66 acquisition is unavailable here; zero never qualifies a door request.
 std::uint64_t DoorAcquisitionGeneration65() noexcept {return 0;}
 bool DoorAcquisitionCurrent65(std::uint64_t) noexcept {return false;}
}

// C62: complete-frik fixtures do not link the separate diagnostic translation
// unit. No native hit/death is invented at this seam; lab integration itself
// is covered separately by its producer/consumer policy fixtures.
#ifndef BRIDGE_HAS_COMBAT_DIAG_IMPL
namespace kcd2vr::combat_diag_probe {
unsigned LabTakeAnsweredLiving58(LabAnsweredLiving58*,unsigned) noexcept {return 0;}
unsigned LabDeathCandidates58(std::uint32_t*,unsigned,std::uint64_t) noexcept {return 0;}
bool LabCorpseQualified58(std::uint32_t,std::uintptr_t,std::uint64_t,std::uint64_t,const char*& why) noexcept {why="HOST_NO_NATIVE_DEATH";return false;}
}
#endif

#ifndef BRIDGE_OWNS_PRESENCE_H24
#include "subsystems/physics/runtime/native_hand_proxy_owner60.h"
namespace kcd2vr::physics::hand_proxy60 {
bool SubmitPresenceH24(int,const kcd2vr::native_hand_proxy_t4::Input&) noexcept{return false;}
void CancelPresenceH24(int) noexcept{}
bool PresenceReadyH24() noexcept{return false;}
bool SetPresencePredicateH24(PresencePredicateH24) noexcept{return false;}
bool ReadPresenceFeedbackH24(int,PresenceFeedbackH24& out) noexcept{out={};return false;}
}

#endif
