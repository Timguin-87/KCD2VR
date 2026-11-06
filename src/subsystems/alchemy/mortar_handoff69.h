#pragma once
#include "bridge67.h"
namespace kcd2vr::subsystems::alchemy::mortar69 {
// One immutable local-stop request. The native controller thread alone services
// it; no Detach/Use/ingredient event is synthesized by this channel.
struct Request {
 alchemy_tool18::Binding left{},right{};
 std::uint64_t serial=0,tick=0,poseEpoch=0,sourceEpoch=0,modeEpoch=0;
 std::uintptr_t parent=0,child=0;
 // D78R2: withdraw only this still-unentered held-tilt intent. The original
 // request identity and tick remain immutable; this is not a native cancel.
 std::uint64_t withdrawTickD78R2=0;
};
inline bool sameRequestD78R2(const Request& a,const Request& b) noexcept {
 return a.serial&&a.serial==b.serial&&a.tick==b.tick&&a.poseEpoch==b.poseEpoch&&
  a.sourceEpoch==b.sourceEpoch&&a.modeEpoch==b.modeEpoch&&a.parent==b.parent&&a.child==b.child&&
  alchemy_tool18::same(a.left,b.left)&&alchemy_tool18::same(a.right,b.right);
}
enum class Status {None,WaitingNativeCycle,ReadyForNativePlace,Refused,Revoked,WithdrawnD78R2};
// Only the native owner can prove that a terminal refusal preceded its local
// Stop call. Old/default receipts are Unknown, never permission to replay it.
enum class StopEntry72 {Unknown,NotEntered,Entered};
struct Result {std::uint64_t serial=0,tick=0;std::uintptr_t instance=0;std::uint64_t lifetime=0;Status status=Status::None;
 StopEntry72 stopEntry72=StopEntry72::Unknown;};
inline core::SnapshotMailbox<Request> request;
inline core::SnapshotMailbox<Result> result;
inline std::atomic<bool> interested{false};
}
