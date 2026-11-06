#include "native_weapon_body_t4.h"
#include "native_weapon_body_t4_policy.h"
#include "native_weapon_body_t4_pins.h"
#include "native_weapon_clearance_t4.h"
#include "native_physics_phase_t4.h"
#include "native_constraint_leaf60.h" // DS-F44: pinned add-constraint leaf (file scope: namespace kcd2vr header)
#include "native_hand_proxy_owner60.h" // DS-F58: hand grip proxy owner (file scope: namespace kcd2vr header)
#include "core/snapshot_mailbox.h"
#include "vrik/phys_bodies59c10r8.h" // Existing exact native POD layouts only; no legacy runtime.
#include "retail_build112.h"
#include <windows.h>
#include <atomic>
#include <array>
#include <cstring>
#if !defined(KCD2VR_WEAPON_BODY_T4_HOST_TEST)
#include "diagnostic_trace.h"
#include <cstdio>
#endif
#include "native_constraint_leaf60.inl" // DS-F44: leaf implementation (file scope before the T4 namespace)
#include "native_hand_proxy_owner60.inl" // DS-F58: proxy owner implementation (file scope)
#include "native_grab_executor60.h" // DS-F62: gated grab executor (file scope: namespace kcd2vr header)
#include "native_grab_executor60.inl" // DS-F62: executor implementation (file scope)
#include "native_grab_producer60.h" // DS-F64: grab producer (file scope: namespace kcd2vr header)
#include "native_grab_producer60.inl" // DS-F64: producer implementation (file scope)
#include "native_grab_backend60.h" // DS-F70: the shared backend contract + bridge (file scope)
#include "native_grab_backend60.inl" // DS-F70: the backend implementation (file scope)
#include "subsystems/physics/native_grab_input60.h" // DS-F88: the game-thread grip-evidence resolver (header-only, inline-shared)
#include "committed_hand_push_h21.h"
#include "self_pair_admission_h24.h"
#include "subsystems/experimental/deepseek_options_h29.h"

namespace kcd2vr::native_weapon_body_t4 {namespace {
namespace phase=native_physics_phase_t4;
namespace pod=phys_bodies59c10r8;
namespace clearance=native_weapon_clearance_t4;
struct Packet {Input input{};std::uint64_t ticket=0;};
core::SnapshotMailbox<Packet> requests;
core::SnapshotMailbox<Achieved> results;
core::SnapshotMailbox<ContactOwnerH24> contactOwnersH24;
struct RetainedOwner {Owner owner{};std::uint64_t version=0;};
core::SnapshotMailbox<RetainedOwner> retainedOwners;
std::atomic<std::uint64_t> retainedVersion{0};
struct InitialPublication {InitialPublication() noexcept {(void)results.publish(Achieved{});}} initialPublication;
std::atomic<bool> installed{false},attempted{false};
std::atomic<std::uint64_t> installCallsH7{0};
std::atomic<const char*> firstMutatingFailureH7{"NONE"};
// Single admission word: disabling cannot race a later unobserved Create.
// bit0=requested, bit1=owned/uncertain native resources outstanding.
std::atomic<unsigned> control{0};
std::atomic_flag installing=ATOMIC_FLAG_INIT;
std::atomic<std::uint64_t> ticket{0},cutoff{0};
std::atomic<std::uint64_t> activeGeneration{0};
std::atomic<std::uint64_t> activation{0};
std::atomic<std::uint64_t> admissions{0};
std::atomic<SourcePredicate> sourcePredicate{nullptr};
std::uintptr_t module=0;
// The engine queues only the foreign POINTER, not our generation counter.
// A process-lifetime unique address prevents delayed-event/body-address ABA.
// This module is pinned by the physics owner; slots are NEVER reused/reset.
std::array<int,65536> cookiePoolH24{};
inline constexpr int foreignType=0x5634; // Native field is int16; private kind, never entity foreign-data2.
struct Record {
    phase::Lease lease;Input input{};policy::Shape shape{};
    int* foreignCookieH24=nullptr;
    void* geometry=nullptr;void* geometryManager=nullptr;
    std::uint64_t generation=0,incarnation=0,createdSerial=0,lastDrive=0;
    bool hasGeometry=false,partAttempted=false,colliding=false,destroyRequested=false,quarantined=false;
    bool constraintPinned=false;
    Reason retiredFor=Reason::None;
} record;
// This pair is independent of external grab/impale pins. Its living reference
// remains owned through the ORIGINAL native id's confirmed Present -> Absent.
struct SelfPairH5 {
    phase::LivingLeaseH24 living{};phase::LivingSourceH24 source{};
    physics::self_pair_admission_h24::Causality causal{};
    std::uint64_t controlWord=0,refusedWord=0,removeTick=0;std::uint32_t id=0;
    bool attempted=false,removeAttempted=false,open=false,uncertain=false,closing=false;
} selfPairH5;
std::uint64_t incarnations=0;
int* ReserveCookieH24() noexcept {
    if(incarnations>=cookiePoolH24.size())return nullptr;
    auto* cookie=&cookiePoolH24[static_cast<std::size_t>(incarnations++)];
    *cookie=0x4B565434;return cookie;
}
struct Rejection {Input input{};std::uint64_t until=0,activation=0;Reason reason=Reason::None;bool valid=false;} rejected;
std::atomic<bool> fault{false};
static void InstallTraceB2(const char* stage,bool ok,std::uintptr_t rva=0) noexcept {
#if !defined(KCD2VR_WEAPON_BODY_T4_HOST_TEST)
    char line[224]{};std::snprintf(line,sizeof(line),"B2 F9_INSTALL stage=%s result=%s rva=%llX nativeBodyCreated=0",stage,ok?"READY":"REFUSED",static_cast<unsigned long long>(rva));
    diagnostic_trace::Write(line);
#else
    (void)stage;(void)ok;(void)rva;
#endif
}
#if defined(KCD2VR_WEAPON_BODY_T4_HOST_TEST)
std::uint64_t HostNow() noexcept;
bool HostInstall() noexcept;
bool HostBind(phase::Context&,void*&) noexcept;
void* HostCreate(phase::Context&,const Pose&) noexcept;
void* HostGeometry(phase::Context&,const policy::Shape&,void*) noexcept;
bool HostAddGeometry(phase::Context&,void*,void*,const policy::Shape&,bool) noexcept;
bool HostRemoveGeometry(phase::Context&,void*) noexcept;
bool HostPart(phase::Context&,void*,void*,bool,double) noexcept;
bool HostConfigure(phase::Context&,void*) noexcept;
bool HostFilters(phase::Context&,void*) noexcept;
bool HostPairClassH5(phase::Context&,void*,unsigned) noexcept;
bool HostUnregister(phase::Context&,void*,void*) noexcept;
int HostDestroy(phase::Context&,void*) noexcept;
std::uint64_t Now() noexcept {return HostNow();}
#else
std::uint64_t Now() noexcept {return GetTickCount64();}
bool Copy(std::uintptr_t p,void* out,std::size_t n) noexcept {
    __try {std::memcpy(out,reinterpret_cast<void*>(p),n);return true;}
    __except(EXCEPTION_EXECUTE_HANDLER){fault=true;return false;}
}
template<class T> bool ReadValue(std::uintptr_t p,T& v) noexcept {return Copy(p,&v,sizeof v);}
bool Slot(std::uintptr_t v,std::uintptr_t offset,std::uintptr_t rva) noexcept {
    std::uintptr_t actual=0;return ReadValue(v+offset,actual)&&actual==module+rva;
}
bool ExactContract() noexcept {
    for(const auto& site:pins::sites){unsigned char actual[16]{};
        if(!Copy(module+site.rva,actual,site.size)||std::memcmp(actual,site.bytes,site.size)){InstallTraceB2(site.name,false,site.rva);return false;}}
    return Slot(module+0x3A48DC0,0x20,0x6F1BE4)&&Slot(module+0x3A48DC0,0x40,0x526194)&&
        Slot(module+0x3A48DC0,0x48,0x38FEE3C)&&Slot(module+0x3A565B0,0x18,0x97CA04);
}
#endif
bool Bind(phase::Context& context,void*& manager) noexcept {
#if defined(KCD2VR_WEAPON_BODY_T4_HOST_TEST)
    return HostBind(context,manager);
#else
    std::uintptr_t v=0,gm=0;
    if(!context.Current()||context.ModuleBase()!=module||!ReadValue(std::uintptr_t(context.World()),v)||v!=module+0x407FC80||
       !Slot(v,0x88,0x8463B8)||!Slot(v,0x98,0x846E10))return false;
    manager=static_cast<unsigned char*>(context.World())+0x10;
    return ReadValue(std::uintptr_t(manager),gm)&&gm==module+0x407FFC0&&
        Slot(gm,0x28,0x8102E0)&&Slot(gm,0x38,0x8100EC)&&Slot(gm,0x48,0x84A954);
#endif
}
void* Create(phase::Context& context,const Pose& body) noexcept {
#if defined(KCD2VR_WEAPON_BODY_T4_HOST_TEST)
    return HostCreate(context,body);
#else
    pod::ParamsPos p{};p.type=0;p.pos={float(body.p.x),float(body.p.y),float(body.p.z)};
    p.q={float(body.q.x),float(body.q.y),float(body.q.z),float(body.q.w)};
    p.scale=pod::unusedFloat();p.iSimClass=pod::kUnusedI;
    using Fn=void*(__fastcall*)(void*,int,void*,void*,int,int,void*);
    __try {return reinterpret_cast<Fn>(module+0x8463B8)(context.World(),2,&p,record.foreignCookieH24,foreignType,-1,nullptr);}
    __except(EXCEPTION_EXECUTE_HANDLER){fault=true;return reinterpret_cast<void*>(-1);}
#endif
}
void* MakeGeometry(phase::Context& context,void* manager,const policy::Shape& shape) noexcept {
#if defined(KCD2VR_WEAPON_BODY_T4_HOST_TEST)
    return HostGeometry(context,shape,manager);
#else
    pod::PrimCapsule p{};p.axis={float(shape.axis.x),float(shape.axis.y),float(shape.axis.z)};
    p.r=float(shape.radius);p.hh=float(shape.halfHeight);
    using Primitive=void*(__fastcall*)(void*,int,const void*);
    using Register=void*(__fastcall*)(void*,void*,int,int*,int);
    using Release=int(__fastcall*)(void*);
    void* geometry=nullptr;void* registered=nullptr;
    __try {
        geometry=reinterpret_cast<Primitive>(module+0x8102E0)(manager,6,&p);
        if(geometry){
            const auto vt=*reinterpret_cast<std::uintptr_t*>(geometry);
            if(vt!=module+0x3A565B0||*reinterpret_cast<std::uintptr_t*>(vt+0x18)!=module+0x97CA04){fault=true;return nullptr;}
            registered=reinterpret_cast<Register>(module+0x8100EC)(manager,geometry,0,nullptr,0);
            reinterpret_cast<Release>(module+0x97CA04)(geometry);
        }
    }__except(EXCEPTION_EXECUTE_HANDLER){fault=true;return nullptr;}
    return registered;
#endif
}
bool AddGeometry(phase::Context& context,void* body,bool collision) noexcept {
    record.partAttempted=true; // Native may mutate before a refused/fault return.
#if defined(KCD2VR_WEAPON_BODY_T4_HOST_TEST)
    return HostAddGeometry(context,body,record.geometry,record.shape,collision);
#else
    pod::GeomParams p{};p.mass=float(record.shape.mass);p.density=0;p.minContactDist=pod::unusedFloat();
    p.flags=collision?1u:0u;p.flagsCollider=collision?1u:0u;
    using Fn=int(__fastcall*)(void*,void*,void*,int,int);
    __try {return reinterpret_cast<Fn>(module+0x526194)(body,record.geometry,&p,0,1)==0;}
    __except(EXCEPTION_EXECUTE_HANDLER){fault=true;return false;}
#endif
}
bool RemoveGeometry(phase::Context& context,void* body) noexcept {
#if defined(KCD2VR_WEAPON_BODY_T4_HOST_TEST)
    return HostRemoveGeometry(context,body);
#else
    // Native RemoveGeometry is void (its epilogue leaves arbitrary EAX).
    // Success is the actual same-stack owned-part absence, never return bits.
    using Fn=void(__fastcall*)(void*,int,int);
    __try {reinterpret_cast<Fn>(module+0x38FEE3C)(body,0,1);}
    __except(EXCEPTION_EXECUTE_HANDLER){fault=true;return false;}
    int count=-1;return ReadValue(std::uintptr_t(body)+0x120,count)&&count==0;
#endif
}
bool Part(phase::Context& context,void* body,bool collision) noexcept {
#if defined(KCD2VR_WEAPON_BODY_T4_HOST_TEST)
    return HostPart(context,body,record.geometry,collision,record.shape.mass);
#else
    // Actual native GetParams3D578D..3D58E7 proves this single owned part layout.
    // The phase context holds mutation exclusion. No geometry pointer escapes.
    std::uintptr_t parts=0,geometry=0;int count=0,id=-1;unsigned flags=0,colliders=0;float mass=0;
    const auto b=std::uintptr_t(body);
    return ReadValue(b+0x120,count)&&count==1&&ReadValue(b+0x118,parts)&&parts&&
        ReadValue(parts+0x48,id)&&id==0&&ReadValue(parts+0x40,geometry)&&geometry==std::uintptr_t(record.geometry)&&
        ReadValue(parts+0x50,flags)&&flags==(collision?1u:0u)&&
        ReadValue(parts+0x54,colliders)&&colliders==(collision?1u:0u)&&ReadValue(parts+0x4C,mass)&&
        std::isfinite(mass)&&std::abs(mass-record.shape.mass)<.01*record.shape.mass;
#endif
}
pod::ParamsFlags ContactFlagsH24() noexcept {
    pod::ParamsFlags flags{};flags.flags=0x80000000u;
    flags.flagsOR=pod::kPefNeverAffectTriggers|pod::kPefIgnoreAreas|kLogCollisionsH24;flags.flagsAND=0xFFFFFFFFu;return flags;
}
bool ContactFlagsCurrentH24(unsigned flags) noexcept {
    constexpr unsigned required=pod::kPefNeverAffectTriggers|pod::kPefIgnoreAreas|kLogCollisionsH24;
    return (flags&required)==required;
}
bool Configure(phase::Context& context,void* body) noexcept {
#if defined(KCD2VR_WEAPON_BODY_T4_HOST_TEST)
    return HostConfigure(context,body);
#else
    auto flags=ContactFlagsH24();
    // Retail class schema has THREE words per OR/AND (getter3D5764 and
    // setter850AA8..850AF0), unlike the legacy20-byte two-word SDK structure.
    struct CollisionClass {int type=26;unsigned orType=0,orIgnore=4,orThird=0,
        andType=0xFFFFFFFFu,andIgnore=0xFFFFFFFFu,andThird=0xFFFFFFFFu;} cc;
    static_assert(sizeof(CollisionClass)==28);
    using Fn=int(__fastcall*)(void*,void*,int);
    __try {const auto fn=reinterpret_cast<Fn>(module+0x6F1BE4);return fn(body,&flags,1)>0&&fn(body,&cc,1)>0;}
    __except(EXCEPTION_EXECUTE_HANDLER){fault=true;return false;}
#endif
}
bool Filters(phase::Context& context,void* body) noexcept {
#if defined(KCD2VR_WEAPON_BODY_T4_HOST_TEST)
    return HostFilters(context,body);
#else
    unsigned flags=0,type=0,ignore=0;
    return ReadValue(std::uintptr_t(body)+0x84,flags)&&
        ContactFlagsCurrentH24(flags)&&
        ReadValue(std::uintptr_t(body)+0xEC,type)&&type==0&&ReadValue(std::uintptr_t(body)+0xF0,ignore)&&ignore==(selfPairH5.open?0u:4u);
#endif
}
bool Unregister(phase::Context& context) noexcept {
    if(!record.geometry)return true;
#if defined(KCD2VR_WEAPON_BODY_T4_HOST_TEST)
    const bool ok=HostUnregister(context,record.geometryManager,record.geometry);
#else
    using Fn=int(__fastcall*)(void*,void*);
    bool ok=false;
    __try {ok=reinterpret_cast<Fn>(module+0x84A954)(record.geometryManager,record.geometry)>=0;}
    __except(EXCEPTION_EXECUTE_HANDLER){fault=true;}
#endif
    if(ok){record.geometry=nullptr;record.geometryManager=nullptr;}return ok;
}
int Destroy(phase::Context& context,void* body) noexcept {
#if defined(KCD2VR_WEAPON_BODY_T4_HOST_TEST)
    return HostDestroy(context,body);
#else
    using Fn=int(__fastcall*)(void*,void*,int,int);
    // Native queued-destroy consumer6F015C/6F0168 itself passes safe1 while
    // holding these locks;847120/847133 skips recursive5FDC acquisition.
    __try {return reinterpret_cast<Fn>(module+0x846E10)(context.World(),body,0,1);}
    __except(EXCEPTION_EXECUTE_HANDLER){fault=true;return -999;}
#endif
}
void Publish(State state,Reason reason,const phase::RigidSnapshot* body=nullptr,void* bodyIdentityH24=nullptr) noexcept {
    Achieved out{};out.owner=record.input.owner;out.state=state;out.reason=reason;
    out.sample=record.input.sample;out.sourceTick=record.input.tick;out.observedTick=Now();
    out.intentFrameH28=record.input.intentFrameH28;
    out.trackingEpoch=record.input.trackingEpoch;out.cancelGeneration=record.input.cancelGeneration;
    out.colliderLease=record.input.colliderLease;out.worldGeneration=record.generation;out.bodyId=record.lease.Id();
    out.bodyGeneration=record.incarnation;
    out.npcContactControlH5=selfPairH5.controlWord;out.npcContactsH5=selfPairH5.open;
    out.colliderRetired=!record.lease.Held()&&!record.geometry&&!record.quarantined;
    out.safeNativeRestore=out.colliderRetired&&!(control.load(std::memory_order_acquire)&1);
    if(body&&state==State::Active&&!record.destroyRequested&&!record.quarantined){
        out.weaponWorld=policy::weaponPose({{body->orientation.x,body->orientation.y,body->orientation.z,body->orientation.w},
            {body->position.x,body->position.y,body->position.z}},record.shape);
        out.linear={body->linearVelocity.x,body->linearVelocity.y,body->linearVelocity.z};
        out.angular={body->angularVelocity.x,body->angularVelocity.y,body->angularVelocity.z};out.mass=body->mass;
        out.poseValid=policy::finite(out.weaponWorld);
    }
    const bool published=results.publish(out);
    if(published&&out.poseValid)activeGeneration.store(record.incarnation,std::memory_order_release);
    ContactOwnerH24 identity{};
    if(published&&out.poseValid&&bodyIdentityH24&&record.colliding&&!record.destroyRequested&&!record.quarantined){
        identity.intentFrameH28=out.intentFrameH28;identity.owner=out.owner;identity.weaponWorld=out.weaponWorld;identity.linear=out.linear;identity.angular=out.angular;identity.mass=out.mass;
        identity.body=reinterpret_cast<std::uintptr_t>(bodyIdentityH24);identity.foreign=reinterpret_cast<std::uintptr_t>(record.foreignCookieH24);
        identity.foreignType=foreignType;identity.bodyId=out.bodyId;identity.worldGeneration=out.worldGeneration;
        identity.bodyGeneration=out.bodyGeneration;identity.colliderLease=out.colliderLease;identity.trackingEpoch=out.trackingEpoch;
        identity.cancelGeneration=out.cancelGeneration;identity.sourceSample=out.sample;identity.sourceTick=out.sourceTick;
        identity.observedTick=out.observedTick;identity.active=true;
    }
    (void)contactOwnersH24.publish(identity);
}
bool Fresh(const Input& in,Packet* latest=nullptr) noexcept {
    Packet live{};
    if((control.load(std::memory_order_acquire)&13u)!=1u||!requests.read(live)||live.ticket<=cutoff.load(std::memory_order_acquire)||
       live.input.owner!=in.owner||live.input.trackingEpoch!=in.trackingEpoch||live.input.cancelGeneration!=in.cancelGeneration||
       live.input.colliderLease!=in.colliderLease||live.input.nativeBody!=in.nativeBody||live.input.nativeBodyId!=in.nativeBodyId||
       live.input.nativeProxy!=in.nativeProxy||live.input.playerBody!=in.playerBody||live.input.playerBodyId!=in.playerBodyId||
       live.input.playerEntityPointer!=in.playerEntityPointer||policy::eligible(live.input,Now())!=Reason::None)return false;
    const auto predicate=sourcePredicate.load(std::memory_order_acquire);
    if(!predicate||!predicate(live.input))return false;
    if(latest)*latest=live;return true;
}
bool PairSourceCurrentH5(phase::Context&,const Input&) noexcept;
Reason SourceReason(phase::Context& context,const Input& in) noexcept {
    if(!Fresh(in))return Reason::StaleInput;
    if(!phase::ObserveSuspended(context,in.nativeBodyId,reinterpret_cast<void*>(in.nativeBody),
        reinterpret_cast<void*>(in.owner.entityPointer)))return Reason::NativeCollider;
    if(!phase::ObserveLivingExcluded(context,in.playerBodyId,reinterpret_cast<void*>(in.playerBody),
        reinterpret_cast<void*>(in.playerEntityPointer)))return Reason::PlayerFilter;
    if(selfPairH5.open&&!PairSourceCurrentH5(context,in))return Reason::SelfPairClosingH5;
    return Fresh(in)?Reason::None:Reason::StaleInput;
}
bool Source(phase::Context& context,const Input& in) noexcept {return SourceReason(context,in)==Reason::None;}
#include "native_weapon_self_pair_h5.inl"
void ClearRecord() noexcept {
    record.constraintPinned=false;
    record.hasGeometry=false;record.partAttempted=false;record.colliding=false;record.destroyRequested=false;
    record.generation=0;record.createdSerial=0;record.lastDrive=0;record.retiredFor=Reason::None;record.shape={};
    record.incarnation=0;record.foreignCookieH24=nullptr;activeGeneration.store(0,std::memory_order_release);control.fetch_and(~2u,std::memory_order_acq_rel);
    selfPairH5.refusedWord=0;
}
void Retiring(phase::Context& context,Reason why) noexcept {
    activeGeneration.store(0,std::memory_order_release);
    if(!record.destroyRequested&&policy::owner(record.input.owner)&&
       (why==Reason::Occupied||why==Reason::ClearanceUnknown||why==Reason::QueryCallerUnavailableH5||why==Reason::NativeUnavailable||
        why==Reason::NativeCollider||why==Reason::PlayerFilter||why==Reason::ExcessError||
        why==Reason::ConfigurePending||why==Reason::MotionUnknown)){
        rejected={record.input,Now()+750,activation.load(std::memory_order_acquire),why,true};
    }
    record.retiredFor=why;
    // Closing the living class precedes even an external pin's deferred
    // retirement. Never remove its part or our pair while that pin is held.
    if((selfPairH5.attempted||selfPairH5.living.Held())&&record.generation==context.Generation()){
        if(!ClosePairClassH5(context)){record.quarantined=true;Publish(State::Quarantined,Reason::NativeFault);return;}
    }
    if(record.constraintPinned){Publish(State::Retiring,Reason::RetirePending);return;}
    if(fault.load(std::memory_order_acquire)&&record.lease.Held()){
        record.quarantined=true;Publish(State::Quarantined,Reason::NativeFault);return;
    }
    if(record.quarantined){Publish(State::Quarantined,why);return;}
    if(!record.lease.Held()){
        if(record.geometry&&!Unregister(context)){record.quarantined=true;Publish(State::Quarantined,Reason::NativeFault);return;}
        ClearRecord();Publish(Enabled()?State::Waiting:State::Off,why);return;
    }
    if(record.generation!=context.Generation()){
        (void)phase::AbandonInvalidated(context,record.lease);
        if(selfPairH5.living.Held())(void)phase::AbandonLivingH24(context,selfPairH5.living);
        record.quarantined=true;
        Publish(State::Quarantined,Reason::WorldChanged);return;
    }
    const int pairRetired=RetirePairH5(context);
    if(pairRetired<=0){if(pairRetired<0)record.quarantined=true;
        Publish(pairRetired<0?State::Quarantined:State::Retiring,Reason::SelfPairClosingH5);return;}
    if(record.destroyRequested){
        const auto registry=phase::ObserveRegistry(context,record.lease);
        if(registry!=phase::Registry::Retired){Publish(State::Retiring,Reason::RetirePending);return;}
        if(!phase::Release(context,record.lease)||!Unregister(context)){
            record.quarantined=true;Publish(State::Quarantined,Reason::NativeFault);return;}
        ClearRecord();Publish(Enabled()?State::Waiting:State::Off,why);return;
    }
    // Creation has a queued insertion record. Cancellation cannot destroy its
    // pointer until the native drain has consumed that record and cleared31.
    if(record.lease.InsertionPending()&&!phase::ReadyRigid(context,record.lease)){
        Publish(State::Retiring,Reason::CreatePending);return;
    }
    void* body=phase::NativeHandle(context,record.lease,true);
    if(!body){Publish(State::Retiring,Reason::BodyUnknown);return;}
    if(record.hasGeometry||record.partAttempted){
        // Actual queue consumer6F026D/6F027A uses RemoveGeometry(...,1) under
        // these exact native mutation locks. No collision survives pending drop.
        if(!RemoveGeometry(context,body)){record.quarantined=true;Publish(State::Quarantined,Reason::NativeFault);return;}
        record.hasGeometry=false;record.partAttempted=false;record.colliding=false;
    }
    record.destroyRequested=true; // Never repeat an uncertain irreversible call.
    const int result=Destroy(context,body);
    Publish(result==-999?State::Quarantined:State::Retiring,result>0?Reason::RetirePending:Reason::NativeFault);
    if(result==-999)record.quarantined=true;
}
void Reset(phase::Context& context,void*) noexcept {
    kcd2vr::physics::committed_push_h21::Reset(context);
    // DS-F85 (Codex reset contract): the grab producer + proxy reset run in the
    // SAME borrowed context BEFORE the weapon's own cleanup - authenticated
    // old-world reset only, never a fabricated Absent or a game-thread reset.
    kcd2vr::physics::grab_producer60::PhysicsReset(&context,Now());
    if(context.Kind()==phase::Phase::Shutdown)kcd2vr::physics::hand_proxy60::ServiceStep(context);
    else kcd2vr::physics::hand_proxy60::ServiceReset(context);
    activeGeneration.store(0,std::memory_order_release);
    if(context.Kind()==phase::Phase::WorldReplaced){
        if(selfPairH5.living.Held()){(void)phase::AbandonLivingH24(context,selfPairH5.living);record.quarantined=true;}
        if(record.lease.Held()||record.geometry){(void)phase::AbandonInvalidated(context,record.lease);record.quarantined=true;}
        Publish(record.quarantined?State::Quarantined:State::Waiting,Reason::WorldChanged);return;
    }
    if(context.Kind()!=phase::Phase::WorldReset){Retiring(context,Reason::Disabled);return;}
    // R96: a reset callback is not proof that an uncertain factory, registered
    // geometry or submitted constraint died. A known current body may still
    // repay its exact cleanup below; preserve every other occurrence unchanged.
    const bool recoveringR96=record.quarantined||fault.load(std::memory_order_acquire);
    const bool knownCleanupR96=context.Current()&&record.generation==context.Generation()&&
        record.lease.Held()&&record.geometry&&record.geometryManager&&
        !record.constraintPinned&&!selfPairH5.uncertain;
    if(recoveringR96&&!knownCleanupR96){
        record.quarantined=true;Publish(State::Quarantined,Reason::NativeFault);return;
    }
    // Native reset will discard the queue and destroy its own bodies. Remove
    // this exact collider and release our strong reference BEFORE that reset.
    const int pairRetired=RetirePairH5(context);
    if(pairRetired<=0){record.quarantined=true;Publish(State::Quarantined,Reason::SelfPairClosingH5);return;}
    if(record.lease.Held()){
        bool removed=true;
        if(record.hasGeometry||record.partAttempted){void* body=phase::NativeHandle(context,record.lease,true);removed=body&&RemoveGeometry(context,body);}
        if(!removed||!phase::Release(context,record.lease)){
            record.quarantined=true;Publish(State::Quarantined,Reason::NativeFault);return;}
    }
    if(!Unregister(context)){record.quarantined=true;Publish(State::Quarantined,Reason::NativeFault);return;}
    // Only the native acknowledgements above repay this known occurrence.
    // Do not let its old fault/quarantine poison the next freshly qualified
    // owner after there is no retained part, body reference or geometry.
    if(recoveringR96){record.quarantined=false;fault.store(false,std::memory_order_release);
        InstallTraceB2("WORLD_RESET_EXACT_CLEANUP_RECOVERED_R96",true);}
    ClearRecord();cutoff.store(ticket.load(std::memory_order_acquire),std::memory_order_release);
    Publish(Enabled()?State::Waiting:State::Off,Reason::WorldChanged);
}
bool NeedsStepD80(void*) noexcept {
    // Called only by the serialized phase owner, before any native reads.
    // F9 admission itself wakes the current world-stamp producer; otherwise a
    // new contact request could never acquire its first qualified world stamp.
    return control.load(std::memory_order_acquire)!=0||record.lease.Held()||record.geometry||record.quarantined||
        record.constraintPinned||selfPairH5.living.Held()||selfPairH5.attempted||selfPairH5.uncertain||
        kcd2vr::collab_h21::enabled.load(std::memory_order_acquire)||
        kcd2vr::physics::grab_producer60::NeedsPhysicsStepD80()||
        kcd2vr::physics::hand_proxy60::NeedsPhysicsStepD80(Now());
}
void Step(phase::Context& context,void*) noexcept {
    if(!context.Current())return;
    kcd2vr::physics::committed_push_h21::Step(context,Now());
    // DS-F72: the grab pipeline runs in the SAME T4 callback - the proxy owner
    // steps FIRST (so its achieved pose/handle are current this frame), then
    // the producer drives the backend against that state. Native grab ops stay
    // same-stack; no raw handle crosses the callback boundary.
    kcd2vr::physics::grab_producer60::PhysicsRefresh(&context,Now());
    kcd2vr::physics::hand_proxy60::ServiceStep(context);
    kcd2vr::physics::grab_producer60::PhysicsStep(&context,Now());
    if(control.load(std::memory_order_acquire)==0&&!record.lease.Held()&&!record.geometry&&!record.quarantined)return;
    if(record.quarantined){Publish(State::Quarantined,Reason::NativeFault);return;}
    if(record.lease.Held()&&selfPairH5.attempted&&
       (selfPairH5.closing||!PairControlCurrentH5(record.input,selfPairH5.controlWord))){
        Retiring(context,Reason::SelfPairClosingH5);return;
    }
    Packet packet{};Reason why=Reason::None;
    if(!Enabled())why=Reason::Disabled;
    else if(!requests.read(packet)||packet.ticket<=cutoff.load(std::memory_order_acquire))why=Reason::NoInput;
    else why=policy::eligible(packet.input,Now());
    if(fault)why=Reason::NativeFault;
    if(why!=Reason::None){Retiring(context,why);return;}
    if(!record.lease.Held()&&rejected.valid){
        const auto& in=packet.input;const auto& old=rejected.input;
        const bool same=in.owner==old.owner&&in.trackingEpoch==old.trackingEpoch&&in.cancelGeneration==old.cancelGeneration&&
            in.colliderLease==old.colliderLease&&rejected.activation==activation.load(std::memory_order_acquire);
        if(same){
            const bool moved=pose_core::length(in.targetWorld.p-old.targetWorld.p)>=.10||
                pose_core::angle(in.targetWorld.q,old.targetWorld.q)>=.26;
            if(Now()<rejected.until||!moved){record.input=in;Publish(State::Unavailable,rejected.reason);return;}
        }
        rejected.valid=false;
    }
    if(record.lease.Held()&&(record.input.owner!=packet.input.owner||record.input.colliderLease!=packet.input.colliderLease||
       record.input.trackingEpoch!=packet.input.trackingEpoch||record.input.cancelGeneration!=packet.input.cancelGeneration)){
        Retiring(context,Reason::BodyReplaced);return;}
    if(record.lease.Held()){
        const auto shape=policy::shape(packet.input);
        if(!shape.valid||pose_core::length(shape.bodyLocal.p-record.shape.bodyLocal.p)>1e-6||
           pose_core::length(shape.axis-record.shape.axis)>1e-6||std::abs(shape.radius-record.shape.radius)>1e-6||
           std::abs(shape.halfHeight-record.shape.halfHeight)>1e-6||std::abs(shape.mass-record.shape.mass)>1e-6){
            Retiring(context,Reason::BodyReplaced);return;}
    }
    if(record.destroyRequested){Retiring(context,record.retiredFor);return;}
    if(!record.lease.Held())record.input=packet.input;
    const auto sourceReason=SourceReason(context,packet.input);
    if(sourceReason!=Reason::None){Retiring(context,sourceReason);return;}
    if(!record.lease.Held()){
        record.input=packet.input;record.shape=policy::shape(record.input);record.generation=context.Generation();
        if(!record.shape.valid){Publish(State::Unavailable,Reason::Unsupported);return;}
        if(!Bind(context,record.geometryManager)){Retiring(context,Reason::NativeUnavailable);return;}
        if(!Source(context,record.input))return;
        const Pose initial=policy::compose(record.input.weaponWorld,record.shape.bodyLocal);
        const auto ownerVersion=retainedVersion.load(std::memory_order_acquire)+1;
        if(!retainedOwners.publish({record.input.owner,ownerVersion}))return;
        retainedVersion.store(ownerVersion,std::memory_order_release);
        unsigned admission=1;
        if(!control.compare_exchange_strong(admission,3,std::memory_order_acq_rel)){Retiring(context,Reason::Disabled);return;}
        admissions.fetch_add(1,std::memory_order_acq_rel);
        activeGeneration.store(0,std::memory_order_release);
        if(!Source(context,record.input)){ClearRecord();Publish(State::Waiting,Reason::NoInput);return;}
        record.foreignCookieH24=ReserveCookieH24();
        if(!record.foreignCookieH24){ClearRecord();Publish(State::Unavailable,Reason::ContactIdentityExhaustedH24);return;}
        record.incarnation=incarnations;
        void* created=Create(context,initial);
        if(created==reinterpret_cast<void*>(-1)){record.quarantined=true;Publish(State::Quarantined,Reason::NativeFault);return;}
        if(!created){Retiring(context,Reason::NativeUnavailable);return;}
        if(!phase::AdoptCreatedRigid(context,created,record.foreignCookieH24,foreignType,record.lease)){
            record.quarantined=true;Publish(State::Quarantined,Reason::BodyUnknown);return;}
        record.createdSerial=context.Serial();Publish(State::Creating,Reason::CreatePending);return;
    }
    record.input=packet.input;
    if(record.generation!=context.Generation()){Retiring(context,Reason::WorldChanged);return;}
    if(!phase::ReadyRigid(context,record.lease)||context.Serial()<=record.createdSerial){Publish(State::Creating,Reason::CreatePending);return;}
    void* body=phase::NativeHandle(context,record.lease);
    if(!body){Retiring(context,Reason::BodyUnknown);return;}
    if(!record.hasGeometry){
        if(!record.geometry)record.geometry=MakeGeometry(context,record.geometryManager,record.shape);
        if(!record.geometry||!Fresh(record.input)||!AddGeometry(context,body,false)||!Configure(context,body)||!Part(context,body,false)){
            Retiring(context,fault?Reason::NativeFault:Reason::ConfigurePending);return;}
        record.hasGeometry=true;record.partAttempted=false;Publish(State::Configuring,Reason::ConfigurePending);return;
    }
    phase::RigidSnapshot actual{};
    if(!phase::ReadRigid(context,record.lease,actual)||!std::isfinite(actual.mass)||
       std::abs(actual.mass-record.shape.mass)>.02*record.shape.mass||!Filters(context,body)){
        Retiring(context,Reason::MotionUnknown);return;}
    if(!record.colliding){
        const Vec3 axis=pose_core::rotate({actual.orientation.x,actual.orientation.y,actual.orientation.z,actual.orientation.w},record.shape.axis);
        clearance::Capsule volume{{actual.position.x,actual.position.y,actual.position.z},
            {float(axis.x),float(axis.y),float(axis.z)},float(record.shape.radius),float(record.shape.halfHeight)};
        const auto clear=clearance::Query(context,volume,body);
        if(clear!=clearance::Result::Clear){Retiring(context,clear==clearance::Result::Occupied?Reason::Occupied:
            clear==clearance::Result::CallerUnavailableH5?Reason::QueryCallerUnavailableH5:Reason::ClearanceUnknown);return;}
        if(!Source(context,record.input)){Retiring(context,Reason::NativeCollider);return;}
        if(!RemoveGeometry(context,body)){Retiring(context,Reason::NativeFault);return;}
        record.hasGeometry=false;
        // Both geometry leaves are immediate only in this exact phase; native
        // queue6F0293..6F029B is the matching AddGeometry(...,1) ABI precedent.
        if(!Fresh(record.input)||!AddGeometry(context,body,true)){Retiring(context,Reason::NativeFault);return;}
        record.hasGeometry=true;record.partAttempted=false;record.colliding=true;
        if(!Part(context,body,true)||!phase::ReadRigid(context,record.lease,actual)||!Filters(context,body)){
            Retiring(context,Reason::ConfigurePending);return;}
        record.lastDrive=Now();Publish(State::Active,Reason::None,&actual,body);return;
    }
    if(!Part(context,body,true)||!Source(context,record.input)){Retiring(context,Reason::BodyUnknown);return;}
    const auto pairReason=PreparePairH5(context,body);
    if(record.quarantined){Publish(State::Quarantined,pairReason);return;}
    if(pairReason==Reason::SelfPairClosingH5){Retiring(context,pairReason);return;}
    const auto now=Now();const double dt=record.lastDrive&&now>=record.lastDrive?double(now-record.lastDrive)/1000.:0;
    if(dt>=.004){
        const Pose achieved{{actual.orientation.x,actual.orientation.y,actual.orientation.z,actual.orientation.w},
            {actual.position.x,actual.position.y,actual.position.z}};
        const auto motion=policy::motor(record.input,record.shape,achieved,
            {actual.linearVelocity.x,actual.linearVelocity.y,actual.linearVelocity.z},
            {actual.angularVelocity.x,actual.angularVelocity.y,actual.angularVelocity.z},actual.mass,std::min(dt,.033));
        if(!motion.valid){Retiring(context,motion.reason);return;}
        const auto motionSource=SourceReason(context,record.input);
        if(motionSource!=Reason::None){Retiring(context,motionSource);return;}
        if(!phase::SetVelocity(context,record.lease,{float(motion.linear.x),float(motion.linear.y),float(motion.linear.z)},
            {float(motion.angular.x),float(motion.angular.y),float(motion.angular.z)})){Retiring(context,Reason::NativeFault);return;}
        record.lastDrive=now;
    }
    if(!Fresh(record.input)){Retiring(context,Reason::StaleInput);return;}
    // Pair preparation/refusal retains the original class4 world-only motor.
    Publish(State::Active,pairReason,&actual,body); // Actual body, not the velocity request or controller target.
}
}
void SetEnabled(bool value) noexcept {
    if(value){if(!(control.fetch_or(1u,std::memory_order_acq_rel)&1))activation.fetch_add(1,std::memory_order_acq_rel);}
    else {control.fetch_and(~1u,std::memory_order_acq_rel);Cancel();}
}
// Bit4 participates in the SAME atomic1->3 admission CAS. If suspension
// wins, no Create can start; if admission wins, bit2 remains an explicit
// actual-retirement obligation. User Enabled remains bit1 only.
void SetEquipmentAdmissionSuspendedH36(bool value) noexcept {
    if(value)control.fetch_or(4u,std::memory_order_acq_rel);
    else control.fetch_and(~4u,std::memory_order_acq_rel);
}
bool EquipmentAdmissionSuspendedH36() noexcept {return (control.load(std::memory_order_acquire)&4u)!=0;}
bool Enabled() noexcept {return (control.load(std::memory_order_acquire)&1)!=0;}
void SetThrowAdmissionSuspendedT23(bool value) noexcept {
    if(value)control.fetch_or(8u,std::memory_order_acq_rel);
    else control.fetch_and(~8u,std::memory_order_acq_rel);
}
bool ThrowAdmissionSuspendedT23() noexcept {return (control.load(std::memory_order_acquire)&8u)!=0;}
bool HasUnretiredCollider() noexcept {return (control.load(std::memory_order_acquire)&2)!=0;}
std::uint64_t AdmissionSerial() noexcept {return admissions.load(std::memory_order_acquire);}
bool ReadRetainedOwner(Owner& out) noexcept {
    const auto version=retainedVersion.load(std::memory_order_acquire);RetainedOwner value{};
    if(!HasUnretiredCollider()||!retainedOwners.read(value)||value.version!=version||
       retainedVersion.load(std::memory_order_acquire)!=version||!HasUnretiredCollider())return false;
    out=value.owner;return true;
}
bool SetSourcePredicate(SourcePredicate value) noexcept {
    if(!value)return false;auto expected=SourcePredicate{};
    return sourcePredicate.compare_exchange_strong(expected,value,std::memory_order_acq_rel)||expected==value;
}
void Submit(const Input& input) noexcept {Packet p{input,ticket.fetch_add(1,std::memory_order_acq_rel)+1};(void)requests.publish(p);}
void Cancel() noexcept {cutoff.store(ticket.load(std::memory_order_acquire),std::memory_order_release);}
void Retire(const Owner& owner) noexcept {Packet p{};if(requests.read(p)&&p.input.owner==owner)Cancel();}
bool Read(Achieved& out) noexcept {
    if(!results.read(out)){
        out={};const bool retired=control.load(std::memory_order_acquire)==0;
        out.colliderRetired=retired;out.safeNativeRestore=retired;return retired;
    }
    const bool pending=(control.load(std::memory_order_acquire)&2)!=0;
    if(pending){out.colliderRetired=false;out.safeNativeRestore=false;}
    if(!out.bodyGeneration||out.bodyGeneration!=activeGeneration.load(std::memory_order_acquire)||
       !Enabled()||!out.sourceTick||Now()<out.sourceTick||Now()-out.sourceTick>80)out.poseValid=false;
    if(out.npcContactsH5&&out.npcContactControlH5!=deepseek_h29::ReadWeaponNpcContactControlH5())out.poseValid=false;
    Packet current{};const auto predicate=sourcePredicate.load(std::memory_order_acquire);
    if(out.poseValid&&(!requests.read(current)||current.ticket<=cutoff.load(std::memory_order_acquire)||
       current.input.owner!=out.owner||current.input.trackingEpoch!=out.trackingEpoch||
       current.input.cancelGeneration!=out.cancelGeneration||current.input.colliderLease!=out.colliderLease||
       !predicate||!predicate(current.input)))out.poseValid=false;
    if(Enabled()){out.safeNativeRestore=false;if(out.state==State::Off){out.state=State::Waiting;out.reason=Reason::NoInput;}}
    if(control.load(std::memory_order_acquire)==0){out.colliderRetired=true;out.safeNativeRestore=true;out.state=State::Off;out.poseValid=false;}
    // SourcePredicate may race UI revocation. Its successful return cannot
    // republish an earlier ON interval after OFF or OFF/ON changed the word.
    if(out.poseValid&&out.npcContactsH5&&out.npcContactControlH5!=deepseek_h29::ReadWeaponNpcContactControlH5())out.poseValid=false;
    return true;
}
bool ReadContactOwnerH24(ContactOwnerH24& out) noexcept {
    out={};ContactOwnerH24 id{};Achieved live{};
    if(!contactOwnersH24.read(id)||!id.active||!id.body||!id.foreign||id.foreignType!=foreignType||
       !id.bodyGeneration||!id.worldGeneration||id.bodyId<0||!Read(live)||!live.poseValid||
       live.state!=State::Active||live.colliderRetired||live.owner!=id.owner||live.bodyId!=id.bodyId||
       live.worldGeneration!=id.worldGeneration||live.bodyGeneration!=id.bodyGeneration||live.colliderLease!=id.colliderLease||
       live.trackingEpoch!=id.trackingEpoch||live.cancelGeneration!=id.cancelGeneration||live.sample!=id.sourceSample||
       live.sourceTick!=id.sourceTick||live.observedTick!=id.observedTick)return false;
    if(live.npcContactsH5&&live.npcContactControlH5!=deepseek_h29::ReadWeaponNpcContactControlH5())return false;
    out=id;return true;
}
bool Install() noexcept {
#if !defined(KCD2VR_WEAPON_BODY_T4_HOST_TEST)
    const auto callH7=installCallsH7.fetch_add(1,std::memory_order_relaxed)+1;
    char entryH7[340]{};std::snprintf(entryH7,sizeof(entryH7),"B2 F9_INSTALL stage=ENTER call=%llu attempted=%d installed=%d fault=%d firstMutatingFailure=%s phaseFailure=%s",
        static_cast<unsigned long long>(callH7),attempted.load()?1:0,installed.load()?1:0,fault.load()?1:0,
        firstMutatingFailureH7.load(std::memory_order_acquire),phase::InstallFailureStage());
    diagnostic_trace::Write(entryH7);
#endif
    if(fault.load(std::memory_order_acquire)){InstallTraceB2("NATIVE_FAULT_LATCH",false);return false;}
    if(installed.load(std::memory_order_acquire))return true;
    if(installing.test_and_set(std::memory_order_acquire))return false;
    if(attempted.load(std::memory_order_acquire)||fault.load(std::memory_order_acquire)){InstallTraceB2("PREVIOUS_MUTATING_ATTEMPT_OR_FAULT",false);installing.clear(std::memory_order_release);return false;}
    bool ready=false;
#if defined(KCD2VR_WEAPON_BODY_T4_HOST_TEST)
    ready=HostInstall();
#else
    module=reinterpret_cast<std::uintptr_t>(GetModuleHandleW(L"WHGame.dll"));
    ready=module&&retail_build112::Ready(module);InstallTraceB2("RETAIL_READY",ready);
    if(ready){ready=ExactContract();InstallTraceB2("BODY_BYTE_PINS_AND_SLOTS",ready);}
#endif
    if(ready){ready=clearance::Install();InstallTraceB2("CLEARANCE",ready);}
    if(ready){ready=phase::Preflight();InstallTraceB2("PHYSICS_PHASE_PREFLIGHT",ready);}
    if(ready){ // DS-F76 (Codex): the grab is OPTIONAL - a failed grab preflight must NEVER fail the existing weapon physics
        const bool grabOk=kcd2vr::physics::hand_proxy60::Install(module);
        if(grabOk){kcd2vr::physics::grab_producer60::Backend be{};kcd2vr::physics::native_grab_backend60::FillBackend(be);(void)kcd2vr::physics::grab_producer60::InstallBackend(be);}
        (void)kcd2vr::physics::grab_input60::Install(module); // DS-F88: the input resolver's base (non-vetoing)
        InstallTraceB2("GRAB_OPTIONAL",grabOk); // informational, not a gate
    }
    // B4: preflight above has no hook/client/body side effects. A transient
    // unavailable retail module or rejected pins can be rechecked by a later
    // explicit F9 request. Once registration/hook mutation may begin, preserve
    // the process-lifetime refusal latch; never retry a partially installed hook.
    if(ready){attempted.store(true,std::memory_order_release);
        ready=phase::RegisterClient({&Step,&Reset,nullptr,&NeedsStepD80});InstallTraceB2("REGISTER_PHASE_CLIENT",ready);
        if(!ready)firstMutatingFailureH7.store("REGISTER_PHASE_CLIENT",std::memory_order_release);}
    if(ready){ready=phase::Install();InstallTraceB2("PHYSICS_PHASE_HOOKS",ready);
        if(!ready)firstMutatingFailureH7.store("PHYSICS_PHASE_HOOKS",std::memory_order_release);}
    installed.store(ready,std::memory_order_release);installing.clear(std::memory_order_release);return ready;
}
phase::Lease* CleanupColliderLease(phase::Context& c,const Owner& owner,std::uint64_t incarnation) noexcept {
    if(!c.Current()||c.ModuleBase()!=module||!incarnation||record.incarnation!=incarnation||
       record.input.owner!=owner||record.generation!=c.Generation()||record.quarantined||!record.lease.Held())return nullptr;
    return phase::NativeHandle(c,record.lease)?&record.lease:nullptr;
}
phase::Lease* CurrentColliderLease(phase::Context& c,const Owner& owner,std::uint64_t incarnation) noexcept {
    if(c.Kind()!=phase::Phase::Step||record.destroyRequested||!Fresh(record.input))return nullptr;
    return CleanupColliderLease(c,owner,incarnation);
}
bool PinColliderConstraint(phase::Context& c,const Owner& owner,std::uint64_t incarnation,bool pin) noexcept {
    if(!(pin?CurrentColliderLease(c,owner,incarnation):CleanupColliderLease(c,owner,incarnation)))return false;
    record.constraintPinned=pin;return true;
}
const char* ReasonName(Reason reason) noexcept {
    constexpr const char* names[]{"NONE","DISABLED","NO_INPUT","UNSUPPORTED","INVALID_INPUT","STALE_INPUT","NATIVE_COLLIDER",
        "NATIVE_UNAVAILABLE","PLAYER_FILTER","OCCUPIED","CLEARANCE_UNKNOWN","CREATE_PENDING","CONFIGURE_PENDING","BODY_UNKNOWN",
        "BODY_REPLACED","MOTION_UNKNOWN","EXCESS_ERROR","NATIVE_FAULT","RETIRE_PENDING","WORLD_CHANGED","CONTACT_IDENTITY_EXHAUSTED",
        "QUERY_CALLER_UNAVAILABLE_H5","SELF_PAIR_PENDING_H5","SELF_PAIR_REFUSED_H5","SELF_PAIR_CLOSING_H5"};
    const auto i=static_cast<unsigned>(reason);return i<std::size(names)?names[i]:"UNKNOWN";
}
}
