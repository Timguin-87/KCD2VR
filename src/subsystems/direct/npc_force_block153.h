#pragma once
#include <atomic>
#include <cstdint>
// R153 NPC DEFENCE BY SKILL (Miki 8 Nov: "make the block or evade or parry for the NPC more often - now I could cut
// through the easier enemies like butter - not one managed to block or evade").
// R146 BY SKILL (npc_defense.h) only makes a defence the NPC has ALREADY chosen faster. If the native NPC never chooses
// one, nothing is sped up. The 8 Nov run (F4 press 1, 46 native strikes, 5 kills) had no defence at all.
// The choice itself is the game's own switch wh_cs_ForceBlock ("Force AI to block by: 0 disabled, 1 normal block,
// 2 perfect block, ..."), the C39 lever. With BY SKILL ON (or VrDirectNpcBlock 1..3) one roll is made for Henry's NEXT
// swing against his current opponent (exact reciprocal C_CombatActor pair, as C39/H24 read it):
//   chance = 25 % at the opponent's best melee skill 0 .. 75 % at skill >= 30 (VrDirectNpcBlock 1: 65 %, 2/3: always).
// A won roll leases wh_cs_ForceBlock = 1 (mode 3: 2) during that swing only (R156: wind-up + strike, never between swings),
// then it is rolled again. A lost roll
// leaves the native value. The lease is written BEFORE the swing starts, so the native block offer of that swing sees it.
// The native block itself (animation, raise speed from R146, direction, result, Henry's own block) is unchanged.
// Off F4 combat / no reciprocal opponent / stale data: the original value is put back. No latch, every tick re-decides.
// R156 (Miki 8 Nov, R155 build: the prologue sparring with Capon - "hit him 4 times" with the wooden sword - never ended):
// the opponent of a scripted sparring / lesson is not an enemy, and "Force AI to block" must never be put on him. The mod
// cannot read faction hostility (no such read exists in the mod), so the gate is Henry's own weapon: a practice / quest
// copy (R136: alias definition without its own weight = the tutorial wooden sword, `V66.98 ITEM ... defSource=ALIAS_SOURCE
// weightDefault=1`) in either hand = no roll, native value kept, for ANY VrDirectNpcBlock mode and for BY SKILL alike.
// A real weapon drawn again = the roll runs again on the next tick (no latch). Published by the equipment snapshot
// (native_equipment_snapshot.inl PhysRefreshEquipment103, game thread), read by the control thread.
namespace kcd2vr::npc_force_block153 {
inline std::atomic<bool> practiceWeapon{false}; // R156: a practice / quest alias weapon is in Henry's hand
inline std::atomic<std::uint64_t> practiceWeaponMs{0}; // R159: when practiceWeapon was last published (tutorial_native_r159.h freshness)
// R156: held melee item whose definition is an alias (quest / tutorial copy) without a usable weight of its own.
inline bool practiceWeaponHeld(bool held,bool melee,bool weightDefault,int defSource) noexcept {return held&&melee&&weightDefault&&defSource==1;}
// Game thread (Henry's own C_CombatActor update) -> control thread. Plain values, no game pointers are dereferenced
// by the reader. opponent is used only as an identity for "same opponent".
inline std::atomic<std::uintptr_t> opponent{0};
inline std::atomic<int> skill{-1},henryPhase{-1};
inline std::atomic<bool> reciprocal{false};
// R164: the opponent's planar world position (his CEntity world TM +58: x at +0C, y at +1C), read in the same Henry update;
// opponentPos=false when he or his entity is not proven. read by the R164 counter-step stand-off (counter_step_r160.h).
inline std::atomic<bool> opponentPos{false};
inline std::atomic<float> opponentX{0.f},opponentY{0.f};
inline std::atomic<std::uint64_t> publishedMs{0};
constexpr std::uint64_t kFreshMs=500;
constexpr float kChanceSkill0=.25f,kChanceSkill30=.75f,kChanceMode1=.65f;

// Best of the five melee weapon skills (sword, heavy, large, dagger, unarmed; 0..40). -1 = unread.
inline int bestSkill(const int (&skills)[5]) noexcept {int best=-1;for(const int s:skills)if(s>=0&&s<=40&&s>best)best=s;return best;}
// Henry's own ordinary attack is running (native phase 4 wind-up or 8 strike): the lease must not change under it.
inline bool attacking(int phase) noexcept {return phase==4||phase==8;}
// mode = VrDirectNpcBlock (0 native, 1 often, 2 always block, 3 always perfect block).
inline float chance(int mode,bool bySkill,int skill) noexcept {
    if(mode==2||mode==3)return 1.f;
    if(mode==1)return kChanceMode1;
    if(!bySkill)return 0.f;
    if(skill<0)return kChanceSkill0;
    const float t=skill>=30?1.f:float(skill)/30.f;
    return kChanceSkill0+(kChanceSkill30-kChanceSkill0)*t;
}
inline int forceValue(int mode) noexcept {return mode==3?2:1;}
// R155: the lease may exist only in an actual fight. R153 gated it on the F4 combat MODE alone (a sticky mode, on for the
// whole session), so a quest NPC that had Henry as his reciprocal combat target while a scripted scene ran (confrontation,
// arranged fight, guard stop) met a forced block between Henry's swings. Now: F4 combat mode on AND Henry's own native
// inCombat flag seen 1 within the last second (HenryInCombat178r31: -1 unknown / stale drops it too) AND no interactive
// dialogue, cutscene, Theater screen or menu. Anything else = the original value is written back on that tick.
inline bool fightActive(bool consoleReady,bool f4Combat,int henryInCombat,bool dialogue,bool cutscene,bool theater,bool menu) noexcept {
    return consoleReady&&f4Combat&&henryInCombat==1&&!dialogue&&!cutscene&&!theater&&!menu;
}
// One step of the per-swing decision. roll01 in [0,1) is only consumed when a new roll is made.
// Returns the wanted wh_cs_ForceBlock value: 0 = do not own it (native value), 1/2 = lease it.
// R156: the lease is written ONLY while Henry's own swing runs (native phase 4 wind-up / 8 strike), never between swings.
// wh_cs_ForceBlock is a GLOBAL cvar for every AI: held between swings (R153-R155, 65 % of the time in his prologue run)
// it also forced Rosa and Capon, who were scripted to walk up and talk, into a block = they jittered in the window and
// the scene never continued. the won roll is decided before the swing and applied the moment phase 4 begins (the NPC's
// block decision comes during the wind-up), then given back the tick the swing ends.
struct Machine {
    bool rolled=false,sawAttack=false;int value=0;std::uintptr_t target=0;int rolls=0,won=0;
    int step(bool active,int mode,bool bySkill,std::uintptr_t opp,bool recip,int oppSkill,int phase,float roll01) noexcept {
        if(!active||!opp||!recip||chance(mode,bySkill,oppSkill)<=0.f){rolled=false;sawAttack=false;value=0;target=0;return 0;}
        if(attacking(phase)){sawAttack=true;return value;}              // the lease exists only during the swing (R156)
        if(rolled&&sawAttack){rolled=false;sawAttack=false;}             // that swing is over: next roll
        if(rolled&&opp!=target)rolled=false;                              // another opponent between swings
        if(!rolled){rolled=true;sawAttack=false;target=opp;++rolls; // R154: a swing seen before the first roll must not end this roll at once
            const bool win=roll01<chance(mode,bySkill,oppSkill);
            value=win?forceValue(mode):0;won+=win?1:0;}
        return 0; // R156: between swings the native value stands - every AI (also a scripted quest NPC) is free
    }
};
// Small deterministic generator for the roll (control thread only).
inline float nextRoll(std::uint32_t& state) noexcept {
    state^=state<<13;state^=state>>17;state^=state<<5;if(!state)state=0x9E3779B9u;
    return float(state>>8)*(1.f/16777216.f);
}
}
