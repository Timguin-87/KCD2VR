#pragma once
// C10R13: native grasp/attachment, controller wrist, explicit Drop OR Stow.
// Only the audited Picking action is owned. Other fragments are observed, not
// frozen with a guessed universal event time. Native eligibility still admits it.
#include <cstdint>
#include "core/native_grasp13.h"
#include "vrik/weapon_release_t1.h"
#include "vrik/native_supply_t2.h"
namespace kcd2vr::action_time_owner59c11 {
enum Mode : int { Off=0, Observe=1, Own=2 };
struct Tuning {
    float holdAt=.50f; // compatibility/display only: actual Attach callback gates holding
    float resumeSpeed=1.0f;
    unsigned holdTimeoutMs=2500; // sparse stale-input receipt; never automatic inventory
    float commitPullM=.10f;
    float cancelGrip=.35f;
    int cancelMode=1; // compatibility: all owned release paths use actual native hand drop
};
bool Install() noexcept;
void Uninstall() noexcept;
bool Installed() noexcept;
// Installed executor + current controller + shared event interval scanner.
// Capability only: ReloadMode2 and current item/action/tracking still admit work.
bool CrossbowNativeReadyXR34() noexcept;
// Transient new-admission fence only; no native calls or cleanup gating.
bool CrossbowNativeAdmissionBusyXR34() noexcept;
void SetMode(int mode) noexcept;
int Mode() noexcept;
// Ordinary pickup preference and independently admitted F5 physical test.
// Both use the same real native Attach/hold/release lifecycle; never instant.
void SetUnownedPickupClockD65(bool) noexcept;
bool UnownedPickupClockD65() noexcept;
void SetAlchemyTransferSpeedD65(float) noexcept;
float AlchemyTransferSpeedD65() noexcept;
void SetPickupHold62(bool) noexcept;
void SetPickupExperiment62(bool) noexcept;
void SetTuning(const Tuning&) noexcept;
void SetPropPresentation35(float outwardMeters,float stowRadiusMeters) noexcept;
Tuning GetTuning() noexcept;
void SetOwnedFragments(const char*) noexcept;
void NoteUseSent(int side,std::uint64_t nowMs) noexcept;
// Capability and one-shot ownership for an actually transported deliberate
// VR Pickable Grip. Native keyboard/gamepad A and non-Pickable actions stay native.
bool NativeGraspAvailable63() noexcept;
// Before transport, permit only read-only native root observation for this
// current physical candidate, so adoption has a true pre-Use reference.
void PreparePhysicalPickup63(bool pending,int side,std::uint64_t nowMs) noexcept;
// The offered entity is expectation only. The exact native Attach request must
// resolve that entity before this serial can acquire held-item presentation.
void NotePhysicalPickupUse63(int side,std::uint64_t nowMs,std::uint32_t expectedEntity) noexcept;
void NoteUseInput(bool pulseSent,bool heldSent,int side,std::uint64_t nowMs) noexcept;
void SetContextAvailable(bool allowed) noexcept;
void PublishAcquisitionInput55(const grasp13::AcquisitionInput55&) noexcept;
void PublishHands(const grasp13::HandFrame&) noexcept;
bool ReadGraspBinding(grasp13::Binding&) noexcept;
// Presentation-only accepted native attachment, before owner socket observation.
// Requires a fresh exact acquisition and releases on every existing owner exit.
bool ReadPendingAttachmentPresentationD72(grasp13::Binding&) noexcept;
// Exact retained item presentation only after native outer cleanup completed.
// Still requires current actual socket/hand membership and completed wrist at the writer.
bool ReadFinishingAttachmentPresentationR94(grasp13::FinishingPresentationR94&) noexcept;
// Exact already-adopted resource only; fresh raw input and current held ownership.
bool ReadAdoptedAttachmentPresentationR95(grasp13::FinishingPresentationR94&) noexcept;
bool AdoptedAttachmentPresentationCurrentR95(const grasp13::FinishingPresentationR94&) noexcept;
bool FinishingOrHeldPresentationCurrentR94(const grasp13::FinishingPresentationR94&) noexcept;
bool ReadPendingUse55(grasp13::PendingUse55&) noexcept;
bool ReadAcquisition54(grasp13::Acquisition54&) noexcept;
// Exact native lifecycle for the shared body/camera basis only; not a grasp lease.
bool ReadAcquisitionFoundationD56(grasp13::Acquisition54&) noexcept;
bool NativeGraspEnabled() noexcept;
// Original native Picking debt survives a live preference change. This is a
// read-only admission veto for D72 ordinary world pickup, never cleanup authority.
bool NativePickupOutstandingD72() noexcept;
// D85 interact: side of the live transported VR PickableItem Use for this exact
// world entity (fresh grip, same tracking source), or false.
bool RequestedPickupSideD85(std::uint32_t entity,int& side) noexcept;
void PublishGraspPlacement(const grasp13::Placement&) noexcept;
void PublishGraspSocket28(const grasp13::SocketProof28&) noexcept;
struct Status {
    int state=0;char fragment[40]{};float layerTime=0;
    std::uint64_t heldMs=0;
    unsigned queued=0,scopeChanges=0,events=0,holds=0,commits=0,cancels=0,timeouts=0;
    bool henryController=false,eventTap=false;
};
void GetStatus(Status&) noexcept;
bool WristOwnedByController(int side) noexcept;
// T1 exact native drop; does not enable the ordinary pickup experiment.
weapon_release_t1::Result DropWeaponT1(const weapon_release_t1::Request&) noexcept;
void TickWeaponDropT1(bool allowed,std::uintptr_t player,std::uint64_t bodyEpoch,std::uint64_t requestEpoch) noexcept;
bool ReadCommittedDropT2(weapon_release_t1::CommittedT2&) noexcept;
void SetThrowingSupplyT2(bool) noexcept;
bool ThrowingSupplyT2() noexcept;
void PublishThrowingSupplyT2(const supply_t2::Input&) noexcept;
bool ReadThrowingSupplyT2(supply_t2::Lease&) noexcept;
// Entry identity only: exact process-live T2 draw hook, never item/action admission.
bool NativeDrawEntryQualified62(std::uintptr_t whBase) noexcept;
}
