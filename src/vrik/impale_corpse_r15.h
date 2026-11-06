#pragma once
// N2 (Claude, 26 Oct 2026, Miki: "impalements... make them work"): impale an ALREADY DEAD body with a stab.
// No native hit receipt exists for a corpse stab (native combat does not hit corpses), so nothing native is
// faked here. The source is the weapon's own completed physical contact (ImpactRecord178r30: the blade sweep
// hit an articulated body, technique THRUST, at this speed) plus the game's OWN death proof for that exact
// entity in this lifetime (gore32::KnownDead, from the native lethal receipt). The latch then takes the same
// hang path a killing stab takes (root Ready via ImpaleRoot47, victim wait, hold60). R93R3: the shared VrImpaleHang menu option owns admission.
#include <atomic>
#include <cmath>
#include <cstdint>
#include "../core/snapshot_mailbox.h"
#include "strike_feature_d41.h"

namespace kcd2vr::impale_corpse_r15 {
inline std::atomic<bool> enabled{true};          // legacy saved key, retained for compatibility; no separate admission gate
inline std::atomic<float> refSpeed{6.f};         // stroke power = min(1, speed / refSpeed), same scale as the C43T corpse cut
struct Candidate {
    std::uint64_t serial=0,tick=0,key=0,generation=0,epoch=0,request=0;
    std::uint32_t target=0;std::uintptr_t identity=0,collider=0;int part=-1,bodyMaterial=-1,side=-1,feature=0,weaponClass=-1;
    double speed=0,power=0,point[3]{},normal[3]{};bool valid=false;
    double tipDepth96=std::nan(""); // R97: tip under the touched surface (m, strike_feature_d41::tipDepth96), NaN unknown
};
inline core::SnapshotMailbox<Candidate> latest;
enum class Why:int {Ok=0,Off,NotThrust,NotArticulated,NotKnownDead,TooWeak,Stale,Invalid,NotOwned,NotPiercingFeature};
inline const char* name(Why w) noexcept {
    switch(w){case Why::Ok:return "OK";case Why::Off:return "OFF";case Why::NotThrust:return "NOT_A_THRUST";
    case Why::NotArticulated:return "NOT_A_RAGDOLL_BODY";case Why::NotKnownDead:return "NOT_KILLED_BY_THE_GAME_THIS_LIFETIME";
    case Why::NotPiercingFeature:return "NO_PROVEN_PIERCING_TIP";
    case Why::TooWeak:return "TOO_WEAK";case Why::Stale:return "STALE";case Why::NotOwned:return "WEAPON_NOT_OWNED";default:return "INVALID";}
}
inline double power(double speed) noexcept {
    const double r=double(refSpeed.load(std::memory_order_relaxed));
    if(!std::isfinite(speed)||speed<=0||!(r>0))return 0;
    return std::min(1.0,speed/r);
}
// kind: ImpactRecord technique (0 cut, 1 thrust). articulated: record body == Articulated. knownDead: gore32::KnownDead
// for this entity identity now. needEnergy: VrImpaleMinEnergy. ageMs: now - record tick.
inline Why qualify(bool on,int kind,bool articulated,bool knownDead,double speed,double needEnergy,std::uint64_t ageMs,
                   bool owned,const double point[3]) noexcept {
    if(!on)return Why::Off;
    if(!point||!std::isfinite(point[0])||!std::isfinite(point[1])||!std::isfinite(point[2])||!std::isfinite(speed))return Why::Invalid;
    if(ageMs>200)return Why::Stale;
    if(!owned)return Why::NotOwned;
    if(kind!=1)return Why::NotThrust;
    if(!articulated)return Why::NotArticulated;
    if(!knownDead)return Why::NotKnownDead;
    if(power(speed)<(std::isfinite(needEnergy)?needEnergy:.3))return Why::TooWeak;
    return Why::Ok;
}
// The physical producer additionally joins the actual struck feature. The
// old motion-only qualifier remains a diagnostic helper, never admission.
// R97: a BLADE contact counts only with the tip under the touched surface (see strike_feature_d41::canPierce).
inline Why qualifyPhysicalD41(bool on,int kind,int feature,int weaponClass,bool articulated,bool knownDead,
    double speed,double needEnergy,std::uint64_t ageMs,bool owned,const double point[3],double tipDepth=std::nan("")) noexcept {
    const auto prior=qualify(on,kind,articulated,knownDead,speed,needEnergy,ageMs,owned,point);
    if(prior!=Why::Ok)return prior;
    return strike_feature_d41::canPierce(feature,weaponClass,tipDepth)?Why::Ok:Why::NotPiercingFeature;
}
// Weapon side: a fresh candidate of THIS weapon (same key/generation, owner epochs) that was not consumed yet.
inline bool admissible(const Candidate& c,std::uint64_t now,std::uint64_t key,std::uint64_t generation,
                       std::uint64_t epoch,std::uint64_t request,std::uint64_t consumed) noexcept {
    return c.valid&&strike_feature_d41::canPierce(c.feature,c.weaponClass,c.tipDepth96)&&c.serial&&c.serial!=consumed&&c.target&&c.identity&&c.collider&&c.key==key&&c.generation==generation&&
        c.epoch==epoch&&c.request==request&&c.tick&&now>=c.tick&&now-c.tick<=250&&
        std::isfinite(c.point[0])&&std::isfinite(c.point[1])&&std::isfinite(c.point[2]);
}
// N2 review (Codex P1): the admitted candidate travels BY VALUE with the weapon request (first request and every
// waiting re-publication). The game thread consumes that request-owned copy, never the global latest mailbox,
// which a later contact B may already have replaced. Identity only - the consumer keeps every live check.
inline bool requestOwned(const Candidate& carried,std::uint64_t serial,std::uint32_t entity,int side) noexcept {
    return carried.valid&&strike_feature_d41::canPierce(carried.feature,carried.weaponClass,carried.tipDepth96)&&carried.serial&&carried.serial==serial&&carried.target&&carried.target==entity&&
        carried.identity&&carried.collider&&carried.side==side&&
        std::isfinite(carried.point[0])&&std::isfinite(carried.point[1])&&std::isfinite(carried.point[2]);
}
}
