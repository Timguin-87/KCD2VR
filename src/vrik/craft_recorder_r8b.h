#pragma once
// R8b crafting recorder ("teacher/student"), F5 rung 3 only. Pure: no native reads, no I/O. The host fills a Sample
// from what semi_workstations46 already holds (station table/offers, held tools, native selection/action, smith
// phase/axes, the VR hands) and writes the formatted lines. One START/STOP pair per station visit, one SAMPLE line
// every PeriodMs, bounded per visit (MaxSamples) and per F5 session (MaxSessionSamples); STOP carries the counters.
// Cost when off: the host calls nothing but stop() (one compare).
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstddef>
#include "station_body_h41.h" // R9C: why the 6DOF station body was refused

namespace kcd2vr::craft_recorder_r8b {
constexpr std::uint64_t PeriodMs=100;
constexpr unsigned MaxSamples=3000;          // 5 min of one visit at 10 Hz
constexpr unsigned MaxSessionSamples=12000;  // 20 min per F5 recording session
constexpr float GripOn=.55f,GripOff=.45f,TriggerOn=.5f,TriggerOff=.4f;

struct HandIn {
    bool valid=false;float pos[3]{},rot[4]{0,0,0,1};float grip=0,trigger=0;
    std::uint32_t held=0;int heldSlot=-1;   // native tool joined to this hand (bridge67::tools), 0 = none
    float targetM=-1;                         // distance palm -> the nominee's native offer point, -1 unknown
};
struct Sample {
    const char* kind="-";const char* mode="-";const char* stage="-"; // alchemy native action class / smith "-"
    std::uintptr_t instance=0;int smithPhase=-1;
    std::uint32_t nominee=0;int nomineeSlot=-1,nomineeSide=-1;float nomineeGap=-1;
    bool nomineePointValid=false;float nomineePoint[3]{};
    std::uint32_t selected=0;int selectedSlot=-1;bool admitted=false;int admittedSide=-1;bool nativeA=false;
    std::uintptr_t action=0;std::uintptr_t workParent=0;
    float axisX=0,axisY=0;bool axesOwned=false;
    bool headValid=false;float head[3]{};
    HandIn hand[2]{};
};
struct HandOut {float speed=0,tiltDeg=-1;bool gripDown=false,gripUp=false,triggerDown=false,triggerUp=false;};

// angle between the hand's local +Y and world +Y (Y up, the workstation sample frame). 0 = level, 90 = on its side,
// 180 = upside down: the pour/tilt signal.
inline float tiltDeg(const float q[4]) noexcept {
    const float x=q[0],y=q[1],z=q[2],w=q[3];
    const float upY=1.f-2.f*(x*x+z*z);
    const float c=upY>1.f?1.f:upY<-1.f?-1.f:upY;
    return float(std::acos(c)*57.29577951308232);
}

class Recorder {
public:
    bool active=false;std::uintptr_t instance=0;const char* kind="-";
    std::uint64_t startTick=0,lastSample=0,session=0;
    unsigned visit=0,samples=0,dropped=0,sessionSamples=0;
    unsigned gripDowns[2]{},triggerDowns[2]{},admissions=0,stageChanges=0;
    float peakSpeed[2]{},peakTilt[2]{};

    // new F5 session: counters reset
    void newSession(std::uint64_t id) noexcept {if(id!=session){session=id;sessionSamples=0;visit=0;}}
    // true -> a START line was written
    bool begin(const char* k,std::uintptr_t inst,const char* mode,std::uint64_t now,char* buf,std::size_t n) noexcept {
        if(active&&inst==instance)return false;
        active=true;instance=inst;kind=k?k:"-";startTick=now;lastSample=0;samples=dropped=0;admissions=stageChanges=0;
        for(int h=0;h<2;++h){gripDowns[h]=triggerDowns[h]=0;peakSpeed[h]=0;peakTilt[h]=0;prevValid[h]=false;prevGrip[h]=prevTrigger[h]=false;}
        prevAdmitted=false;prevStage=nullptr;++visit;
        if(buf&&n)std::snprintf(buf,n,
            "R8B CRAFT_REC START kind=%s visit=%u instance=%llX mode=%s periodMs=%llu budget=%u/%u "
            "sources=VR_hands,native_selection,native_action,held_tools,station_offers,%s "
            "henryClip=see_V66.90-PLAYBACK_CLIP(same_F5_session) animEvents=see_R59.16C10R11_ANIM_EVENT(tap_state_not_visible_here) "
            "animatedPalm=see_C70_F5_HELD_POSE smithDepthFlipInspect=NOT_READ minigameStage=%s",
            kind,visit,(unsigned long long)inst,mode?mode:"-",(unsigned long long)PeriodMs,MaxSamples,MaxSessionSamples,
            k&&k[0]=='s'?"smith_phase,smith_axes":"alchemy_stage(native_action_class)",k&&k[0]=='s'?"smithPhase":"alchemyStage");
        return true;
    }
    bool due(std::uint64_t now) noexcept {return active&&(!lastSample||now<lastSample||now-lastSample>=PeriodMs);}
    // true -> a SAMPLE line was written. derived values (speed/edges/tilt) in out
    bool sample(const Sample& s,std::uint64_t now,char* buf,std::size_t n,HandOut out[2]) noexcept {
        if(!active||s.instance!=instance||!due(now))return false;
        for(int h=0;h<2;++h){
            const auto& in=s.hand[h];auto& o=out[h];o={};
            if(in.valid){
                if(prevValid[h]&&now>prevTick[h]){
                    const float dx=in.pos[0]-prevPos[h][0],dy=in.pos[1]-prevPos[h][1],dz=in.pos[2]-prevPos[h][2];
                    o.speed=float(std::sqrt(dx*dx+dy*dy+dz*dz)/(double(now-prevTick[h])*.001));}
                o.tiltDeg=tiltDeg(in.rot);
                const bool g=prevGrip[h]?in.grip>GripOff:in.grip>=GripOn,t=prevTrigger[h]?in.trigger>TriggerOff:in.trigger>=TriggerOn;
                o.gripDown=g&&!prevGrip[h];o.gripUp=!g&&prevGrip[h];o.triggerDown=t&&!prevTrigger[h];o.triggerUp=!t&&prevTrigger[h];
                prevGrip[h]=g;prevTrigger[h]=t;gripDowns[h]+=o.gripDown;triggerDowns[h]+=o.triggerDown;
                for(int i=0;i<3;++i)prevPos[h][i]=in.pos[i];prevTick[h]=now;prevValid[h]=true;
                if(o.speed>peakSpeed[h])peakSpeed[h]=o.speed;if(o.tiltDeg>peakTilt[h])peakTilt[h]=o.tiltDeg;
            }else prevValid[h]=false;
        }
        if(s.admitted&&!prevAdmitted)++admissions;prevAdmitted=s.admitted;
        if(prevStage&&s.stage&&!same(prevStage,s.stage))++stageChanges;prevStage=s.stage;
        lastSample=now;
        if(samples>=MaxSamples||sessionSamples>=MaxSessionSamples){++dropped;return false;}
        ++samples;++sessionSamples;
        if(buf&&n){
            const auto& r=s.hand[0];const auto& l=s.hand[1];
            std::snprintf(buf,n,
                "R8B CRAFT_REC SAMPLE kind=%s visit=%u n=%u tMs=%llu mode=%s stage=%s smithPhase=%d "
                "nominee=%u/%d side=%d gap=%.3f nomineePoint=%s%.3f,%.3f,%.3f selected=%u/%d admitted=%d/%d nativeA=%d action=%llX work=%llX "
                "axes=%d,%.2f,%.2f head=%d,%.3f,%.3f,%.3f "
                "R=%d,%.3f,%.3f,%.3f q=%.3f,%.3f,%.3f,%.3f grip=%.2f trig=%.2f edges=%d%d%d%d v=%.2f tilt=%.0f held=%u/%d target=%.3f "
                "L=%d,%.3f,%.3f,%.3f q=%.3f,%.3f,%.3f,%.3f grip=%.2f trig=%.2f edges=%d%d%d%d v=%.2f tilt=%.0f held=%u/%d target=%.3f",
                kind,visit,samples,(unsigned long long)(now-startTick),s.mode?s.mode:"-",s.stage?s.stage:"-",s.smithPhase,
                s.nominee,s.nomineeSlot,s.nomineeSide,s.nomineeGap,s.nomineePointValid?"":"none:",
                s.nomineePoint[0],s.nomineePoint[1],s.nomineePoint[2],s.selected,s.selectedSlot,s.admitted?1:0,s.admittedSide,s.nativeA?1:0,
                (unsigned long long)s.action,(unsigned long long)s.workParent,s.axesOwned?1:0,s.axisX,s.axisY,
                s.headValid?1:0,s.head[0],s.head[1],s.head[2],
                r.valid?1:0,r.pos[0],r.pos[1],r.pos[2],r.rot[0],r.rot[1],r.rot[2],r.rot[3],r.grip,r.trigger,
                out[0].gripDown,out[0].gripUp,out[0].triggerDown,out[0].triggerUp,out[0].speed,out[0].tiltDeg,r.held,r.heldSlot,r.targetM,
                l.valid?1:0,l.pos[0],l.pos[1],l.pos[2],l.rot[0],l.rot[1],l.rot[2],l.rot[3],l.grip,l.trigger,
                out[1].gripDown,out[1].gripUp,out[1].triggerDown,out[1].triggerUp,out[1].speed,out[1].tiltDeg,l.held,l.heldSlot,l.targetM);
        }
        return true;
    }
    // true -> a STOP line was written (visit ended: left the station, recorder turned off, or session changed)
    bool stop(std::uint64_t now,const char* reason,char* buf,std::size_t n) noexcept {
        if(!active)return false;active=false;
        if(buf&&n)std::snprintf(buf,n,
            "R8B CRAFT_REC STOP kind=%s visit=%u instance=%llX reason=%s durationMs=%llu samples=%u dropped=%u sessionSamples=%u "
            "gripDowns=%u,%u triggerDowns=%u,%u admissions=%u stageChanges=%u peakSpeed=%.2f,%.2f peakTilt=%.0f,%.0f "
            "stationRefuse=%s stationRefusals=%llu stationFallback=%d fallbackWhy=%s",
            kind,visit,(unsigned long long)instance,reason?reason:"-",(unsigned long long)(now>=startTick?now-startTick:0),
            samples,dropped,sessionSamples,gripDowns[0],gripDowns[1],triggerDowns[0],triggerDowns[1],admissions,stageChanges,
            peakSpeed[0],peakSpeed[1],peakTilt[0],peakTilt[1],
            station_body_h41::refuseName(station_body_h41::Refuse(station_body_h41::lastRefuse.load(std::memory_order_relaxed))),
            (unsigned long long)station_body_h41::refusals.load(std::memory_order_relaxed),station_body_h41::fallbackNow.load(std::memory_order_relaxed)?1:0,
            station_body_h41::refuseName(station_body_h41::Refuse(station_body_h41::fallbackWhy.load(std::memory_order_relaxed))));
        instance=0;return true;
    }
private:
    static bool same(const char* a,const char* b) noexcept {if(a==b)return true;while(*a&&*a==*b){++a;++b;}return *a==*b;}
    bool prevValid[2]{},prevGrip[2]{},prevTrigger[2]{},prevAdmitted=false;float prevPos[2][3]{};std::uint64_t prevTick[2]{};
    const char* prevStage=nullptr;
};
}
