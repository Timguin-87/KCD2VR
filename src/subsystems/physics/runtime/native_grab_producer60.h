#pragma once
#include "vrik/constraint_service60.h"
#include <cstdint>
// Grip intents cross threads as POD evidence only. Native operations run in the
// T4 callback; the backend owns source/proxy leases until retire acknowledges.
namespace kcd2vr::physics::grab_producer60 {
enum class Progress : unsigned char { Pending, Complete, Refused, Uncertain };
enum class State : unsigned char { Idle, Preparing, WaitingPresent, Held, Releasing, Retiring, Quarantined, Fault };
struct Intent {
    std::uintptr_t player=0,expectedBody=0;
    std::uint64_t press=0,poseEpoch=0,trackingEpoch=0,sourceGeneration=0,tick=0,poseTick=0,profileEpoch=0;
    std::uint32_t entity=0;int side=-1,physicsId=-1;
    float p[3]{},q[4]{0,0,0,1};
    bool held=false,allowed=false,nativeAttached=false;
};
struct Owner {
    std::uintptr_t player=0,world=0;
    std::uint64_t requestToken=0,worldGeneration=0,sourceGeneration=0,proxyGeneration=0;
    std::uint64_t poseEpoch=0,trackingEpoch=0,press=0;
    std::uint32_t entity=0;int side=-1;
};
struct Stamp {std::uintptr_t world=0;std::uint64_t generation=0,serial=0;};
struct Result {Owner owner{};std::uint64_t constraintToken=0,serial=0;std::uint32_t id=0;Progress progress=Progress::Pending;};
struct Sample {
    Owner owner{};std::uint64_t constraintToken=0,serial=0;std::uint32_t id=0;
    constraint60::Status::Kind kind=constraint60::Status::Kind::Unknown;
    bool following=false; // measured prop/proxy grip-frame error within the backend's explicit tolerance
};
struct Status {Owner owner{};State state=State::Idle;std::uint32_t id=0;std::uint64_t constraintToken=0,tick=0;char reason[80]{};};
struct Backend {
    // Installed before T4 client publication; all callbacks are SAME T4 STACK.
    bool (*stamp)(void*,Stamp&) noexcept=nullptr;
    // Pending means retryable with NO constraint submitted. Retain partially
    // created resources by requestToken; retire must handle every prepare prefix.
    Progress (*prepare)(void*,const Intent&,std::uint64_t requestToken,Owner&) noexcept=nullptr;
    Progress (*drive)(void*,const Owner&,const Intent&) noexcept=nullptr;
    Result (*add)(void*,const Owner&,std::uint64_t constraintToken) noexcept=nullptr;
    Sample (*read)(void*,const Owner&,std::uint64_t constraintToken,std::uint32_t id) noexcept=nullptr;
    Result (*remove)(void*,const Owner&,std::uint64_t constraintToken,std::uint32_t id) noexcept=nullptr;
    // Complete ONLY when references/proxy resources have actually retired.
    // Refused/Uncertain retains ownership; no silent clear. Never called while
    // a submitted constraint may still exist or a queued Add is unresolved.
    Progress (*retire)(void*,const Owner&) noexcept=nullptr;
    // Authenticated OLD-world native Reset only; releases retained references
    // before native queue/body reset. Not a substitute for ordinary Absent.
    Progress (*reset)(void*,const Owner&) noexcept=nullptr;
    // Pose refresh before the proxy step. No native body/action is borrowed.
    bool (*refresh)(void*,const Owner&,const Intent&) noexcept=nullptr;
    // Revoke only the retained proxy motor target before its phase step. The
    // constraint/source leases remain owned until original Remove/Absent.
    bool (*withdraw)(void*,const Owner&) noexcept=nullptr;
};
#define KCD2VR_GRAB_CONTEXT_D41 1
// Read-only live owner check, called on the physics stack. No gameplay/native
// mutations, waits or borrowed engine pointers are allowed in this callback.
using SourcePredicateD41=bool(*)(const Intent&) noexcept;
bool SetSourcePredicateD41(SourcePredicateD41) noexcept; // immutable registration; absent refuses new drive
bool SourceCurrentD41(const Intent&) noexcept;
bool Enabled() noexcept;
void SetEnabled(bool) noexcept; // disabling stops admission, NEVER teardown
bool PublishGrip(const Intent&) noexcept;
void Cancel(int side) noexcept; // atomic fail-closed cancellation even if mailbox busy
bool ReadStatus(int side,Status&) noexcept;
// A transfer queues ONLY this side's exact original Held identity. Read-only
// Lua captures the item first; T4 alone must still find the same original Held
// request before cancelling it. T4 cleanup/reset never waits for Lua/metadata.
// New same-side admission waits for EndTransfer; an abort must also EndTransfer.
using TransferPrepare=bool(*)(void*) noexcept;
enum class TransferProgress : unsigned char { Unknown, Waiting, Retired, Declined };
bool PrepareTransfer(const Owner&,std::uint64_t serial,std::uint64_t now,TransferPrepare,void*) noexcept;
TransferProgress ReadTransfer(const Owner&,std::uint64_t serial) noexcept;
bool EndTransfer(const Owner&,std::uint64_t serial) noexcept;
bool InstallBackend(const Backend&) noexcept; // startup only, before client registration
// Physics-owner stack only; no engine reads. Retained/queued work must keep
// the shared phase awake after control is disabled, until exact retirement.
bool NeedsPhysicsStepD80() noexcept;
void PhysicsStep(void* borrowedContext,std::uint64_t now) noexcept;
void PhysicsRefresh(void* borrowedContext,std::uint64_t now) noexcept;
void PhysicsReset(void* borrowedContext,std::uint64_t now) noexcept;
}
