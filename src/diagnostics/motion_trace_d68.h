#pragma once
// Shift+F2-only, finite copied observations. No renderer/pose authority.
#include "performance_gate_d61.h"
#include <array>
#include <atomic>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <type_traits>
namespace kcd2vr::diagnostics::motion_d68 {
enum Stage : unsigned {InputPublished=1,TrackingRead=2,Mapping=3,CoreBefore=4,CoreAfter=5,
    PosePublished=6,RenderRequest=7,Submit=8,NativeFrame=9,Attachment=10,HandPrediction=11};
// High bits are shared CORE_BEFORE/CORE_AFTER owner observations, never an
// inference that zero bits prove geometric clearance. StateKnown is required.
enum HandFlags : unsigned {StateKnown=1u<<8,PhysicalOwner=1u<<9,PropOwner=1u<<10,
    PresenceOwner=1u<<11,DoorOwner=1u<<12,PassengerOwner=1u<<13,ReadingOwner=1u<<14,
    CaptureOwner=1u<<15,HeldKey=1u<<16,ItemOccupied=1u<<17,ItemUnknown=1u<<18,
    FreeContactEligible=1u<<19,ContactFresh=1u<<20,ContactBlocked=1u<<21,
    ContactPlanes=1u<<22,SelfContact=1u<<23,PairContact=1u<<24,Handover=1u<<25,
    OtherOwner=1u<<26,TrackingHeld=1u<<27,PhysicalBlocked=1u<<28,PhysicalContactUnknown=1u<<29,ContactRelease=1u<<30,FingerContact=1u<<31};
struct Event {
    std::uint64_t session=0,ns=0,endNs=0,sourceNs=0,sourceTick=0,sample=0,trackingEpoch=0,poseEpoch=0;
    std::uint64_t publication=0,frame=0,cycle=0,order=0,inputTick=0,priorSample=0,priorSourceNs=0,request=0;
    std::uintptr_t object=0,actor=0,descriptor=0,pose=0,output=0;
    std::int64_t targetXrNs=0,leftTargetD69=0,rightTargetD69=0;
    std::uint64_t handPolicyD69=0;unsigned leftLeadD69=0,rightLeadD69=0;
    unsigned stage=0,flags=0,count=0,thread=0;int side=-1;
    float v[48]{};
};
static_assert(std::is_trivially_copyable_v<Event>);
// Stage-local flags/values are intentionally documented at their producer. IDs
// and times above never stand for an inferred presented-frame association.
inline std::array<Event,512> queue{};
inline unsigned head=0,tail=0;
inline std::atomic_flag busy=ATOMIC_FLAG_INIT;
inline std::atomic<std::uint64_t> dropped{0},copied{0},drained{0},expired{0},formatDropped{0},order{0},cycles{0};
inline constexpr std::uint64_t BurstNs=250000000ull,PeriodNs=2000000000ull;
inline std::array<std::atomic<std::uint64_t>,12> observed{},outsideWindow{},cadenceSkipped{},pushed{},stageDropped{};
inline bool requested() noexcept {return perf_d61::requested();}
inline bool current(std::uint64_t session) noexcept {
    return requested()&&perf_d61::gate.current(session,perf_d61::nowMs());
}
inline bool begin(Event& e,Stage stage,bool countObservation=true) noexcept {
    if(!requested())return false;
    const auto session=perf_d61::gate.serial.load(std::memory_order_acquire),ns=perf_d61::nowNs();
    if(!perf_d61::gate.current(session,ns/1000000ull))return false;
    e={};e.session=session;e.ns=ns;e.stage=unsigned(stage);
    if(countObservation&&e.stage<observed.size())observed[e.stage].fetch_add(1,std::memory_order_relaxed);
    return true;
}
// Synchronous 250 ms bursts every 2 s across ALL producers, anchored to the
// existing finite Shift+F2 lease. Whole-session aggregate metrics remain active.
inline bool inWindow(const Event& e,bool countOutside=true) noexcept {
    const auto end=perf_d61::gate.until.load(std::memory_order_acquire);
    const auto start=end>=perf_d61::DurationMs?end-perf_d61::DurationMs:0;
    const bool yes=e.session&&e.session==perf_d61::gate.serial.load(std::memory_order_acquire)&&
        start&&e.ns>=start*1000000ull&&((e.ns-start*1000000ull)%PeriodNs)<BurstNs;
    if(!yes&&countOutside&&e.stage<outsideWindow.size())outsideWindow[e.stage].fetch_add(1,std::memory_order_relaxed);
    return yes;
}
inline void noteCadenceSkip(Stage s) noexcept {cadenceSkipped[unsigned(s)].fetch_add(1,std::memory_order_relaxed);}
// Caller owns this tiny object (pose mutex, native owner, or thread local).
// It cannot keep a capture alive or turn a recorder on.
struct Cadence {
    std::uint64_t session=0,lastNs=0,skipped=0;
    bool due(std::uint64_t id,std::uint64_t ns,std::uint64_t periodNs=3000000ull) noexcept {
        if(!id||!ns)return false;
        if(session!=id){session=id;lastNs=0;skipped=0;}
        if(lastNs&&ns>=lastNs&&ns-lastNs<periodNs){++skipped;return false;}
        lastNs=ns;return true;
    }
};
inline std::uint64_t nextCycle() noexcept {return cycles.fetch_add(1,std::memory_order_relaxed)+1;}
inline bool push(Event e) noexcept {
    if(e.count>48||e.stage<InputPublished||e.stage>HandPrediction||!current(e.session))return false;
    if(busy.test_and_set(std::memory_order_acquire)){dropped.fetch_add(1,std::memory_order_relaxed);stageDropped[e.stage].fetch_add(1,std::memory_order_relaxed);return false;}
    const unsigned next=unsigned((head+1)%queue.size());
    if(next==tail){busy.clear(std::memory_order_release);dropped.fetch_add(1,std::memory_order_relaxed);stageDropped[e.stage].fetch_add(1,std::memory_order_relaxed);return false;}
    e.order=order.fetch_add(1,std::memory_order_relaxed)+1;
    queue[head]=e;head=next;copied.fetch_add(1,std::memory_order_relaxed);pushed[e.stage].fetch_add(1,std::memory_order_relaxed);
    busy.clear(std::memory_order_release);return true;
}
inline bool pop(Event& e) noexcept {
    if(busy.test_and_set(std::memory_order_acquire))return false;
    const bool have=tail!=head;
    if(have){e=queue[tail];tail=unsigned((tail+1)%queue.size());drained.fetch_add(1,std::memory_order_relaxed);}
    busy.clear(std::memory_order_release);return have;
}
inline const char* stageName(unsigned s) noexcept {
    switch(s){case InputPublished:return "INPUT_PUBLISHED";case TrackingRead:return "TRACKING_READ";
    case Mapping:return "MAPPING";case CoreBefore:return "CORE_BEFORE";case CoreAfter:return "CORE_AFTER";
    case PosePublished:return "POSE_PUBLISHED";case RenderRequest:return "RENDER_REQUEST";case Submit:return "SUBMIT";
    case NativeFrame:return "NATIVE_FRAME";case Attachment:return "ATTACHMENT";case HandPrediction:return "HAND_PREDICTION";default:return "UNKNOWN";}
}
inline const char* schema(unsigned s) noexcept {
    switch(s){
    case InputPublished:return "preGraceH_L_R_P3Q4,preparedH_L_R_P3Q4;D69lead0baseline1velocity2futureLocate;bit16=permissionPacket21Only;ns=prepareEntry;endNs=publishedBy";
    case TrackingRead:return "selectedH_L_R_P3Q4;reason1Empty2Epoch3Current4Invalid5HJ1_6Busy7Legacy;bit31=finalSkinRead;ns=readEntry;endNs=readReturned";
    case RenderRequest:case Submit:return "stage9Wait10Pickup11Copy12End13Enqueue,result;sample=IMAGE_SERIAL;publication=CAMERA_FRAME;frame=XR_FRAME;request=ENQUEUE_NS;scanout=UNOBSERVED";
    case Mapping:return "calQ4,trackedBase3,sharedAnchor3,rawXR3,effectiveXR3,rawRequestedModel3,effectiveRequestedModel3,manualModelOffset3,palmModelOffset3,rawQ4,effectiveQ4,modelWorldQ4,modelWorldT3,scale1,rawPalmModelOffset3,wristBackM1";
    case CoreBefore:return "nativeB1Q4T3,stagedQ4T3,originalQ4T3,jointId1";
    case CoreAfter:return "nativePaletteRaw8,stagedQ4T3,jointId1";
    case PosePublished:return "originalReturned1,restored1,restoreOK1,committed1,packetPublished1,pairedPublished1,finalPublished1,consumers1";
    case NativeFrame:return "nativeFrameId=UNOBSERVED,observerEntry=1";
    case HandPrediction:return "baseLeftP3Q4,baseRightP3Q4,ledLeftP3Q4,ledRightP3Q4,leftLinear3Angular3,rightLinear3Angular3;flags0leftLinear1leftAngular2rightLinear3rightAngular;lead0baseline1velocity2futureLocate;sample0=prepublication";
    case Attachment:return "wantedWorldQ4T3S1,referenceWorldQ4T3S1,producerModelWorldQ4T3S1,currentModelWorldQ4T3S1;bit16=sameCyclePriorOrdinaryReference_notFreshNative;CPUonly";
    default:return "PRODUCER_DOCUMENTED";}
}
// Worker only; neither formatting nor file I/O executes in the producing hook.
inline bool format(const Event& e,char* out,std::size_t size) noexcept {
    if(!out||!size||e.count>48)return false;
    int n=std::snprintf(out,size,
        "D68_MOTION session=%llu order=%llu stage=%s ns=%llu endNs=%llu sourceNs=%llu sourceTick=%llu sample=%llu trackingEpoch=%llu poseEpoch=%llu publication=%llu frame=%llu cycle=%llu actor=%llX object=%llX descriptor=%llX pose=%llX output=%llX targetXrNs=%lld leftTargetD69=%lld rightTargetD69=%lld handPolicyD69=%llu leftLeadD69=%u rightLeadD69=%u inputTick=%llu priorSample=%llu priorSourceNs=%llu request=%llu thread=%u side=%d flags=%X count=%u schema=%s meshSubmission=UNOBSERVED v=",
        (unsigned long long)e.session,(unsigned long long)e.order,stageName(e.stage),(unsigned long long)e.ns,
        (unsigned long long)e.endNs,(unsigned long long)e.sourceNs,(unsigned long long)e.sourceTick,(unsigned long long)e.sample,
        (unsigned long long)e.trackingEpoch,(unsigned long long)e.poseEpoch,(unsigned long long)e.publication,
        (unsigned long long)e.frame,(unsigned long long)e.cycle,(unsigned long long)e.actor,(unsigned long long)e.object,
        (unsigned long long)e.descriptor,(unsigned long long)e.pose,(unsigned long long)e.output,(long long)e.targetXrNs,
        (long long)e.leftTargetD69,(long long)e.rightTargetD69,(unsigned long long)e.handPolicyD69,e.leftLeadD69,e.rightLeadD69,(unsigned long long)e.inputTick,(unsigned long long)e.priorSample,(unsigned long long)e.priorSourceNs,(unsigned long long)e.request,e.thread,e.side,e.flags,e.count,schema(e.stage));
    if(n<0||std::size_t(n)>=size){out[size-1]=0;return false;}
    for(unsigned i=0;i<e.count;++i){const int add=std::snprintf(out+n,size-std::size_t(n),i?",%.9g":"%.9g",double(e.v[i]));
        if(add<0||std::size_t(add)>=size-std::size_t(n)){out[size-1]=0;return false;}n+=add;}
    return true;
}
inline bool accept(const char* line,std::uint64_t now) noexcept {
    constexpr char prefix[]="D68_MOTION session=";
    if(!line||std::strncmp(line,prefix,sizeof(prefix)-1))return false;
    const char* p=line+sizeof(prefix)-1;if(*p<'0'||*p>'9')return false;
    char* end=nullptr;const auto id=std::strtoull(p,&end,10);
    return end&&(*end==0||*end==' ')&&perf_d61::gate.receipt(id,now);
}
template<class Writer> inline void drain(std::uint64_t now,Writer&& write,unsigned budget=64) noexcept {
    Event e{};char line[2048]{};
    while(budget--&&pop(e)){
        if(!perf_d61::gate.receipt(e.session,now)){expired.fetch_add(1,std::memory_order_relaxed);continue;}
        if(format(e,line,sizeof(line)))write(line);else formatDropped.fetch_add(1,std::memory_order_relaxed);
    }
}
}
