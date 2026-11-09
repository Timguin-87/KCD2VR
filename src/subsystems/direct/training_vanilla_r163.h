#pragma once
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include "npc_force_block153.h"
#include "tutorial_native_r159.h"
// R163 "Training fights: vanilla NPCs" (VrTrainingVanillaNpcR163, default ON). Bisect / safety switch for the prologue sparring
// with Hans Capon (quest prepadeni / serm_s_ptackem, native CombatTutorialNode): the scene stays stuck with Hans in combat mode
// (Shift+F5 QUEST_NPC tkop_ptacek combatMode=1 arrangedFight=1, R162 log 22:18:09+), also with native controls.
// While Henry holds the practice / quest weapon (the R156 detector npc_force_block153::practiceWeapon + the R159 freshness,
// tutorial_native_r159::tutorialNow) EVERY mod feature that writes to an NPC, to NPC AI, NPC animation or NPC action timing
// skips its write, for all NPCs. Henry's own VR features (his swing, his announced attack, his body, camera, input) stay as
// they are. At the ON edge anything a feature still holds on an NPC is given back (each feature's own restore path); when the
// practice weapon is gone everything runs again on the next tick. No latch: the state is decided again every control tick.
// The full list of NPC-writing features (gated or not, and why) is REVIEW163_van.md.
namespace kcd2vr::training_vanilla_r163 {
inline constexpr const char* kKey="VrTrainingVanillaNpcR163";
inline std::atomic<bool> enabled{true}; // the Experiments switch (VRIK page 2 row 4), ON = the feature runs (never refused)
inline std::atomic<bool> held{false};   // published by the control-thread service below; every gate reads only this
// one bit per gated feature (order = the log text order)
enum Feature : unsigned {
    NpcDefenseSpeed=0, // R146 / D34 npc_defense: block / parry raise ETA view + dodge animation rate
    ForceBlock,        // R153 wh_cs_ForceBlock lease (already off with the practice weapon since R156)
    NpcFlinch,         // V66.178R11 VrNpcFlinch: torso bend written into the NPC skinning pose
    BladeReaction,     // NR1 / I10 VrNpcReactsToBlade + VrNpcReactsStaminaHits: borrowed reaction zone around the victim reaction build
    LivelyNpc,         // D73 VrLivelyNpcD73 + D76 VrNpcListeningGesturesD76: Lua look / face / listening animations on dialogue members
    DialogueLook,      // C63 / D52 VrDialogueNpcLookHmd: NPC look-at point replaced by the HMD
    UnarmedSpacing,    // VrNpcUnarmedSpacingPercent: unarmed NPC preferred-distance view
    NpcDifficulty,     // VrNpcDifficulty: C8 AI formula input view
    CounterStab,       // C40 / R120 counter stab strength + native major stab reaction request
    StaggerRule,       // C44 VrDirectStaggerRule 2: strength of a strike out of Henry's own stagger
    DirectionalBlock,  // C39 VrDirectNpcDirectionalBlock: announced zone chosen for the waiting NPC's block row
    DefenseTiming,     // C39 VrDirectNpcBlockBySkill timing: announced ETA shortened by the NPC's skill
    RouterWindows,     // H3 VrCombatRouter 2: global wh_cs_* slot cvars incl. wh_cs_AISlotDurationMod
    DodgeWindow,       // D70 VrDodgeWindowD70: global Dodge_WindowMod (ALL actors)
    SharedRules,       // R52 VrSharedCombatRules: global wh_cs_PerfectBlock_SpatialVolumeRadius / MaxAttackDistanceMoveOffset / BlockCooldown
    Count
};
inline const char* featureName(unsigned f) noexcept {
    static constexpr const char* n[Count]{"npcDefenseSpeed","forceBlock","npcFlinch","bladeReaction","livelyNpc","dialogueLook",
        "unarmedSpacing","npcDifficulty","counterStab","staggerRule","directionalBlock","defenseTiming","routerWindows","dodgeWindow","sharedRules"};
    return f<Count?n[f]:"?";}
// the feature knows the NPC's C_CombatActor at its write (so a tutor count exists); false = global / entity id only ("-")
inline bool knowsActor(unsigned f) noexcept {
    return f==NpcDefenseSpeed||f==ForceBlock||f==BladeReaction||f==UnarmedSpacing||f==CounterStab||f==StaggerRule||f==DirectionalBlock||f==DefenseTiming;}
// skipped writes while held: all NPCs / the tutor (= Henry's exact reciprocal opponent, npc_force_block153::opponent)
inline std::atomic<std::uint32_t> skipped[Count]{},tutorSkipped[Count]{};
inline bool holdNow() noexcept {return held.load(std::memory_order_acquire);}
inline bool isTutor(std::uintptr_t actor) noexcept {
    return actor&&actor==kcd2vr::npc_force_block153::opponent.load(std::memory_order_relaxed);}
// THE gate of one write. true = skip this write (held), counted. actor = the NPC's C_CombatActor when the caller knows it (0 = unknown).
inline bool skip(Feature f,std::uintptr_t actor=0) noexcept {
    if(!holdNow())return false;
    if(f<Count){skipped[f].fetch_add(1,std::memory_order_relaxed);if(isTutor(actor))tutorSkipped[f].fetch_add(1,std::memory_order_relaxed);}
    return true;}
// held period number (the service adds 1 at every ON edge): per-period "once" keys start again each period
inline std::atomic<std::uintptr_t> period{0};
inline std::uintptr_t onceKey(std::uintptr_t key) noexcept {return key*31u+period.load(std::memory_order_relaxed)*0x9E3779B1u+1u;}
// per-frame writers (look point, dialogue presentation, AI input views) count once per scene / NPC / site key and period,
// not once per frame. still skips every call while held.
inline bool skipOnce(Feature f,std::atomic<std::uintptr_t>& lastKey,std::uintptr_t key,std::uintptr_t actor=0) noexcept {
    if(!holdNow())return false;
    const auto k=onceKey(key);
    if(lastKey.exchange(k,std::memory_order_relaxed)!=k)(void)skip(f,actor);
    return true;}
// pure: the state the service publishes
inline bool wanted(bool on,bool tutorial) noexcept {return on&&tutorial;}
// one edge per change: 1 = vanilla began, 0 = ended, -1 = no change (the first OFF is not an edge)
struct Edge {
    int told=-1;
    int step(bool v) noexcept {const int x=v?1:0;if(x==told)return -1;const bool first=told<0;told=x;return first&&!x?-1:x;}
};
// counts since the last report (game/control threads add, the service takes them)
struct Counts {std::uint32_t all[Count]{},tutor[Count]{};};
inline Counts take() noexcept {Counts c{};
    for(unsigned i=0;i<Count;++i){c.all[i]=skipped[i].exchange(0,std::memory_order_relaxed);c.tutor[i]=tutorSkipped[i].exchange(0,std::memory_order_relaxed);}
    return c;}
inline bool any(const Counts& c) noexcept {for(unsigned i=0;i<Count;++i)if(c.all[i])return true;return false;}
// "name=all/tutor,..." (tutor "-" = the feature has no NPC identity at its write) of the features that skipped at least one write; "none" when nothing. returns the text length.
inline int formatCounts(char* out,std::size_t cap,const Counts& c) noexcept {
    if(!out||!cap)return 0;
    out[0]=0;std::size_t at=0;
    for(unsigned i=0;i<Count;++i){if(!c.all[i])continue;
        char tutor[16]{'-'};
        if(knowsActor(i))std::snprintf(tutor,sizeof(tutor),"%u",c.tutor[i]);
        const int n=std::snprintf(out+at,cap-at,"%s%s=%u/%s",at?",":"",featureName(i),c.all[i],tutor);
        if(n<0||std::size_t(n)>=cap-at)break;
        at+=std::size_t(n);}
    if(!at){std::snprintf(out,cap,"none");return 4;}
    return int(at);}
// the held list of the edge row: every gated feature (all of them hold off while active)
inline int formatHeld(char* out,std::size_t cap) noexcept {
    if(!out||!cap)return 0;
    out[0]=0;std::size_t at=0;
    for(unsigned i=0;i<Count;++i){const int n=std::snprintf(out+at,cap-at,"%s%s",at?",":"",featureName(i));
        if(n<0||std::size_t(n)>=cap-at)break;
        at+=std::size_t(n);}
    return int(at);}
// always-on tutor rows: at most one per kTutorRowMs while active (+ the final one at the OFF edge), and only with new counts
constexpr std::uint64_t kTutorRowMs=30000;
inline bool tutorRowDue(bool counted,std::uint64_t last,std::uint64_t now) noexcept {return counted&&(!last||now<last||now-last>=kTutorRowMs);}
}
