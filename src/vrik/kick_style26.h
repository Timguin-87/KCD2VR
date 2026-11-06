#pragma once
#include <algorithm>
#include <atomic>
#include <cstdint>
#include "kick_profile52.h" // C52: leg profile timing (the hit time is the lead the service uses)

// C26 KICKS page (Miki, 16 Oct): choose how Henry's R3 kick LOOKS. The game's rules stay native in styles 0 and 1:
// the native kick action (attack type 4) decides block break / push back / dodge / stamina / dice. The mod only
// chooses who draws the kicking leg.
//   0 GAME KICK          - native kick action and the game's own kick clip (legs native while it plays).
//   1 VR LEG, GAME RULES - native kick action; our procedural leg (kick176) draws the kicking leg on top.
//   2 VR LEG ONLY        - the old V176 procedural leg; no native kick, no effect on the enemy (visual test).
// No hook, no timer thread: the animation thread stores the clip's start tick, the XR loop reads two atomics.
namespace kcd2vr::kick26 {
enum class Style : int {Game=0, VrLegGameRules=1, VrLegOnly=2};
inline std::atomic<int> style{0};
inline std::atomic<int> legSide{2};          // 0 right, 1 left, 2 alternate (procedural leg only; the INI/menu default is 0 = right, the atomic keeps the V176 alternate until loaded)
inline std::atomic<int> reachMs{730};        // native kick: foot reaches out this long after the clip starts (table time_to_hit 0.733)
inline std::atomic<bool> freesImpale{true};  // the kick frees the blade from an impale latch at foot reach
inline std::atomic<std::uint64_t> nativeClipStart{0}; // rising edge of the native kick clip (animation thread)
inline std::atomic<std::uint64_t> requestTick{0};     // last R3 kick request that went to the game
inline std::atomic<std::uint64_t> reachTick{0};       // when the foot reaches out (impale release)
// CL-E (Claude, 23 Oct, H46 headset log): R3 kicks that the GAME refused or cut short were silent, so Miki saw "no kick".
// The game stays the judge (stamina, hit stun, attack in progress). These only let the mod SAY which one, in game.
inline std::atomic<bool> henryExhaustedCLE{false};       // Henry at 0 stamina (the handling profile exhaustion edge)
inline std::atomic<std::uint64_t> exhaustedSeenCLE{0};   // CL-E2: tick of the LAST live-attribute frame that was exhausted (0 = not)
inline std::atomic<std::uint64_t> lastHitReactionCLE{0}; // last tick a hit-reaction clip was seen on Henry (animation thread)
// CL-E2 (Codex review): these are inferences from our own reads, not the game's refusal reason, so the text says
// "probably", and the stamina guess only counts if a live exhausted frame was seen in the last 500 ms (current actor/episode).
inline const char* RefusedTextCLE(std::uint64_t now) noexcept {
    const auto seen=exhaustedSeenCLE.load(std::memory_order_relaxed);
    const bool exhausted=seen&&now>=seen&&now-seen<=500;
    return exhausted?"kick: refused - probably no stamina":"kick: refused by the game (busy?)";}
inline constexpr std::uint64_t kLegExtendMs=200;      // OLD profile: knee up 0..110, foot out at 200 ms, held to 300 (C34b)
// C52 (Miki 18 Oct): KICKS page KICK STYLE. 0 NATIVE TIMING (default): the leg follows the game's kick clip timing
// (chamber 0.3 s, foot on the target at 0.733 s, 1.167 s long) and aims at the opponent; 1 OLD 110-200-300.
inline std::atomic<int> profile52{0};               // VrKickProfile
inline std::atomic<int> reachPercent52{92};         // VrKickReach: extended leg = this % of thigh+shin (60..100)
inline kick52::Profile Profile52() noexcept {return kick52::clampProfile(profile52.load(std::memory_order_relaxed));}
inline double ReachScale52() noexcept {return kick52::clampReachScale(reachPercent52.load(std::memory_order_relaxed)/100.0);}
// How long after the leg starts the foot is on the target: 733 ms (native timing) or 200 ms (old). Style 1 starts the
// leg this long before the native hit; the fallback leg and the impale release use the same lead.
inline std::uint64_t LegLeadMs() noexcept {return kick52::hitMs(Profile52());}
inline constexpr std::uint64_t kClipWaitMs=1500;      // a native clip later than this is not this request's kick
// H26: GAME KICK always retains the game's native presentation. An unaccepted
// request must not be disguised by a procedural leg, even with an older saved
// fallback=true. The optional fallback belongs only to explicit VR LEG style1.
inline constexpr std::uint64_t kFallbackMs=250;
inline std::atomic<bool> fallbackLeg{false};   // VrKickFallbackLeg, explicit style1 only
inline std::atomic<bool> nativeSpecial33{true}; // VrKickSpecialInput: R3 armed = the game's special attack input (C33 pak: kick at zone 5)
// Legacy VrKickRealHitTest preference is retained for settings compatibility only.
// Procedural contact is observation-only until it joins an admitted native kick action.
inline std::atomic<bool> realHitTest37c2{false};
// C72 VrKickAssist (cm, 0..30, default 0 = OFF): accessibility option. When on, the foot-sweep ray is lengthened ALONG THE
// CURRENT SAMPLE'S OWN SWEEP by this much while the leg is extending. The visible foot does not move, so a hit found only
// by the extra length is logged with its visible gap (KICK_ASSIST_GAP). Default OFF so no hit can happen without a touch.
inline std::atomic<int> assistCm72{0};
inline float AssistM72() noexcept {return float(std::clamp(assistCm72.load(std::memory_order_relaxed),0,30))*.01f;}

inline Style Current() noexcept {return Style(std::clamp(style.load(std::memory_order_relaxed),0,2));}
inline const char* Name(Style s) noexcept {return s==Style::VrLegGameRules?"VR_LEG_GAME_RULES":s==Style::VrLegOnly?"VR_LEG_ONLY":"GAME_KICK";}
// VrKickTransport (INI) keeps 1/2/3 as advanced choices for the native styles; style 2 is transport 0.
inline int TransportFor(Style s,int iniTransport) noexcept {
    if(s==Style::VrLegOnly)return 0;
    return iniTransport>=1&&iniTransport<=3?iniTransport:3;
}
// While the native kick clip plays, the procedural leg is blocked except in style 1.
inline bool NativeBlocksVrLeg(bool nativeKickActive) noexcept {return nativeKickActive&&Current()!=Style::VrLegGameRules;}
inline int ForcedSide() noexcept {const int s=legSide.load(std::memory_order_relaxed);return s==0||s==1?s:-1;}

struct Service {std::uint64_t handledClip=0,reportedRequest=0,legRequest=0;};
struct Decision {bool requestLeg=false,clipSeen=false,clipMissing=false,fallback=false;std::uint64_t afterMs=0,reach=0;};
// Pure step (tested). clip/request are ticks (0 = none).
inline Decision Step(Service& s,Style style,std::uint64_t now,std::uint64_t clip,std::uint64_t request,std::uint64_t reach) noexcept {
    Decision d{};if(!now)return d;
    const bool clipForRequest=clip&&request&&clip>=request&&clip-request<=kClipWaitMs;
    if(clipForRequest&&clip!=s.handledClip&&now>=clip){
        if(clipForRequest&&s.reportedRequest!=request){s.reportedRequest=request;d.clipSeen=true;d.afterMs=clip-request;}
        d.reach=clip+reach;
        if(style!=Style::VrLegGameRules){s.handledClip=clip;}
        else {
            // C52: the leg starts so that its hit lands at reach (native timing: at the clip start, the profile IS the clip)
            const std::uint64_t lead=LegLeadMs();
            std::uint64_t legStart=d.reach>lead?d.reach-lead:clip;if(legStart<clip)legStart=clip;
            if(now>=legStart){
                s.handledClip=clip;
                if(s.legRequest!=request&&now-legStart<=200){s.legRequest=request;d.requestLeg=true;}
            } // A late clip cannot redraw a fallback already issued for this click.
        }
    }
    // C33 fallback leg: no native clip for this request after kFallbackMs -> draw our leg once, foot reach ~250 ms later
    if(style==Style::VrLegGameRules&&fallbackLeg.load(std::memory_order_relaxed)&&request&&s.legRequest!=request&&s.reportedRequest!=request&&
       now>=request&&now-request>=kFallbackMs&&now-request<=kClipWaitMs&&!(clip&&clip>=request)){
        s.legRequest=request;d.requestLeg=true;d.fallback=true;d.reach=now+LegLeadMs();}
    if(style!=Style::VrLegOnly&&request&&s.reportedRequest!=request&&now>=request&&now-request>kClipWaitMs){ /* style 2 sends no game kick */ s.reportedRequest=request;d.clipMissing=true;}
    return d;
}
// Impale: free the blade inside [reach, reach+300 ms]; hold the latch while the kick is on its way.
inline bool ReleaseImpale(std::uint64_t now,std::uint64_t reach) noexcept {return freesImpale.load(std::memory_order_relaxed)&&reach&&now>=reach&&now-reach<=300;}
inline bool HoldImpale(std::uint64_t now,std::uint64_t request,std::uint64_t reach) noexcept {
    if(!freesImpale.load(std::memory_order_relaxed)||!request||now<request)return false;
    if(reach&&reach>=request)return now<reach;
    return now-request<=kClipWaitMs;
}
}
