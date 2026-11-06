#pragma once
#include "subsystems/subsystem.h"
#include "../../vrik/contact_authority176.h"
namespace kcd2vr::subsystems::masterstrike_combos {
const ModuleDescriptor& Descriptor() noexcept;
// Copied same-callback identity of a CURRENT native ordinary Henry action.
// This observation grants timing only after the native combo trigger opens;
// it is not damage, a hit flag, a recipe step or permission to start an action.
enum class NativeSlotReasonF1 : unsigned {
    Ready=0,NativeClockUnavailable=1,DirectComparison=2,Binding=3,CurrentMainAction=4,
    Frame=5,Tracking=6,HeldWeapon=7,Release=8,Recheck=9
};
struct NativeSlotF1 {
    contact176::Ticket release{};
    std::uintptr_t actor=0,state=0,action=0;
    std::uintptr_t helper=0,clock=0,manager=0,row=0,entity=0;
    std::uint64_t admission=0,policy=0;
    NativeSlotReasonF1 reason=NativeSlotReasonF1::NativeClockUnavailable;
};
}
namespace kcd2vr::combo_lineage19 {struct SlotLeaseF1;}
namespace kcd2vr::frik_pose_probe {
bool ReadNativeComboSlotF1(const void* actor,subsystems::masterstrike_combos::NativeSlotF1& out) noexcept;
bool ReadNativeComboContinuationH19(const void* actor,subsystems::masterstrike_combos::NativeSlotF1& out,
    const combo_lineage19::SlotLeaseF1& originalLease) noexcept;
// Native input thread only. A later prepare/release, including a failed one,
// cannot inherit an earlier slot's post-cleanup read continuation.
std::uint64_t NativeComboInputGenerationH10() noexcept;
}
