#pragma once
#define KCD2VR_KICK34_H 1
// C37C1: one-click foot CONTACT OBSERVATION ledger. No native application is enabled.
// The current game-thread drain records the claimed contact and refuses it with AdmissionUnproven.
// Cost/effort helpers retained below are donor research tested in isolation, not runtime native kick admission.
// Native Game / VR leg with game rules routes remain separate and unchanged.
#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <mutex>
namespace kcd2vr::kick34 {
inline constexpr std::uint64_t kExpiryMs=1300;      // R3 click -> claim. C52: the native-timing leg holds the foot out until 830 ms (+100 ms request wait, +160 ms contact age); the old leg was done at 520.
inline constexpr std::uint64_t kContactAgeMs=160;   // contact -> claim, the same rendezvous bound as a blade contact
inline constexpr std::uint64_t kRepeatTargetMs=1500;// D53610 per-target guard, wh_cs_RepeatHitTimeout retail default 1.5 s
inline constexpr int kAttackType=4;                 // combat_attack_type row 4 'kick' (cannot_kill, damage type 4)
enum class Stage : unsigned {Empty,Open,Contact,Claimed,Applied,Refused,Expired};
enum class Why : unsigned {None,NoTransaction,SerialMismatch,Expired,EpochChanged,Duplicate,NotExtend,NotBody,ContactStale,
    NotHenry,HenryStateChanged,Cancelled,Superseded,
    // game-thread outcomes (Drain); reported through finish()
    AdmissionUnproven,TargetUnknown,NotInCombat,NotOpponent,HenryBusy,RepeatTarget,TransportRefused,CostUnavailable,NoStamina,
    EffortNotCalled,EffortIndeterminate,SubmitFault,NativeRefused,Applied};
inline const char* name(Why w) noexcept {
    switch(w){
    case Why::None:return "NONE";case Why::NoTransaction:return "NO_TRANSACTION";case Why::SerialMismatch:return "OTHER_CLICK";
    case Why::Expired:return "EXPIRED_CLICK_WINDOW";case Why::EpochChanged:return "POSE_EPOCH_CHANGED";case Why::Duplicate:return "DUPLICATE_CONTACT";
    case Why::NotExtend:return "NOT_FOOT_EXTEND";case Why::NotBody:return "NOT_A_BODY";case Why::ContactStale:return "CONTACT_OLDER_THAN_160MS";
    case Why::NotHenry:return "NOT_THE_HENRY_OF_THE_CLICK";case Why::HenryStateChanged:return "HENRY_COMBAT_STATE_REPLACED";
    case Why::Cancelled:return "CANCELLED";case Why::Superseded:return "SUPERSEDED_BY_NEW_CLICK";
    case Why::AdmissionUnproven:return "NATIVE_KICK_ADMISSION_UNPROVEN";
    case Why::TargetUnknown:return "TARGET_UNKNOWN";case Why::NotInCombat:return "HENRY_NOT_IN_COMBAT";case Why::NotOpponent:return "NOT_HENRYS_OPPONENT";
    case Why::HenryBusy:return "HENRY_ACTION_BUSY";case Why::RepeatTarget:return "REPEAT_TARGET_1500MS";case Why::TransportRefused:return "TRANSPORT_REFUSED";
    case Why::CostUnavailable:return "NATIVE_COST_UNAVAILABLE";case Why::NoStamina:return "NATIVE_CAN_ATTACK_FALSE";
    case Why::EffortNotCalled:return "EFFORT_NOT_CALLED";case Why::EffortIndeterminate:return "EFFORT_INDETERMINATE";
    case Why::SubmitFault:return "SUBMIT_FAULT";case Why::NativeRefused:return "NATIVE_RETURNED_FALSE";case Why::Applied:return "APPLIED";
    default:return "?";}
}
inline bool fresh(std::uint64_t t,std::uint64_t now,std::uint64_t cap) noexcept {return t&&now>=t&&now-t<=cap;}
// Bound at the R3 click (XR thread): exact Henry (C_CombatPlayer actor + its combat state + C_Player) and pose epoch.
struct Binding {std::uint64_t serial=0,tick=0,poseEpoch=0;std::uintptr_t actor=0,state=0,player=0;};
// Offered by the pose callback: the kicking foot's ray contact, copied (entity/part/body/material/collider).
struct Contact {std::uint64_t serial=0,tick=0,poseEpoch=0;std::uint32_t entity=0;int part=-1;unsigned body=0;int bodyMaterial=-1;
    std::uintptr_t collider=0;float pos[3]{};bool extend=false,living=false;};
struct Claim {bool ready=false;Why why=Why::None;Binding binding{};Contact contact{};};
struct Note {std::uint64_t serial=0,sequence=0;Why why=Why::None;Stage stage=Stage::Empty;};
struct Counters {std::uint64_t opened=0,contacts=0,duplicates=0,claimed=0,applied=0,refused=0,expired=0;};
class Ledger {
    mutable std::mutex m_;Binding b_{};Contact c_{};Stage stage_=Stage::Empty;std::uint64_t last_=0;Counters n_{};Note note_{};
    std::atomic<std::uint64_t> open_{0},offered_{0},pending_{0},openTick_{0},noteSeq_{0};
    void noteLocked(Why why) noexcept {note_={b_.serial,note_.sequence+1,why,stage_};noteSeq_.store(note_.sequence,std::memory_order_release);}
    void closeLocked(Stage s,Why why) noexcept {
        stage_=s;open_.store(0,std::memory_order_release);openTick_.store(0,std::memory_order_release);pending_.store(0,std::memory_order_release);
        if(s==Stage::Expired)++n_.expired;else if(s==Stage::Refused)++n_.refused;else if(s==Stage::Applied)++n_.applied;
        noteLocked(why);}
public:
    bool pending() const noexcept {return pending_.load(std::memory_order_acquire)!=0;} // C37 review: idle check for the per-update drain
    void clear() noexcept {std::lock_guard<std::mutex> l(m_);b_={};c_={};stage_=Stage::Empty;last_=0;n_={};note_={};
        open_=0;offered_=0;pending_=0;openTick_=0;noteSeq_=0;}
    // XR thread, once per R3 click. A newer click retires an older transaction that never applied.
    bool open(const Binding& n) noexcept {
        if(!n.serial||!n.tick||!n.poseEpoch||!n.actor||!n.state||!n.player)return false;
        std::lock_guard<std::mutex> l(m_);
        if(n.serial<=last_)return false; // serials are monotonic; a replayed/old click never reopens
        if(stage_==Stage::Open||stage_==Stage::Contact)closeLocked(Stage::Refused,Why::Superseded);
        b_=n;c_={};stage_=Stage::Open;last_=n.serial;++n_.opened;
        offered_.store(0,std::memory_order_release);pending_.store(0,std::memory_order_release);
        openTick_.store(n.tick,std::memory_order_release);open_.store(n.serial,std::memory_order_release);return true;
    }
    // Pose callback fast path: an open transaction for this kick that has not taken a contact yet.
    bool wants(std::uint64_t serial) const noexcept {return serial&&open_.load(std::memory_order_acquire)==serial&&offered_.load(std::memory_order_acquire)!=serial;}
    Why offer(const Contact& k,std::uint64_t now) noexcept {
        if(!k.serial)return Why::NoTransaction;
        if(offered_.load(std::memory_order_acquire)==k.serial){std::lock_guard<std::mutex> l(m_);++n_.duplicates;return Why::Duplicate;}
        std::lock_guard<std::mutex> l(m_);
        if(stage_==Stage::Empty)return Why::NoTransaction;
        if(k.serial!=b_.serial)return Why::SerialMismatch;
        if(stage_!=Stage::Open){++n_.duplicates;return Why::Duplicate;}
        if(!fresh(b_.tick,now,kExpiryMs)||k.tick<b_.tick){closeLocked(Stage::Expired,Why::Expired);return Why::Expired;}
        if(k.poseEpoch!=b_.poseEpoch){closeLocked(Stage::Refused,Why::EpochChanged);return Why::EpochChanged;}
        // Not consumed: the knee coming up or a wall touch is not the kick; the foot may still land in this click.
        if(!k.entity||!k.living)return Why::NotBody;
        if(!k.extend)return Why::NotExtend;
        c_=k;stage_=Stage::Contact;++n_.contacts;offered_.store(k.serial,std::memory_order_release);
        pending_.store(k.serial,std::memory_order_release);return Why::None;
    }
    // Game thread, Henry's own update. Exactly once per transaction; any identity change refuses it for good.
    Claim claim(std::uintptr_t actor,std::uintptr_t state,std::uint64_t now) noexcept {
        Claim r{};if(!pending_.load(std::memory_order_acquire))return r;
        std::lock_guard<std::mutex> l(m_);pending_.store(0,std::memory_order_release);
        if(stage_!=Stage::Contact)return r;
        r.binding=b_;r.contact=c_;
        if(!fresh(b_.tick,now,kExpiryMs)){r.why=Why::Expired;closeLocked(Stage::Expired,r.why);return r;}
        if(!fresh(c_.tick,now,kContactAgeMs)){r.why=Why::ContactStale;closeLocked(Stage::Refused,r.why);return r;}
        if(!actor||actor!=b_.actor){r.why=Why::NotHenry;closeLocked(Stage::Refused,r.why);return r;}
        if(!state||state!=b_.state){r.why=Why::HenryStateChanged;closeLocked(Stage::Refused,r.why);return r;}
        stage_=Stage::Claimed;++n_.claimed;open_.store(0,std::memory_order_release);openTick_.store(0,std::memory_order_release);r.ready=true;return r;
    }
    // Only the claimer finishes; Applied once, everything else is a refusal of this click.
    bool finish(std::uint64_t serial,Why why) noexcept {
        std::lock_guard<std::mutex> l(m_);
        if(stage_!=Stage::Claimed||b_.serial!=serial)return false;
        closeLocked(why==Why::Applied?Stage::Applied:Stage::Refused,why);return true;
    }
    // Context loss / F4 exit / leg cancel / recenter: an open transaction can no longer apply.
    void retire(Why why) noexcept {
        if(!open_.load(std::memory_order_acquire)&&!pending_.load(std::memory_order_acquire))return;
        std::lock_guard<std::mutex> l(m_);
        if(stage_==Stage::Open||stage_==Stage::Contact)closeLocked(why==Why::Expired?Stage::Expired:Stage::Refused,why);
    }
    // Cheap edge: an open transaction that saw no contact closes as EXPIRED once (two atomic loads when idle).
    void sweep(std::uint64_t now) noexcept {
        const auto t=openTick_.load(std::memory_order_acquire);if(!t||(now>=t&&now-t<=kExpiryMs))return;
        std::lock_guard<std::mutex> l(m_);
        if((stage_==Stage::Open||stage_==Stage::Contact)&&!fresh(b_.tick,now,kExpiryMs))closeLocked(Stage::Expired,Why::Expired);
    }
    // One reader per edge: returns true once per new note (outcome of a transaction).
    bool takeNote(std::uint64_t& seen,Note& out) const noexcept {
        if(noteSeq_.load(std::memory_order_acquire)==seen)return false;
        std::lock_guard<std::mutex> l(m_);out=note_;seen=note_.sequence;return true;
    }
    Stage stage() const noexcept {std::lock_guard<std::mutex> l(m_);return stage_;}
    Binding binding() const noexcept {std::lock_guard<std::mutex> l(m_);return b_;}
    Counters counters() const noexcept {std::lock_guard<std::mutex> l(m_);return n_;}
};
inline Ledger ledger;

// ---- native effort rules (WHGame 6A350E20), mirrored for the receipt and host tests ----
// C_RPGCombat slot 1 (686F40) CanAttack: cost==0 || cur/cost > AttackRequiredStamRatio (rpg constant +0x0C, 0.0).
inline bool CanAttack(float cur,float cost,float requiredRatio=0.f) noexcept {
    return std::isfinite(cur)&&std::isfinite(cost)&&(cost==0.f||cur/cost>requiredRatio);}
// 4C7484 AttackStamMod: cur>=cost ? 1 : AttackStamModMin + (1-AttackStamModMin)*sqrt(cur/cost) (constant +0x08, 0.5).
// This is the AttackStrength C64CE4 publishes for a native strike; D53610 passes it to BuildAndSubmitMeleeHit.
inline float AttackStamMod(float cur,float cost,float minMod=.5f) noexcept {
    if(!std::isfinite(cur)||!std::isfinite(cost)||cost<=0.f||cur>=cost)return 1.f;
    return minMod+(1.f-minMod)*std::sqrt(std::max(cur,0.f)/cost);}
// Our own synchronous raise pair C5BA24(rpg,true/false) builds ONE native ActStamina effect (call site C5BBEB, flag 1).
// Inside this scope that one effect's delta is set to exactly -cost, where cost is the native GetAttackStaminaCost
// (C_RPGCombat slot 0, 687130) for attack type 4 and Henry's own soul/hand WUID. The game builds and applies the effect.
struct KickRaise {std::uint64_t serial=0,wuid=0;float cost=0.f,nativeDelta=0.f;bool entered=false;};
inline thread_local KickRaise* kickRaise=nullptr;
struct KickRaiseScope {
    KickRaise value;KickRaise* old;
    KickRaiseScope(std::uint64_t serial,std::uint64_t wuid,float cost) noexcept:value{serial,wuid,cost,0.f,false},old(kickRaise){kickRaise=&value;}
    ~KickRaiseScope(){kickRaise=old;}
    KickRaiseScope(const KickRaiseScope&)=delete;KickRaiseScope& operator=(const KickRaiseScope&)=delete;
};
inline bool scaleKickEffect(std::uint64_t wuid,std::uintptr_t callerRva,unsigned char flag,float& delta) noexcept {
    auto* k=kickRaise;
    if(!k||callerRva!=0xC5BBEB||flag!=1||!wuid||wuid!=k->wuid||k->entered||!std::isfinite(delta)||delta>0.f||
       !std::isfinite(k->cost)||k->cost<=0.f||k->cost>1000.f)return false;
    k->entered=true;k->nativeDelta=delta;delta=-k->cost;return true;
}
// D53610 keeps a per-target 1.5 s repeat map for collision hits; a direct BuildAndSubmitMeleeHit bypasses it,
// so the kick keeps the same rule itself (game thread only).
struct RepeatGuard {
    std::array<std::uint32_t,8> target{};std::array<std::uint64_t,8> tick{};
    bool blocked(std::uint32_t id,std::uint64_t now) const noexcept {
        for(std::size_t i=0;i<target.size();++i){if(target[i]==id&&fresh(tick[i],now,kRepeatTargetMs-1))return true;}return false;}
    void note(std::uint32_t id,std::uint64_t now) noexcept {
        std::size_t slot=0;for(std::size_t i=0;i<target.size();++i){if(target[i]==id){slot=i;break;}if(tick[i]<tick[slot])slot=i;}
        target[slot]=id;tick[slot]=now;}
};
// Body location of the kick: the foot ray's own idmatOrg (ray_hit +0x1A) when present; otherwise the native
// approximation for attack zone 5 ('lower', combat_zone approx_orig_mat_id 6) - labelled ZONE5_APPROX in receipts.
inline int LocationMaterial(int rayMaterial,bool& fromRay) noexcept {fromRay=rayMaterial>=1&&rayMaterial<=38;return fromRay?rayMaterial:6;}
}
