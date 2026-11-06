#pragma once
#include "vrik/pose_math.h"
#include <cstdint>
namespace kcd2vr::native_physics_phase_t4 {class Context;class Lease;}

namespace kcd2vr::native_weapon_body_t4 {
using Vec3=pose_core::Vec3;
using Quat=pose_core::Quat;
struct Pose {Quat q{};Vec3 p{};};
// Opaque equality identities. The physics backend never resolves these pointers.
struct Owner {
    std::uint64_t key=0,generation=0,request=0,epoch=0;
    std::uintptr_t player=0,manager=0,character=0,entityPointer=0,attachment=0,attachmentObject=0;
    std::uint32_t entityId=0;int side=0;
    friend bool operator==(const Owner& a,const Owner& b) noexcept {
        return a.key==b.key&&a.generation==b.generation&&a.request==b.request&&a.epoch==b.epoch&&
            a.player==b.player&&a.manager==b.manager&&a.character==b.character&&a.entityPointer==b.entityPointer&&
            a.attachment==b.attachment&&a.attachmentObject==b.attachmentObject&&a.entityId==b.entityId&&a.side==b.side;
    }
    friend bool operator!=(const Owner& a,const Owner& b) noexcept {return !(a==b);}
};
enum class State : unsigned {Off,Waiting,Creating,Configuring,Active,Retiring,Quarantined,Unavailable};
enum class Reason : unsigned {None,Disabled,NoInput,Unsupported,InvalidInput,StaleInput,NativeCollider,
    NativeUnavailable,PlayerFilter,Occupied,ClearanceUnknown,CreatePending,ConfigurePending,BodyUnknown,
    BodyReplaced,MotionUnknown,ExcessError,NativeFault,RetirePending,WorldChanged,ContactIdentityExhaustedH24,
    QueryCallerUnavailableH5,SelfPairPendingH5,SelfPairRefusedH5,SelfPairClosingH5};
struct IntentFrameH28 {Pose modelWorld{},trackingFromModel{};double scale=1;bool valid=false;};
struct Input {
    Owner owner{};
    IntentFrameH28 intentFrameH28{};
    Pose weaponWorld{},targetWorld{},gripLocal{};
    Vec3 boundsMin{},boundsMax{}; // Entire native weapon-local bounds, including hilt.
    double scale=1,mass=0;
    std::uint64_t sample=0,tick=0,trackingEpoch=0,cancelGeneration=0,colliderLease=0;
    std::uintptr_t nativeBody=0,nativeProxy=0;int nativeBodyId=-1;
    std::uintptr_t playerBody=0,playerEntityPointer=0;int playerBodyId=-1;
    int classId=0;
    bool allowed=false,tracked=false,emptyLeft=false,nativeColliderExcluded=false;
    std::uint64_t npcContactControlH5=0; // Original session control word, never refreshed in T4.
};
struct Achieved {
    Owner owner{};Pose weaponWorld{};Vec3 linear{},angular{};double mass=0;
    IntentFrameH28 intentFrameH28{};
    std::uint64_t sample=0,sourceTick=0,observedTick=0,worldGeneration=0,bodyGeneration=0,
        trackingEpoch=0,cancelGeneration=0,colliderLease=0;
    int bodyId=-1;State state=State::Off;Reason reason=Reason::Disabled;
    // colliderRetired concerns this exact item owner; safeNativeRestore also
    // requires the F9 feature OFF, for restoring the separate legacy experiment.
    bool poseValid=false,colliderRetired=true,safeNativeRestore=true;
    std::uint64_t npcContactControlH5=0;bool npcContactsH5=false;
};
// Immutable callback identity only: opaque addresses are compared, never
// resolved/dereferenced by a collision callback or retained as a phase lease.
inline constexpr unsigned kLogCollisionsH24=0x02000000u;
struct ContactOwnerH24 {
    Owner owner{};Pose weaponWorld{};Vec3 linear{},angular{};double mass=0;
    IntentFrameH28 intentFrameH28{};
    std::uintptr_t body=0,foreign=0;int foreignType=-1,bodyId=-1;
    std::uint64_t worldGeneration=0,bodyGeneration=0,colliderLease=0,trackingEpoch=0,cancelGeneration=0,
        sourceSample=0,sourceTick=0,observedTick=0;
    bool active=false;
};
bool ReadContactOwnerH24(ContactOwnerH24&) noexcept;
// No source intent is synthesized here. Submit retains the original XR tick.
void SetEnabled(bool enabled) noexcept;
bool Enabled() noexcept;
// Independent throwing admission bit8; preserve user1/native-unretired2/equipment4.
void SetThrowAdmissionSuspendedT23(bool) noexcept;
bool ThrowAdmissionSuspendedT23() noexcept;
bool Install() noexcept;
using SourcePredicate=bool(*)(const Input&) noexcept;
// Startup-only. Bounded immutable-source/atomic checks; no native calls/logging.
bool SetSourcePredicate(SourcePredicate) noexcept;
void Submit(const Input&) noexcept;
void Retire(const Owner&) noexcept;
void Cancel() noexcept;
bool Read(Achieved&) noexcept;
bool HasUnretiredCollider() noexcept;
// One native equipment owner only. Suspends atomic admission without changing
// the user F9 enable bit; retain until original transition proof/NOT_INVOKED.
void SetEquipmentAdmissionSuspendedH36(bool) noexcept;
// Retained original LEFT equipment work, independent of the user's F9 bit.
// Read-only exclusion for a NEW optional working-hand owner; never cleanup.
bool EquipmentAdmissionSuspendedH36() noexcept;
std::uint64_t AdmissionSerial() noexcept; // Monotonic, includes admitted-but-cancelled creation.
bool ReadRetainedOwner(Owner&) noexcept;
const char* ReasonName(Reason) noexcept;
// Same-stack buddy access. A submitted joint pins this exact incarnation
// until native Absent; input loss stops its motor without destroying its body.
native_physics_phase_t4::Lease* CurrentColliderLease(native_physics_phase_t4::Context&,const Owner&,std::uint64_t bodyGeneration) noexcept;
native_physics_phase_t4::Lease* CleanupColliderLease(native_physics_phase_t4::Context&,const Owner&,std::uint64_t bodyGeneration) noexcept;
bool PinColliderConstraint(native_physics_phase_t4::Context&,const Owner&,std::uint64_t bodyGeneration,bool pin) noexcept;
}
