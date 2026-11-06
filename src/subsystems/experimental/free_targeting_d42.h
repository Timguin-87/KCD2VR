#pragma once
// D42: explicit camera-only combat preference. Opponent selection, targets,
// attack dispatch and RPG outcomes remain owned by the native game.
#include <array>
#include <atomic>
#include <cstdint>
namespace kcd2vr::free_targeting_d42 {
inline std::atomic<bool> enabled{false},contextReady{false},cameraLeased{false};
inline std::atomic<std::uint64_t> generation{1};
inline void set(bool on) noexcept {
    if(enabled.exchange(on,std::memory_order_acq_rel)!=on)
        generation.fetch_add(1,std::memory_order_acq_rel);
}
// Cumulative counters only advance under F3 at the actual queued-command
// boundary. No additional native reads, formatting or allocation in that hook.
// The service records session-start baselines and prints deltas, including all
// refusal buckets: a retained native command is never called a successful veto.
inline std::atomic<std::uint64_t> rotationCalls{0},rotationVetoes{0};
inline std::array<std::atomic<std::uint64_t>,64> rotationRefused{};
// R103: Henry's native combat phase when a rotation was kept because of the
// phase gate (bit n = phase 1<<n, bit 31 = phase 0/unknown). Only written
// while F3 records, read by the F3 summary row.
inline std::atomic<std::uint32_t> keptPhaseMask{0};
inline std::atomic<int> lastRefusedPhase{-1};
inline void recordRotation(bool veto,unsigned reason) noexcept {
    rotationCalls.fetch_add(1,std::memory_order_relaxed);
    if(veto)rotationVetoes.fetch_add(1,std::memory_order_relaxed);
    else rotationRefused[reason<rotationRefused.size()?reason:rotationRefused.size()-1].fetch_add(1,std::memory_order_relaxed);
}
inline std::uint32_t phaseBit(int phase) noexcept {
    if(phase<=0)return 1u<<31;
    for(unsigned i=0;i<31;++i)if(phase==(1<<i))return 1u<<i;
    return 1u<<31;
}
// R103 free targeting policy. Native phases of Henry's S_CombatActorState+0x38
// (native_combat104.h / combat_state_zone_d85.h): 1/2 neutral+guard,
// 4 readyToStrike, 8 strike, 16 recovery, 32 after a strike/hit/block,
// 64 Henry's own hit reaction (stagger), 128 block prepare, 256 block / free block.
// Attack phases keep the old proof (exact live ordinary slash/thrust/punch
// main action, so masterstrike/kick/clinch rows stay native).
inline bool attackPhase(int phase) noexcept {return phase==4||phase==8||phase==16;}
// Ordinary non-attack phases where the free-targeting switch now also stops
// the automatic turn toward the locked opponent. 0/unknown and 64 (stagger,
// the game plays a directional hit reaction) stay native. Paired actions
// (finishers, clinch, masterstrike/riposte pairs, death, knockdown) are
// excluded separately by the protected-body census in every phase.
inline bool guardPhase(int phase) noexcept {
    return phase==1||phase==2||phase==32||phase==128||phase==256;
}
// +218 pending request type: none, stab, slash/block. A pending special
// (masterstrike, kick, clinch...) keeps the native turn.
inline bool guardType(int type) noexcept {return type==-1||type==0||type==1;}
inline bool freeRotationPhase(int phase,int type) noexcept {
    return attackPhase(phase)||(guardPhase(phase)&&guardType(type));
}
}
