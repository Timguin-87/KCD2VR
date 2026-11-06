#pragma once
// C10R19C1: energy is copied with the exact validated VR command identity.
// Native asynchronous attack-cost callbacks are not joined by time or latest slot.
// Only our synchronous native raise/lower pair may scale its one known effect.
#include "motion_owned59c4.h"
#include "solver_rpg_provenance_h29.h"
#include <array>
#include <atomic>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <mutex>
namespace kcd2vr::swing_effort59c14 {
namespace mo=kcd2vr::motion59c4;
inline std::atomic<double> scale{1.0};
inline double factor(double strikeScale) noexcept {
    const double k=scale.load(std::memory_order_relaxed);
    if(!std::isfinite(k)||k<0)return 1.;
    return std::clamp(.30+.72*std::clamp(std::isfinite(strikeScale)?strikeScale:1.,0.,1.25),.30,1.20)*std::min(k,2.);
}
inline double ownRaiseMultiplier(double f,bool raisePaid=false) noexcept {return std::clamp((raisePaid?f-.30:f)/.30,0.,8.);}
enum class Status : unsigned { Empty,Pending,Reserved,Complete,Refused,Indeterminate,Cancelled,NativePending54,PreparationUncertainB3 };
enum class Claim : unsigned { Ready,Complete,Pending,Missing,Expired,Refused,Indeterminate,Cancelled };
enum class ContactProof54 : unsigned { Refuse,Complete,NativePending };
struct Record {mo::Key key{};std::uint64_t tick=0;double factor=1.,mass=1.1;bool support=false;Status status=Status::Empty;};
class Ledger {
    std::array<Record,32> entries_{};mutable std::mutex mutex_;
    // Native preparation can precede the physical energy publication. Retain
    // ONLY exact-key status; never fabricate a default energy/cost Record.
    struct NativeMarker54 {mo::Key key{};std::uint64_t tick=0;Status status=Status::Empty;};
    std::array<NativeMarker54,32> nativeMarkers54_{};std::uint64_t nativeOverflowUntil54_=0;
    bool marker54(const mo::Key& key,Status status,std::uint64_t now) noexcept {
        NativeMarker54* slot=nullptr;for(auto& m:nativeMarkers54_)if(mo::same(m.key,key)){slot=&m;break;}
        if(slot){if(slot->status==Status::Complete||slot->status==Status::Indeterminate||slot->status==Status::Cancelled)return slot->status==status;
            slot->status=status;slot->tick=now;return true;}
        for(auto& m:nativeMarkers54_)if(m.status==Status::Empty||!mo::fresh(m.tick,now,3500)){slot=&m;break;}
        if(!slot){nativeOverflowUntil54_=std::max(nativeOverflowUntil54_,now+3500);return false;}
        *slot={key,now,status};return true;
    }
public:
    void clear() noexcept {std::lock_guard<std::mutex> lock(mutex_);entries_={};nativeMarkers54_={};nativeOverflowUntil54_=0;}
    bool publish(const mo::Key& key,double energy,double mass,bool support,std::uint64_t tick) noexcept {
        if(!mo::valid(key)||!tick||!std::isfinite(energy)||!std::isfinite(mass))return false;
        std::lock_guard<std::mutex> lock(mutex_);
        for(const auto& r:entries_)if(r.status!=Status::Empty&&mo::same(r.key,key))return false; // immutable, including tombstones
        Record* slot=nullptr;
        for(auto& r:entries_)if(r.status==Status::Empty){slot=&r;break;}
        // Never overwrite a live pending debit or an in-flight native call.
        if(!slot)for(auto& r:entries_)if(r.status!=Status::Reserved&&tick>=r.tick&&tick-r.tick>1500&&(!slot||r.tick<slot->tick))slot=&r;
        if(!slot)return false;
        Status status=tick<=nativeOverflowUntil54_?Status::Indeterminate:Status::Pending;std::uint64_t costTick=tick;
        for(const auto& m:nativeMarkers54_)if(mo::same(m.key,key)&&m.status!=Status::Empty){
            const bool recent=mo::fresh(m.tick,tick,3500)||(m.tick>tick&&m.tick-tick<=100);
            // A completed native submission must not acquire a new lease from
            // late energy publication. The completed-pose timestamp can be up
            // to one existing input-frame lease before the observed callback.
            status=m.status==Status::Complete&&!recent?Status::Indeterminate:m.status;
            if(status==Status::Complete)costTick=std::min(tick,m.tick); // native completion never receives a fresh payment lease
            break;}
        *slot={key,costTick,factor(energy),std::clamp(mass,.3,6.),support,status};return true;
    }
    Claim reserve(const mo::Key& key,std::uint64_t now,Record& out) noexcept {
        std::lock_guard<std::mutex> lock(mutex_);
        for(auto& r:entries_)if(r.status!=Status::Empty&&mo::same(r.key,key)){
            out=r;
            switch(r.status){
            case Status::Complete:return Claim::Complete;
            case Status::Reserved:return Claim::Pending;
            case Status::NativePending54:return Claim::Pending; // never debit over a possibly-paid native quantum
            case Status::Refused:return Claim::Refused;
            case Status::Indeterminate:return Claim::Indeterminate;
            case Status::PreparationUncertainB3:return Claim::Indeterminate; // never retry/debit an unproved native preparation
            case Status::Cancelled:return Claim::Cancelled;
            default:break;
            }
            if(!mo::fresh(r.tick,now,1500))return Claim::Expired;
            r.status=Status::Reserved;out=r;return Claim::Ready;
        }
        return Claim::Missing;
    }
    // C54: read only proof of THIS gesture's completed synchronous native
    // effort. Pending/unknown asynchronous native costs never count as paid.
    bool paidFor54(const mo::Key& key,std::uint64_t now) const noexcept {
        std::lock_guard<std::mutex> lock(mutex_);
        for(const auto& r:entries_)if(mo::same(r.key,key))
            return r.status==Status::Complete&&mo::fresh(r.tick,now,1500);
        return false;
    }
    ContactProof54 contactProof54(const mo::Key& key,std::uint64_t now) const noexcept {
        // One locked snapshot: a concurrent native completion cannot fall
        // between separate paid/pending reads and incorrectly drop contact.
        std::lock_guard<std::mutex> lock(mutex_);
        for(const auto& r:entries_)if(mo::same(r.key,key)&&mo::fresh(r.tick,now,1500)){
            if(r.status==Status::Complete)return ContactProof54::Complete;
            if(r.status==Status::NativePending54)return ContactProof54::NativePending;
            break;
        }
        return ContactProof54::Refuse; // no energy record, stale or generic pending is not proof
    }
    // A completed actual native action may credit its OWN pending effort.
    // This does not debit, scale, or assume that an input request was accepted.
    bool nativeComplete54(const mo::Key& key,std::uint64_t now) noexcept {
        if(!mo::valid(key)||!now)return false;
        std::lock_guard<std::mutex> lock(mutex_);
        for(auto& r:entries_)if(mo::same(r.key,key)){
            if((r.status!=Status::Pending&&r.status!=Status::NativePending54&&r.status!=Status::PreparationUncertainB3)||!mo::fresh(r.tick,now,1500))return false;
            r.status=Status::Complete;return true;
        }
        return marker54(key,Status::Complete,now);
    }
    void nativeObserved54(const mo::Key& key,std::uint64_t now) noexcept {
        if(!mo::valid(key)||!now)return;std::lock_guard<std::mutex> lock(mutex_);
        for(auto& r:entries_)if(mo::same(r.key,key)){if(r.status==Status::Pending)r.status=Status::NativePending54;return;}
        (void)marker54(key,Status::NativePending54,now);
    }
    void nativeUncertain54(const mo::Key& key,std::uint64_t now) noexcept {
        if(!mo::valid(key)||!now)return;std::lock_guard<std::mutex> lock(mutex_);
        for(auto& r:entries_)if(mo::same(r.key,key)){
            if(r.status==Status::Pending||r.status==Status::NativePending54||r.status==Status::PreparationUncertainB3)r.status=Status::Indeterminate;return;}
        (void)marker54(key,Status::Indeterminate,now);
    }
    // B3: lack of a preparation witness is not an unknown *entered action*
    // debit. It still forbids own-debit retry and grants no payment. Only this
    // same key's separately proven, completed native action may supersede it.
    // Reserved/Indeterminate own work and failed native action work stay terminal.
    void preparationUncertainB3(const mo::Key& key,std::uint64_t now) noexcept {
        if(!mo::valid(key)||!now)return;std::lock_guard<std::mutex> lock(mutex_);
        for(auto& r:entries_)if(mo::same(r.key,key)){
            if(r.status==Status::Pending||r.status==Status::NativePending54)r.status=Status::PreparationUncertainB3;return;}
        (void)marker54(key,Status::PreparationUncertainB3,now);
    }
    bool reserved(const mo::Key& key) const noexcept {
        std::lock_guard<std::mutex> lock(mutex_);
        for(const auto& r:entries_)if(mo::same(r.key,key))return r.status==Status::Reserved;
        return false;
    }
    bool finish(const mo::Key& key,Status status) noexcept {
        if(status!=Status::Complete&&status!=Status::Refused&&status!=Status::Indeterminate)return false;
        std::lock_guard<std::mutex> lock(mutex_);
        for(auto& r:entries_)if(mo::same(r.key,key)&&r.status==Status::Reserved){r.status=status;return true;}
        return false;
    }
    void cancel(const mo::Key& key) noexcept {
        std::lock_guard<std::mutex> lock(mutex_);
        // An already entered/reserved native debit must finish; cancellation cannot reopen it.
        for(auto& r:entries_)if(mo::same(r.key,key)&&(r.status==Status::Pending||r.status==Status::NativePending54||r.status==Status::PreparationUncertainB3))r.status=Status::Cancelled;
        for(auto& m:nativeMarkers54_)if(mo::same(m.key,key)&&(m.status==Status::NativePending54||m.status==Status::PreparationUncertainB3))m.status=Status::Cancelled;
    }
};
inline Ledger ledger;
struct Input54 {mo::Key key{};std::uintptr_t actor=0,state=0;std::uint64_t tick=0;unsigned packetSerial54=0,packetStroke54=0;solver_command_h28::Stamp solverH28{};};
inline bool validInput54(const Input54& i) noexcept {return mo::valid(i.key)&&i.actor&&i.state&&i.tick;}
class InputRegistry54 {
    struct Entry {unsigned nonce=0;Input54 input{};};
    std::array<Entry,32> entries_{};unsigned serial_=0;std::mutex mutex_;
public:
    unsigned publish(const Input54& i,std::uint64_t now,unsigned serial=0,unsigned stroke=0) noexcept {
        if(!validInput54(i)||!mo::fresh(i.tick,now,100))return 0;
        std::lock_guard<std::mutex> lock(mutex_);if(serial_==65535)return 0; // NEVER reuse within this ASI lifetime
        Entry* slot=nullptr;for(auto& e:entries_)if(!e.nonce||!mo::fresh(e.input.tick,now,3500)){slot=&e;break;}
        if(!slot)return 0;*slot={++serial_,i};slot->input.packetSerial54=serial&255u;slot->input.packetStroke54=stroke&3u;return slot->nonce;
    }
    bool read(unsigned nonce,std::uint64_t now,Input54& out) noexcept {
        out={};if(!nonce||nonce>65535)return false;std::lock_guard<std::mutex> lock(mutex_);
        for(const auto& e:entries_)if(e.nonce==nonce&&mo::fresh(e.input.tick,now,100)){out=e.input;return true;}return false;
    }
    // Separate held-input ancestry lease. This does not extend input admission
    // or the initial packet's 100 ms transport validity. The actual XI latch
    // retains THIS nonce through its release, even across a held wind-up.
    bool readHeld54(unsigned nonce,std::uint64_t now,Input54& out) noexcept {
        out={};if(!nonce||nonce>65535)return false;std::lock_guard<std::mutex> lock(mutex_);
        for(const auto& e:entries_)if(e.nonce==nonce&&mo::fresh(e.input.tick,now,3500)){out=e.input;return true;}return false;
    }
};
inline InputRegistry54 inputRegistry54;
inline thread_local const Input54* queuedInput54=nullptr;
struct InputScope54 {const Input54* old;explicit InputScope54(const Input54* i) noexcept:old(queuedInput54){queuedInput54=i;}~InputScope54(){queuedInput54=old;}};
inline thread_local const Input54* releasedInput54=nullptr;
struct ReleaseScope54 {const Input54* old;explicit ReleaseScope54(const Input54* i) noexcept:old(releasedInput54){releasedInput54=i;}~ReleaseScope54(){releasedInput54=old;}};
inline std::atomic<std::uintptr_t> actionContractBase54{0};
// An immutable physical Key may acquire an action only at the actual owned
// D4B06C->90C850 release dispatch. Timing/latest-intent is never authority here.
// Every allocation through the verified 91590C factory replaces its generation,
// including NPC allocations, so recycled addresses cannot inherit a binding.
struct Action54 {
    std::uintptr_t action=0,actor=0,state=0,helper=0,row=0;
    std::uint64_t generation=0,tick=0;mo::Key key{};
    bool constructed=false,bound=false,dispatchReturned=false,accepted=false,startEntered=false,startReturned=false,submitted=false,terminal=false;
    solver_command_h28::Stamp solverH28{};
};
class ActionRegistry54 {
    std::array<Action54,64> entries_{};std::uint64_t generation_=0;std::mutex mutex_;
    static bool sameBinding(const Action54& a,const Action54& b) noexcept {
        return a.action==b.action&&a.generation==b.generation&&a.actor==b.actor&&a.state==b.state&&
            a.helper==b.helper&&a.row==b.row&&mo::same(a.key,b.key)&&solver_rpg_h29::same(a.solverH28,b.solverH28);
    }
    static bool ready(Action54& a,mo::Key& key) noexcept {
        if(a.terminal||!a.dispatchReturned||!a.accepted||!a.startReturned||!a.submitted)return false;
        a.terminal=true;key=a.key;return true;
    }
public:
    void invalidate() noexcept {std::lock_guard<std::mutex> lock(mutex_);entries_={};} // generation never reused
    std::uint64_t construct(std::uintptr_t action,std::uintptr_t actor,bool valid,std::uint64_t now) noexcept {
        if(!action||!now)return 0;std::lock_guard<std::mutex> lock(mutex_);
        Action54* slot=nullptr;for(auto& a:entries_)if(a.action==action){slot=&a;break;}
        if(!slot)for(auto& a:entries_)if(!a.action||a.terminal||!mo::fresh(a.tick,now,3500)){slot=&a;break;}
        if(!slot)return 0; // pressure disables credit, never borrows a slot
        if(generation_==UINT64_MAX){*slot={};return 0;} // exhaustion cannot leave old-address ancestry alive
        *slot={};slot->action=action;slot->actor=actor;slot->generation=++generation_;slot->tick=now;slot->constructed=valid;return slot->generation;
    }
    bool bind(const Input54& input,std::uintptr_t action,std::uintptr_t helper,std::uintptr_t row,std::uint64_t now,Action54& out) noexcept {
        out={};if(!validInput54(input)||!mo::fresh(input.tick,now,3500)||!action||!helper||!row)return false;
        std::lock_guard<std::mutex> lock(mutex_);
        for(auto& a:entries_)if(a.action==action){
            if(!a.constructed||a.bound||a.terminal||a.actor!=input.actor||!mo::fresh(a.tick,now,100))return false;
            a.key=input.key;a.state=input.state;a.helper=helper;a.row=row;a.bound=true;a.tick=now;a.solverH28=input.solverH28;out=a;return true;
        }return false;
    }
    bool start(std::uintptr_t action,std::uint64_t now,Action54& out) noexcept {
        out={};std::lock_guard<std::mutex> lock(mutex_);
        for(auto& a:entries_)if(a.action==action){
            if(!a.constructed||!a.bound||a.terminal||a.startEntered||!mo::fresh(a.tick,now,1500)||
               (a.dispatchReturned&&!a.accepted))return false;
            a.startEntered=true;out=a;return true;
        }return false;
    }
    bool dispatchFinish(const Action54& binding,bool accepted,std::uint64_t now,mo::Key& credit) noexcept {
        credit={};std::lock_guard<std::mutex> lock(mutex_);
        for(auto& a:entries_)if(sameBinding(a,binding)){
            if(a.terminal||a.dispatchReturned||!mo::fresh(a.tick,now,1500))return false;
            a.dispatchReturned=true;a.accepted=accepted;if(!accepted){a.terminal=true;return false;}return ready(a,credit);
        }return false;
    }
    bool startFinish(const Action54& binding,bool exactSubmitted,std::uint64_t now,mo::Key& credit) noexcept {
        credit={};std::lock_guard<std::mutex> lock(mutex_);
        for(auto& a:entries_)if(sameBinding(a,binding)){
            if(a.terminal||a.startReturned||!mo::fresh(a.tick,now,1500))return false;
            a.startReturned=true;a.submitted=exactSubmitted;if(!exactSubmitted){a.terminal=true;return false;}return ready(a,credit);
        }return false;
    }
};
inline ActionRegistry54 actionRegistry54;
// Synchronous exact MainAttackStart ancestry. Both native preparation and
// strike wrappers must build their own effect AND return through submit before
// native start succeeds. The caller revalidates the same action/helper/row.
// This observes original native costs; it never supplies a second debit.
struct NativeCost54 {
    mo::Key key{};std::uintptr_t actor=0,state=0,action=0,helper=0,row=0;
    int type=-1,hand=-1;std::uint64_t wuid=0,nativeWeapon54=0;unsigned raised=0,struck=0;
    bool invalid=false,preparationOnly54=false,nativeFields54=false,inputProof54=false;
    Action54 causal54{};
    std::uint64_t observedTick54=0;
    bool valid() const noexcept {return mo::valid(key)&&actor&&state&&
        (preparationOnly54?(nativeFields54&&type>=-1&&type<=1&&hand>=0&&hand<=1):
            (action&&helper&&row&&(type==0||type==1)&&hand==0&&causal54.bound&&causal54.constructed&&
                causal54.action==action&&causal54.actor==actor&&causal54.state==state&&causal54.helper==helper&&causal54.row==row&&mo::same(causal54.key,key)));}
};
class PreparationProof54 {
    struct Proof {NativeCost54 cost{};std::uint64_t tick=0;bool used=false;};
    std::array<Proof,32> proofs_{};std::mutex mutex_;
public:
    bool publish(const NativeCost54& cost,std::uint64_t now) noexcept {
        if(!cost.valid()||!cost.preparationOnly54||cost.invalid||!cost.wuid||cost.raised!=1||cost.struck)return false;
        std::lock_guard<std::mutex> lock(mutex_);
        for(auto& p:proofs_)if(mo::same(p.cost.key,cost.key)&&mo::fresh(p.tick,now,3500)){p.used=true;return false;}
        for(auto& p:proofs_)if(!p.tick||!mo::fresh(p.tick,now,3500)){p={cost,now,false};return true;}return false;
    }
    bool consume(const NativeCost54& cost,std::uint64_t now) noexcept {
        std::lock_guard<std::mutex> lock(mutex_);
        for(auto& p:proofs_)if(mo::same(p.cost.key,cost.key)){
            if(p.used||!mo::fresh(p.tick,now,3500)||p.cost.actor!=cost.actor||p.cost.state!=cost.state||p.cost.wuid!=cost.wuid)return false;
            p.used=true;return true;
        }return false;
    }
};
inline PreparationProof54 preparationProof54;
inline thread_local NativeCost54* nativeCost54=nullptr;
struct NativeCostScope54 {
    NativeCost54 value;NativeCost54* old;bool finished54=false;
    explicit NativeCostScope54(NativeCost54 v) noexcept:value(v),old(nativeCost54){nativeCost54=value.valid()?&value:nullptr;
        if(nativeCost54)ledger.nativeObserved54(value.key,value.observedTick54?value.observedTick54:value.causal54.tick);}
    ~NativeCostScope54(){if(value.valid()&&!finished54){const auto tick=value.observedTick54?value.observedTick54:value.causal54.tick;
        if(value.preparationOnly54)ledger.preparationUncertainB3(value.key,tick);else ledger.nativeUncertain54(value.key,tick);}nativeCost54=old;}
    bool finish(bool sameStartedAction,std::uint64_t now) noexcept {
        if(finished54)return false;finished54=true;
        const bool exact=sameStartedAction&&value.valid()&&!value.invalid&&value.wuid;
        if(value.preparationOnly54){const bool proof=exact&&preparationProof54.publish(value,now);if(!proof)ledger.preparationUncertainB3(value.key,now);return proof;}
        bool submitted=exact&&value.struck==1&&value.raised<=1;
        if(submitted&&!value.raised){submitted=preparationProof54.consume(value,now);value.inputProof54=submitted;}
        if(!submitted)ledger.nativeUncertain54(value.key,now);
        mo::Key credit{};return actionRegistry54.startFinish(value.causal54,submitted,now,credit)&&ledger.nativeComplete54(credit,now);
    }
};
struct NativeWrapper54 {NativeCost54* owner=nullptr;unsigned kind=0,effects=0;std::uint64_t wuid=0;bool valid=false;};
inline thread_local NativeWrapper54* nativeWrapper54=nullptr;
inline bool nativeEffect54(std::uint64_t wuid,std::uintptr_t caller,unsigned char flag,float delta,bool built) noexcept {
    auto* w=nativeWrapper54;
    if(!w||!w->valid||!w->owner)return false;
    const bool expected=(w->kind==1&&caller==0xC5BBEB&&flag==1)||(w->kind==2&&caller==0xC64DCA&&flag==2);
    if(!expected)return false;
    if(!built||!wuid||wuid!=w->wuid||!std::isfinite(delta)||delta>0||++w->effects!=1){w->owner->invalid=true;return false;}
    return true;
}
// The native factory constructs an effect; it does not acknowledge application.
// Only the qualified enclosing wrapper/readback may finish the reserved debit.
struct OwnRaise {mo::Key key{};std::uint64_t wuid=0;float multiplier=1.f;bool effectEntered=false;};
inline thread_local OwnRaise* ownRaise=nullptr;
struct OwnRaiseScope {
    OwnRaise value;OwnRaise* old;
    OwnRaiseScope(const mo::Key& key,std::uint64_t wuid,float multiplier) noexcept:
        value{key,wuid,multiplier,false},old(ownRaise){ownRaise=&value;}
    ~OwnRaiseScope(){ownRaise=old;}
    OwnRaiseScope(const OwnRaiseScope&)=delete;OwnRaiseScope& operator=(const OwnRaiseScope&)=delete;
};
inline bool scaleOwnedEffect(std::uint64_t wuid,std::uintptr_t callerRva,unsigned char flag,float& delta) noexcept {
    auto* own=ownRaise;
    if(!own||callerRva!=0xC5BBEB||flag!=1||!wuid||wuid!=own->wuid||own->effectEntered||
       !std::isfinite(delta)||delta>0.f||!std::isfinite(own->multiplier)||own->multiplier<0.f||
       !ledger.reserved(own->key))return false;
    own->effectEntered=true;delta*=own->multiplier;return true;
}
}
