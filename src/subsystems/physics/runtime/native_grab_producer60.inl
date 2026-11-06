// Actual producer: native leaves alone are injected; this lifecycle is shared
// verbatim by the engine adapter and the connected host tests.
#include "core/snapshot_mailbox.h"
#include "core/collab_test_h21.h"
#include "core/mutual_admission_t25.h"
#include "vrik/static_grab_r89.h"
#include <atomic>
#include <cmath>
namespace kcd2vr::physics::grab_producer60 {
namespace cs=constraint60;
struct Envelope {Intent intent{};std::uint64_t cancellation=0;};
struct Session {
    Intent intent{},completed{};Owner owner{},retiredOwner{};cs::Service machine{};
    State state=State::Idle;bool owns=false,addPending=false,submitted=false,removeSubmitted=false;
    std::uint64_t addSerial=0,removeSerial=0,cancellation=0;
    char reason[80]{};
};
static core::SnapshotMailbox<Envelope> g_input[2];
static core::SnapshotMailbox<Status> g_output[2];
static std::atomic<std::uint64_t> g_cancel[2]{};
static std::atomic<bool> g_enabled{true},g_fixturePassed{false},g_backendReady{false};
static Backend g_backend{};static Session g_session[2]{};
static std::atomic<SourcePredicateD41> g_sourcePredicateD41{nullptr};
bool SetSourcePredicateD41(SourcePredicateD41 predicate) noexcept {
    if(!predicate)return false;
    SourcePredicateD41 expected=nullptr;
    return g_sourcePredicateD41.compare_exchange_strong(expected,predicate,std::memory_order_acq_rel)||expected==predicate;
}
bool SourceCurrentD41(const Intent& in) noexcept {
    const auto predicate=g_sourcePredicateD41.load(std::memory_order_acquire);
    // The normal small-prop motor/native pickup has one hand/entity owner.
    // A queued F9 constraint must retire instead of writing that same body.
    return in.side>=0&&in.side<2&&!static_grab_r89::ownsEntityR94(in.entity)&&
        !mutual_admission_t25::pickupClaimsD72[in.side].load(std::memory_order_acquire)&&predicate&&predicate(in);
}
// Only T4 reads/writes Session. Game/script callbacks exchange immutable POD
// requests/receipts; they never lock or borrow the native physics owner.
struct TransferRequest {Owner owner{};std::uint64_t serial=0;};
struct TransferReceipt {TransferRequest request{};TransferProgress progress=TransferProgress::Waiting;};
struct TransferLocal {TransferRequest request{};bool accepted=false;};
static core::SnapshotMailbox<TransferRequest> g_transferInput[2];
static core::SnapshotMailbox<TransferReceipt> g_transferOutput[2];
static std::atomic<std::uint64_t> g_transferTicket[2]{};
static TransferLocal g_transferLocal[2]{};
static void Reason(Session& s,const char* why) noexcept {unsigned i=0;if(why)for(;why[i]&&i<79;++i)s.reason[i]=why[i];s.reason[i]=0;}
static bool SamePress(const Intent& a,const Intent& b) noexcept {
    return a.press&&a.press==b.press&&a.player==b.player&&a.poseEpoch==b.poseEpoch&&a.trackingEpoch==b.trackingEpoch&&
        a.sourceGeneration==b.sourceGeneration&&a.entity==b.entity&&a.physicsId==b.physicsId&&a.expectedBody==b.expectedBody&&a.side==b.side&&a.profileEpoch==b.profileEpoch;
}
static bool SameSqueezeD41(const Intent& a,const Intent& b) noexcept {
    // Epoch/source replacement retires ownership; it is not an open/regrip.
    return a.press&&a.press==b.press&&a.player==b.player&&a.side==b.side;
}
static bool Fresh(const Intent& i,std::uint64_t now) noexcept {
    if(!SourceCurrentD41(i)||!collab_h21::current(i.profileEpoch)||!i.held||!i.allowed||i.nativeAttached||!i.player||!i.press||!i.entity||i.side<0||i.side>1||i.physicsId<0||!i.expectedBody||
       !i.poseEpoch||!i.trackingEpoch||!i.sourceGeneration||!i.tick||!i.poseTick||now<i.tick||now<i.poseTick||now-i.tick>80||now-i.poseTick>80)return false;
    double n=0;for(float x:i.q){if(!std::isfinite(x))return false;n+=double(x)*x;}
    for(float x:i.p)if(!std::isfinite(x))return false;
    return n>=.98&&n<=1.02;
}
static bool SameOwner(const Owner& a,const Owner& b) noexcept {
    return a.requestToken&&a.requestToken==b.requestToken&&a.player==b.player&&a.world==b.world&&a.worldGeneration==b.worldGeneration&&
        a.sourceGeneration==b.sourceGeneration&&a.proxyGeneration==b.proxyGeneration&&a.poseEpoch==b.poseEpoch&&
        a.trackingEpoch==b.trackingEpoch&&a.press==b.press&&a.entity==b.entity&&a.side==b.side;
}
static bool Qualified(const Owner& o,const Intent& i,const Stamp& t,std::uint64_t token) noexcept {
    return o.requestToken==token&&token&&o.player==i.player&&o.entity==i.entity&&o.side==i.side&&o.press==i.press&&
        o.poseEpoch==i.poseEpoch&&o.trackingEpoch==i.trackingEpoch&&o.sourceGeneration==i.sourceGeneration&&o.proxyGeneration&&
        o.world&&o.world==t.world&&o.worldGeneration&&o.worldGeneration==t.generation;
}
static void Quarantine(Session& s,const char* why) noexcept {s.state=State::Quarantined;Reason(s,why);}
static void Finish(Session& s,const char* why) noexcept {
    const auto completed=s.intent;const auto retired=s.owner;s=Session{};s.completed=completed;s.retiredOwner=retired;Reason(s,why);
}
static void Retire(const Backend& b,void* c,Session& s) noexcept {
    s.state=State::Retiring;
    const auto result=b.retire(c,s.owner);
    if(result==Progress::Complete)Finish(s,"RETIRED_CONFIRMED");
    else if(result==Progress::Uncertain||result==Progress::Refused)Quarantine(s,"RESOURCE_RETIRE_UNCONFIRMED");
}
static void StepSession(const Backend& b,void* c,const Stamp& stamp,Session& s,const Intent& in,bool enabled,bool cancelled,std::uint64_t now) noexcept {
    const bool wanted=enabled&&!cancelled&&Fresh(in,now);
    if(s.state==State::Idle){
        if(!wanted||SameSqueezeD41(in,s.completed))return;
        const auto token=cs::mintAcquireToken60();if(!token){Reason(s,"REQUEST_TOKEN_UNAVAILABLE");return;}
        s.intent=in;s.owner={};s.owner.requestToken=token;s.owner.player=in.player;s.owner.side=in.side;s.owner.entity=in.entity;
        s.owner.press=in.press;s.owner.poseEpoch=in.poseEpoch;s.owner.trackingEpoch=in.trackingEpoch;s.owner.sourceGeneration=in.sourceGeneration;
        s.owns=true;s.state=State::Preparing;Reason(s,"PREPARING_NATIVE_RESOURCES");
    }
    const bool keep=wanted&&SamePress(s.intent,in);
    if(s.state==State::Preparing){
        if(!keep){Retire(b,c,s);return;}
        Owner prepared=s.owner;const auto r=b.prepare(c,in,s.owner.requestToken,prepared);
        if(r==Progress::Uncertain){Quarantine(s,"PREPARE_UNCERTAIN_RESOURCES_RETAINED");return;}
        if(r==Progress::Refused){Reason(s,"PREPARE_REFUSED");Retire(b,c,s);return;}
        if(r!=Progress::Complete)return;
        if(!Qualified(prepared,s.intent,stamp,s.owner.requestToken)){Quarantine(s,"PREPARE_IDENTITY_MISMATCH");return;}
        s.owner=prepared;
        if(!s.machine.acquire(s.owner.proxyGeneration,s.owner.world,now)){Retire(b,c,s);return;}
        s.state=State::WaitingPresent;Reason(s,"PREPARED_WAITING_CONSTRAINT");
    }
    if(s.state==State::Retiring){Retire(b,c,s);return;}
    if(!s.owns)return;
    if(s.owner.world&&(s.owner.world!=stamp.world||s.owner.worldGeneration!=stamp.generation)){
        Quarantine(s,"WORLD_REPLACED_RESOURCES_RETAINED");return;
    }
    if(s.state==State::Quarantined){
        // No automatic erase: only a freshly joined Absent may resolve a submitted resource.
        if(!s.submitted){Retire(b,c,s);return;}
        const auto x=b.read(c,s.owner,s.machine.acquireToken,s.machine.id);
        const bool joined=SameOwner(x.owner,s.owner)&&x.constraintToken==s.machine.acquireToken&&x.serial==stamp.serial&&x.serial>s.addSerial;
        if(joined&&!s.machine.id&&x.id){
            cs::Snap late{};late.world=s.owner.world;late.acceptedIdValid=true;late.acceptedId=x.id;late.acceptedToken=x.constraintToken;
            (void)s.machine.release(now);(void)s.machine.step(late,now);
            s.state=State::Releasing;Reason(s,"LATE_ADD_ID_DRAINING");return;
        }
        if(joined&&s.machine.id&&x.id==s.machine.id){
            if(x.kind==cs::Status::Kind::Absent&&(!s.removeSubmitted||x.serial>s.removeSerial)){Retire(b,c,s);return;}
            if(x.kind==cs::Status::Kind::Present&&!s.removeSubmitted){
                s.removeSubmitted=true;s.removeSerial=stamp.serial; // an uncertain entry is never submitted twice
                const auto result=b.remove(c,s.owner,s.machine.acquireToken,s.machine.id);
                if(result.progress==Progress::Refused)s.removeSubmitted=false; // backend proves no native entry
                if(result.progress==Progress::Complete&&SameOwner(result.owner,s.owner)&&result.constraintToken==s.machine.acquireToken&&
                   result.id==s.machine.id&&result.serial==stamp.serial){s.removeSubmitted=true;s.removeSerial=result.serial;Reason(s,"QUARANTINE_REMOVE_WAITING_ABSENT");}
            }
        }
        return;
    }
    if(!keep){
        if(!s.submitted){s.addPending=false;Retire(b,c,s);return;}
        (void)s.machine.release(now);s.state=State::Releasing;Reason(s,"RELEASE_REQUESTED_WAITING_ABSENT");
    }
    cs::Snap snap{};snap.world=s.owner.world;Sample sample{};
    if(s.machine.id){
        sample=b.read(c,s.owner,s.machine.acquireToken,s.machine.id);
        const bool joined=SameOwner(sample.owner,s.owner)&&sample.constraintToken==s.machine.acquireToken&&sample.id==s.machine.id&&
            sample.serial==stamp.serial&&sample.serial>s.addSerial;
        if(joined){
            snap.status={sample.kind,s.machine.id,s.owner.world};
            if(s.machine.stage==cs::Stage::Removing&&sample.kind==cs::Status::Kind::Absent&&
               (!s.removeSubmitted||sample.serial<=s.removeSerial))snap.status.kind=cs::Status::Kind::Unknown;
        }else sample={};
    }
    if(s.state==State::Held&&(snap.status.kind!=cs::Status::Kind::Present||!sample.following)){
        s.state=State::WaitingPresent;Reason(s,"HELD_PROOF_UNAVAILABLE_RESOURCES_RETAINED");
    }
    const auto next=s.machine.step(snap,now);
    if(s.machine.stage==cs::Stage::Removed){Retire(b,c,s);return;}
    if(s.machine.stage==cs::Stage::Fault){if(!s.submitted)Retire(b,c,s);else Quarantine(s,next.reason);return;}
    if(s.machine.stage==cs::Stage::Quarantined){Quarantine(s,next.reason);return;}
    if(next.action==cs::Action::Create)s.addPending=true;
    if(s.addPending){
        // A native prepare/read callback can retire gameplay ownership after
        // the first frame check. Never add a new constraint from that old input.
        if(!Fresh(in,now)||s.cancellation!=g_cancel[in.side].load(std::memory_order_acquire)){Cancel(in.side);s.addPending=false;Retire(b,c,s);return;}
        const auto result=b.add(c,s.owner,s.machine.acquireToken);
        if(result.progress==Progress::Uncertain){s.submitted=true;s.addPending=false;s.addSerial=stamp.serial;Quarantine(s,"ADD_UNCERTAIN_RESOURCES_RETAINED");return;}
        if(result.progress==Progress::Refused){s.addPending=false;Retire(b,c,s);return;}
        if(result.progress==Progress::Complete){
            // A malformed successful reply may already own a native constraint: never retire it blindly.
            s.submitted=true;s.addPending=false;
            if(!SameOwner(result.owner,s.owner)||result.constraintToken!=s.machine.acquireToken||!result.id||result.serial!=stamp.serial){Quarantine(s,"ADD_REPLY_IDENTITY_MISMATCH");return;}
            s.addSerial=result.serial;cs::Snap reply{};reply.world=s.owner.world;reply.acceptedIdValid=true;
            reply.acceptedId=result.id;reply.acceptedToken=result.constraintToken;
            (void)s.machine.step(reply,now);Reason(s,"ADD_SUBMITTED_WAITING_NATIVE_PRESENT");
        }
        return;
    }
    if(next.action==cs::Action::Remove&&!s.removeSubmitted){
        s.removeSubmitted=true;s.removeSerial=stamp.serial;
        const auto result=b.remove(c,s.owner,s.machine.acquireToken,s.machine.id);
        if(result.progress==Progress::Refused)s.removeSubmitted=false;
        if(result.progress==Progress::Uncertain){Quarantine(s,"REMOVE_UNCERTAIN_ID_RETAINED");return;}
        if(result.progress==Progress::Complete){
            if(!SameOwner(result.owner,s.owner)||result.constraintToken!=s.machine.acquireToken||result.id!=s.machine.id||result.serial!=stamp.serial){Quarantine(s,"REMOVE_REPLY_IDENTITY_MISMATCH");return;}
            s.removeSubmitted=true;s.removeSerial=result.serial;Reason(s,"REMOVE_SUBMITTED_WAITING_NATIVE_ABSENT");
        }
        return;
    }
    if(next.action==cs::Action::Drive&&keep){
        if(!Fresh(in,now)||s.cancellation!=g_cancel[in.side].load(std::memory_order_acquire)){
            Cancel(in.side);if(b.withdraw)(void)b.withdraw(c,s.owner);
            (void)s.machine.release(now);s.state=State::Releasing;Reason(s,"LIVE_CONTEXT_REVOKED_RELEASING");return;
        }
        const auto result=b.drive(c,s.owner,in);
        if(result==Progress::Uncertain){Quarantine(s,"DRIVE_UNCERTAIN");return;}
        if(result==Progress::Refused){(void)s.machine.release(now);s.state=State::Releasing;Reason(s,"DRIVE_REFUSED_RELEASING");return;}
        if(result==Progress::Complete&&sample.kind==cs::Status::Kind::Present&&sample.following){s.state=State::Held;Reason(s,"HELD_NATIVE_PRESENT_MEASURED_FOLLOW");}
        else {s.state=State::WaitingPresent;Reason(s,"WAITING_MEASURED_FOLLOW");}
    }
}
bool Enabled() noexcept{return g_enabled.load(std::memory_order_acquire)&&collab_h21::enabled.load(std::memory_order_acquire);}
void SetEnabled(bool on) noexcept{g_enabled.store(on,std::memory_order_release);}
bool PublishGrip(const Intent& i) noexcept{
    if(i.side<0||i.side>1)return false;
    if(!i.held||!i.allowed){Cancel(i.side);return true;}
    const auto cancellation=g_cancel[i.side].load(std::memory_order_acquire);
    auto owned=i;owned.profileEpoch=collab_h21::epoch.load(std::memory_order_acquire);
    return g_input[i.side].publish({owned,cancellation});
}
void Cancel(int side) noexcept{if(side>=0&&side<2)g_cancel[side].fetch_add(1,std::memory_order_acq_rel);}
bool ReadStatus(int side,Status& out) noexcept{return side>=0&&side<2&&g_output[side].read(out);}
bool PrepareTransfer(const Owner& o,std::uint64_t serial,std::uint64_t now,TransferPrepare prepare,void* context) noexcept {
    if(o.side<0||o.side>1||!serial||!prepare||!Enabled())return false;
    Status before{};if(!ReadStatus(o.side,before)||before.state!=State::Held||!SameOwner(before.owner,o)||!before.tick||now<before.tick||now-before.tick>80)return false;
    std::uint64_t empty=0;if(!g_transferTicket[o.side].compare_exchange_strong(empty,serial,std::memory_order_acq_rel))return false;
    // Read-only native Lua capture. T4 cleanup/reset remains completely free.
    // The later T4 consumer must find this SAME never-reused request still Held.
    if(!prepare(context)||!g_transferInput[o.side].publish({o,serial})){
        (void)EndTransfer(o,serial);return false;
    }
    return true; // queued, NOT an accepted cancellation or retirement receipt
}
TransferProgress ReadTransfer(const Owner& o,std::uint64_t serial) noexcept {
    if(o.side<0||o.side>1||!serial||g_transferTicket[o.side].load(std::memory_order_acquire)!=serial)return TransferProgress::Unknown;
    TransferReceipt r{};if(!g_transferOutput[o.side].read(r))return TransferProgress::Unknown;
    if(r.request.serial!=serial||!SameOwner(r.request.owner,o))return TransferProgress::Waiting;
    return r.progress;
}
bool EndTransfer(const Owner& o,std::uint64_t serial) noexcept {
    if(o.side<0||o.side>1||!serial)return false;
    auto expected=serial;if(g_transferTicket[o.side].compare_exchange_strong(expected,0,std::memory_order_acq_rel))return true;
    return expected==0;
}
static void TransferConsume(int side,Session& s) noexcept {
    const auto ticket=g_transferTicket[side].load(std::memory_order_acquire);auto& x=g_transferLocal[side];
    if(!ticket){x={};return;}
    if(x.request.serial==ticket)return;
    TransferRequest r{};if(!g_transferInput[side].read(r)||r.serial!=ticket)return;
    x={};x.request=r;x.accepted=s.state==State::Held&&s.owns&&SameOwner(s.owner,r.owner);
    if(x.accepted)Cancel(side); // original T4 owner, exact original request only
}
static void TransferPublish(int side,const Session&s) noexcept {
    const auto& x=g_transferLocal[side];if(!x.request.serial)return;
    TransferReceipt r{};r.request=x.request;
    r.progress=!x.accepted?TransferProgress::Declined:
        s.state==State::Idle&&!s.owns&&SameOwner(s.retiredOwner,x.request.owner)?TransferProgress::Retired:TransferProgress::Waiting;
    (void)g_transferOutput[side].publish(r);
}
bool InstallBackend(const Backend& b) noexcept{
    if(g_backendReady.load(std::memory_order_acquire)||!b.stamp||!b.prepare||!b.drive||!b.add||!b.read||!b.remove||!b.retire)return false;
    g_backend=b;g_backendReady.store(true,std::memory_order_release);return true;
}
bool NeedsPhysicsStepD80() noexcept {
    if(!g_backendReady.load(std::memory_order_acquire))return false;
    if(Enabled())return true; // first real request must have a current world stamp
    for(int side=0;side<2;++side){const auto& s=g_session[side];
        if(s.state!=State::Idle||s.owns||s.addPending||s.submitted||s.removeSubmitted||
           g_transferTicket[side].load(std::memory_order_acquire))return true;
    }
    return false;
}
void PhysicsStep(void* c,std::uint64_t now) noexcept{
    if(!c||!now||!g_backendReady.load(std::memory_order_acquire))return;
    Stamp stamp{};if(!g_backend.stamp(c,stamp)||!stamp.world||!stamp.generation||!stamp.serial)return;
    for(int side=0;side<2;++side){
        Envelope e{};(void)g_input[side].read(e);auto& s=g_session[side];
        TransferConsume(side,s);
        const auto cancellation=g_cancel[side].load(std::memory_order_acquire);
        // Cancellation belongs to the original acquisition. A later producer
        // packet for the still-held squeeze cannot restamp and revive it.
        if(s.state==State::Idle&&!s.owns)s.cancellation=cancellation;
        const auto& transfer=g_transferLocal[side];
        const bool cancelled=e.cancellation!=cancellation||e.intent.side!=side||
            (s.owns&&s.cancellation!=cancellation)||
            (transfer.accepted&&SameOwner(s.owner,transfer.request.owner));
        const bool admit=Enabled()&&!(s.state==State::Idle&&g_transferTicket[side].load(std::memory_order_acquire));
        StepSession(g_backend,c,stamp,s,e.intent,admit,cancelled,now);
        TransferPublish(side,s);
        Status out{};out.owner=s.owner;out.state=s.state;out.id=s.machine.id;out.constraintToken=s.machine.acquireToken;out.tick=now;
        for(unsigned i=0;i<80;++i)out.reason[i]=s.reason[i];(void)g_output[side].publish(out);
    }
}
void PhysicsRefresh(void* c,std::uint64_t now) noexcept {
    if(!c||!now||!g_backendReady.load(std::memory_order_acquire))return;
    Stamp stamp{};if(!g_backend.stamp(c,stamp))return;
    for(int side=0;side<2;++side){Envelope e{};auto& s=g_session[side];
        if(!s.owns)continue;
        const auto cancellation=g_cancel[side].load(std::memory_order_acquire);
        const bool driveState=s.state==State::Preparing||s.state==State::WaitingPresent||s.state==State::Held;
        const bool fresh=driveState&&Enabled()&&g_input[side].read(e)&&s.cancellation==cancellation&&
            e.cancellation==cancellation&&Fresh(e.intent,now)&&SamePress(s.intent,e.intent);
        if(!fresh){
            if(s.cancellation==cancellation)Cancel(side);
            // This function precedes the proxy motor in the SAME physics
            // callback. Skipping refresh alone left its old <=80ms target live
            // through release/menu/cutscene and the Remove/Absent interval.
            if(g_backend.withdraw)(void)g_backend.withdraw(c,s.owner);
            continue;
        }
        if(!g_backend.refresh||!s.owner.proxyGeneration||
           s.owner.world!=stamp.world||s.owner.worldGeneration!=stamp.generation||
           (s.state!=State::Held&&s.state!=State::WaitingPresent))continue;
        if(!g_backend.refresh(c,s.owner,e.intent)){
            Cancel(side);if(g_backend.withdraw)(void)g_backend.withdraw(c,s.owner);
        }
    }
}
void PhysicsReset(void* c,std::uint64_t now) noexcept {
    if(!c||!now||!g_backendReady.load(std::memory_order_acquire))return;
    Stamp cleanupStamp{};const bool sameWorldCleanup=g_backend.stamp(c,cleanupStamp);
    for(int side=0;side<2;++side){
        Cancel(side);auto& s=g_session[side];
        if(sameWorldCleanup){Intent none{};StepSession(g_backend,c,cleanupStamp,s,none,false,true,now);}
        else
        if(s.owns){
            const auto result=g_backend.reset?g_backend.reset(c,s.owner):Progress::Uncertain;
            if(result==Progress::Complete)Finish(s,"NATIVE_WORLD_RESET_REFERENCES_RETIRED");
            else Quarantine(s,"NATIVE_RESET_CLEANUP_UNCONFIRMED");
        }
        TransferPublish(side,s);
        Status out{};out.owner=s.owner;out.state=s.state;out.id=s.machine.id;out.constraintToken=s.machine.acquireToken;out.tick=now;
        for(unsigned i=0;i<80;++i)out.reason[i]=s.reason[i];(void)g_output[side].publish(out);
    }
}
}
