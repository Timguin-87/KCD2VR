#pragma once
#include "vrik/native_supply_t2.h"
#include "subsystems/throw_drop/hip_selection_t10.h"
#include "subsystems/throw_drop/ammo_carry_t18.h"
#include "subsystems/physics/runtime/native_weapon_body_t4.h"
#include "tracking_sample59c6.h"
#include <atomic>
#include <cmath>

// Deliberate shared-physics seam, implemented by the integrating root. Its bit
// must be independent of H36 equipment suspension and never changes F9 enabled.
namespace kcd2vr::native_weapon_body_t4 {
void SetThrowAdmissionSuspendedT23(bool) noexcept;
bool ThrowAdmissionSuspendedT23() noexcept;
}
namespace kcd2vr::throw_admission_t23 {
using Input=supply_t2::Input;
struct Pending {Input owner{};std::uint64_t serial=0;int side=-1,family=-1;};
struct Source {
    Input frame{};std::uint64_t tracking=0;
    bool complete=false,handKnown[2]{},handEmpty[2]{};
    std::uint64_t publication=0;
};
struct Grant {
    Pending pending{};Input frame{};std::uint64_t tracking=0,admission=0;
    bool allowed=false;
    std::uint64_t publication=0;
    Source source{}; // original immutable completed packet; its deadline never renews
};
// Current native identity/lifecycle facts only. Never carries a wrist, intent,
// source sample, or a replacement pose deadline.
struct Live {
    std::uintptr_t player=0,manager=0,character=0;
    std::uint64_t entity=0,body=0,request=0,tracking=0,observed=0;
    bool allowed=false,handKnown[2]{},handEmpty[2]{},tracked[2]{};
};
inline core::SnapshotMailbox<Pending> pending[2];
inline core::SnapshotMailbox<Grant> grants[2];
inline core::SnapshotMailbox<Source> completed;
inline core::SnapshotMailbox<Live> nativeLive;
inline std::atomic_flag liveWriter{};
inline std::atomic<std::uint64_t> liveAttempt{0},liveAccepted{0};
inline std::atomic_flag writers[2]{};
inline std::atomic<std::uint64_t> highwater[2]{};
inline std::atomic<std::uint64_t> pendingVersion[2]{};
inline const bool pendingInitialized=[]() noexcept {
    return pending[0].publish({})&&pending[1].publish({});
}();
enum class PendingState:unsigned char {Unknown,Absent,Present};

inline bool SameOwner(const Input& a,const Input& b) noexcept {
    return a.player&&a.manager&&a.character&&a.entity&&a.body&&a.request&&
        a.player==b.player&&a.manager==b.manager&&a.character==b.character&&a.entity==b.entity&&
        a.body==b.body&&a.request==b.request;
}
// A native Arrow is not an equipment-hand key. Only this same actor/body's
// retained claim excludes boxing; foreign or stale claims grant no intent.
inline bool OwnAmmoClaim(int side,const Input& frame) noexcept {
    ammo_carry_t18::Claim claim{};
    return SameOwner(frame,frame)&&ammo_carry_t18::read(side,claim)&&claim.serial&&
        claim.player==frame.player&&claim.manager==frame.manager&&claim.character==frame.character&&
        claim.body==frame.body&&claim.request==frame.request;
}
inline bool PublicationQualified(const Source& source) noexcept {
    return source.publication&&!(source.publication&1);
}
// Compare fields, never padding. A freshly stamped metadata shell around older
// wrists/intent is NOT the same completed source packet.
inline bool SameFrame(const Input& a,const Input& b) noexcept {
    if(!SameOwner(a,b)||a.sample!=b.sample||a.tick!=b.tick||a.grip!=b.grip||
       a.chestKnivesT26!=b.chestKnivesT26||a.rightHipPouchD66!=b.rightHipPouchD66||a.allowed!=b.allowed||a.tracked!=b.tracked||a.bone!=b.bone||a.beltKey!=b.beltKey||
       a.beltClass!=b.beltClass||a.beltShow!=b.beltShow)return false;
    for(int side=0;side<2;++side){
        if(a.hand[side]!=b.hand[side]||a.grip18[side]!=b.grip18[side]||a.trigger18[side]!=b.trigger18[side]||a.pose18[side]!=b.pose18[side]||
           a.poseTick18[side]!=b.poseTick18[side]||a.pouchNear[side]!=b.pouchNear[side])return false;
        for(int j=0;j<7;++j)if(a.wrist18[side][j]!=b.wrist18[side][j])return false;
        for(int j=0;j<3;++j)if(a.pos[side][j]!=b.pos[side][j]||a.offset[side][j]!=b.offset[side][j])return false;
        for(int k=0;k<2;++k){
            if(a.near18[side][k]!=b.near18[side][k])return false;
            for(int j=0;j<3;++j)if(a.offset18[side][k][j]!=b.offset18[side][k][j])return false;
        }
    }
    for(int j=0;j<3;++j)if(a.angles[j]!=b.angles[j]||a.beltPos[j]!=b.beltPos[j]||a.beltAngles[j]!=b.beltAngles[j])return false;
    return true;
}
inline bool SamePending(const Pending& a,const Pending& b) noexcept {
    return a.serial&&a.serial==b.serial&&a.side==b.side&&a.family==b.family&&SameFrame(a.owner,b.owner);
}
inline bool ValidateSource(const Source& source,const Input& frame,std::uint64_t now) noexcept {
    return frame.allowed&&frame.sample&&supply_t2::fresh(now,frame.tick,80)&&source.complete&&PublicationQualified(source)&&
        source.tracking&&source.tracking==tracking59c6::epoch.load(std::memory_order_acquire)&&SameFrame(source.frame,frame);
}
inline bool PublishLive(const Live& live) noexcept {
    const auto attempt=liveAttempt.fetch_add(1,std::memory_order_acq_rel)+1;
    // A failed/overlapping update is UNKNOWN, never permission from old facts.
    if(liveWriter.test_and_set(std::memory_order_acquire))return false;
    const bool done=nativeLive.publish(live);
    if(done&&attempt==liveAttempt.load(std::memory_order_acquire))liveAccepted.store(attempt,std::memory_order_release);
    liveWriter.clear(std::memory_order_release);return done;
}
inline bool LiveOwnerCurrent(const Input& owner,std::uint64_t now,Live* out=nullptr) noexcept {
    if(out)*out={};if(!SameOwner(owner,owner))return false;Live live{};
    const auto attempt=liveAttempt.load(std::memory_order_acquire);
    if(!attempt||liveAccepted.load(std::memory_order_acquire)!=attempt||!nativeLive.read(live)||
        attempt!=liveAttempt.load(std::memory_order_acquire)||liveAccepted.load(std::memory_order_acquire)!=attempt||
        !live.allowed||!supply_t2::fresh(now,live.observed,80)||
        !live.tracking||live.tracking!=tracking59c6::epoch.load(std::memory_order_acquire)||
        live.player!=owner.player||live.manager!=owner.manager||live.character!=owner.character||
        live.entity!=owner.entity||live.body!=owner.body||live.request!=owner.request)return false;
    if(out)*out=live;return true;
}
inline bool LiveCurrent(const Input& owner,int side,std::uint64_t now) noexcept {
    if(side<0||side>1)return false;Live live{};
    return LiveOwnerCurrent(owner,now,&live)&&
        live.tracking&&live.tracking==tracking59c6::epoch.load(std::memory_order_acquire)&&
        live.handKnown[side]&&live.handEmpty[side]&&live.tracked[side];
}
// D72: the script service consumes one COMPLETE source, not a frame from a
// second mailbox that may already lag the completed publisher. Keep that exact
// immutable packet across its synchronous native calls, just like a captured
// Grant. New live ownership/hand/tracking evidence is still checked at each
// operation and its original 80-ms deadline is never renewed.
inline thread_local const Source* callbackSourceD72=nullptr;
struct SourceScopeD72 {
    const Source* previous=nullptr;
    explicit SourceScopeD72(const Source& source) noexcept : previous(callbackSourceD72) {callbackSourceD72=&source;}
    ~SourceScopeD72(){callbackSourceD72=previous;}
};
inline bool CaptureSourceD72(Source& source,std::uint64_t now) noexcept {
    source={};Source candidate{};
    if(!completed.read(candidate)||!ValidateSource(candidate,candidate.frame,now)||!LiveOwnerCurrent(candidate.frame,now))return false;
    source=candidate;return true;
}
inline bool CurrentSource(const Input& frame,std::uint64_t now,Source* out=nullptr) noexcept {
    if(out)*out={};Source source{};
    if(callbackSourceD72&&SameFrame(callbackSourceD72->frame,frame))source=*callbackSourceD72;
    else if(!completed.read(source))return false;
    if(!ValidateSource(source,frame,now)||!LiveOwnerCurrent(frame,now))return false;
    if(out)*out=source;return true;
}
inline PendingState ObservePending(int side,Pending& out) noexcept {
    out={};if(side<0||side>1||!pendingInitialized)return PendingState::Unknown;
    const auto version=pendingVersion[side].load(std::memory_order_acquire);
    if((version&1)||!pending[side].read(out)||version!=pendingVersion[side].load(std::memory_order_acquire))return PendingState::Unknown;
    if(!out.serial)return PendingState::Absent;
    return out.side==side?PendingState::Present:PendingState::Unknown;
}
inline bool ReadPending(int side,Pending& out) noexcept {
    return ObservePending(side,out)==PendingState::Present;
}
struct Writer {
    int side=-1;explicit Writer(int s) noexcept {
        if(s>=0&&s<2&&pendingInitialized&&!writers[s].test_and_set(std::memory_order_acquire)){
            side=s;pendingVersion[s].fetch_add(1,std::memory_order_acq_rel);
        }
    }
    ~Writer(){if(side>=0){pendingVersion[side].fetch_add(1,std::memory_order_release);writers[side].clear(std::memory_order_release);}}
    explicit operator bool() const noexcept{return side>=0;}
};
inline bool PublishPending(const Pending& request,std::uint64_t now=0) noexcept {
    if(request.side<0||request.side>1||request.family<0||request.family>1||!request.serial||
       !request.owner.sample||!request.owner.tick||!SameOwner(request.owner,request.owner))return false;
    Writer guard(request.side);if(!guard)return false;
    Pending old{};if(!pending[request.side].read(old))return false;
    if(old.serial)return SamePending(old,request);
    if(request.serial<=highwater[request.side].load(std::memory_order_acquire))return false;
    Source source{};
    if(now&&callbackSourceD72&&SameFrame(callbackSourceD72->frame,request.owner)){
        source=*callbackSourceD72;
        if(!ValidateSource(source,request.owner,now)||!LiveCurrent(request.owner,request.side,now))return false;
    }else if(!completed.read(source))return false;
    if(!source.complete||!PublicationQualified(source)||!source.handKnown[request.side]||!source.handEmpty[request.side]||
       !source.tracking||source.tracking!=tracking59c6::epoch.load(std::memory_order_acquire)||
       !SameFrame(source.frame,request.owner)||!request.owner.pose18[request.side]||
       request.owner.poseTick18[request.side]!=request.owner.tick||request.owner.hand[request.side]||
       (request.side==0&&!request.owner.pouchNear[request.family]))return false;
    if(!pending[request.side].publish(request))return false;
    highwater[request.side].store(request.serial,std::memory_order_release);return true;
}
inline bool ClearPending(const Pending& request) noexcept {
    if(request.side<0||request.side>1||!request.serial)return false;
    Writer guard(request.side);if(!guard)return false;
    Pending current{};if(!pending[request.side].read(current))return false;
    if(!current.serial)return true;
    return SamePending(current,request)&&pending[request.side].publish({});
}
inline bool PublishSource(const Source& source) noexcept {return completed.publish(source);}
inline bool PublishGrant(int side,const Grant& grant) noexcept {
    return side>=0&&side<2&&grants[side].publish(grant);
}
// Validate a callback-local capture. A newer unrelated pose/publication alone
// cannot revoke this <=80ms source or replace any wrist/intent/deadline.
inline bool ValidateGrant(const Pending& request,const Input& frame,std::uint64_t now,const Grant& grant) noexcept {
    if(request.side<0||request.side>1)return false;
    namespace physics=native_weapon_body_t4;
    const auto before=physics::AdmissionSerial();
    if(request.side==0&&(!physics::ThrowAdmissionSuspendedT23()||physics::HasUnretiredCollider()))return false;
    Pending current{};const auto& source=grant.source;
    if(!ReadPending(request.side,current)||!SamePending(current,request)||!grant.allowed||
       !SamePending(grant.pending,request)||!SameOwner(request.owner,frame)||!SameFrame(grant.frame,frame)||
       !ValidateSource(source,frame,now)||!LiveCurrent(frame,request.side,now)||grant.tracking!=source.tracking||grant.publication!=source.publication||
       !source.handKnown[request.side]||!source.handEmpty[request.side]||frame.hand[request.side]||
       !frame.pose18[request.side]||frame.poseTick18[request.side]!=frame.tick)return false;
    if(request.side==0&&(before!=grant.admission||before!=physics::AdmissionSerial()||
       physics::HasUnretiredCollider()||!physics::ThrowAdmissionSuspendedT23()))return false;
    if(!ReadPending(request.side,current)||!SamePending(current,request)||
       grant.tracking!=tracking59c6::epoch.load(std::memory_order_acquire)||!LiveCurrent(frame,request.side,now))return false;
    return true;
}
inline bool CurrentGrant(const Pending& request,const Input& frame,std::uint64_t now,Grant* out=nullptr) noexcept {
    if(out)*out={};if(request.side<0||request.side>1)return false;
    Grant grant{};if(!grants[request.side].read(grant)||!ValidateGrant(request,frame,now,grant))return false;
    if(out)*out=grant;return true;
}
// Shared right-hip arbitration BEFORE final solver/holster suppression. The
// caller supplies the SAME native equipment candidate/distances/known-empty
// proof as normal holsters. No generic radius bypass or inventory creation.
inline hip_t10::Grant Reach(hip_t10::State& state,const hip_t10::Input& input,
                           bool nativeKnown,bool nativeEmpty,bool boxing) noexcept {
    auto qualified=input;qualified.allowed=qualified.allowed&&nativeKnown&&nativeEmpty&&!boxing;
    return hip_t10::update(state,qualified);
}
}
