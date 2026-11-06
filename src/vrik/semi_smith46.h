#pragma once
#include "workstation_sample46.h"
#include "minigame_hands45.h"
#include "../core/snapshot_mailbox.h"
#include "../subsystems/smithing/stroke_receipt_d57.h"
#include "../subsystems/smithing/fall_timing_d81.h"
#include "../subsystems/crafting/options_d67.h"
#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdint>

namespace kcd2vr::semi_smith46 {
// C46: gestures feed the existing native axis and stroke actions. The native
// workpiece clamp, heat, quality and impact evaluator remain authoritative.
// The right hand supplies WORK to one exact native raise/finish animation;
// it does not teleport a hammer, forge a collision, or write stroke intensity.
constexpr std::uint64_t kAxisHoldMs=32,kPressAckMs=450,kFinishAckMs=450,kForgePulseMs=70;
constexpr float kAxisMetresPerSecond=.28f,kMotionSecondsPerMetre=4.5f;
constexpr float kLiftStartM=.03f,kDownReleaseM=.018f;
inline std::atomic<bool> clockReady{false},clockInterested{false};
struct ClockControl {
    std::uintptr_t instance=0;
    std::uint64_t lifetime=0,request=0,poseEpoch=0,sourceEpoch=0,sequence=0,tick=0,modeEpoch67=0;
    bool enabled=false;
    int requestedPhase=-1;
    // Cumulative authored seconds earned by new hand displacement. Consumption
    // is on Henry's existing native action-controller thread, never input time.
    double raiseWork=0,finishWork=0;
    // Reuse this existing snapshot for F5 metadata; native clock consumers
    // ignore it and continue to require enabled plus their original contract.
    subsystems::smithing::stroke_receipt_d57::Receipt diagnosticD57{};
    subsystems::smithing::fall_d81::Receipt fallD81{};
};
inline core::SnapshotMailbox<ClockControl> clockControl;
struct ClockStatus {
    std::uintptr_t instance=0;std::uint64_t lifetime=0,request=0,tick=0;
    int phase=-1;bool owned=false;
};
inline core::SnapshotMailbox<ClockStatus> clockStatus;
struct Context {
    bool enabled=false,nativeControls=false,clockAvailable=false;
    std::uintptr_t instance=0;
    std::uint64_t lifetime=0;
    int phase=-1;
    bool nativeHeight=false; // saved smith modes use native RT duration, not displacement-clock pacing
};
inline bool sameOwner(const Context& a,const Context& b) noexcept {
    return a.instance&&a.instance==b.instance&&a.lifetime==b.lifetime;
}
inline bool anvil(int p) noexcept {return p==11||p==14||p==16||p==17;}
// D83R1: the recorded native fall also passes through phase15 before idle.
// It preserves an existing stroke/recovery only; it cannot request RT or axes.
// The legacy displacement-clock path retains its original phase contract.
inline bool strokeCycleD83R1(int p) noexcept {return anvil(p)||p==15;}
inline bool fallPhaseD83R1(int p) noexcept {return p==15||p==16||p==17;}
inline bool inputContext(const Context& c) noexcept {
    return c.enabled&&c.nativeControls&&c.instance&&c.lifetime&&
        ((c.nativeHeight?strokeCycleD83R1(c.phase):anvil(c.phase))||c.phase==8);
}
inline bool finiteHead(const workstation_sample46::Frame& f) noexcept {
    for(float v:f.head)if(!std::isfinite(v))return false;
    double n=0;for(float v:f.headQ){if(!std::isfinite(v))return false;n+=double(v)*v;}
    return n>.90&&n<1.10;
}
enum class Stroke : unsigned {Idle,RaiseRequested,Raising,FinishRequested,Falling,NativeContinuation};
enum class Reason : unsigned {
    Inactive,Rebased,Tracking,ManualAxes,ManualStroke,ClockUnavailable,Idle,
    IronMotion,ForgeStep,RaiseRequest,Raise,Pause,FinishRequest,Fall,NativeComplete,
    NativeContinuation,PressNotAccepted,FinishNotAccepted,ClockNotOwned,Discontinuity,DiscontinuityHeldD67
};
inline const char* name(Reason r) noexcept {
    switch(r){
    case Reason::Inactive:return "INACTIVE";case Reason::Rebased:return "REBASED";
    case Reason::Tracking:return "TRACKING_UNAVAILABLE";case Reason::ManualAxes:return "MANUAL_AXES";
    case Reason::ManualStroke:return "MANUAL_STROKE";case Reason::ClockUnavailable:return "NATIVE_CLOCK_UNAVAILABLE";
    case Reason::Idle:return "READY";case Reason::IronMotion:return "IRON_MOTION";
    case Reason::ForgeStep:return "FORGE_STEP";case Reason::RaiseRequest:return "RAISE_REQUEST";
    case Reason::Raise:return "RAISE_WORK";case Reason::Pause:return "WORK_PAUSED";
    case Reason::FinishRequest:return "FINISH_REQUEST";case Reason::Fall:return "FINISH_WORK";
    case Reason::NativeComplete:return "NATIVE_PHASE_COMPLETED";case Reason::NativeContinuation:return "NATIVE_CONTINUATION";
    case Reason::PressNotAccepted:return "PRESS_NOT_ACKNOWLEDGED";case Reason::FinishNotAccepted:return "FINISH_NOT_ACKNOWLEDGED";case Reason::ClockNotOwned:return "NATIVE_CLOCK_NOT_OWNED";case Reason::Discontinuity:return "POSE_DISCONTINUITY";case Reason::DiscontinuityHeldD67:return "POSE_DISCONTINUITY_NATIVE_HOLD";
    }
    return "UNKNOWN";
}
struct Output {
    bool axesOwned=false,requestRT=false,strokeStarted=false,strokeReleased=false,forgeStep=false;
    float x=0,y=0;
    Reason reason=Reason::Inactive;
    ClockControl clock{};
};
// One instance on the mod input thread. Native phase changes never erase an
// accepted RT transaction: 11 -> 14 and 14 -> 16 are its acknowledgements.
class Controller {
    Context owner_{};
    std::uint64_t poseEpoch_=0,sourceEpoch_=0,sequence_=0,timeNs_=0,tick_=0,nextRequest_=0,modeEpoch67_=0;
    float previous_[2][3]{},right_[3]{1,0,0},forward_[3]{0,0,-1};
    bool haveFrame_=false,haveHand_[2]{},manualAxes_=false,manualStroke_=false;
    bool manualReleasedD85_=false; // D85 craft: RT let go since the manual stroke (any phase)
    float axisX_=0,axisY_=0,forgeOrigin_=0;
    bool forgeArmed_=true;
    int axisPhase_=-1,forgeDirection_=0;
    std::uint64_t forgeUntil_=0,forgeNext_=0,requestTick_=0;
    Stroke stroke_=Stroke::Idle;
    bool heightHeld_=false;
    // One physical raise/lower cycle: native completion alone cannot rearm a raised hand.
    bool returnToStartD85R1_=false;float strokeStartHeightD85R1_=0.f;
    std::uint64_t heightGoodTickD67_=0,heightGoodNsD67_=0,recoveryTickD67_=0;
    float recoveryLowD67_=0;bool recoveryHaveD67_=false,recoveryLiftD67_=false;
    enum class HeightCycle {None,Requested,Raised,Released,Fell};
    HeightCycle heightCycle_=HeightCycle::None;
    bool rightArmed_=false,wantFinish_=false,raiseOwned_=false,finishOwned_=false;
    std::uint64_t finishTick_=0,raiseAckTick_=0,finishAckTick_=0;
    unsigned quiet_=0;
    // After returning from the forge, uninterrupted hand motion may never
    // meet the old two-quiet-sample gate. A fresh idle down/up cycle is an
    // alternative neutral gesture; it earns no native charge or impact.
    float idlePeakD82_=0.f;bool idleMotionD82_=false;
    float low_=0,peak_=0,lastDownMps_=0;
    ClockControl command_{};
    subsystems::smithing::fall_d81::Tracker fallD81_;
    static float dot(const float p[3],const float a[3]) noexcept {return p[0]*a[0]+p[1]*a[1]+p[2]*a[2];}
    static float norm(const float d[3]) noexcept {return std::sqrt(dot(d,d));}
    static float axis(float metres,float dt) noexcept {
        if(std::fabs(metres)<.00035f||dt<=0)return 0.f;
        const float speed=metres/dt;if(std::fabs(speed)<.018f)return 0.f;
        return std::clamp(speed/kAxisMetresPerSecond,-1.f,1.f);
    }
    void retire(Reason reason,Output& out) noexcept {
        out.reason=reason;out.strokeReleased=heightHeld_||command_.request!=0;
        command_={};stroke_=out.strokeReleased?Stroke::NativeContinuation:Stroke::Idle;
        rightArmed_=false;quiet_=0;wantFinish_=false;heightHeld_=false;heightCycle_=HeightCycle::None;
        heightGoodTickD67_=heightGoodNsD67_=recoveryTickD67_=0;recoveryHaveD67_=recoveryLiftD67_=false;
        idleMotionD82_=false;returnToStartD85R1_=false;
    }
    void baseline(const workstation_sample46::Frame& f,std::uint64_t modeEpoch) noexcept {
        modeEpoch67_=modeEpoch;
        sequence_=f.sequence;timeNs_=f.timeNs;tick_=f.tick;haveFrame_=true;
        poseEpoch_=f.poseEpoch;sourceEpoch_=f.sourceEpoch;
        for(int side=0;side<2;++side){haveHand_[side]=f.hand[side].valid&&workstation_sample46::finite(f.hand[side]);
            for(int a=0;a<3;++a)previous_[side][a]=haveHand_[side]?f.hand[side].position[a]:0.f;}
        axisX_=axisY_=0;forgeUntil_=0;forgeDirection_=0;forgeArmed_=true;
        forgeOrigin_=dot(f.hand[1].position,forward_);
        low_=peak_=f.hand[0].position[1];rightArmed_=false;quiet_=0;heightHeld_=false;heightCycle_=HeightCycle::None;
        idleMotionD82_=false;returnToStartD85R1_=false;
    }
    bool latchPlane(const workstation_sample46::Frame& f) noexcept {
        if(!finiteHead(f))return false;
        const float qx=f.headQ[0],qy=f.headQ[1],qz=f.headQ[2],qw=f.headQ[3];
        float fx=-2.f*(qw*qy+qx*qz),fz=-1.f+2.f*(qx*qx+qy*qy);
        const float n=std::sqrt(fx*fx+fz*fz);if(!std::isfinite(n)||n<.25f)return false;
        forward_[0]=fx/n;forward_[1]=0;forward_[2]=fz/n;
        right_[0]=-forward_[2];right_[1]=0;right_[2]=forward_[0];return true;
    }
public:
    Stroke stroke() const noexcept {return stroke_;}
    float fastestDown() const noexcept {return lastDownMps_;}
    std::uint64_t request() const noexcept {return command_.request;}
    Output stepInput(const Context& context,const workstation_sample46::Frame& f,std::uint64_t now,
                float manualX,float manualY,float manualRT) noexcept {
        Output out{};
        const bool live=inputContext(context);
        const bool same=sameOwner(owner_,context);
        const auto modeEpoch=minigame_hands45::modeEpoch67.load(std::memory_order_acquire);
        const bool changed=!same||poseEpoch_!=f.poseEpoch||sourceEpoch_!=f.sourceEpoch||modeEpoch67_!=modeEpoch;
        const bool usable=live&&f.allowed&&workstation_sample46::fresh(f,now)&&finiteHead(f);
        if(!std::isfinite(manualX)||!std::isfinite(manualY)||!std::isfinite(manualRT)){
            retire(Reason::Tracking,out);haveFrame_=false;axisX_=axisY_=0;return out;}
        if(!live){retire(Reason::Inactive,out);owner_=context;haveFrame_=false;haveHand_[0]=haveHand_[1]=false;
            axisX_=axisY_=0;forgeUntil_=0;manualAxes_=manualStroke_=false;return out;}
        if(!usable){retire(Reason::Tracking,out);haveFrame_=false;haveHand_[0]=haveHand_[1]=false;
            axisX_=axisY_=0;forgeUntil_=0;owner_=context;return out;}
        if(changed||!haveFrame_){retire(Reason::Rebased,out);owner_=context;
            if(!latchPlane(f)){haveFrame_=false;out.reason=Reason::Tracking;return out;}
            baseline(f,modeEpoch);axisPhase_=context.phase;return out;}
        owner_=context;
        const bool newSample=f.sequence!=sequence_;
        float delta[2][3]{},dt=0;
        bool good[2]{f.hand[0].valid&&workstation_sample46::finite(f.hand[0]),
                     f.hand[1].valid&&workstation_sample46::finite(f.hand[1])};
        bool discontinuity=false,timingDiscontinuityD83R1=false;
        if(newSample){
            if(f.sequence<sequence_||f.timeNs<=timeNs_||f.timeNs-timeNs_>80000000ull||f.tick<tick_)
                timingDiscontinuityD83R1=discontinuity=true;
            else dt=float(double(f.timeNs-timeNs_)*1.e-9);
            if(dt<.001f||dt>.080f)timingDiscontinuityD83R1=discontinuity=true;
            for(int side=0;side<2;++side){
                if(!good[side]||!haveHand_[side]){good[side]=false;continue;}
                for(int a=0;a<3;++a)delta[side][a]=f.hand[side].position[a]-previous_[side][a];
                if(norm(delta[side])>.25f||(dt>0&&norm(delta[side])/dt>6.f)){
                    // Workpiece/left discontinuity cannot release an accepted
                    // right-hand hammer. Its own axis is rebased below.
                    if(side==1&&context.nativeHeight&&crafting_d67::smithRecovery.load(std::memory_order_acquire)){
                        good[side]=false;axisX_=axisY_=0;forgeUntil_=0;
                    }else discontinuity=true;
                }
            }
            sequence_=f.sequence;timeNs_=f.timeNs;tick_=f.tick;
            for(int side=0;side<2;++side){haveHand_[side]=f.hand[side].valid&&workstation_sample46::finite(f.hand[side]);
                for(int a=0;a<3;++a)previous_[side][a]=haveHand_[side]?f.hand[side].position[a]:0.f;}
        }
        if(discontinuity){
            const bool hold=context.nativeHeight&&crafting_d67::smithRecovery.load(std::memory_order_acquire)&&
                heightHeld_&&context.phase==14&&good[0]&&heightGoodTickD67_&&heightGoodNsD67_&&
                now>=heightGoodTickD67_&&now-heightGoodTickD67_<=80&&f.timeNs>=heightGoodNsD67_&&
                f.timeNs-heightGoodNsD67_<=80000000ull&&f.tick>=heightGoodTickD67_;
            if(hold){
                // Current tracking is still fresh. Rebase the rejected delta,
                // earn no work and preserve native RT for at most hand grace.
                // A release is a native strike, not a neutral cancellation.
                const auto goodTick=heightGoodTickD67_,goodNs=heightGoodNsD67_;
                baseline(f,modeEpoch);heightHeld_=true;heightCycle_=HeightCycle::Raised;
                heightGoodTickD67_=goodTick;heightGoodNsD67_=goodNs;
                recoveryHaveD67_=recoveryLiftD67_=false;recoveryTickD67_=0;
                out.requestRT=true;out.reason=Reason::DiscontinuityHeldD67;axisPhase_=context.phase;return out;
            }
            // INVESTIGATE_D67R1[SMITH_NATIVE_CANCEL]: D66 log line 1064815 at
            // 12:14:34.619 released RT on a guard; phase 16/.640 and StrokeHit/.889
            // followed. Native 0x8577FD selects StrokeFinish code 11, not a neutral
            // abort. Identify a proven phase-14 cancel/withdraw contract before
            // changing long-gap, invalid-pose or epoch-loss release semantics;
            // preserve native charge/exhaustion and never fabricate a hit.
            // A finite spatial jump during OUR already-released native fall
            // consumes no work, but need not discard that observed completion.
            // Clock regressions, long gaps, invalid tracking and ownership
            // changes still retire it. No recovery lift crosses this rebase.
            const bool keepFallD83R1=context.nativeHeight&&
                crafting_d67::smithRecovery.load(std::memory_order_acquire)&&
                !heightHeld_&&!command_.request&&fallPhaseD83R1(context.phase)&&
                (heightCycle_==HeightCycle::Released||heightCycle_==HeightCycle::Fell)&&
                good[0]&&!timingDiscontinuityD83R1&&heightGoodTickD67_&&heightGoodNsD67_&&
                now>=heightGoodTickD67_&&now-heightGoodTickD67_<=80&&
                f.timeNs>=heightGoodNsD67_&&f.timeNs-heightGoodNsD67_<=80000000ull;
            retire(Reason::Discontinuity,out);baseline(f,modeEpoch);axisPhase_=context.phase;
            if(keepFallD83R1)heightCycle_=HeightCycle::Fell;
            return out;
        }
        const bool heldTooLong=now<f.tick||now-f.tick>kAxisHoldMs;
        if(heldTooLong)haveHand_[0]=haveHand_[1]=false; // next live sample rebases, never bridges a dropout
        // A real stick owns the whole native Vec2 until neutral. Never sum it
        // with a hand rate; the neutral edge rebases without a deferred jump.
        if(std::max(std::fabs(manualX),std::fabs(manualY))>.20f)manualAxes_=true;
        if(manualAxes_){axisX_=axisY_=0;forgeUntil_=0;forgeOrigin_=dot(f.hand[1].position,forward_);
            if(std::max(std::fabs(manualX),std::fabs(manualY))<=.10f){manualAxes_=false;good[1]=false;}
            out.reason=Reason::ManualAxes;
        }else if(axisPhase_!=context.phase){axisX_=axisY_=0;axisPhase_=context.phase;
            forgeUntil_=0;forgeOrigin_=dot(f.hand[1].position,forward_);forgeArmed_=true;out.reason=Reason::Rebased;
        }else if(!good[1]||heldTooLong){axisX_=axisY_=0;forgeUntil_=0;forgeArmed_=true;
            forgeOrigin_=haveHand_[1]?dot(f.hand[1].position,forward_):0.f;}
        else if(anvil(context.phase)){
            out.axesOwned=true;
            if(newSample){axisX_=axis(dot(delta[1],right_),dt);axisY_=-axis(dot(delta[1],forward_),dt);}
            out.x=axisX_;out.y=axisY_;if(out.x||out.y)out.reason=Reason::IronMotion;
        }else if(context.phase==8){
            out.axesOwned=true;
            // Motion during the native repeat guard is consumed, not queued
            // for a delayed step after the hand has stopped.
            if(newSample&&now<forgeNext_)forgeOrigin_=dot(f.hand[1].position,forward_);
            const float travel=dot(f.hand[1].position,forward_)-forgeOrigin_;
            if(std::fabs(travel)<.02f)forgeArmed_=true;
            if(newSample&&forgeArmed_&&now>=forgeNext_&&std::fabs(travel)>=.06f){
                forgeDirection_=travel>0?1:-1;forgeUntil_=now+kForgePulseMs;forgeNext_=now+550;
                // Each deliberate travel segment requests one native step. Rebase
                // at the hand so further movement can request the next step;
                // a stationary hand never repeats, even after the guard expires.
                forgeOrigin_=dot(f.hand[1].position,forward_);
                forgeArmed_=true;out.forgeStep=true;out.reason=Reason::ForgeStep;
            }
            // Retail forge repeat is 0.5 s; this 70 ms pulse can request at most
            // one native index action. Native refusal/clamp remains possible.
            out.y=now<forgeUntil_?float(forgeDirection_):0.f;
        }
        // Manual trigger always keeps the native action path. Taking over an
        // already admitted gesture resumes native completion, never a fake abort.
        // D85 craft: phase 8 is the forge, where RT is the game's own forge
        // input (D80 21:54:34-58 and 21:55:38-56:01 were MANUAL_STROKE rows at
        // phase 8 only). It is not an anvil stroke and takes no hammer lock.
        if(manualRT>.20f&&context.phase!=8){manualStroke_=true;manualReleasedD85_=false;retire(Reason::ManualStroke,out);return out;}
        if(manualStroke_){
            // The release is latched at whatever phase it happens (native 14/16
            // can still be running) and the lock ends at the next native idle,
            // even with a finger resting on RT (.10..20). The next stroke still
            // arms by the existing D82 neutral rule (still hand or 18 mm lower):
            // the manual hit's own follow-through lift is never a stroke.
            if(manualRT<=.10f)manualReleasedD85_=true;
            if(!manualReleasedD85_||context.phase!=11){out.reason=Reason::ManualStroke;return out;}
            manualStroke_=manualReleasedD85_=false;
        }
        if(context.nativeHeight){
            // Only motion sampled in native anvil idle can arm the fresh
            // down/up gesture. Transfer, heating and native strokes cannot.
            if(context.phase!=11)idleMotionD82_=false;
            // Saved smith modes hold the native trigger while the hammer is raised.
            // Native time, full-charge/exhaustion, admission and impact run untouched.
            // The optional legacy work-clock path below is not admitted here.
            if(!good[0]||heldTooLong||!strokeCycleD83R1(context.phase)){
                retire(Reason::NativeContinuation,out);return out;
            }
            const float y=f.hand[0].position[1];
            if(newSample&&!heightHeld_&&returnToStartD85R1_&&y<=strokeStartHeightD85R1_+.01f)
                returnToStartD85R1_=false;
            if(newSample){heightGoodTickD67_=f.tick;heightGoodNsD67_=f.timeNs;}
            if(context.phase!=11&&context.phase!=14){
                if(heightCycle_==HeightCycle::Raised||heightCycle_==HeightCycle::Released)heightCycle_=HeightCycle::Fell;
                else if(heightCycle_!=HeightCycle::Fell)heightCycle_=HeightCycle::None;
                out.strokeReleased=heightHeld_;heightHeld_=false;rightArmed_=false;quiet_=0;
                if(!returnToStartD85R1_&&crafting_d67::smithRecovery.load(std::memory_order_acquire)&&heightCycle_==HeightCycle::Fell&&newSample){
                    if(!recoveryHaveD67_){recoveryHaveD67_=true;recoveryLowD67_=y;}
                    recoveryLowD67_=std::min(recoveryLowD67_,y);
                    if(delta[0][1]>.00035f){recoveryTickD67_=f.tick;if(y-recoveryLowD67_>=kLiftStartM)recoveryLiftD67_=true;}
                    if(delta[0][1]<-.00035f)recoveryLiftD67_=false;
                }
                out.reason=Reason::NativeContinuation;return out;
            }
            if(heightHeld_){
                // D62: an acknowledged native raise that returned directly to
                // idle was cancelled/refused, not a new trigger admission. An
                // unacknowledged press also expires instead of remaining high
                // indefinitely and firing later when the native game is ready.
                // A real phase14 hold retains native charge/exhaustion timing.
                if(context.phase==11&&heightCycle_==HeightCycle::Raised){
                    retire(Reason::NativeContinuation,out);return out;
                }
                if(context.phase==11&&heightCycle_==HeightCycle::Requested&&
                   now>=requestTick_&&now-requestTick_>kPressAckMs){
                    retire(Reason::PressNotAccepted,out);return out;
                }
                if(context.phase==14)heightCycle_=HeightCycle::Raised;
                // D56: release the native trigger at a deliberate downward
                // reversal, while the player's hammer is still falling. Waiting
                // for the low/impact height starts the native fall too late.
                if(newSample)peak_=std::max(peak_,y);
                if(newSample&&delta[0][1]<-.00035f&&peak_-y>=kDownReleaseM){
                    heightHeld_=false;rightArmed_=false;quiet_=0;out.strokeReleased=true;
                    heightCycle_=heightCycle_==HeightCycle::Raised?HeightCycle::Released:HeightCycle::None;
                    out.reason=Reason::FinishRequest;
                }else{out.requestRT=true;out.reason=Reason::Raise;}
                return out;
            }
            // A native/manual raise is not adopted halfway through its transaction.
            if(context.phase!=11){
                if(heightCycle_==HeightCycle::Fell)heightCycle_=HeightCycle::None;
                out.reason=Reason::NativeContinuation;return out;
            }
            // After our observed native raise/fall, a new lift may begin without
            // two stationary samples. D67 permits only a recent, unreversed lift
            // from that owned recovery; all other busy motion rebases at idle.
            if(returnToStartD85R1_){
                recoveryHaveD67_=recoveryLiftD67_=false;recoveryTickD67_=0;
                rightArmed_=false;quiet_=0;idleMotionD82_=false;
                out.reason=Reason::Idle;return out;
            }
            if(heightCycle_==HeightCycle::Fell){
                const bool pending=crafting_d67::smithRecovery.load(std::memory_order_acquire)&&
                    recoveryHaveD67_&&recoveryLiftD67_&&recoveryTickD67_&&now>=recoveryTickD67_&&now-recoveryTickD67_<=80&&
                    y-recoveryLowD67_>=kLiftStartM;
                // D67R1: native phase can reach idle before tracking publishes
                // another packet. A duplicate cannot consume OR erase the lift.
                // Existing freshness/lifetime/epoch guards above still retire it;
                // the next fresh packet must pass expiry and reversal checks.
                if(pending&&!newSample){out.reason=Reason::Idle;return out;}
                const bool queued=pending&&newSample&&delta[0][1]>=-.00035f;
                heightCycle_=HeightCycle::None;rightArmed_=true;quiet_=0;low_=peak_=y;
                recoveryHaveD67_=recoveryLiftD67_=false;recoveryTickD67_=0;
                if(queued){heightHeld_=true;heightCycle_=HeightCycle::Requested;requestTick_=now;
                    strokeStartHeightD85R1_=y;returnToStartD85R1_=true;
                    out.requestRT=true;out.strokeStarted=true;rightArmed_=false;out.reason=Reason::RaiseRequest;
                }else out.reason=Reason::Idle;
                return out;
            }
            heightCycle_=HeightCycle::None;
            if(newSample){
                if(!rightArmed_){
                    if(!idleMotionD82_){idleMotionD82_=true;idlePeakD82_=y;}
                    else idlePeakD82_=std::max(idlePeakD82_,y);
                    if(norm(delta[0])<=.006f)++quiet_;else quiet_=0;
                    // Do not require the player to stop between heating and
                    // hammering. A measured 18 mm lowering only arms us; the
                    // existing separate 30 mm lift must still request RT, and
                    // the native phase14 acknowledgement must precede a fall.
                    const bool lowered=delta[0][1]<-.00035f&&idlePeakD82_-y>=kDownReleaseM;
                    if(quiet_>=2||lowered){rightArmed_=true;low_=peak_=y;idleMotionD82_=false;}
                }else{
                    low_=std::min(low_,y);
                    if(y-low_>=kLiftStartM&&delta[0][1]>.00035f){
                        heightHeld_=true;heightCycle_=HeightCycle::Requested;requestTick_=now;peak_=y;strokeStartHeightD85R1_=y;returnToStartD85R1_=true;
                    out.requestRT=true;out.strokeStarted=true;
                        out.reason=Reason::RaiseRequest;rightArmed_=false;
                    }
                }
            }
            if(!out.requestRT)out.reason=Reason::Idle;
            return out;
        }
        if(!context.clockAvailable){retire(Reason::ClockUnavailable,out);return out;}
        if(!good[0]||heldTooLong){retire(Reason::NativeContinuation,out);return out;}
        if(!anvil(context.phase)){retire(Reason::Inactive,out);return out;}
        if(command_.request&&(context.phase==11||context.phase==17)&&
           (stroke_==Stroke::Falling||stroke_==Stroke::FinishRequested)){
            retire(Reason::NativeComplete,out);low_=peak_=f.hand[0].position[1];return out;
        }
        if(command_.request&&(stroke_==Stroke::RaiseRequested||stroke_==Stroke::Raising)){
            if((context.phase!=11&&context.phase!=14)||(stroke_==Stroke::Raising&&context.phase==11)){
                retire(Reason::NativeContinuation,out);return out;}
        }
        if(command_.request&&stroke_==Stroke::Falling&&context.phase==14){retire(Reason::NativeContinuation,out);return out;}
        if(command_.request&&stroke_==Stroke::FinishRequested&&context.phase==14&&finishTick_&&now>=finishTick_&&now-finishTick_>kFinishAckMs){
            retire(Reason::FinishNotAccepted,out);return out;}
        if(stroke_==Stroke::NativeContinuation){
            if(context.phase!=11){out.reason=Reason::NativeContinuation;return out;}
            stroke_=Stroke::Idle;low_=peak_=f.hand[0].position[1];quiet_=0;rightArmed_=false;
        }
        const float y=f.hand[0].position[1],dy=newSample?delta[0][1]:0.f;
        if(stroke_==Stroke::Idle){
            if(context.phase!=11){out.reason=Reason::NativeContinuation;return out;}
            if(newSample){
                if(!rightArmed_){if(norm(delta[0])<=.006f){if(++quiet_>=2){rightArmed_=true;low_=peak_=y;}}
                    else quiet_=0;}
                else {
                    low_=std::min(low_,y);peak_=std::max(peak_,y);
                    if(y-low_>=kLiftStartM&&dy>.00035f){
                        command_={};command_.instance=context.instance;command_.lifetime=context.lifetime;
                        command_.request=++nextRequest_;if(!command_.request)command_.request=++nextRequest_;
                        command_.raiseWork=double(y-low_)*kMotionSecondsPerMetre;
                        command_.requestedPhase=14;stroke_=Stroke::RaiseRequested;requestTick_=now;wantFinish_=false;
                        out.strokeStarted=true;out.reason=Reason::RaiseRequest;rightArmed_=false;
                        raiseOwned_=finishOwned_=false;finishTick_=raiseAckTick_=finishAckTick_=0;lastDownMps_=0;
                    }
                }
            }
            if(stroke_==Stroke::Idle){if(out.reason==Reason::Inactive)out.reason=Reason::Idle;return out;}
        }else if(newSample){
            peak_=std::max(peak_,y);
            if(stroke_==Stroke::RaiseRequested||stroke_==Stroke::Raising){
                if(dy>.00035f)command_.raiseWork=std::min(4.0,command_.raiseWork+double(dy)*kMotionSecondsPerMetre);
                if(peak_-y>=kDownReleaseM&&dy<-.00035f)wantFinish_=true;
                if(wantFinish_&&dy<-.00035f)command_.finishWork=std::min(4.0,command_.finishWork-double(dy)*kMotionSecondsPerMetre);
            }else if((stroke_==Stroke::FinishRequested||stroke_==Stroke::Falling)&&dy<-.00035f){
                command_.finishWork=std::min(4.0,command_.finishWork-double(dy)*kMotionSecondsPerMetre);
            }
            if(dy<0&&dt>0)lastDownMps_=std::max(lastDownMps_,-dy/dt);
        }
        if(stroke_==Stroke::RaiseRequested&&context.phase==14){stroke_=Stroke::Raising;raiseAckTick_=now;}
        if(stroke_==Stroke::FinishRequested&&context.phase==16&&!finishAckTick_)finishAckTick_=now;
        if(stroke_==Stroke::RaiseRequested&&now>=requestTick_&&now-requestTick_>kPressAckMs){
            retire(Reason::PressNotAccepted,out);return out;}
        // Phase acknowledgement proves native input acceptance; the independent
        // queue lease acknowledgement proves work-clock ownership. A refused
        // lease resumes natively and requires a fresh neutral gesture.
        if((context.phase==14&&command_.requestedPhase==14)||context.phase==16){
            ClockStatus status{};const bool got=clockStatus.read(status)&&status.instance==context.instance&&
                status.lifetime==context.lifetime&&status.request==command_.request&&status.phase==context.phase;
            const bool owns=got&&status.owned&&status.tick&&now>=status.tick&&now-status.tick<=80;
            bool& was=context.phase==14?raiseOwned_:finishOwned_;
            const auto since=context.phase==14?raiseAckTick_:finishAckTick_;
            if((was&&!owns)||(!owns&&since&&now>=since&&now-since>150)){
                retire(Reason::ClockNotOwned,out);return out;}
            was=was||owns;
        }
        if(stroke_==Stroke::Raising&&wantFinish_&&raiseOwned_){stroke_=Stroke::FinishRequested;command_.requestedPhase=16;
            out.strokeReleased=true;out.reason=Reason::FinishRequest;finishTick_=now;}
        if(stroke_==Stroke::FinishRequested&&context.phase==16)stroke_=Stroke::Falling;
        if(stroke_==Stroke::RaiseRequested||stroke_==Stroke::Raising){out.requestRT=true;
            if(!out.strokeStarted)out.reason=dy>.00035f?Reason::Raise:Reason::Pause;}
        else if(stroke_==Stroke::FinishRequested||stroke_==Stroke::Falling){
            if(!out.strokeReleased)out.reason=dy<-.00035f?Reason::Fall:Reason::Pause;}
        command_.enabled=true;command_.poseEpoch=f.poseEpoch;command_.sourceEpoch=f.sourceEpoch;command_.modeEpoch67=modeEpoch67_;
        command_.sequence=f.sequence;command_.tick=f.tick;out.clock=command_;
        return out;
    }
    Output step(const Context& c,const workstation_sample46::Frame& f,std::uint64_t now,
                float manualX,float manualY,float manualRT) noexcept {
        auto out=stepInput(c,f,now,manualX,manualY,manualRT);
        subsystems::smithing::fall_d81::Input in{};
        auto& r=in.identity;r.instance=c.instance;r.lifetime=c.lifetime;
        r.poseEpoch=f.poseEpoch;r.sourceEpoch=f.sourceEpoch;r.modeEpoch=minigame_hands45::modeEpoch67.load(std::memory_order_acquire);
        r.sequence=f.sequence;r.tick=f.tick;r.phase=c.phase;
        in.current=c.nativeHeight&&inputContext(c)&&f.allowed&&workstation_sample46::fresh(f,now)&&finiteHead(f)&&
            f.hand[0].valid&&workstation_sample46::finite(f.hand[0])&&std::isfinite(manualRT)&&manualRT<=.20f;
        in.start=out.strokeStarted&&out.reason==Reason::RaiseRequest;
        in.held=out.requestRT;in.release=out.strokeReleased&&out.reason==Reason::FinishRequest;
        in.abort=out.reason==Reason::Tracking||out.reason==Reason::Rebased||out.reason==Reason::Inactive||
            out.reason==Reason::Discontinuity||out.reason==Reason::ManualStroke||out.reason==Reason::PressNotAccepted||
            out.reason==Reason::FinishNotAccepted||out.reason==Reason::ClockNotOwned;
        out.clock.fallD81=fallD81_.step(in,now);return out;
    }
};
// Uses the already-built pad. The caller gates this by native semantic activity
// and chooses this OR the old height pulse, so a stroke has one producer.
template<class Pad> inline void apply(const Output& out,Pad& pad) noexcept {
    if(out.axesOwned){pad.lx=out.x;pad.ly=out.y;}
    if(out.requestRT)pad.rt=std::max(pad.rt,1.f);
}
}
