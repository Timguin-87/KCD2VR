#pragma once
// C46: comfortable gesture credits for exact native alchemy working children.
// No native calls, input injection, clock reads or rendering dependency here.
#include "../core/snapshot_mailbox.h"
#include "minigame_hands45.h"
#include "guided_crafting55.h"
#include "alchemy_work_clock_d47.h"
#include "../subsystems/alchemy/bridge67.h"
#include <atomic>
#include <cmath>
#include <cstdint>
namespace kcd2vr::alchemy_semi46 {
enum Kind : unsigned { None=0, Bellows=1, Mortar=2, Resource=3, Book=4, Stir=5, MortarPlace70=6 };
struct WorkWitness {
    std::uintptr_t instance=0,parent=0,child=0;std::uint64_t lifetime=0,tick=0;
    std::uint32_t token=0;unsigned kind=None;int phase=-1;unsigned hands=0;
    bool working=false,ready=false;
};
struct Control {
    std::uintptr_t instance=0,parent=0,child=0;std::uint64_t lifetime=0,tick=0,sequence=0,poseEpoch=0,sourceEpoch=0;
    std::uint32_t token=0;bool enabled=false,manualUse=false,tracked[2]{};int hand=-1;double totalSeconds=0;
    bool use=false;std::uint64_t useUntil=0;
    bool clockStop=true; // current physical sample explicitly permits/halts work
    bool resourceReturnHomeD86=false; // current phase2 joined held carafe at its original home, upright
};
struct BookEnable {
    std::uintptr_t instance=0,action=0;std::uint64_t lifetime=0,generation=0;bool enabled=false;
};
inline core::SnapshotMailbox<WorkWitness> work;
inline core::SnapshotMailbox<Control> control;
inline core::SnapshotMailbox<BookEnable> bookEnable;
inline std::atomic<bool> hooksReady{false},mortarReady70{false};
inline std::atomic<std::uint64_t> bookGeneration{1};
inline constexpr std::uint64_t kFreshMs=80;
inline bool fresh(std::uint64_t tick,std::uint64_t now) noexcept {return tick&&now>=tick&&now-tick<=kFreshMs;}
inline bool same(const WorkWitness& a,const WorkWitness& b) noexcept {
    return a.instance&&a.instance==b.instance&&a.lifetime==b.lifetime&&a.parent==b.parent&&
        a.child==b.child&&a.token==b.token&&a.kind==b.kind;
}
inline bool nativeHandOwned(std::uint64_t now,int side) noexcept {
    WorkWitness w{};Control c{};
    return side>=0&&side<2&&(kcd2vr::minigame_hands45::modeFor(kcd2vr::minigame_hands45::Alchemy)==kcd2vr::minigame_hands45::SemiPassenger||guided_crafting55::active(now)||kcd2vr::subsystems::alchemy::bridge67::visualPassenger71(now))&&hooksReady.load(std::memory_order_acquire)&&work.read(w)&&control.read(c)&&
        c.enabled&&fresh(c.tick,now)&&fresh(w.tick,now)&&c.instance==w.instance&&c.lifetime==w.lifetime&&
        (w.hands&(1u<<side));
}
struct Producer {
    WorkWitness owner{};std::uint64_t sequence=0,timeNs=0,poseEpoch=0,sourceEpoch=0,useUntil=0;
    float last[2][3]{},reference[2][3]{},lastUp[2][3]{},referenceUp[2][3]{};bool have[2]{},haveUp[2]{},readyUsed=false,physicalA=false,manualWork=false,lastClockStop=true;int selectedHand=-1;double totalSeconds=0;
    // Per-sample observations are separate from the quantized work reference.
    // A subthreshold moving sample permits existing credit; an actual stationary
    // sample stops immediately even when an old fractional stroke remains.
    float observedD49[2][3]{},observedUpD49[2][3]{};bool observedValidD49[2]{},observedUpValidD49[2]{};
    void reset() noexcept {*this=Producer{};}
    // Frame is workstation_sample46::Frame. The template keeps this production
    // producer host-testable without inventing a second tracking data source.
    template<class Frame> Control step(const Frame& f,bool enabled,std::uintptr_t instance,std::uint64_t lifetime,
                                      const WorkWitness& w,bool manualUse=false,bool cancel=false,int forcedHand=-1,bool workAllowed=true,bool readyAllowed=true) noexcept {
        // A that selected a tool remains admission while held through pickup.
        // A NEW press during a witnessed working segment is manual takeover.
        const bool pressed=manualUse&&!physicalA,released=!manualUse&&physicalA;physicalA=manualUse;
        const auto clear=[&](){reset();physicalA=manualUse;};
        Control c{};c.instance=instance;c.lifetime=lifetime;c.tick=f.tick;c.sequence=f.sequence;
        c.poseEpoch=f.poseEpoch;c.sourceEpoch=f.sourceEpoch;c.hand=forcedHand;c.tracked[0]=f.hand[0].valid;c.tracked[1]=f.hand[1].valid;
        bool finite=f.headValid;
        for(unsigned n=0;n<3;++n)finite=finite&&std::isfinite(f.head[n]);
        if(!enabled||cancel||!f.allowed||!finite||!instance||!lifetime||!f.tick||!f.sequence||!f.timeNs||
           !f.poseEpoch||!f.sourceEpoch||(!f.hand[0].valid&&!f.hand[1].valid)){clear();return c;}
        c.enabled=true;
        const bool sameAction=same(owner,w);
        // Mortar queues its working child in phase2, then stores phase3 after
        // Queue returns. That exact child/token is one continuous work ledger.
        const bool mortarQueueToLive=sameAction&&w.child&&w.kind==Mortar&&owner.phase==2&&w.phase==3&&
            owner.working&&w.working&&!owner.ready&&!w.ready;
        if(!sameAction||(owner.phase!=w.phase&&!mortarQueueToLive)||owner.working!=w.working||owner.ready!=w.ready||
           poseEpoch!=f.poseEpoch||sourceEpoch!=f.sourceEpoch||
           (sequence&&f.sequence<sequence)||(timeNs&&f.timeNs<=timeNs&&sequence!=f.sequence)||
           (timeNs&&f.timeNs>timeNs&&f.timeNs-timeNs>80000000ull)){
            clear();owner=w;poseEpoch=f.poseEpoch;sourceEpoch=f.sourceEpoch;
        }else if(mortarQueueToLive)owner=w;
        c.parent=w.parent;c.child=w.child;c.token=w.token;
        const bool witnessed=w.instance==instance&&w.lifetime==lifetime&&w.parent&&w.tick&&
            (w.tick>f.tick?w.tick-f.tick:f.tick-w.tick)<=kFreshMs;
        if(!witnessed){clear();return c;}
        if(released&&w.ready&&!readyUsed){
            // If the admission press never queued work, READY remains live.
            // Rebase at its release, allowing the NEXT deliberate gesture.
            // An already emitted READY gesture keeps its consumed latch.
            selectedHand=-1;useUntil=0;
            for(unsigned side=0;side<2;++side){bool ok=f.hand[side].valid;
                for(unsigned n=0;n<3;++n){const float v=f.hand[side].position[n];ok=ok&&std::isfinite(v);last[side][n]=reference[side][n]=v;}
                have[side]=ok;haveUp[side]=false;}
        }
        if(!manualUse)manualWork=false;
        if(pressed&&w.working)manualWork=true;
        c.manualUse=manualWork;c.hand=forcedHand>=0?forcedHand:selectedHand;
        if(!readyAllowed)useUntil=0; // a mode change cannot replay the prior Semi READY pulse
        // Manual A takes the current working clip back to native playback.
        // Mode/button/owner changes remain effective even on the same XR frame;
        // retaining the cumulative ledger prevents either replay or rewind.
        if(manualUse)useUntil=0;
        if(sequence==f.sequence){
            c.totalSeconds=totalSeconds;c.clockStop=lastClockStop||manualWork||!workAllowed;
            c.use=!manualUse&&useUntil&&f.tick<useUntil;c.useUntil=useUntil;return c;
        }
        sequence=f.sequence;timeNs=f.timeNs;
        float amount[2]{};float downward[2]{},intent[2]{};bool physicalMotionD49[2]{};
        if(forcedHand>=0&&forcedHand<2)selectedHand=forcedHand;
        for(unsigned side=0;side<2;++side){
            const auto& h=f.hand[side];bool ok=h.valid;float p[3]{};
            if(w.kind==Resource){
                // A gentle POUR tilt drives this clip. Rotation about the
                // controller's own up axis (yaw) and translation add no pour.
                float n=0;for(unsigned j=0;j<4;++j){ok=ok&&std::isfinite(h.rotation[j]);n+=h.rotation[j]*h.rotation[j];}
                if(!ok||n<.8f||n>1.2f){haveUp[side]=false;have[side]=false;observedUpValidD49[side]=false;continue;}
                const float qn=1.f/std::sqrt(n),x=h.rotation[0]*qn,y=h.rotation[1]*qn,z=h.rotation[2]*qn,qw=h.rotation[3]*qn;
                const float up[3]{2.f*(x*y-qw*z),1.f-2.f*(x*x+z*z),2.f*(y*z+qw*x)};
                float motionSq=0;for(unsigned j=0;j<3;++j){const float v=up[j]-observedUpD49[side][j];motionSq+=v*v;observedUpD49[side][j]=up[j];}
                physicalMotionD49[side]=observedUpValidD49[side]&&motionSq>1.e-12f;
                observedUpValidD49[side]=true;
                if(!haveUp[side]){for(unsigned j=0;j<3;++j)lastUp[side][j]=referenceUp[side][j]=up[j];haveUp[side]=have[side]=true;continue;}
                float d=0,r=0;for(unsigned j=0;j<3;++j){d+=up[j]*lastUp[side][j];r+=up[j]*referenceUp[side][j];}
                const auto angle=[](float c){return std::acos(c>1.f?1.f:c<-1.f?-1.f:c);};
                const float tilt=angle(d);intent[side]=angle(r);
                if(tilt>.7f){physicalMotionD49[side]=false;for(unsigned j=0;j<3;++j)lastUp[side][j]=referenceUp[side][j]=up[j];continue;}
                if(tilt>=.004f){amount[side]=tilt;for(unsigned j=0;j<3;++j)lastUp[side][j]=up[j];}
                continue;
            }
            for(unsigned n=0;n<3;++n){p[n]=h.position[n];ok=ok&&std::isfinite(p[n]);}
            if(!ok){have[side]=false;observedValidD49[side]=false;continue;}
            float sampleDistanceSq=0;
            for(unsigned n=0;n<3;++n){const float v=p[n]-observedD49[side][n];
                if(w.kind!=Bellows||n==1)sampleDistanceSq+=v*v;observedD49[side][n]=p[n];}
            physicalMotionD49[side]=observedValidD49[side]&&sampleDistanceSq>1.e-12f;
            observedValidD49[side]=true;
            if(!have[side]){for(unsigned n=0;n<3;++n)last[side][n]=reference[side][n]=p[n];have[side]=true;continue;}
            float d[3]{};for(unsigned n=0;n<3;++n)d[n]=p[n]-last[side][n];
            // Reject a discontinuous sample; it cannot be a pump or a grind.
            const float distance=std::sqrt(d[0]*d[0]+d[1]*d[1]+d[2]*d[2]);
            if(distance>.10f){physicalMotionD49[side]=false;for(unsigned n=0;n<3;++n)last[side][n]=reference[side][n]=p[n];continue;}
            downward[side]=reference[side][1]-p[1];
            if(w.kind==Bellows)intent[side]=std::fabs(downward[side]);
            else{float dist=0;for(unsigned n=0;n<3;++n){const float off=p[n]-reference[side][n];dist+=off*off;}intent[side]=std::sqrt(dist);}
            amount[side]=w.kind==Bellows?std::fabs(d[1]):std::sqrt(d[0]*d[0]+d[1]*d[1]+d[2]*d[2]);
            // Position deadband accumulates very slow deliberate motion instead
            // of deleting every sub-millimetre frame. Once used, the reference
            // advances; a stationary hand contributes exactly zero new credit.
            if(amount[side]<.0008f)amount[side]=0.f;
            else for(unsigned n=0;n<3;++n)last[side][n]=p[n];
        }
        if(w.kind!=Bellows&&w.kind!=Mortar&&w.kind!=Resource&&w.kind!=Stir)return c;
        if(selectedHand<0){
            if(w.kind==Mortar||w.kind==Stir){if(intent[0]>=.003f)selectedHand=0;}
            else{const float threshold=w.kind==Resource?.02f:.003f;
                if(intent[0]>=threshold||intent[1]>=threshold)selectedHand=intent[1]>intent[0]?1:0;}
        }
        const unsigned side=selectedHand<0?0u:unsigned(selectedHand);
        if(w.working&&selectedHand>=0&&!f.hand[side].valid){c.enabled=false;clear();return c;}
        lastClockStop=true;
        if(w.working&&workAllowed&&!c.manualUse&&selectedHand>=0&&have[side]){
            // 16 cm/s gives normal native playback. Publish a cumulative
            // counter so XR90/native60 cannot lose intermediate samples. The
            // native boundary consumes each delta once and caps catch-up.
            double seconds=double(amount[side])/(w.kind==Resource?.4:.16);
            if(seconds>.12)seconds=.12;
            totalSeconds+=seconds;
            lastClockStop=!physicalMotionD49[side];
        }
        c.hand=selectedHand;c.totalSeconds=totalSeconds;c.clockStop=lastClockStop;
        if(w.ready&&readyAllowed&&!readyUsed&&!manualUse&&selectedHand>=0&&have[side]){
            float travel=0;for(unsigned n=0;n<3;++n){const float d=last[side][n]-reference[side][n];travel+=d*d;}
            const bool deliberate=w.kind==Bellows?downward[side]>=.025f:travel>=.000625f;
            if(deliberate){useUntil=f.tick+80;readyUsed=true;}
        }
        if(manualUse)useUntil=0;
        c.use=useUntil&&f.tick<useUntil;c.useUntil=useUntil;
        return c;
    }
};
inline Producer producer;
inline std::uint64_t publishedSequence=0,publishedTick=0;
template<class Frame> inline void publish(const Frame& frame,bool enabled,std::uintptr_t instance,std::uint64_t lifetime,
                                          bool manualUse=false,bool cancel=false,int forcedHand=-1,bool workAllowed=true,bool readyAllowed=true) noexcept {
    WorkWitness w{};(void)work.read(w);
    // Producer owns duplicate suppression. Calling it again retains original
    // XR stamps while allowing native action/phase, manual A and B changes.
    const auto c=producer.step(frame,enabled,instance,lifetime,w,manualUse,cancel,forcedHand,workAllowed,readyAllowed);
    publishedSequence=frame.sequence;publishedTick=frame.tick;(void)control.publish(c);
}
inline bool pulse(std::uint64_t now) noexcept {
    Control c{};WorkWitness w{};
    return (kcd2vr::minigame_hands45::modeFor(kcd2vr::minigame_hands45::Alchemy)!=kcd2vr::minigame_hands45::Native||guided_crafting55::active(now))&&
        hooksReady.load(std::memory_order_acquire)&&control.read(c)&&c.enabled&&!c.manualUse&&fresh(c.tick,now)&&c.use&&now<c.useUntil&&
        work.read(w)&&fresh(w.tick,now)&&w.ready&&c.instance==w.instance&&c.lifetime==w.lifetime&&c.parent==w.parent&&c.child==w.child&&c.token==w.token;
}
inline void apply(std::uint64_t now,bool& nativeA,float& nativeLT) noexcept {
    if(!pulse(now))return;
    WorkWitness w{};if(!work.read(w))return;
    // defaultProfile: alch_use_bag is xi_triggerl_btn; other Use is xi_a.
    if(w.kind==Bellows)nativeLT=1.f;else nativeA=true;
}
}
