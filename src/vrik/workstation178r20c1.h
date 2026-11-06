#pragma once
#include "combat_rules178r10.h"
#include "hands_mode59c7f.h"
#include "minigame_hands45.h"
#include "guided_crafting55.h"
#include "../subsystems/alchemy/bridge67.h"
#include "../activity_input163.h"
#include "../activity_eye59c19.h"
#include "../diagnostics/capture168.h"
#include <array>
#include <cmath>

namespace kcd2vr::workstation178r20c1 {
// Requested rung and admitted ownership are distinct. A blocked rung 2 still
// advances to OFF on the next press; later readiness never silently enables it.
inline std::atomic<bool> experimentArmed36{false};
inline std::atomic<std::uint64_t> experimentEpoch36{0};
inline void setExperiment36(bool on) noexcept {
    experimentArmed36.store(on,std::memory_order_release);
    guided_crafting55::enabled.store(on&&combat_rules178r10::f5Rung.load(std::memory_order_acquire)==3,std::memory_order_release);
    experimentEpoch36.fetch_add(1,std::memory_order_acq_rel);
    combat_rules178r10::useHeld=false;combat_rules178r10::strokeUntil=0;
}
inline bool ownsWrist(std::uint64_t now) noexcept {
    return combat_rules178r10::workstationEnabled.load(std::memory_order_relaxed)&&
        ((combat_rules178r10::f5Rung.load(std::memory_order_acquire)==2&&experimentArmed36.load(std::memory_order_acquire))||
         (hands59c7f::active()&&hands59c7f::pickupReady.load()&&(hands59c7f::grabs(now)!=0||hands59c7f::activityVisual.load())));
}
// C45 MINIGAME HANDS: SEMI-PASSENGER admits the tracked wrist/body at the alchemy table / smithy without F5 or F10 rung 2.
// Only the SMITHING / ALCHEMY witness consumers (smith auto-hold gate, workstation tool pose, workstation body producer, the
// coherent-pose tracked-interaction flag) ask ownsActivityWrist45; ownsWrist itself (staged pickup, F5 placement, fists) is unchanged.
// The explicit F5=2 test owns full tracked alchemy. Normal and guided F5=3
// retain native torso/head/work hands and HMD gaze selection.
inline bool fullVrAlchemy63(std::uint64_t now,std::uint64_t w) noexcept {
    const auto tick=minigame_hands45::tickOf(w);
    return minigame_hands45::kindOf(w)==minigame_hands45::Alchemy&&tick&&now>=tick&&now-tick<=80&&
        combat_rules178r10::workstationEnabled.load(std::memory_order_acquire)&&
        minigame_hands45::modeFor(minigame_hands45::Alchemy)!=minigame_hands45::Native&&
        (minigame_hands45::freeTrackedMode(minigame_hands45::modeFor(minigame_hands45::Alchemy))||
         (combat_rules178r10::f5Rung.load(std::memory_order_acquire)==2&&experimentArmed36.load(std::memory_order_acquire)));
}
inline bool fullVrAlchemy63(std::uint64_t now) noexcept {
    return fullVrAlchemy63(now,minigame_hands45::witness.load(std::memory_order_acquire));
}
// Saved FREE VRIK owns the existing shared body/head presentation, not native
// work inputs. The old H51 switch only retains its explicit F5 alchemy trial.
// Native lifetime and setting epoch also distinguish same-address End/Begin.
struct WorkstationOwnerH51 {std::uint64_t modeEpoch=0,lifetime=0;std::uintptr_t instance=0;unsigned kind=0;bool alchemyBookAS1=false;std::uint64_t stationLifetimeH46=0;};
// AS1 review (25 Oct: "the alchemy book is making the HMD move from Henry's eyes and the arms too").
// the recipe book at the alchemy table is a NESTED native Reading activity (C37 class=OpenBook, native FOV 95->50,
// native head 0.11-0.62 m from the calibrated head). it set the reading context, which retired the FREE VRIK
// workstation frame (body) AND the C51 camera eye at once: the camera fell back to the native authored book camera
// and the body to the live native head, so head and hands no longer matched the controllers. while the native
// alchemy instance stays alive and the saved alchemy mode is FREE (Gesture), the nested book keeps the SAME
// workstation owner: one C51 body + camera, current HMD once. native book/recipe input and selection stay native.
namespace alchemy_book_as1 {
inline std::atomic<bool> enabled{true}; // INI VrWorkstationHenryEyeAS1
inline std::atomic<std::uint64_t> ownerFrames{0},cameraFallbacks{0};
}
inline bool alchemyBookNestedAS1() noexcept {
    const auto& w=activity_input163::witness175;using K=activity_input163::Kind175;
    return w.ready.load(std::memory_order_acquire)&&w.active[unsigned(K::Alchemy)].load(std::memory_order_acquire)&&
        w.active[unsigned(K::Reading)].load(std::memory_order_acquire)&&!w.active[unsigned(K::Smithing)].load(std::memory_order_acquire)&&
        !w.active[unsigned(K::Sharpening)].load(std::memory_order_acquire);
}
// pure rule (host tested): the nested book inherits the FREE alchemy owner only with the switch on, FREE alchemy
// selected, the workstation option on and the native alchemy+reading pair live.
constexpr bool bookOwnerRuleAS1(bool enabled,int alchemyMode,bool workstationEnabled,bool nested) noexcept {
    return enabled&&minigame_hands45::freeTrackedMode(alchemyMode)&&workstationEnabled&&nested;
}
inline bool workstationOwnerH51(std::uint64_t now,WorkstationOwnerH51& out) noexcept {
    out={};const auto& w=activity_input163::witness175;
    const auto sample=minigame_hands45::witness.load(std::memory_order_acquire);
    const bool bookAS1=bookOwnerRuleAS1(alchemy_book_as1::enabled.load(std::memory_order_relaxed),
        minigame_hands45::modeFor(minigame_hands45::Alchemy),combat_rules178r10::workstationEnabled.load(std::memory_order_acquire),
        alchemyBookNestedAS1());
    // the native minigame-controls witness can go quiet while the book owns input; the alchemy instance cannot.
    const auto kind=bookAS1?unsigned(minigame_hands45::Alchemy):minigame_hands45::kindOf(sample);
    const auto mode=minigame_hands45::modeEpoch67.load(std::memory_order_acquire);
    // The controls witness refreshes its timestamp every input frame. Select
    // from this one fresh snapshot; a same-kind refresh is not a new owner.
    const auto sampleTick=minigame_hands45::tickOf(sample);
    const bool sampleFresh=sampleTick&&now>=sampleTick&&now-sampleTick<=minigame_hands45::kFreshMs;
    const bool gesture=sampleFresh&&minigame_hands45::freeTrackedMode(minigame_hands45::modeFor(kind));
    const bool selected=kind==minigame_hands45::Alchemy?
        (bookAS1||gesture||
         (alchemy_own_head_h51::enabled.load(std::memory_order_relaxed)&&fullVrAlchemy63(now,sample))):
        kind==minigame_hands45::Smithing&&gesture;
    if(!selected||!combat_rules178r10::workstationEnabled.load(std::memory_order_acquire))return false;
    const auto index=kind==minigame_hands45::Alchemy?unsigned(activity_input163::Kind175::Alchemy):unsigned(activity_input163::Kind175::Smithing);
    const auto lifetime=w.lifetime175.load(std::memory_order_acquire);
    const auto instance=w.active[index].load(std::memory_order_acquire);
    const auto stationLifetimeH46=w.activityEpochH46[index].load(std::memory_order_acquire);
    const auto currentSample=minigame_hands45::witness.load(std::memory_order_acquire);
    if(!w.ready.load(std::memory_order_acquire)||w.transitionDepth175.load(std::memory_order_acquire)||!instance||
       mode!=minigame_hands45::modeEpoch67.load(std::memory_order_acquire)||
       (minigame_hands45::kindOf(currentSample)!=minigame_hands45::kindOf(sample)||
        minigame_hands45::tickOf(currentSample)<sampleTick)||
       lifetime!=w.lifetime175.load(std::memory_order_acquire)||
       instance!=w.active[index].load(std::memory_order_acquire)||
       stationLifetimeH46!=w.activityEpochH46[index].load(std::memory_order_acquire)||
       w.transitionDepth175.load(std::memory_order_acquire))return false;
    out={mode,lifetime,instance,kind,bookAS1,stationLifetimeH46};return true;
}
inline bool gestureSmith67(std::uint64_t now) noexcept {
    return combat_rules178r10::workstationEnabled.load(std::memory_order_acquire)&&minigame_hands45::gestureNow(minigame_hands45::Smithing,now);
}
inline bool gestureWork67(std::uint64_t now) noexcept {return fullVrAlchemy63(now)||gestureSmith67(now);}
inline bool passengerAlchemy63(std::uint64_t now) noexcept {
    const auto w=minigame_hands45::witness.load(std::memory_order_acquire);
    const auto tick=minigame_hands45::tickOf(w);
    return minigame_hands45::kindOf(w)==minigame_hands45::Alchemy&&tick&&now>=tick&&now-tick<=80&&!fullVrAlchemy63(now);
}
inline bool semiPassengerOwns45(std::uint64_t now) noexcept {
    if(gestureWork67(now))return false;
    if(guided_crafting55::active(now))return true;
    return minigame_hands45::reasonNow(combat_rules178r10::workstationEnabled.load(std::memory_order_relaxed),now)==nullptr;
}
inline bool ownsActivityWrist45(std::uint64_t now) noexcept {
    // C46R1: the NATIVE setting is authoritative even if a legacy F5/F10 rung
    // remains selected. It does not revoke ordinary world/pickup handling.
    const auto w=minigame_hands45::witness.load(std::memory_order_acquire);
    if(!guided_crafting55::active(now)&&minigame_hands45::currentMode()==minigame_hands45::Native&&
       minigame_hands45::kindOf(w)!=minigame_hands45::None&&minigame_hands45::tickOf(w)&&
       now>=minigame_hands45::tickOf(w)&&now-minigame_hands45::tickOf(w)<=minigame_hands45::kFreshMs)return false;
    return gestureWork67(now)||ownsWrist(now)||semiPassengerOwns45(now);
}
// R59.16C2: grip=alch_use and hammer-swing=stroke are plain input gestures, useful in normal play.
// They no longer require the F5 rung-2 experiment (which still alone owns the tracked wrist/body
// at workstations and the staged physics pickup). INI VrWorkstationGestures=false restores R59.16.
inline std::atomic<bool> gesturesEverywhere59c2{true};
inline bool gesturesOn(std::uint64_t now) noexcept {
    // H18: an explicit fresh Native activity must also veto the old global
    // gesturesEverywhere compatibility path. The separately armed guided F5
    // experiment keeps its existing explicit override; unrelated/stale data
    // cannot impose an activity's saved mode on ordinary world interactions.
    const auto witness=minigame_hands45::witness.load(std::memory_order_acquire);
    const auto kind=minigame_hands45::kindOf(witness);
    if(!guided_crafting55::active(now)&&(kind==minigame_hands45::Alchemy||kind==minigame_hands45::Smithing)&&
       minigame_hands45::witnessCurrent67(kind,now)&&minigame_hands45::modeFor(kind)==minigame_hands45::Native)return false;
    return combat_rules178r10::workstationEnabled.load(std::memory_order_relaxed)&&
        (gestureWork67(now)||ownsWrist(now)||semiPassengerOwns45(now)||gesturesEverywhere59c2.load(std::memory_order_relaxed));
}
// R59.16C2: inside the alchemy minigame the hand (not the head) may aim KCD2's own interactor at the
// table's tools while it is held out in front (pointing posture). INI VrAlchemyHandRay=false: head only.
inline std::atomic<bool> alchemyHandRay59c2{true};
inline std::atomic<bool> alchemyPassenger42{true}; // native action while holding tool; free hands between actions
// C37: the native smithing hammer / tongs / sword workpiece follow the VR hands
// during the smithing minigame without Grip (menu ACTIONS > SMITHING TOOLS,
// INI VrSmithingAutoHold, default on). Its own request epoch: a change revokes
// every current smithing hold at once. F5 is not involved.
inline std::atomic<bool> smithAutoHold36{true};
inline std::atomic<std::uint64_t> smithHoldEpoch36{1};
inline void setSmithAutoHold36(bool on) noexcept {
    if(smithAutoHold36.exchange(on,std::memory_order_acq_rel)!=on)smithHoldEpoch36.fetch_add(1,std::memory_order_acq_rel);
}
// Pure posture rule (host-tested). forward = metres along the head's look direction, drop = metres below
// the head. Hysteresis keeps the current owner until it is clearly lowered.
inline int pointingHand59c2(int current,const double forward[2],const double drop[2],const bool valid[2]) noexcept {
    auto ok=[&](int s,double need){return valid[s]&&std::isfinite(forward[s])&&std::isfinite(drop[s])&&forward[s]>=need&&drop[s]<=.70;};
    if(current>=0&&current<2&&ok(current,.18))return current;
    int best=-1;for(int s=0;s<2;++s)if(ok(s,.25)&&(best<0||forward[s]>forward[best]))best=s;
    return best;
}
enum class Kind { None, Alchemy, Anvil, SmithingOther };
// Copied native-stage witness for the attachment consumer. No native polling.
inline std::atomic<int> heldToolActivity59c10{0};
inline std::atomic<std::uint64_t> heldToolActivityTick59c10{0};
inline bool nativeAlchemy42(std::uint64_t now) noexcept {
 const auto t=heldToolActivityTick59c10.load(std::memory_order_acquire);
 return !fullVrAlchemy63(now)&&alchemyPassenger42.load()&&heldToolActivity59c10.load()==int(Kind::Alchemy)&&t&&now>=t&&now-t<=80;
}
struct Sample { bool valid=false;float height=0,grip=0;std::uint64_t sequence=0,tick=0,timeNs=0; }; // C37C4: timeNs for the stroke speed
struct Result { bool changed=false,use=false,stroke=false,alchemy=false; };
// Mod input thread only. Native eligibility is supplied by event witnesses.
struct Controller {
    Kind kind=Kind::None;std::uintptr_t instance=0;int phase=-1;
    combat_rules178r10::HammerDetector hammer{};
    std::array<bool,2> armed{},held{};
    std::uint64_t sequence=0,strokeEnd=0;
    std::uint64_t experimentEpoch=0;
    bool nativeUseOverride19c4=false,nativeUse19c4=false;
    Result update(Kind next,std::uintptr_t object,int nativePhase,const std::array<Sample,2>& hands,std::uint64_t now) noexcept {
        heldToolActivity59c10.store(object?int(next):0,std::memory_order_relaxed);
        heldToolActivityTick59c10.store(now,std::memory_order_release);
        const auto epoch=experimentEpoch36.load(std::memory_order_acquire);
        Result out{};out.changed=kind!=next||instance!=object||phase!=nativePhase||experimentEpoch!=epoch;
        experimentEpoch=epoch;
        if(out.changed){kind=next;instance=object;phase=nativePhase;hammer.reset();armed={};held={};sequence=0;strokeEnd=0;}
        out.alchemy=kind==Kind::Alchemy;
        // C46R1: this retained adapter publishes native stage identity only.
        // Motion work is owned by alchemy_semi46 / semi_smith46; native mode
        // emits no gestures. Never leave the old height pulse alongside them.
        armed={};held={};hammer.reset();sequence=0;strokeEnd=0;
        return out;
    }
    bool ownsGrip(unsigned,std::uint64_t) const noexcept { return false; }
    // The new adapters apply after ordinary pad mapping. Manual native buttons
    // remain untouched here; no hidden second A/RT writer survives a mode change.
    void apply(std::uint64_t,bool&,bool&,bool&,float&) const noexcept {}
};
}
