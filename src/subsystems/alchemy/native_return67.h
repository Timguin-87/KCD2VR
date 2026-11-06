#pragma once
#include "../../diagnostics/capture168.h"
#include "../../vrik/alchemy_tool18.h"
#include "../../core/snapshot_mailbox.h"
#include "../../vrik/workstation_sample46.h"
#include <atomic>
#include <cstdint>
namespace kcd2vr::subsystems::alchemy::return67 {
struct Request {
    alchemy_tool18::Binding binding{};
    std::uintptr_t child=0;
    std::uint32_t token=0;
    std::uint64_t serial=0,tick=0,poseEpoch=0,sourceEpoch=0,modeEpoch=0;
};
enum class Status { None,Rejected,RestorePending,CancelRequested,CancelNotAcknowledged,
    WaitingChildRetired,NativeOwnerEnded,ForeignAttachment,NativeReturnDispatched,FaultQuarantined,
    CancelRefusedPausedNativeA };
struct Result {std::uint64_t serial=0;std::uintptr_t instance=0;std::uint64_t lifetime=0;
    Status status=Status::None;bool cancelReturned=false,detachReturned=false,homeReturned=false;};
inline core::SnapshotMailbox<Request> request;
inline core::SnapshotMailbox<Result> result;
inline std::atomic<bool> interested{false};
}
