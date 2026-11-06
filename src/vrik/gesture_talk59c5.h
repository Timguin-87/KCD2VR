#pragma once
// R59.16C5 GESTURE QUICK TALK (pure logic, host-tested; no native state).
//
// KCD2's own quick talk ("chat") owns greetings, taunts and battle cries: the chat opens on the gamepad shoulder
// button (the companion labels physical Quick Talk as X) and its options are A/B/X/Y (chat_first..fourth_controller
// in defaultProfile.xml). In combat option 2 is the battle cry, option 3 the taunt (the Auto Taunt mod presses
// Alt+2 / Alt+3 for exactly these). So a gesture only has to open that menu and, for a combat gesture, choose the
// option once the menu is really open. Perks, buffs, morale and NPC reactions stay the game's.
//
//   POINT    : Grip held, Trigger not squeezed, hand >= 35 cm in front of the head, held 300 ms.
//              The gameplay adapter holds the native chooser in/out of combat.
//              Capacitive Trigger contact alone does not cancel Grip-only input.
//   SWORD PAT: committed empty-palm/shield-bounds contact -> native battle cry (option 2).
//   RAISE    : Grip held, hand >= 20 cm ABOVE the head, held 400 ms, in combat only -> battle cry (option 2).
// One gesture per 1.5 s; a gesture re-arms only after its pose was left. The option button is pressed ONLY while
// the chat menu is reported open (never a blind B, which would be a jump/dodge in gameplay).
#include <cmath>
#include <cstdint>

namespace kcd2vr::gesture_talk59c5 {
struct Hand {bool valid=false;float grip=0,trigger=0;bool triggerTouch=true;double forward=0,up=0;bool empty=true;bool outwardR26=true;};
struct Config {
    bool holdPoint=false;
    bool automaticVoice=true; // Legacy callers only; optional index-point host requests a chooser, never a voice option.
    bool manualPoint54=false; // pointing holds the chooser in/out of combat; never auto-select a taunt
    bool requireIndexOffTrigger=true; // index-point entry and retention; legacy non-index clients may opt out
    float gripOn=.60f,triggerOff=.10f;
    double pointForward=.35,pointMinUp=-.45,raiseUp=.20,raiseMaxForward=.45;
    std::uint64_t pointHoldMs=300,raiseHoldMs=400,cooldownMs=1500,chatWaitMs=800,pulseMs=120;
};
enum class Kind {None,Talk,Taunt,BattleCry};
struct State {
    std::uint64_t pointSince[2]{},raiseSince[2]{};bool rearm[2]{true,true};
    std::uint64_t lastFire=0,openUntil=0,optionUntil=0,waitUntil=0;int pendingOption=0;Kind lastKind=Kind::None;int pointOwner=-1;
    std::uint64_t fired=0,optionsSent=0,chatTimeouts=0;int optionOwnerH4=-1;bool chooserSeenH4=false;std::uint64_t selectionUntilH42=0;
};
struct Output {bool openChat=false;int option=0;Kind fired=Kind::None;bool chatTimeout=false;unsigned claimMaskH4=0;};
inline bool currentChoiceHandH4(const Hand& h) noexcept {
 return h.valid&&h.empty&&std::isfinite(h.grip)&&std::isfinite(h.trigger)&&std::isfinite(h.forward)&&std::isfinite(h.up);
}
inline bool retainedPointH4(const Hand& h,const Config& c) noexcept {
 return h.outwardR26&&h.empty&&h.valid&&std::isfinite(h.grip)&&std::isfinite(h.trigger)&&std::isfinite(h.forward)&&std::isfinite(h.up)&&
  h.grip>=.35f&&(!c.requireIndexOffTrigger||!h.triggerTouch)&&h.trigger<c.triggerOff&&h.forward>=c.pointForward-.10&&h.up>=c.pointMinUp-.10;
}

// The hand's location alone is not a pointing gesture. In particular, a
// hand withdrawing from a shoulder can be in front while its aim faces the body.
inline bool OutwardPointR26(double rx,double ry,double rz,double ax,double ay,double az,double fx,double fz) noexcept {
    const double r2=rx*rx+ry*ry+rz*rz,a2=ax*ax+ay*ay+az*az,f2=fx*fx+fz*fz;
    if(!std::isfinite(r2)||!std::isfinite(a2)||!std::isfinite(f2)||r2<.01||a2<.01||f2<.01)return false;
    return (rx*ax+ry*ay+rz*az)/std::sqrt(r2*a2)>.35 && (ax*fx+az*fz)/std::sqrt(a2*f2)>.25;
}
inline bool pointing(const Hand& h,const Config& c) noexcept {
    return h.outwardR26&&h.empty&&h.valid&&std::isfinite(h.grip)&&std::isfinite(h.trigger)&&std::isfinite(h.forward)&&std::isfinite(h.up)&&h.grip>=c.gripOn&&(!c.requireIndexOffTrigger||!h.triggerTouch)&&h.trigger<c.triggerOff&&
        h.forward>=c.pointForward&&h.up>=c.pointMinUp;
}
inline bool raised(const Hand& h,const Config& c) noexcept {
    return h.empty&&h.valid&&std::isfinite(h.grip)&&std::isfinite(h.trigger)&&std::isfinite(h.forward)&&std::isfinite(h.up)&&h.grip>=c.gripOn&&h.trigger<c.triggerOff&&h.up>=c.raiseUp&&h.forward<=c.raiseMaxForward;
}
inline bool combatChord(const Hand hands[2]) noexcept {
    for(int side=0;side<2;++side)
        // A qualified EMPTY-hand fist owns boxing input. A held weapon's
        // Trigger+Grip charge/feint must not cancel the OTHER hand's utility.
        if(hands[side].empty&&hands[side].valid&&hands[side].grip>.60f&&hands[side].trigger>.60f)return true;
    return false;
}
inline void reset(State& s) noexcept {s=State{};}
// A committed current-pose self-contact may request the SAME native voice
// option as the existing raise gesture. The native game still owns its result.
inline bool battleCryContact54(State& s,std::uint64_t now,bool verifiedContact,bool enabled,bool inCombat,const Config& c={}) noexcept {
    if(!c.automaticVoice||!verifiedContact||!enabled||!inCombat||!now||s.pendingOption||s.pointOwner>=0||now<s.openUntil||
       (s.lastFire&&(now<s.lastFire||now-s.lastFire<c.cooldownMs)))return false;
    s.lastFire=now;s.lastKind=Kind::BattleCry;++s.fired;
    s.optionOwnerH4=-1;s.pendingOption=2;s.waitUntil=now+c.chatWaitMs;s.optionUntil=0;s.openUntil=now+c.pulseMs;
    return true;
}
// enabled: menu/INI switch AND a gameplay context (no menus, dialogue, minigames). chatOpen: the native chat map is on.
inline Output update(State& s,std::uint64_t now,const Hand hands[2],bool enabled,bool inCombat,bool chatOpen,const Config& c={},bool selected54=false) noexcept {
    Output o{};
    if(!enabled||(inCombat&&combatChord(hands))){
        s.pointOwner=s.optionOwnerH4=-1;s.chooserSeenH4=false;s.selectionUntilH42=0;s.openUntil=s.optionUntil=s.waitUntil=0;s.pendingOption=0;
        for(int i=0;i<2;++i){s.pointSince[i]=s.raiseSince[i]=0;s.rearm[i]=false;}
        return o; // no opener/option may leak into a menu, dialogue or native workstation
    }
    // Explicit choice delivery relaxes posture, not the current hand owner.
    if(selected54&&chatOpen&&s.pointOwner>=0&&s.pointOwner<2&&
       currentChoiceHandH4(hands[s.pointOwner])){
        // Keep the SAME chooser owner while its native face-button choice is
        // delivered. Releasing RB on the next pose frame closed the menu before
        // the native input thread could consume the user's option.
        s.selectionUntilH42=now+c.pulseMs;s.waitUntil=now+c.chatWaitMs;
        o.claimMaskH4=1u<<s.pointOwner;o.openChat=true;return o;
    }
    if(c.holdPoint&&s.pointOwner>=0){
        const auto& hand=hands[s.pointOwner];
        const bool delivery=chatOpen&&now<s.selectionUntilH42&&currentChoiceHandH4(hand);
        const bool pose=delivery||(chatOpen?retainedPointH4(hand,c):pointing(hand,c));
        if(pose&&(!inCombat||c.manualPoint54)&&(!s.chooserSeenH4||chatOpen)&&now<s.waitUntil){o.openChat=true;o.claimMaskH4|=1u<<s.pointOwner;if(chatOpen){s.chooserSeenH4=true;s.waitUntil=now+c.chatWaitMs;}}
        else {if(!s.chooserSeenH4&&now>=s.waitUntil){++s.chatTimeouts;o.chatTimeout=true;}s.pointOwner=-1;s.chooserSeenH4=false;s.selectionUntilH42=0;s.openUntil=0;}
    }
    // Pulses already in flight finish only while the native gameplay context remains allowed.
    if(now<s.openUntil)o.openChat=true;
    // Like Alt+2 in the Auto Taunt mod, the opener stays HELD until the option was pressed (the chat menu is a
    // hold-to-show menu); a plain quick talk is the same short tap the physical B button sends.
    if(s.pendingOption){
        if(s.optionOwnerH4>=0){const auto& h=hands[s.optionOwnerH4];if(!h.valid||!h.empty||!std::isfinite(h.grip)||h.grip<.25f||!std::isfinite(h.trigger)||h.trigger>=c.triggerOff){s.pendingOption=0;s.optionOwnerH4=-1;s.openUntil=s.optionUntil=0;o.openChat=false;o.claimMaskH4=0;return o;}o.claimMaskH4|=1u<<s.optionOwnerH4;}
        o.openChat=true;
        if(chatOpen&&!s.optionUntil){s.optionUntil=now+c.pulseMs;++s.optionsSent;}
        if(s.optionUntil){if(now<s.optionUntil&&chatOpen)o.option=s.pendingOption;else{s.pendingOption=0;s.optionUntil=0;}}
        else if(now>=s.waitUntil){s.pendingOption=0;++s.chatTimeouts;o.chatTimeout=true;}
    }
    for(int i=0;i<2;++i){
        if(s.pointOwner>=0)continue;
        const bool p=pointing(hands[i],c)||(s.pointSince[i]&&(!c.requireIndexOffTrigger||!hands[i].triggerTouch)&&retainedPointH4(hands[i],c)),r=c.automaticVoice&&inCombat&&raised(hands[i],c);
        if(!p&&!r){s.rearm[i]=true;s.pointSince[i]=s.raiseSince[i]=0;continue;}
        if(!s.rearm[i])continue;
        if(p){if(!s.pointSince[i])s.pointSince[i]=now;}else s.pointSince[i]=0;
        if(r){if(!s.raiseSince[i])s.raiseSince[i]=now;}else s.raiseSince[i]=0;
        if(s.pendingOption||now<s.openUntil||(s.lastFire&&now-s.lastFire<c.cooldownMs))continue;
        Kind k=Kind::None;
        // A delayed recognition frame must not turn a forward chooser point
        // into an automatic battle cry merely because both dwell times passed.
        if(c.manualPoint54&&s.pointSince[i]&&now-s.pointSince[i]>=c.pointHoldMs)k=Kind::Talk;
        else if(s.raiseSince[i]&&now-s.raiseSince[i]>=c.raiseHoldMs)k=Kind::BattleCry;
        else if(s.pointSince[i]&&now-s.pointSince[i]>=c.pointHoldMs)k=inCombat&&!c.manualPoint54?Kind::Taunt:Kind::Talk;
        if(k==Kind::None)continue;
        // Recognition owns the chooser; merely approaching a pointing pose
        // does not consume a holster Grip before any native opener exists.
        o.claimMaskH4|=1u<<i;
        s.rearm[i]=false;s.lastFire=now;s.lastKind=k;++s.fired;o.fired=k;
        if(c.holdPoint&&k==Kind::Talk){s.pointOwner=i;s.chooserSeenH4=chatOpen;s.waitUntil=now+c.chatWaitMs;o.openChat=true;}
        if(!chatOpen){s.openUntil=now+c.pulseMs;o.openChat=true;}
        if(k!=Kind::Talk){s.optionOwnerH4=i;s.pendingOption=k==Kind::Taunt?3:2;s.waitUntil=now+c.chatWaitMs;s.optionUntil=0;}
        break;
    }
    return o;
}
}

namespace kcd2vr::gesture_talk59c5 {
// Existing companion binds RB as the hold opener/release and A/B/X/Y as choices.
// X is the visible physical Quick Talk binding, but holding synthetic X in the
// native chat map collides with choice3. This leaves every face button free.
template<class Pad> inline void apply54(Pad& pad,const Output& out) noexcept {
 if(out.claimMaskH4||out.openChat){pad.dpad_left=pad.dpad_right=false;} // chooser gesture, not a quick-weapon chord
 if(out.openChat)pad.rb=true;
 if(out.option==2)pad.b=true;else if(out.option==3)pad.x=true;
}
}
