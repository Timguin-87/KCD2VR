#pragma once
// D85 zone: what the S_CombatActorState zone fields really are (WHGame 0x6A350E20, Steam 1.5.6).
// there is no name registration for these properties in WHGame (C_ModelProperty<int,...> with
// C_CombatModelNoTrace, no strings), so the meaning comes from the native writers and readers:
//
//  attack start  C_CombatActionHelperAttack slot 7 = 0x938FD0 (vtable 0x3A61040):
//      params = [helper+0x20]->vfn1()   (S_CombatActorActionAttackParams, vtable 0x3A61540)
//      state+0x150 <- params slot1  0x1A7D2D0 = [row+0x140]->id   attack_zone_id   (raw row 0xD4)
//      state+0x210 <- params slot10 0x1A7D2B0 = [row+0x148]->id   attack_type_id   (raw row 0xD8)
//      state+0x270 <- params slot11 0x1A7D200 = [row+0x150]->id   input_class_id   (raw row 0xDC)
//      (row floats follow in the same order: +0x158 time_to_start (0xE0) ... +0x164 time_to_end (0xEC))
//  attack end    0x93A1BC: 0x150 <- its default, 0x210 <- -1, 0x270 <- -1
//  so +0x270 is the INPUT CLASS (0 light, 1 heavy, 2 special; combat_input_class.xml), never a zone.
//  that is why the old "executedZone" was 1 or -1 in every row: player attack rows are input class 1.
//  +0x150 is the attack_zone_id of the native attack row that is running, in the row's own frame.
//
//  combo manager phase observer 0x911250 (bound to the phase signal state+0x48 at 0x10F0B6A):
//      phase 4 (readyToStrike): comboZone <- state+0x180, "advanced" flag +0xEA cleared
//      phase 8 (strike), state+0x360 false: comboZone <- combat_zone[state+0x150].symetric_combat_zone_id
//                                         (zone row +0x7C = raw 0x74 symetric_combat_zone_id)
//      phase 32: advance (0xC5C52C) unless the hit flag state+0x480 is already set
//  hit flag delegate 0xC5B404: state+0x480 -> true advances (0xC5C52C) once per attack (+0xEA).
//  advance 0xC5C594 -> 0x91338C: a candidate step at the cursor matches when
//      (step InputClass == -1 || == state+0x270) && (step GuardZone == -1 || == comboZone)
//  CombatCombos__generated.xml has no InputClass on any of its 165 steps, so only GuardZone counts.
//
// so the zone the game really executed, in Henry's own frame (the frame of state+0xF0/+0x180 and of
// the recipe GuardZone), is symmetric(state+0x150). the 2 Nov D81R1 F3 log agrees: at 30 of 31 hybrid
// attack starts state+0x150 == symmetric(state+0xF0), and state+0xF0 == the zone we sent in 27 of 31.
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <cstdio>
namespace kcd2vr::zone_d85 {
constexpr std::uint32_t kRequestedZone=0x0F0;  // Henry frame, written by the native zone input (NativeZone104)
constexpr std::uint32_t kCommittedZone=0x180;  // Henry frame, read by the combo manager at phase 4 and by 9397CC
constexpr std::uint32_t kRowAttackZone=0x150;  // running attack row attack_zone_id (row frame); D4921C reaction zone
constexpr std::uint32_t kRowAttackType=0x210;  // running attack row attack_type_id (0 stab, 1 slash, ...)
constexpr std::uint32_t kRowInputClass=0x270;  // running attack row input_class_id (0 light, 1 heavy, 2 special)

// combat_zone.xml symetric_combat_zone_id: 0->0, 1->2, 2->1, 3->4, 4->3, 5->5. -1 and unknown stay unknown.
constexpr int symmetric(int zone) noexcept {
    switch(zone){case 0:return 0;case 1:return 2;case 2:return 1;case 3:return 4;case 4:return 3;case 5:return 5;default:return -1;}
}
// the zone the native attack executed, in Henry's frame (what the combo manager stores at phase 8)
constexpr int executedHenryZone(int rowAttackZone) noexcept {return symmetric(rowAttackZone);}

// 0x911250: the combo zone kept by Henry's combo manager after a phase change
constexpr int comboZoneAfterPhase(int phase,int previous,int committed,int rowAttackZone,bool flag360) noexcept {
    if(phase==4)return committed;
    if(phase==8&&!flag360)return symmetric(rowAttackZone);
    return previous;
}
// 0x91338C: one recipe step at the cursor
struct Step { int guardZone=-1,inputClass=-1; };
constexpr bool stepMatches(Step s,int comboZone,int inputClass) noexcept {
    return (s.inputClass==-1||s.inputClass==inputClass)&&(s.guardZone==-1||s.guardZone==comboZone);
}

// D85: the direct route publishes an attack context on Henry's state. +0x270 is the input class, so a
// zone number must never be written there (zone 2 would read as "special", 5 is no input class at all).
// the context keeps whatever input class the native already has.
template<class SetInt,class SetFloat>
bool writeAttackContext(SetInt setInt,SetFloat setFloat,bool sameStage,bool technique,int zone,
                        std::uint32_t attackType,std::uint32_t strength,std::uint32_t pending,std::uint32_t attackZone,
                        std::uint32_t committed) noexcept {
    if(!sameStage&&!setInt(attackType,technique?0:1))return false;
    if(!sameStage&&!setFloat(strength,1.f))return false;
    return setInt(pending,zone)&&setInt(attackZone,zone)&&setInt(committed,zone);
}

// the zone our native release carried, for the F3 ZONE row (one value, newest gesture wins)
inline std::atomic<std::uint64_t> sentPacked{0};
inline void noteSent(std::uint64_t gesture,int zone) noexcept {
    if(!gesture||gesture>(~0ull>>8)||zone<-1||zone>250)return;
    sentPacked.store((gesture<<8)|std::uint64_t(zone+1),std::memory_order_release);
}
inline int sentFor(std::uint64_t gesture) noexcept {
    const auto v=sentPacked.load(std::memory_order_acquire);
    return gesture&&(v>>8)==gesture?int(v&0xFF)-1:-1;
}

struct Snapshot { int requested=-99,committed=-99,rowZone=-99,inputClass=-99; };
inline const char* verdict(int sent,int executed) noexcept {
    if(executed<0)return "NO_NATIVE_ATTACK_ZONE";
    if(sent<0)return "SENT_UNKNOWN";
    return sent==executed?"NATIVE_USED_SENT":"NATIVE_DIFFERS_FROM_SENT";
}
// one F3 row per strike result: where the swing came from, what we sent, what the game committed and ran
inline int formatRow(char* out,std::size_t cap,std::uint64_t now,std::uint64_t gesture,const char* route,
                     int origin,int sent,const Snapshot& s) noexcept {
    const int executed=executedHenryZone(s.rowZone);
    return std::snprintf(out,cap,"D85 F3 ZONE tick=%llu gesture=%llu route=%s origin=%d sent=%d requested=%d committed=%d executed=%d "
        "rowZone=%d inputClass=%d verdict=%s originMatch=%d frame=HENRY executedFrom=SYMMETRIC_STATE_0x150",
        (unsigned long long)now,(unsigned long long)gesture,route?route:"?",origin,sent,s.requested,s.committed,executed,
        s.rowZone,s.inputClass,verdict(sent,executed),origin>=0&&executed>=0?(origin==executed?1:0):-1);
}
}
