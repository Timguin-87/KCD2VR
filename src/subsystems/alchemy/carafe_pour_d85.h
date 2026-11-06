#pragma once
// D85 craft: carafe (oil/water/spirits, table slots 13..16) poured by the hand.
//
// Evidence (D79 headset F5, 1 Nov 17:12:46.675-53.011): the native UseResource
// child ran 6.3 s of Henry's own clip. Attach at normalized 0.165, native Use at
// 0.4209, native Detach at 0.8664; the left tool was joined to that same parent
// (toolL=952230/13, controlHeld=0,1) and the semi46 work witness existed with
// phase 0, but no clock owner ever took the child. The pause/advance tool used
// here is the already pinned IAction +5C speed of that exact child, written
// before native Update (D81 presentation lease); speed 0 is the same pause the
// semi46 grinding owner uses (D79 C46 HAND_STOPPED / HAND_ADVANCING rows).
//
// This header is pure: it decides one rate for the current native child.
//   reach   : Attach not yet joined         -> fast (Henry's reach nearly gone)
//   hold    : joined, Use not yet fired     -> 0 until held + tilted over pot, then
//             native 1x through Use/FX (it fires once, inside that update)
//   carry   : Use fired, Detach not yet     -> 0 until the hand brings it home, then 1x
//   finish  : home reached or Detach fired  -> native 1x; no qualified FX boundary here
// The native Use and Detach events are never sent, skipped or repeated: the
// clock only runs through them once. Any loss of the exact inputs hands the
// child back to native time for good (one reason line).
#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdint>

namespace kcd2vr::subsystems::alchemy::carafe_d85 {
// Bits copied by the input adapter into bridge67::Control::carafeD85.
enum Bits : unsigned {
    Valid=1u,      // a left-hand carafe admission (slot 13..16) owns this control
    Grip=2u,       // the left hand still holds (.35 retained grip, same as the gesture owner)
    PotKnown=4u,   // current native pot (slot 6) bounds exist
    Pour=8u,       // palm within 20 cm of the pot bounds and tilted >= .40 rad
    HomeKnown=16u, // carafe's own native bounds were copied at admission
    Home=32u       // palm within 10 cm of those bounds and upright (<= .35 rad)
};
// Same thresholds as the existing pour gesture (gesture_work67: pot .20 m,
// tilt .40 rad) and mortar home (pour70: 10 cm, upright .35 rad).
inline constexpr double kPotRadius=.20,kHomeRadius=.10;
inline constexpr float kPourTilt=.40f,kUprightTilt=.35f;

inline unsigned intent(bool leftCarafe,bool grip,bool potKnown,double potGap,bool homeKnown,double homeGap,
                       bool tiltKnown,float tilt) noexcept {
    if(!leftCarafe)return 0;
    unsigned b=Valid;
    if(grip)b|=Grip;
    if(potKnown){b|=PotKnown;
        if(tiltKnown&&std::isfinite(tilt)&&std::isfinite(potGap)&&potGap<=kPotRadius&&tilt>=kPourTilt)b|=Pour;}
    if(homeKnown){b|=HomeKnown;
        if(tiltKnown&&std::isfinite(tilt)&&std::isfinite(homeGap)&&homeGap<=kHomeRadius&&tilt<=kUprightTilt)b|=Home;}
    return b;
}

// Native resource events of the current child, copied by the two existing
// resource event detours after the original handler returned. Attach (when the
// detour sees it), Use and Detach CRCs are the same retail names the C46 path
// reads from parent+68/+70/+6C. Owner thread + foreign animation thread.
enum EventBits : unsigned {AttachSeen=1u,UseSeen=2u,DetachSeen=4u};
inline constexpr std::uint32_t kAttachCrc=0xDEC97C81u,kUseCrc=0x94B1CC4Bu,kDetachCrc=0xD3845300u;
struct Ledger {
    std::atomic<std::uintptr_t> child{0};std::atomic<std::uint32_t> token{0};std::atomic<unsigned> seen{0};
    void note(std::uintptr_t c,std::uint32_t t,std::uint32_t crc) noexcept {
        const unsigned bit=crc==kAttachCrc?AttachSeen:crc==kUseCrc?UseSeen:crc==kDetachCrc?DetachSeen:0u;
        if(!c||!t||!bit)return;
        if(child.load(std::memory_order_acquire)!=c||token.load(std::memory_order_acquire)!=t){
            seen.store(0,std::memory_order_release);token.store(t,std::memory_order_release);child.store(c,std::memory_order_release);}
        seen.fetch_or(bit,std::memory_order_acq_rel);
    }
    unsigned read(std::uintptr_t c,std::uint32_t t) const noexcept {
        const unsigned s=seen.load(std::memory_order_acquire);
        return c&&t&&child.load(std::memory_order_acquire)==c&&token.load(std::memory_order_acquire)==t?s:0u;
    }
    void clear() noexcept {seen.store(0);token.store(0);child.store(0);}
};
inline Ledger ledger;

enum class Stage : unsigned char {Off,Reach,Hold,Pour,Carry,Finish,Native};
enum class Why : unsigned char {None,Released,ReleasedAway,Tracking,NoPot,NoHome,Control};
inline const char* name(Stage s) noexcept {
    switch(s){case Stage::Reach:return "REACH_FAST";case Stage::Hold:return "WAIT_TILT_OVER_POT";
    case Stage::Pour:return "TILTED_NATIVE_POUR";case Stage::Carry:return "WAIT_RETURN_HOME";
    case Stage::Finish:return "RETURNED_NATIVE";case Stage::Native:return "NATIVE";default:return "OFF";}
}
// One short line, shown/logged once per carafe when the hand cannot drive it.
inline const char* reason(Why w) noexcept {
    switch(w){
    case Why::Released:return "Alchemy: carafe let go before pouring, Henry pours it";
    case Why::ReleasedAway:return "Alchemy: carafe let go away from its place, Henry puts it back";
    case Why::Tracking:return "Alchemy: hand tracking lost, Henry finishes the carafe";
    case Why::NoPot:return "Alchemy: pot position unknown, Henry pours the carafe";
    case Why::NoHome:return "Alchemy: carafe place unknown, Henry puts it back";
    case Why::Control:return "Alchemy: carafe hand input lost, Henry finishes it";
    default:return "";
    }
}
struct Key {
    std::uintptr_t instance=0,parent=0,child=0;std::uint64_t lifetime=0;std::uint32_t token=0;
    bool operator==(const Key& o) const noexcept {
        return instance==o.instance&&parent==o.parent&&child==o.child&&lifetime==o.lifetime&&token==o.token;}
};
struct Observation {
    Key key{};
    bool attached=false; // current left tool joined to THIS parent (native Attach happened)
    bool used=false;     // native Use callback of THIS child/token returned
    bool detached=false; // native Detach callback of THIS child/token returned
    bool tracked=false;  // fresh workstation frame with a finite left hand
    bool control=false;  // fresh bridge control for this instance/lifetime/parent
    unsigned bits=0;     // Bits from the input adapter
};
struct Decision {float rate=1.f;Stage stage=Stage::Off;Why why=Why::None;bool announce=false;};

// A tracking/grip/control gap shorter than this keeps the child paused (the
// held vessel does not jump); only a lasting loss hands it back to native time.
inline constexpr std::uint64_t kLossGraceMs=200;
class Controller {
    Key key_{};Stage stage_=Stage::Off;Why why_=Why::None;bool announced_=false,admitted_=false;
    std::uint64_t lossSince_=0;Why lossWhy_=Why::None;
    bool lost(Why why,std::uint64_t now) noexcept {
        if(lossWhy_!=why||!lossSince_||now<lossSince_){lossWhy_=why;lossSince_=now?now:1;}
        return now>=lossSince_&&now-lossSince_>=kLossGraceMs;
    }
    Decision native(Why why) noexcept {
        stage_=Stage::Native;Decision d{1.f,stage_,why_,false};
        if(why_==Why::None){why_=why;d.why=why;}
        if(!announced_&&why_!=Why::None){announced_=true;d.announce=true;}
        return d;
    }
public:
    Stage stage() const noexcept {return stage_;}
    const Key& key() const noexcept {return key_;}
    void reset() noexcept {*this=Controller{};}
    // fast: the user's alchemy speed (>=1). The returned rate is the IAction +5C
    // value for the NEXT native update of this exact child.
    Decision step(const Observation& o,float fast,std::uint64_t now) noexcept {
        if(!std::isfinite(fast)||fast<1.f)fast=1.f;
        if(!o.key.child||!o.key.parent||!o.key.token){reset();return {};}
        if(!(o.key==key_)){reset();key_=o.key;stage_=Stage::Reach;}
        if(!admitted_){
            admitted_=o.tracked&&o.control&&(o.bits&Valid);
            if(!admitted_)return {}; // foreign/unobserved resource always keeps native time
        }
        if(stage_==Stage::Native)return {1.f,stage_,why_,false};
        // No generic resource gets accelerated just because Attach has not
        // happened. A fresh admitted left-hand carafe must own the reach.
        if(!o.attached&&!o.used&&!o.detached&&stage_!=Stage::Finish){
            stage_=Stage::Reach;
            const bool reach=o.tracked&&o.control&&(o.bits&Valid)&&(o.bits&Grip);
            return {reach?fast:1.f,stage_,Why::None,false};
        }
        // Native Use can precede authored particles. This fallback has no C46
        // current CAF/procedural proof, so neither Home nor a latched Finish
        // may turn the remaining work/FX tail into an 8x presentation clock.
        if(o.detached||stage_==Stage::Finish){stage_=Stage::Finish;return {1.f,stage_,Why::None,false};}
        // Geometry absence is immediate: nothing could ever complete the wait.
        if(!o.used&&o.control&&(o.bits&Valid)&&!(o.bits&PotKnown))return native(Why::NoPot);
        if(o.used&&o.control&&(o.bits&Valid)&&!(o.bits&HomeKnown))return native(Why::NoHome);
        const Why missing=!o.tracked?Why::Tracking:(!o.control||!(o.bits&Valid))?Why::Control:
            (!(o.bits&Grip)&&!(o.used&&(o.bits&Home)))?(o.used?Why::ReleasedAway:Why::Released):Why::None;
        if(missing!=Why::None){
            if(lost(missing,now))return native(missing);
            const Stage waiting=o.used?Stage::Carry:Stage::Hold;
            if(stage_!=Stage::Pour||o.used)stage_=waiting;
            return {0.f,stage_,Why::None,false};
        }
        lossSince_=0;lossWhy_=Why::None;
        if(!o.used){
            stage_=(o.bits&Pour)?Stage::Pour:Stage::Hold;
            return {stage_==Stage::Pour?1.f:0.f,stage_,Why::None,false};
        }
        if(o.bits&Home){stage_=Stage::Finish;return {1.f,stage_,Why::None,false};}
        stage_=Stage::Carry;return {0.f,stage_,Why::None,false};
    }
};
}
