#pragma once
#include <cmath>
#include <cstdint>
// C39 DIRECT DEFENSE TIMING (pure logic, host tested; used by geometry_hit178r27.inl SelectPrediction19c3).
// WHGame 6A350E20 static proof (see C39 report):
//  - the NPC block helper C_CombatActionHelperBlock (vft 3A8A2D0) slot6 C53B30 sets the NPC phase 0x80 (preparing) and calls
//    D4D4FC(npc timing [actor+3C0], {..,MinBlockSpeedBias cfg+4FC 0.5, MaxBlockSpeedBias cfg+4F8 10.0}); D4D4FC reads the
//    OPPONENT's +450 (Henry's announced time to hit) and time-aligns the block raise to it (speed = event time / +450,
//    clamped 0.5..10, x BlockSpeedMod cfg+500 0.8). slot7 C554B8 switches 0x80 -> 0x100 (ready) when NPC +3F0 <= 0;
//    +3F0 is written every update by the NPC timing (9E3B80 / 13B7D20) from that aligned timeline.
//  - 914204 (phase-4 offer) waits min(max(0,+450 - lead), RPG reaction) before it activates the block trigger.
//  So the only VR-side input to "how fast does this NPC raise his block" is Henry's announced +450, which VR DIRECT
//  publishes itself (C19C3 configured prediction). This module shortens ONLY that announcement, ONLY for the reciprocal
//  opponent while he is waiting/guarding, by a factor from his own melee skill. The native aligner then plays the
//  block raise faster (bounded by the native 10x clamp); eligibility, RNG, direction and the block result stay native.
namespace kcd2vr::defense_timing39 {
enum class Reason : int { Applied=0, Disabled, NoOpponent, NotReciprocal, Attacking, PhaseUnknown, NoSeed };
inline const char* name(Reason r) noexcept {
    switch(r){case Reason::Applied:return "APPLIED";case Reason::Disabled:return "DISABLED";case Reason::NoOpponent:return "NO_OPPONENT";
        case Reason::NotReciprocal:return "NOT_RECIPROCAL";case Reason::Attacking:return "NPC_BUSY_NOT_WAITING";
        case Reason::PhaseUnknown:return "NPC_PHASE_UNKNOWN";case Reason::NoSeed:return "NO_SEED";}
    return "?";
}
// Native NPC phases seen in the C37C2 log: 1/2 idle-guard, 128 block preparing, 256 block ready (waiting/guarding);
// 4/8/16/32 own attack wind-up/strike/withdraw/recover, 64 hit reaction, 512 other action (busy: never scaled).
inline bool waitingPhase(int phase) noexcept {return phase==1||phase==2||phase==128||phase==256;}
constexpr float kMinFactor=1.f,kMaxFactor=3.f,kFloorSeconds=.08f;
inline float clampFactor(float f) noexcept {return std::isfinite(f)?(f<kMinFactor?kMinFactor:(f>kMaxFactor?kMaxFactor:f)):kMinFactor;}
// Best of the five melee weapon skills (sword, heavy, large, dagger, unarmed; soul+5E8+8*id, 0..40). -1 = unread.
inline int bestSkill(const int (&skills)[5]) noexcept {
    int best=-1;for(const int s:skills)if(s>=0&&s<=40&&s>best)best=s;return best;
}
// skill 0 -> atSkill0, skill >= 30 -> atSkill30 (linear). Unread skill -> atSkill0 (the menu's minimum speed-up).
inline float skillFactor(int skill,float atSkill0,float atSkill30) noexcept {
    const float lo=clampFactor(atSkill0),hi=clampFactor(atSkill30);
    if(skill<0)return lo;
    const float t=skill>=30?1.f:float(skill)/30.f;
    return clampFactor(lo+(hi-lo)*t);
}
// The announced contact ETA. Never longer than the configured seed, never below the floor (unless the seed is lower).
inline float effectiveEta(float seed,float factor) noexcept {
    if(!std::isfinite(seed)||seed<=0.f)return seed;
    float eta=seed/clampFactor(factor);
    if(eta<kFloorSeconds)eta=seed<kFloorSeconds?seed:kFloorSeconds;
    return eta>seed?seed:eta;
}
struct Decision {float factor=1.f,eta=0.f;int skill=-1,phase=-99;Reason reason=Reason::Disabled;};
// reciprocal: Henry +CF8 == opponent and opponent +CF8 == Henry (exact C_CombatActor identities, checked by the caller).
inline Decision decide(bool enabled,float seed,bool opponent,bool reciprocal,int npcPhase,int skill,float atSkill0,float atSkill30) noexcept {
    Decision d{};d.eta=seed;d.phase=npcPhase;d.skill=skill;
    if(!std::isfinite(seed)||seed<=0.f){d.reason=Reason::NoSeed;return d;}
    if(!enabled){d.reason=Reason::Disabled;return d;}
    if(!opponent){d.reason=Reason::NoOpponent;return d;}
    if(!reciprocal){d.reason=Reason::NotReciprocal;return d;}
    if(npcPhase==-99||npcPhase<0){d.reason=Reason::PhaseUnknown;return d;}
    if(!waitingPhase(npcPhase)){d.reason=Reason::Attacking;return d;} // Miki: never while his own attack/reaction runs
    d.factor=skillFactor(skill,atSkill0,atSkill30);d.eta=effectiveEta(seed,d.factor);d.reason=Reason::Applied;return d;
}
}
