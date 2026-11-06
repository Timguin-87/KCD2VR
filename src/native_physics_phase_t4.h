#pragma once
#include <cstdint>
#include <cstddef>
#include "vrik/constraint_packet60.h"

// Exact-retail native physics mutation owner. Context is borrowed on the current
// callback stack; neither it nor a native pointer obtained through it is a frame
// or gameplay/item lease. No ordinary mod-thread fallback exists.
namespace kcd2vr::native_physics_phase_t4 {
struct V3 { float x=0,y=0,z=0; };
struct Q { float x=0,y=0,z=0,w=1; };
enum class Phase : std::uint8_t { Step, WorldReset, Shutdown, WorldReplaced };
enum class Registry : std::uint8_t { Unknown, Same, Retired };
class Context;
struct RigidSnapshot;
struct ConstraintSnapshot;
class Lease {
public:
    Lease() noexcept=default;
    Lease(const Lease&)=delete;
    Lease& operator=(const Lease&)=delete;
    bool Held() const noexcept { return held_; }
    bool InsertionPending() const noexcept { return pending_; }
    int Id() const noexcept { return id_; }
private:
    friend struct Access;
    friend struct ArticulatedAccess;
    friend struct LivingAccessH24;
    friend Registry ObserveRegistry(Context&,const Lease&) noexcept;
    friend bool ReadyRigid(Context&,Lease&) noexcept;
    friend bool Release(Context&,Lease&) noexcept;
    friend bool AbandonInvalidated(Context&,Lease&) noexcept;
    friend void* NativeHandle(Context&,Lease&,bool) noexcept;
    void* body_=nullptr;
    void* world_=nullptr;
    std::uint64_t generation_=0;
    int id_=-1;
    bool held_=false,pending_=false;
};
class Context {
public:
    Context(const Context&)=delete;
    Context& operator=(const Context&)=delete;
    bool Current() const noexcept;
    void* World() const noexcept;
    std::uintptr_t ModuleBase() const noexcept;
    std::uint64_t Generation() const noexcept;
    std::uint64_t Serial() const noexcept;
    Phase Kind() const noexcept;
private:
    friend struct Access;
    Context() noexcept=default;
    void* world_=nullptr;
    std::uintptr_t base_=0;
    std::uint64_t generation_=0,serial_=0;
    std::uint32_t thread_=0;
    Phase phase_=Phase::Step;
};
using Callback=void(*)(Context&,void*) noexcept;
// Optional idle admission, evaluated on the serialized physics-owner stack
// BEFORE native world reads. Only copied/atomic requests and this owner's local
// cleanup state may be inspected. False must mean no requested work or debt.
// Reset/shutdown are never gated; absent predicates retain previous behavior.
using NeedsStep=bool(*)(void*) noexcept;
struct Client { Callback step=nullptr,reset=nullptr; void* user=nullptr; NeedsStep needsStep=nullptr; };
struct RigidSnapshot {
    V3 position{},linearVelocity{},angularVelocity{},centerOfMass{};
    Q orientation{};
    float mass=0,scale=0;
    std::uint32_t flags=0;
    int simulationClass=-1;
    bool active=false;
};
struct ConstraintSnapshot { bool exists=false; std::uint32_t flags=0; void* buddy=nullptr; };
// A current physical-body/part binding, NOT proof that an actor is dead.
// The existing native death/relinquish/character/contact producer must join it
// before admission. foreign/type are copied from that CURRENT body, not guessed.
struct ArticulatedSource {
    int physicsId=-1,part=-1; // -1 resolves through the pinned native world id getter
    void* body=nullptr;
    const void* foreign=nullptr;
    int foreignType=-1;
};
class ArticulatedLease {
public:
    ArticulatedLease() noexcept=default;
    ArticulatedLease(const ArticulatedLease&)=delete;
    ArticulatedLease& operator=(const ArticulatedLease&)=delete;
    bool Held()const noexcept{return retained_.Held();}
    int Id()const noexcept{return retained_.Id();}
    int Part()const noexcept{return source_.part;}
private:
    friend struct ArticulatedAccess;
    Lease retained_{};
    ArticulatedSource source_{};
};
struct ArticulatedSnapshot {
    V3 position{};Q orientation{};float scale=0;int part=-1,simulationClass=-1;
};
struct Receipt {
    std::uint64_t steps=0,resets=0,worldReplacements=0,rejectedContexts=0,
        acquisitions=0,releases=0,abandonedReferences=0,faults=0,referenceOwnerRefusals=0;
    bool installed=false,shutdownRequested=false;
};

// Startup/control-plane only. Fixed4-client storage; clients and their code must
// remain mapped for process lifetime. Installation pins this module before hooks.
// Callbacks may use bounded POD/atomic mailboxes and separately pinned native
// physics leaves; no application heap allocation, I/O, blocking application mutex, script/actor
// gameplay API or borrowed Context/native-pointer retention is permitted.
// Read-only retail/phase prerequisites, before client registration or hook mutation.
bool Preflight() noexcept;
const char* InstallFailureStage() noexcept;
bool RegisterClient(Client) noexcept;
bool Install() noexcept;
void RequestShutdown() noexcept;
Receipt ReadReceipt() noexcept;

bool AcquireRigid(Context&,int id,void* expected,Lease&) noexcept;
// Only for a body just returned by a pinned native factory in THIS context.
// Requires exact private foreign cookie/type and retains insertion-pending state.
bool AdoptCreatedRigid(Context&,void* body,const void* cookie,int foreignType,Lease&) noexcept;
Registry ObserveRegistry(Context&,const Lease&) noexcept;
bool ReadyRigid(Context&,Lease&) noexcept;
bool Release(Context&,Lease&) noexcept;
// Explicit unavailable-world retirement: forget identity, count an UNCONFIRMED
// reference, never call through an old body/world or claim native release.
bool AbandonInvalidated(Context&,Lease&) noexcept;
bool ReadRigid(Context&,Lease&,RigidSnapshot&) noexcept;
bool RigidEntityIdentity(Context&,Lease&,const void* expectedEntity,std::uint32_t expectedId) noexcept;
bool SetVelocity(Context&,Lease&,V3 linear,V3 angular) noexcept;
bool ReadConstraint(Context&,Lease&,int exactId,ConstraintSnapshot&) noexcept;
struct ConstraintResult {
    enum class Kind : std::uint8_t { Refused, Submitted, Uncertain };
    Kind kind=Kind::Refused;int nativeReturn=0;std::uint32_t id=0;
};
// SAME locked T4 context only. Both leases revalidate before the native action;
// native Action(...,1) is immediate here. The caller retains both leases through
// typed Present/Absent. A positive add ID is NOT proof of a working attachment.
ConstraintResult AddConstraint(Context&,Lease& source,Lease& buddy,const constraint_packet60::Packet&) noexcept;
ConstraintResult RemoveConstraint(Context&,Lease& source,std::uint32_t originalId) noexcept;

// Separate typed articulated source; rigid acquisition/velocity/native handles
// remain rigid-only. These APIs never call actor/script/GetCharacter in T4.
// A false acquisition may leave Held()==true after successful AddRef followed
// by changed readback: retain/quarantine it until proper Release/reset cleanup.
bool AcquireArticulated(Context&,const ArticulatedSource&,ArticulatedLease&) noexcept;
Registry ObserveRegistry(Context&,const ArticulatedLease&) noexcept;
bool ReadArticulatedPart(Context&,ArticulatedLease&,ArticulatedSnapshot&) noexcept;
bool Release(Context&,ArticulatedLease&) noexcept;
bool AbandonInvalidated(Context&,ArticulatedLease&) noexcept;
// Type26 Present/Absent observes the ORIGINAL id on the retained exact body.
// Cleanup remains available if its part/foreign owner is no longer admissible;
// no raw part is silently redirected and no id is cleared by these operations.
bool ReadArticulatedConstraint(Context&,ArticulatedLease&,int originalId,ConstraintSnapshot&) noexcept;
ConstraintResult AddArticulatedConstraint(Context&,ArticulatedLease&,Lease& rigidBuddy,const constraint_packet60::Packet&) noexcept;
// Remove first reads current Present and verifies the retained buddy. A refusal
// or exception is never an Absent ACK; caller owns its original id and no-retry.
ConstraintResult RemoveArticulatedConstraint(Context&,ArticulatedLease&,Lease& retainedRigidBuddy,std::uint32_t originalId) noexcept;


// Exact living-body reference for a self-only collision exclusion. This type
// exposes no velocity/action API and never mutates Henry's body.
struct LivingSourceH24 {int physicsId=-1;void* body=nullptr;const void* entity=nullptr;};
class LivingLeaseH24 {
public:
    LivingLeaseH24() noexcept=default;
    LivingLeaseH24(const LivingLeaseH24&)=delete;
    LivingLeaseH24& operator=(const LivingLeaseH24&)=delete;
    bool Held()const noexcept{return retained_.Held();}
private:
    friend struct LivingAccessH24;
    Lease retained_{};LivingSourceH24 source_{};
};
bool AcquireLivingH24(Context&,const LivingSourceH24&,LivingLeaseH24&) noexcept;
bool CurrentLivingH24(Context&,LivingLeaseH24&,const LivingSourceH24&) noexcept;
bool ReleaseLivingH24(Context&,LivingLeaseH24&) noexcept;
bool AbandonLivingH24(Context&,LivingLeaseH24&) noexcept;
// This explicit operation is the ONLY path allowing inactive 0x300. It is
// a no-motor ignore-pair, not a grasp and never a successful physical hold.
ConstraintResult AddSelfIgnorePairH24(Context&,Lease& ownedRigid,LivingLeaseH24&) noexcept;
ConstraintResult RemoveSelfIgnorePairH24(Context&,Lease& ownedRigid,std::uint32_t originalId) noexcept;
bool SelfIgnorePairPresentH24(Context&,Lease& ownedRigid,LivingLeaseH24&,std::uint32_t originalId) noexcept;

// Same-stack observation only, with current registry/entity foreign-data join.
// Does not acquire/retain a source static body or authorize later mutation.
bool ObserveSuspended(Context&,int id,void* expectedBody,const void* expectedEntity) noexcept;
bool ObserveLivingExcluded(Context&,int id,void* expectedBody,const void* expectedEntity) noexcept;

// A same-stack handle for a separately pinned typed native leaf. Current+ready
// lease validation is repeated here. The caller cannot retain this raw pointer.
void* NativeHandle(Context&,Lease&,bool allowInsertionPending=false) noexcept;
} // namespace kcd2vr::native_physics_phase_t4

