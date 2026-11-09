#pragma once
#include <atomic>
#include <cstdint>

namespace kcd2vr::npc_defense {
// One multiplier for defenses the native NPC has already selected. This does
// not select a defense, change Henry's windows, or qualify a player attack.
inline std::atomic<float> speed{1.f};      // legacy VrNpcDefenseSpeed: >1 = old shared ETA+dodge path
// D34 per-type NPC keys. Dodge is a pure animation-rate change (native accept,
// exit restore untouched). Block/parry speeds only act with raiseExperiment:
// the native follower 9E3B80 retargets the clip peak (contact) to the ETA it
// reads, so a shortened ETA moves the contact earlier (I10: 0 perfect blocks).
inline std::atomic<float> blockSpeed{1.4f};
inline std::atomic<float> parrySpeed{1.33f};
inline std::atomic<float> dodgeSpeed{1.25f};
inline std::atomic<bool> raiseExperiment{false};
// R146 (Miki 8 Nov: "npc level/skill should decide the speed of their evasions (only that)"): with
// VrDirectNpcBlockBySkill ON the enemy's own best melee skill picks his block raise, parry raise and dodge
// clip speed between VrDirectNpcBlockSpeedLow (skill 0) and VrDirectNpcBlockSpeedHigh (skill 30), the same
// linear map as C39. It then replaces VrNpcDefenseSpeed and the 3 per-type rows. Nothing else uses the skill.
inline std::atomic<bool> bySkill{false};
inline std::atomic<float> skillLo{2.f},skillHi{3.f};
inline float ValidSpeed(float v) noexcept {return v==v&&v>1.f&&v<=3.f?v:1.f;}
inline void SetBySkill(bool on,float atSkill0,float atSkill30) noexcept {
    skillLo.store(ValidSpeed(atSkill0),std::memory_order_relaxed);skillHi.store(ValidSpeed(atSkill30),std::memory_order_relaxed);
    bySkill.store(on,std::memory_order_release);
}
// skill 0 -> Low, skill >= 30 -> High, unread skill (-1) -> Low. Always 1..3.
inline float SkillFactor(int skill) noexcept {
    const float lo=ValidSpeed(skillLo.load(std::memory_order_relaxed)),hi=ValidSpeed(skillHi.load(std::memory_order_relaxed));
    if(skill<0)return lo;
    const float t=skill>=30?1.f:float(skill)/30.f;
    return ValidSpeed(lo+(hi-lo)*t);
}
inline float DodgeFactor() noexcept {
    const float legacy=ValidSpeed(speed.load(std::memory_order_relaxed));
    return legacy>1.f?legacy:ValidSpeed(dodgeSpeed.load(std::memory_order_relaxed));
}
inline float RaiseFactor(bool parry) noexcept {
    const float legacy=ValidSpeed(speed.load(std::memory_order_relaxed));
    if(legacy>1.f)return legacy;
    if(!raiseExperiment.load(std::memory_order_relaxed))return 1.f;
    return ValidSpeed((parry?parrySpeed:blockSpeed).load(std::memory_order_relaxed));
}
// Install wanted when any effective factor changes native behaviour.
inline bool Wanted() noexcept {
    if(bySkill.load(std::memory_order_acquire))return ValidSpeed(skillLo.load(std::memory_order_relaxed))>1.f||ValidSpeed(skillHi.load(std::memory_order_relaxed))>1.f;
    return DodgeFactor()>1.f||RaiseFactor(false)>1.f||RaiseFactor(true)>1.f;
}
bool Install(std::uintptr_t base) noexcept;
bool Ready() noexcept;
const char* Failure() noexcept;
void Shutdown() noexcept;
}
