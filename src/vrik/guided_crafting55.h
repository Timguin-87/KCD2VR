#pragma once
// C55 F5 rung 3: native selection/attachments/effects stay authoritative.
// This first guided test supplies touch admission and clutched native work.
// It does not move effect emitters or claim free 3D tool/destination ownership.
#include "../core/snapshot_mailbox.h"
#include "workstation_sample46.h"
#include "alchemy_input19c4.h"
#include "../interaction_alignment.h"
#include "minigame_hands45.h"
#include <atomic>
#include <cmath>
#include <cstdint>
namespace kcd2vr::guided_crafting55 {
inline std::atomic<bool> enabled{false};
inline bool active(std::uint64_t now) noexcept {
    const auto w=minigame_hands45::witness.load(std::memory_order_acquire);
    const auto kind=minigame_hands45::kindOf(w);const auto t=minigame_hands45::tickOf(w);
    return enabled.load(std::memory_order_acquire)&&(kind==minigame_hands45::Alchemy||kind==minigame_hands45::Smithing)&&
        t&&now>=t&&now-t<=80;
}
struct Dock {
    std::uintptr_t instance=0,character=0,attachment=0,object=0;
    std::uint64_t lifetime=0,tick=0,sequence=0,poseEpoch=0,sourceEpoch=0;
    std::uint32_t entity=0;int side=-1,type=-1,kind=0,phase=-1;float gap=0;
};
inline core::SnapshotMailbox<Dock> dock[2];
inline bool joined(const Dock& d,const workstation_sample46::Frame& f,std::uintptr_t instance,
                   std::uint64_t lifetime,int side,int kind,std::uint64_t now,float limit=.12f) noexcept {
    return d.instance==instance&&d.lifetime==lifetime&&d.character&&d.attachment&&d.object&&d.entity&&
        d.side==side&&d.kind==kind&&d.poseEpoch==f.poseEpoch&&d.sourceEpoch==f.sourceEpoch&&
        d.tick&&now>=d.tick&&now-d.tick<=80&&d.sequence&&d.sequence<=f.sequence&&
        std::isfinite(d.gap)&&d.gap>=0&&d.gap<=limit&&f.hand[side].valid&&workstation_sample46::finite(f.hand[side]);
}
enum class Status { Off,NeutralRequired,TouchRequired,NativeActionRequested,WaitingNativeGrip,GuidedWork,
    NativeButtonPassenger,GripReleased,TrackingLost,SmithReady,SmithFlipRequested,SmithFlipWaiting,GazeRequired };
inline const char* name(Status s) noexcept {
    switch(s){case Status::Off:return "OFF";case Status::NeutralRequired:return "OPEN_GRIP_TO_ARM";
    case Status::TouchRequired:return "TOUCH_CURRENT_NATIVE_USE_POINT";case Status::NativeActionRequested:return "NATIVE_ACTION_REQUESTED";
    case Status::GazeRequired:return "LOOK_AT_CURRENT_NATIVE_USE_POINT";case Status::WaitingNativeGrip:return "WAITING_REAL_NATIVE_GRIP";case Status::GuidedWork:return "GUIDED_NATIVE_WORK";
    case Status::NativeButtonPassenger:return "NATIVE_BUTTON_PASSENGER";case Status::GripReleased:return "GRIP_RELEASE_NATIVE_CONTINUATION";
    case Status::TrackingLost:return "TRACKING_REVOKED";case Status::SmithReady:return "SMITH_READY";
    case Status::SmithFlipRequested:return "NATIVE_ANVIL_FLIP_REQUESTED";case Status::SmithFlipWaiting:return "WAITING_NATIVE_FLIP_OR_NEUTRAL";}
    return "UNKNOWN";
}
struct AlchemyOutput {bool admitted=false,use=false,work=false,clockEligible=false;int side=-1;Status status=Status::Off;};
struct Alchemy {
    std::uintptr_t instance=0;std::uint64_t lifetime=0,poseEpoch=0,sourceEpoch=0,pulseUntil=0,manualRequestedAt=0;
    bool previous[2]{true,true},holdHint=false,nativePassenger=false,sawNativeParent=false;
    std::uint32_t entity=0,type=21;int side=-1;std::uintptr_t nativeParent=0;
    void reset(const workstation_sample46::Frame& f) noexcept {
        *this=Alchemy{};for(int i=0;i<2;++i)previous[i]=!(f.hand[i].valid&&workstation_sample46::finite(f.hand[i])&&f.hand[i].grip<.35f);
    }
    AlchemyOutput step(bool on,std::uintptr_t next,std::uint64_t life,const workstation_sample46::Frame& f,
        std::uint64_t now,const interaction_alignment::HandCandidate& candidate,
        bool nativeA,bool nativeB,std::uintptr_t parent,bool gazeSelection63=false) noexcept {
        AlchemyOutput out{};
        if(!on||!next||!life){reset(f);return out;}
        const bool usable=f.allowed&&workstation_sample46::fresh(f,now);
        if(!usable){reset(f);out.status=Status::TrackingLost;return out;}
        if(instance!=next||lifetime!=life||poseEpoch!=f.poseEpoch||sourceEpoch!=f.sourceEpoch){
            reset(f);instance=next;lifetime=life;poseEpoch=f.poseEpoch;sourceEpoch=f.sourceEpoch;
            out.status=Status::NeutralRequired;return out;}
        if(nativeA||nativeB){if(!nativePassenger)manualRequestedAt=now;nativePassenger=true;side=-1;entity=0;pulseUntil=0;nativeParent=parent;sawNativeParent=parent!=0;}
        if(nativePassenger){
            if(parent){sawNativeParent=true;nativeParent=parent;}
            if(!sawNativeParent&&!nativeA&&!nativeB&&manualRequestedAt&&now>=manualRequestedAt&&now-manualRequestedAt>=1500&&
               f.hand[0].valid&&f.hand[1].valid&&workstation_sample46::finite(f.hand[0])&&workstation_sample46::finite(f.hand[1])&&
               f.hand[0].grip<.35f&&f.hand[1].grip<.35f){
                nativePassenger=false;previous[0]=previous[1]=false;out.status=Status::NeutralRequired;return out;}
            if(sawNativeParent&&!parent&&!nativeA&&!nativeB){nativePassenger=false;for(int i=0;i<2;++i)previous[i]=!(f.hand[i].valid&&workstation_sample46::finite(f.hand[i])&&f.hand[i].grip<.35f);}
            out.status=Status::NativeButtonPassenger;return out;
        }
        if(side>=0){
            if(parent){sawNativeParent=true;nativeParent=parent;}
            else if(sawNativeParent){side=-1;entity=0;pulseUntil=0;sawNativeParent=false;
                out.status=Status::GripReleased;return out;}
            if(!f.hand[side].valid||!workstation_sample46::finite(f.hand[side])){side=-1;entity=0;pulseUntil=0;out.status=Status::TrackingLost;return out;}
            if(f.hand[side].grip<.35f){previous[side]=false;side=-1;entity=0;pulseUntil=0;out.status=Status::GripReleased;return out;}
            // One squeeze is one admission. No retry through target/action changes.
            out.admitted=true;out.side=side;out.use=holdHint||(pulseUntil&&now<pulseUntil);
            out.clockEligible=(type>=13&&type<=16)||type==2||type==3;
            Dock d{};out.work=dock[side].read(d)&&joined(d,f,instance,lifetime,side,1,now)&&
                ((d.entity==entity&&d.type==int(type))||((type==2||type==3)&&side==0&&d.type==3));
            out.status=out.work?Status::GuidedWork:Status::WaitingNativeGrip;return out;
        }
        out.status=gazeSelection63?Status::GazeRequired:Status::TouchRequired;
        for(int i=0;i<2;++i){
            const auto& h=f.hand[i];const bool held=h.valid&&workstation_sample46::finite(h)&&h.grip>=.55f;
            if(!held){if(h.valid&&workstation_sample46::finite(h)&&h.grip<.35f)previous[i]=false;continue;}
            if(previous[i])continue;previous[i]=true;
            alchemy_input19c4::Observation selected{};
            if(!alchemy_input19c4::lookupSelected(instance,lifetime,now,selected))continue;
            if(!gazeSelection63&&(candidate.owner!=i||!candidate.object||
               candidate.entity!=selected.entity||!candidate.tick||now<candidate.tick||now-candidate.tick>80||
               !candidate.sourceTick||now<candidate.sourceTick||now-candidate.sourceTick>80||
               candidate.sourceEpoch!=f.poseEpoch||!candidate.sourcePlayer||!candidate.sourceGeneration||
               !std::isfinite(candidate.distance)||candidate.distance<0||candidate.distance>.08f))continue;
            // Retail C_UseResourceAction attaches the selected carafe LEFT.
            if(selected.type>=13&&selected.type<=16&&i!=1)continue;
            if((selected.type==2||selected.type==3)&&i!=0)continue;
            side=i;entity=selected.entity;type=selected.type;holdHint=selected.hold;pulseUntil=holdHint?0:now+80;
            out={true,true,false,(type>=13&&type<=16)||type==2||type==3,side,Status::NativeActionRequested};break;
        }
        return out;
    }
};
struct SmithOutput {bool left=false,right=false,flip=false;Status status=Status::Off;};
struct Smith {
    std::uintptr_t instance=0;std::uint64_t lifetime=0,poseEpoch=0,sourceEpoch=0,dwell=0,sequence=0,flipUntil=0,flipReleaseTick=0,flipReleaseSequence=0;
    bool clutched[2]{},haveRoll=false,flipConsumed=false,flipAcknowledged=false;float neutral[4]{0,0,0,1};
    void reset() noexcept {*this=Smith{};}
    SmithOutput step(bool on,std::uintptr_t next,std::uint64_t life,int phase,const workstation_sample46::Frame& f,
        std::uint64_t now,bool manualA,bool manualB,bool manualX,const bool* nativeHeldToolsH12=nullptr) noexcept {
        SmithOutput out{};
        if(!on||!next||!life){reset();return out;}
        if(!f.allowed||!workstation_sample46::fresh(f,now)){reset();out.status=Status::TrackingLost;return out;}
        if(instance!=next||lifetime!=life||poseEpoch!=f.poseEpoch||sourceEpoch!=f.sourceEpoch){
            reset();instance=next;lifetime=life;poseEpoch=f.poseEpoch;sourceEpoch=f.sourceEpoch;
        }
        if(manualA||manualB||manualX){haveRoll=false;dwell=flipReleaseTick=flipReleaseSequence=0;out.status=Status::NativeButtonPassenger;return out;}
        out.status=Status::SmithReady;
        for(int i=0;i<2;++i){Dock d{};
            // Gesture mode already owns the actual native-attached tool with
            // the tracked hand. Its caller supplies CURRENT validated tool
            // membership, not a fabricated near-Dock or F5-only publication.
            // The guided native-path mode retains its original gap witness.
            const bool ok=nativeHeldToolsH12?
                nativeHeldToolsH12[i]&&f.hand[i].valid&&workstation_sample46::finite(f.hand[i]):
                dock[i].read(d)&&joined(d,f,instance,lifetime,i,2,now,clutched[i]?.16f:.08f)&&
                    d.phase==phase&&(i?d.type==1||d.type==2:d.type==0);
            clutched[i]=ok&&f.hand[i].grip>=(clutched[i]?.35f:.55f);
        }
        out.left=clutched[1];out.right=clutched[0];
        if(phase==12||phase==13)flipAcknowledged=true;
        const auto* q=f.hand[1].rotation;
        // Signed twist about the controller's local forward. Large pitch/yaw
        // is not an anvil flip and cannot also become forge inspection.
        const float x=neutral[3]*q[0]-neutral[0]*q[3]-neutral[1]*q[2]+neutral[2]*q[1];
        const float y=neutral[3]*q[1]+neutral[0]*q[2]-neutral[1]*q[3]-neutral[2]*q[0];
        const float z=neutral[3]*q[2]-neutral[0]*q[1]+neutral[1]*q[0]-neutral[2]*q[3];
        const float w=neutral[3]*q[3]+neutral[0]*q[0]+neutral[1]*q[1]+neutral[2]*q[2];
        // q and -q are one physical orientation. Never turn a sign flip into 360 degrees.
        const float roll=std::fabs(2.f*std::atan2(w<0?-z:z,std::fabs(w)));
        // An unacknowledged native X is still one request, never a held-button retry.
        // After its pulse ends, a valid open/neutral sample plus a later fresh
        // joined clutch may retire ONLY the unacknowledged request. Keep the
        // old neutral through failed re-clutches; tracking/dock loss is not Grip0.
        const bool neutralNow=f.hand[1].valid&&workstation_sample46::finite(f.hand[1])&&x*x+y*y<=.08f&&roll<.25f;
        if(!out.left){haveRoll=false;dwell=0;
            if(flipConsumed&&!flipAcknowledged){
                if(phase!=11||!neutralNow||f.hand[1].grip>=.35f||now<flipUntil||f.tick<flipUntil||f.sequence<=sequence)
                    flipReleaseTick=flipReleaseSequence=0;
                else if(!flipReleaseTick){flipReleaseTick=f.tick;flipReleaseSequence=f.sequence;}
            }
            return out;
        }
        if(flipReleaseTick){
            const bool retry=flipConsumed&&!flipAcknowledged&&phase==11&&neutralNow&&now>=flipReleaseTick&&
                now-flipReleaseTick<=1000&&f.tick>flipReleaseTick&&f.sequence>flipReleaseSequence;
            flipReleaseTick=flipReleaseSequence=0;
            if(retry){flipConsumed=false;haveRoll=false;dwell=0;sequence=f.sequence;}
        }
        if(!haveRoll){
            if(flipConsumed&&!flipAcknowledged){out.status=Status::SmithFlipWaiting;return out;}
            for(int i=0;i<4;++i)neutral[i]=q[i];haveRoll=true;return out;
        }
        if(flipConsumed){out.status=Status::SmithFlipWaiting;out.flip=!flipAcknowledged&&now<flipUntil;
            if(flipAcknowledged&&phase==11&&roll<.25f){flipConsumed=flipAcknowledged=false;dwell=0;}
            return out;}
        if(phase!=11||x*x+y*y>.08f||roll<.65f){dwell=0;return out;}
        if(sequence==f.sequence)return out;sequence=f.sequence;
        if(!dwell)dwell=now;
        if(now>=dwell&&now-dwell>=100){out.flip=true;flipConsumed=true;flipAcknowledged=false;flipUntil=now+70;dwell=0;out.status=Status::SmithFlipRequested;}
        return out;
    }
};
}
