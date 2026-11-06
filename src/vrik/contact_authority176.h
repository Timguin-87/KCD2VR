#pragma once
#include "../core/snapshot_mailbox.h"
#include <atomic>
#include <cmath>
#include <cstdint>

namespace kcd2vr::contact176 {
// Gameplay authority, independent of every recorder. These are mod-owned PODs,
// never replacements for a retail contact, request, sample vector or hit object.
struct Identity {
    std::uintptr_t player=0,entity=0,actor=0,collisions=0;
    std::uint64_t epoch=0,request=0,key=0,generation=0,sample=0,tick=0;
};
struct Ticket {
    std::uintptr_t player=0,entity=0;
    std::uint64_t serial=0,tick=0,sample=0,epoch=0,request=0,key=0,generation=0,policy=0;
    std::uint64_t gesture59c3=0,recovery59c3=0;
};
struct Live { Identity identity{}; Ticket ticket{}; std::uint64_t now=0; };
struct Payload {
    Identity identity{}; Ticket ticket{};
    std::uintptr_t wrapper=0,action=0;
    int hand=-1,bone=-1;
    float dt=0;
    bool redirected=false,moving=false;
    bool nativeFistShapeH31=false; // scoped original native query, never a hit permission
};
inline kcd2vr::core::SnapshotMailbox<Ticket> released;
inline std::atomic<std::uint64_t> nextSerial{0},claimedSerial{0},policy{1};
// Unlike an input policy change, handling recovery can never reuse an admitted
// action's old contact. Queue acquisition and final dispatch compare this epoch.
inline std::atomic<std::uint64_t> recoveryEpoch55c1{1};
// C2: release only an undelivered reservation. A native entry (including an
// exception or native rejection) is terminal. Compare-before-rollback cannot
// overwrite a newer claim, input-policy change or handling recovery.
class SerialReservationC2 {
    std::uint64_t serial_=0,prior_=0,policy_=0,recovery_=0;
public:
    SerialReservationC2() noexcept = default;
    SerialReservationC2(const SerialReservationC2&)=delete;
    SerialReservationC2& operator=(const SerialReservationC2&)=delete;
    bool reserve(std::uint64_t serial) noexcept {
        if(serial_||!serial)return false;
        const auto p=policy.load(std::memory_order_acquire),r=recoveryEpoch55c1.load(std::memory_order_acquire);
        auto used=claimedSerial.load(std::memory_order_acquire);const auto prior=used;
        if(used>=serial||!claimedSerial.compare_exchange_strong(used,serial,std::memory_order_acq_rel))return false;
        serial_=serial;prior_=prior;policy_=p;recovery_=r;return true;
    }
    void nativeCallStarted() noexcept {serial_=0;}
    ~SerialReservationC2(){
        if(serial_&&policy.load(std::memory_order_acquire)==policy_&&
           recoveryEpoch55c1.load(std::memory_order_acquire)==recovery_){
            auto expected=serial_;(void)claimedSerial.compare_exchange_strong(expected,prior_,std::memory_order_acq_rel);
        }
    }
};
inline thread_local const Payload* current=nullptr;
struct SweepScope {
    const Payload* prior; Payload value;
    explicit SweepScope(Payload p={}) noexcept:prior(current),value(p){current=&value;}
    ~SweepScope(){current=prior;}
    SweepScope(const SweepScope&)=delete; SweepScope& operator=(const SweepScope&)=delete;
};
inline bool fresh(std::uint64_t tick,std::uint64_t now,std::uint64_t maximum) noexcept {
    return tick&&now>=tick&&now-tick<=maximum;
}
inline bool same(const Identity& a,const Identity& b) noexcept {
    return a.player&&a.entity&&a.actor&&a.collisions&&a.epoch&&a.request&&a.key&&a.generation&&a.sample&&
        a.player==b.player&&a.entity==b.entity&&a.actor==b.actor&&a.collisions==b.collisions&&
        a.epoch==b.epoch&&a.request==b.request&&a.key==b.key&&a.generation==b.generation&&
        a.sample==b.sample&&a.tick==b.tick;
}
// V66.178R6: a physical RELEASE owns the native attack it started until that attack is
// replaced (new PREPARE), cancelled or ends - all of which bump `policy`. KCD2 processes
// the ordinary hit inside the attack's own sweep, 0.4-1.0 s after the blade passed
// (animation-bound), so the old 120 ms / 24-sample window could never be met and every
// F4 hit was refused as NO_FRESH_PHYSICAL_RELEASE (2026-10-08 runs, V179 review repro B).
// The live tracked geometry keeps its own freshness check in admit(); the ticket is
// one-use (claimedSerial). 1.5 s is only a safety cap, not the design window.
inline constexpr std::uint64_t kReleaseSafetyCapMs=1500; // sequence density is not a clock
inline bool ticketMatches(const Ticket& t,const Identity& i,std::uint64_t now) noexcept {
    return t.serial&&t.policy==policy.load(std::memory_order_acquire)&&fresh(t.tick,now,kReleaseSafetyCapMs)&&t.sample&&i.sample>=t.sample&& /* R59.16C6: monotonic identity, age is measured above */
        t.player==i.player&&t.entity==i.entity&&t.epoch==i.epoch&&t.request==i.request&&
        t.key==i.key&&t.generation==i.generation;
}
enum class Decision : unsigned { Admit,NoScope,Identity,PoseAge,NoRedirect,NoMotion,Ticket,Details,Target,Repeat };
inline const char* name(Decision d) noexcept {
    switch(d){case Decision::Admit:return "ADMIT_NATIVE_TRACKED_CONTACT";case Decision::NoScope:return "NO_SYNCHRONOUS_SWEEP";
    case Decision::Identity:return "LIVE_IDENTITY_CHANGED";case Decision::PoseAge:return "POSE_EXPIRED";
    case Decision::NoRedirect:return "NO_QUALIFIED_PREDICTOR_REDIRECT";case Decision::NoMotion:return "ZERO_TRACKED_MOTION";
    case Decision::Ticket:return "NO_FRESH_PHYSICAL_RELEASE";case Decision::Details:return "BORROWED_DETAILS_MISMATCH";
    case Decision::Target:return "TARGET_UNQUALIFIED";case Decision::Repeat:return "STROKE_ALREADY_CONSUMED";}
    return "UNKNOWN";
}
// Caller must first prove exact retail C52534 ordinary outgoing Henry producer.
// One native primary Process invocation per physical release, including rejected
// native outcomes. Native request sample vectors and all RPG decisions survive.
// Claim before entering native code, preventing reentrant duplicate admission.
inline Decision admit(const Live& live,std::uintptr_t details,std::uint32_t targetId) noexcept {
    const auto* s=current;
    if(!s)return Decision::NoScope;
    if(!same(s->identity,live.identity))return Decision::Identity;
    // V66.178R6: 150 ms / 125 ms instead of 80 / 50: the headset runs at 18-30 fps
    // (36-55 ms frames) with occasional 300 ms hitches; 80 ms was 1.5 frames.
    if(!fresh(s->identity.tick,live.now,150))return Decision::PoseAge;
    if(!s->redirected||!s->wrapper||!s->action||s->hand!=0||s->bone<0||
       !std::isfinite(s->dt)||s->dt<=0||s->dt>.125f)return Decision::NoRedirect;
    if(!s->moving)return Decision::NoMotion;
    if(!ticketMatches(s->ticket,s->identity,live.now)||!ticketMatches(live.ticket,live.identity,live.now)||
       s->ticket.serial!=live.ticket.serial||s->ticket.tick!=live.ticket.tick||s->ticket.sample!=live.ticket.sample)return Decision::Ticket;
    // A7A472 publishes this exact live object. C5235C/D53AD4 borrow it at rq+18.
    // No pointer survives the original synchronous A7A124 invocation.
    if(s->identity.collisions>UINTPTR_MAX-0x170||details!=s->identity.collisions+0x170)return Decision::Details;
    if(!targetId)return Decision::Target;
    auto used=claimedSerial.load(std::memory_order_acquire);
    if(used>=s->ticket.serial||!claimedSerial.compare_exchange_strong(used,s->ticket.serial,std::memory_order_acq_rel))return Decision::Repeat;
    return Decision::Admit;
}
}
