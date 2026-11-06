#pragma once
// F3-only observations. Producer copies computed PODs; worker owns formatting.
// No native reads, heap allocation, gameplay mutation or blocking producer lock.
#include "capture168.h"
#include "../vrik/fist_motion.h"
#include <algorithm>
#include <array>
#include <atomic>
#include <cstdio>
namespace kcd2vr::diagnostics::punch_c54r3 {
namespace fm=kcd2vr::fists;
inline bool active(std::uint64_t tick) noexcept {return capture168::active(capture168::Domain::Combat,tick);}
inline std::uint64_t session() noexcept {return capture168::gate.session[unsigned(capture168::Domain::Combat)].load(std::memory_order_acquire);}
enum class Stage : unsigned { Recognizer,ProducerGate,Publication,NativeGate,OwnerAcquired,Frame,Pending,Prepare,Release,Cancel,NativeSnapshot };
enum Why : unsigned { None,NotSelected,Fault,Mode,NativeReference,NativeBody,Gameplay,Tracking,OwnerUnresolved,Occupied,
    FrameAbsent,FrameRequest,FramePlayer,FrameEpoch,FrameInvalid,FrameFuture,FrameStale,OwnerChanged,EntryAbortFailed,
    NoPunch,AlreadySeen,StrokeFuture,StrokeStale,Cooldown,PreparedFuture,PreparedExpired,PendingOccupied,GripReleased,
    WaitingNextFrame,PrepareFailed,Returned,ReleaseFailed,Retired,OwnerPublicationStale,MailboxBusy,ReplacedPending,
    PendingExpired,PendingSeen,NativeStateUnreadable,NativeStateObserved,NativeCallGuard,NativeInputIdentity,NativeString,NativeException,ManagerVft,ManagerPlayerRead,PlayerVft,PlayerManagerRead,OwnerBacklink,RightHeldKeyRead,LeftHeldKeyRead };
inline const char* stageName(Stage s) noexcept {static const char* n[]{"RECOGNIZER","PRODUCER_GATE","PUBLICATION","NATIVE_GATE","OWNER_ACQUIRED","FRAME","PENDING","PREPARE","RELEASE","CANCEL","NATIVE_SNAPSHOT"};return n[unsigned(s)];}
inline const char* whyName(unsigned w) noexcept {static const char* n[]{"NONE","NOT_SELECTED","NATIVE_FAULT","MODE_DISALLOWED","NATIVE_REFERENCE","NATIVE_BODY_OWNER","GAMEPLAY_DISALLOWED","TRACKING_INVALID","OWNER_UNRESOLVED","HANDS_OCCUPIED","FRAME_ABSENT","FRAME_REQUEST_MISMATCH","FRAME_PLAYER_MISMATCH","FRAME_EPOCH_MISMATCH","FRAME_INVALID","FRAME_FROM_FUTURE","FRAME_STALE","OWNER_CHANGED","ENTRY_ABORT_FAILED","NO_RECOGNIZED_PUNCH","ALREADY_CONSUMED","STROKE_FROM_FUTURE","STROKE_STALE","NATIVE_COOLDOWN_220MS","PREPARED_FROM_FUTURE","PREPARED_EXPIRED_150MS","PENDING_HAND_OCCUPIED","TRIGGER_OR_GRIP_NOT_ABOVE_0_60","WAIT_NEXT_NATIVE_FRAME","PREPARE_FAILED","CALL_RETURNED_NOT_ADMISSION","RELEASE_FAILED","RETIRED","OWNER_PUBLICATION_NOT_CURRENT","MAILBOX_BUSY","EXISTING_PENDING_WINS","PENDING_EXPIRED_100MS","PENDING_ALREADY_SEEN","NATIVE_STATE_UNREADABLE","NATIVE_STATE_SNAPSHOT_NOT_ADMISSION","NATIVE_CALL_BINARY_OR_PLAYER_GUARD","NATIVE_INPUT_IDENTITY","NATIVE_STRING_UNAVAILABLE","NATIVE_CALL_EXCEPTION","MANAGER_VFT_NOT_EXACT","MANAGER_PLAYER_READ_FAILED","PLAYER_VFT_NOT_EXACT","PLAYER_MANAGER_READ_FAILED","PLAYER_MANAGER_BACKLINK_MISMATCH","RIGHT_HELD_KEY_READ_FAILED","LEFT_HELD_KEY_READ_FAILED"};return w<std::size(n)?n[w]:"UNKNOWN";}
struct Join {std::uintptr_t player=0,actor=0;std::uint64_t request=0,epoch=0,sample=0,onset=0;int side=-1;};
// A context window is a transport observation, never an exact collision/result join.
struct NativeContext {Join join{};std::uintptr_t nativeActor=0;std::uint64_t tick=0,session=0;Stage stage=Stage::Prepare;};
inline NativeContext nativeContext{};inline std::atomic_flag contextLock=ATOMIC_FLAG_INIT;
inline bool publishContext(Join join,std::uint64_t tick,Stage stage,std::uintptr_t nativeActor=0) noexcept {
    if(!active(tick)||contextLock.test_and_set(std::memory_order_acquire))return false;
    nativeContext={join,nativeActor,tick,punch_c54r3::session(),stage};contextLock.clear(std::memory_order_release);return true;
}
inline void retireContext() noexcept {if(contextLock.test_and_set(std::memory_order_acquire))return;nativeContext={};contextLock.clear(std::memory_order_release);}
inline bool readContext(NativeContext& out,std::uint64_t now) noexcept {
    if(!active(now)||contextLock.test_and_set(std::memory_order_acquire))return false;
    out=nativeContext;contextLock.clear(std::memory_order_release);
    return out.tick&&out.session==punch_c54r3::session()&&now>=out.tick&&now-out.tick<=500;
}
struct Record {
    Join join{};Stage stage=Stage::Recognizer;unsigned why=0,mask=0,inputMask=0;std::uint64_t session=0,tick=0,age=0,intervalSamples=0,suppressed=0;std::uintptr_t nativeActor=0;
    fm::TraceHand hand{};double grip=0,trigger=0,maxSpeed=0,maxTravel=0;int nativePhase=-1,nativeType=-1,nativeInput=-1,nativeHand=-1,nativeZone=-1;
    double controller[3]{},allowed[3]{},headPosition[3]{};
    bool blocked=false,freshContact=false,nativeRequested=false,nativeCandidate=false,nativeBlocking=false;
};
inline std::array<Record,256> queue{};inline unsigned head=0,tail=0;inline std::atomic_flag lock=ATOMIC_FLAG_INIT;
inline std::atomic<std::uint64_t> dropped{0},accepted{0},drained{0},staleDiscarded{0},offDiscarded{0},formatClipped{0};
inline bool push(Record r) noexcept {
    if(!active(r.tick))return false;r.session=session();
    if(lock.test_and_set(std::memory_order_acquire)){++dropped;return false;}
    const auto next=(head+1)%queue.size();if(next==tail){lock.clear(std::memory_order_release);++dropped;return false;}
    queue[head]=r;head=unsigned(next);++accepted;lock.clear(std::memory_order_release);return true;
}
inline bool pop(Record& r) noexcept {if(lock.test_and_set(std::memory_order_acquire))return false;const bool have=tail!=head;if(have){r=queue[tail];tail=(tail+1)%queue.size();++drained;}lock.clear(std::memory_order_release);return have;}
struct Rate {std::uint64_t session=0,last=0,suppressed=0;unsigned code=~0u;bool permit(std::uint64_t now,unsigned value,bool edge=false) noexcept {
    if(!active(now)){*this={};return false;}const auto current=punch_c54r3::session();
    if(session!=current){*this={};session=current;code=~0u;}
    if(!last||now<last||now-last>=1000||edge||(value!=code&&now-last>=100)){last=now;code=value;return true;}++suppressed;return false;
}};
struct HandObserver {Rate rate{};std::uint64_t onset=0,samples=0;double maxSpeed=0,maxTravel=0;};
struct Observer {std::array<HandObserver,2> hand{};std::uint64_t session=0,request=0,epoch=0;std::uintptr_t actor=0;
    void observe(const fm::Sample& sample,const fm::Trace& trace,const fm::Punch& result,std::uint64_t request,std::uintptr_t player) noexcept {
        if(!active(sample.tick)){*this={};return;}const auto current=punch_c54r3::session();if(session!=current||this->request!=request||epoch!=sample.epoch||actor!=sample.actor){*this={};session=current;this->request=request;epoch=sample.epoch;actor=sample.actor;}
        for(int side=0;side<2;++side){auto& o=hand[side];const auto& h=trace.hand[side];++o.samples;o.maxSpeed=std::max(o.maxSpeed,h.speed);o.maxTravel=std::max(o.maxTravel,h.travel);
            const bool onset=h.after==fm::Phase::Moving&&h.before!=fm::Phase::Moving;
            if(onset||(h.qualified&&h.before==fm::Phase::Ready))o.onset=sample.sequence;
            const bool edge=h.before!=h.after||h.qualified||(result.valid&&result.side==side);
            if(!o.rate.permit(sample.tick,h.reject,edge))continue;
            Record r{};r.join={player,sample.actor,request,sample.epoch,sample.sequence,o.onset,side};r.tick=sample.tick;r.hand=h;r.grip=sample.grip[side];r.trigger=sample.trigger[side];
            r.controller[0]=sample.controller[side].x;r.controller[1]=sample.controller[side].y;r.controller[2]=sample.controller[side].z;
            r.allowed[0]=sample.allowed[side].x;r.allowed[1]=sample.allowed[side].y;r.allowed[2]=sample.allowed[side].z;
            r.headPosition[0]=sample.head.x;r.headPosition[1]=sample.head.y;r.headPosition[2]=sample.head.z;
            r.blocked=sample.blocked[side];r.freshContact=sample.freshContact174[side];r.mask=h.reject;r.inputMask=trace.invalidInputs;r.maxSpeed=o.maxSpeed;r.maxTravel=o.maxTravel;r.intervalSamples=o.samples;r.suppressed=o.rate.suppressed;
            (void)push(r);o.samples=o.rate.suppressed=0;o.maxSpeed=o.maxTravel=0;
            if(!trace.valid||h.after==fm::Phase::Release)o.onset=0;
        }
    }
};
inline void native(Stage stage,unsigned why,Join join,std::uint64_t now,Rate* rate=nullptr,unsigned mask=0,std::uint64_t age=0,double grip=0,double trigger=0) noexcept {
    if(!active(now))return;if(rate&&!rate->permit(now,why))return;Record r{};r.join=join;r.stage=stage;r.why=why;r.mask=mask;r.age=age;r.tick=now;r.grip=grip;r.trigger=trigger;
    if(rate){r.suppressed=rate->suppressed;rate->suppressed=0;}(void)push(r);
}
inline int format(const Record& r,char* out,std::size_t cap) noexcept {
    return std::snprintf(out,cap,"C54R3 PUNCH_PATH session=%llu tick=%llu player=%llX actor=%llX request=%llu epoch=%llu sample=%llu onset=%llu side=%d stage=%s why=%s mask=%08X invalidInputs=%08X phase=%u->%u released=%d closed=%d openPalm=%d grip=%.3f trigger=%.3f triggerGate=GT_0_60 gripGate=GT_0_60 dt=%.6f speed=%.3f stillSpeed=%.3f onsetMin=%.3f speedMin=%.3f travel=%.4f travelMin=%.4f allowedTravel=%.4f allowedMin=%.4f allowedSpeed=%.3f allowedSpeedMin=0.100 directionDot=%.5f reverseRule=%s quietMs=%llu settlingQuietMin=120 rearmQuietMin=%u strokeMs=%llu strokeMax=500 retract=%.4f rearmBelow=%.4f blocked=%d freshContact=%d recentStop=%d qualified=%d ageMs=%llu maxSpeed=%.3f maxTravel=%.4f samples=%llu suppressed=%llu controllerXR=(%.4f,%.4f,%.4f) headXR=(%.4f,%.4f,%.4f) allowedNative=(%.4f,%.4f,%.4f) native{actor:%llX phase:%d type:%d input:%d hand:%d zone:%d requested:%d candidate:%d blocking:%d} strokeExperimentD49=%d strokeSpeedD49=%.3f reversalDistanceD49=%.4f motionRearmD53=%d towardHeadRejectBit=00200000 join=EXACT_REQUEST_EPOCH_SAMPLE_ONLY nativeContact=NOT_INFERRED nativeAdmission=NOT_INFERRED writes=NONE",
        (unsigned long long)r.session,(unsigned long long)r.tick,(unsigned long long)r.join.player,(unsigned long long)r.join.actor,(unsigned long long)r.join.request,(unsigned long long)r.join.epoch,(unsigned long long)r.join.sample,(unsigned long long)r.join.onset,r.join.side,stageName(r.stage),whyName(r.why),r.mask,r.inputMask,unsigned(r.hand.before),unsigned(r.hand.after),r.hand.released,r.hand.closed,r.hand.openStrike,r.grip,r.trigger,r.hand.dt,r.hand.speed,r.hand.stillSpeed,r.hand.onsetMin,r.hand.speedMin,r.hand.travel,r.hand.travelMin,r.hand.allowedTravel,r.hand.allowedTravelMin,r.hand.allowedSpeed,r.hand.directionDot,r.hand.strokeExperimentD49?"PEAK_RETRACT_GT_0.015":"SAMPLE_DOT_LT_-0.002",(unsigned long long)r.hand.quietMs,r.hand.rearmQuietMin,(unsigned long long)r.hand.strokeMs,r.hand.retract,r.hand.retractMin,r.blocked,r.freshContact,r.hand.recentStop,r.hand.qualified,(unsigned long long)r.age,r.maxSpeed,r.maxTravel,(unsigned long long)r.intervalSamples,(unsigned long long)r.suppressed,r.controller[0],r.controller[1],r.controller[2],r.headPosition[0],r.headPosition[1],r.headPosition[2],r.allowed[0],r.allowed[1],r.allowed[2],(unsigned long long)r.nativeActor,r.nativePhase,r.nativeType,r.nativeInput,r.nativeHand,r.nativeZone,r.nativeRequested,r.nativeCandidate,r.nativeBlocking,r.hand.strokeExperimentD49,r.hand.strokeSpeedD49,r.hand.reversalDistanceD49,r.hand.motionRearmD53);
}
} // namespace
