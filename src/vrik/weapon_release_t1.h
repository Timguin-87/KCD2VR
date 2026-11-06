#pragma once
// T1: preference and POD release contract. No native calls or item creation.
#include "../core/native_grasp13.h"
#include "../core/collab_test_h21.h"
#include <atomic>
namespace kcd2vr::weapon_release_t1 {
inline std::atomic<int> mode{0}; // 0 automatic holster, 1 actual native drop/throw
inline std::atomic<std::uint64_t> modeEpochT9{1};
inline void SetMode(int value) noexcept { const int next=value==1?1:0;
    if(mode.load(std::memory_order_acquire)!=next){
        mode.store(0,std::memory_order_release); // revoke permission before advancing receipt epochs
        modeEpochT9.fetch_add(1,std::memory_order_acq_rel);
        collab_h21::throwEpoch.fetch_add(1,std::memory_order_acq_rel); // revoke old release receipts on preference edges
        mode.store(next,std::memory_order_release);
    } }
inline int Mode() noexcept { return mode.load(std::memory_order_acquire); }
// The saved release preference authorizes drop. F4 supplies test admission only;
// toggling a recorder/test must never change or persist this preference.
inline bool TestEnabledH21() noexcept {return Mode()==1||collab_h21::throwing.load(std::memory_order_acquire);}
inline bool SupportedClass(int id) noexcept {return (id>=0&&id<=8)||id==16||id==17;}
// A deliberate return to the known current holster always keeps native stow.
inline bool DropOutsideHolster(bool nativeHolsterRequest,bool drawn,bool intentional,
    bool mountKnown,double gap,double radius,int selectedMode) noexcept {
    return TestEnabledH21()&&selectedMode==1&&nativeHolsterRequest&&drawn&&intentional&&
        !(mountKnown&&std::isfinite(gap)&&std::isfinite(radius)&&gap<=radius);
}
struct Request {
    std::uintptr_t manager=0,player=0,character=0,attachment=0;
    std::uint64_t key=0,generation=0,bodyEpoch=0,requestEpoch=0,sample=0,tick=0;
    std::uint32_t entity=0;int side=-1,classId=-1;
    float linear[3]{},angular[3]{};bool motion=false;
    grasp13::Pose lastAllowed{};std::uint64_t motionTick=0;
    std::uint64_t throwEpochH21=0,physicsEpochH21=0;
};
inline bool CurrentTestH21(const Request& r) noexcept {
    return TestEnabledH21()&&r.throwEpochH21&&r.throwEpochH21==collab_h21::throwEpoch.load(std::memory_order_acquire)&&
        r.physicsEpochH21&&r.physicsEpochH21==collab_h21::epoch.load(std::memory_order_acquire);
}
struct Result {
    bool invoked=false,handClear=false,worldInventory=false,equipmentClear=false;
    bool dispatched=false,queued=false;unsigned poseReads=0,dispatchCalls=0;
    bool acknowledged() const noexcept {return invoked&&handClear&&worldInventory&&equipmentClear&&
        dispatched&&poseReads==1&&dispatchCalls==1;}
    bool ownerQueuedT9=false; // POD intent accepted only; never a native dispatch/ACK
};
// T2 observation after the MATCHING native drop consumer has returned, actual
// hand/inventory/equipment release and current rigid-body dynamics agree.
// Consumers must re-resolve every pointer; this POD does not retain a body.
struct CommittedT2 {
    Request source{};grasp13::Pose world{};
    std::uintptr_t body=0;std::uint32_t entity=0;
    std::uint64_t serial=0,consumerSerial=0,tick=0;
    float linear[3]{},angular[3]{};bool spinSubmitted=false;
};
struct Sample {
    grasp13::Binding identity{};grasp13::Motion motion{};
    std::uintptr_t attachment=0;std::uint64_t generation=0,requestEpoch=0;
    std::uint64_t throwEpochH21=0,physicsEpochH21=0;
};
inline bool matches(const Sample& s,const Request& r,std::uint64_t now) noexcept {
    const auto& b=s.identity;
    return CurrentTestH21(r)&&s.throwEpochH21==r.throwEpochH21&&s.physicsEpochH21==r.physicsEpochH21&&
        b.key==r.key&&b.manager==r.manager&&b.character==r.character&&b.entity==r.entity&&
        b.epoch==r.bodyEpoch&&b.side==r.side&&s.generation==r.generation&&
        s.requestEpoch==r.requestEpoch&&s.attachment==r.attachment&&s.motion.have&&
        s.motion.newer.sample<=r.sample&&s.motion.newer.tick<=r.tick&&
        grasp13::fresh(now,s.motion.newer.tick,80);
}
inline bool continuous(const Request& r,const grasp13::Pose& current,std::uint64_t now) noexcept {
    if(!grasp13::valid(r.lastAllowed)||!grasp13::valid(current)||!grasp13::fresh(now,r.motionTick,80)||
       std::abs(current.scale-r.lastAllowed.scale)>.001f)return false;
    const double dt=double(now-r.motionTick)*.001;double distance2=0,speed2=0,dot=0,w2=0;
    for(int i=0;i<3;++i){const double d=current.p[i]-r.lastAllowed.p[i];distance2+=d*d;
        speed2+=double(r.linear[i])*r.linear[i];w2+=double(r.angular[i])*r.angular[i];}
    for(int i=0;i<4;++i)dot+=double(current.q[i])*r.lastAllowed.q[i];
    const double maxMove=std::min(.25,.02+std::sqrt(speed2)*dt);
    const double maxAngle=std::min(.50,.08+std::sqrt(w2)*dt);
    return std::isfinite(distance2)&&distance2<=maxMove*maxMove&&
        2*std::acos(std::clamp(std::abs(dot),0.,1.))<=maxAngle;
}
}
