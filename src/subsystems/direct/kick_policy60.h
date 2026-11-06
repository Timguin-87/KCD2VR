#pragma once
#include <cmath>
#include <cstdint>
// DS-F80/F83 (Claude impale/kick plan B3; Codex correction): the kick reach
// gate. The native kick (attack type 4) has cannot_kill=true in the tables,
// but Codex correctly flags that cannot_kill does NOT prove stamina-only
// damage (a nonlethal can still reduce health). The tier clamp is therefore
// NOT applied until Claude proves whether type-4 cannot_kill means zero-health
// or a nonlethal floor. Pure policy + the visible-line reason, no native calls.
namespace kcd2vr::kick60 {
inline bool isKick(int attackType) noexcept {return attackType==4;}
// NOT-PROVEN (Codex H19): do NOT clamp a kick's tier without native proof.
// Kept as an explicit pass-through + documentation until Claude answers the
// type-4 damage-path question.
inline int tier(int attackType,int nativeTier) noexcept {(void)attackType;return nativeTier;}
inline bool reachOk(float distanceM) noexcept {return std::isfinite(distanceM)&&distanceM>=0.f&&distanceM<=1.0f;}
// The ONE visible line names the exact refused step (Miki's rule). null = admit.
inline const char* refusalReason(bool reach,bool footProxyValid) noexcept {
    if(!footProxyValid)return "Kick refused: no foot proxy";
    if(!reach)return "Kick refused: target out of reach";
    return nullptr;
}
}
