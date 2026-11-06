#pragma once
// C4R4 F9 baseline observer. The old global hit-reaction/impulse multiplier
// experiment is retired: it amplified every NPC and stacked with explicit
// contact impulses. Entries remain readable for comparison, not writable
// targets. F9 weapon/corpse experiments have separate native owners; this
// observer does not enable NPC skeleton physics or overwrite their reactions.
// Legacy compare-owned restoration is retained; unreadable/foreign values
// are never overwritten. No save-load reassertion of global physics values.
#include <cmath>
#include <cstdint>
#include <cstddef>

namespace kcd2vr::ragdoll59 {
enum class Kind : unsigned char { Absolute, Multiply, AtMost, AtLeast }; // R59.16C10R7: AtLeast = raise, never lower
struct Entry { const char* name; Kind kind; float value; float floor; };
// Historical target catalog retained for source compatibility and read-only
// comparison. Apply ignores the old VrRagdollImpactScale multiplier.
inline constexpr Entry kEntries[]{
    {"wh_HitReaction_Enabled",                 Kind::Absolute, 1.f,  0.f},
    {"wh_HitReaction_CollisionsEnabled",       Kind::Absolute, 1.f,  0.f},
    {"g_hitDeathReactions_disableRagdoll",     Kind::Absolute, 0.f,  0.f},
    {"wh_HitReaction_PhysicalHitCoef",         Kind::Multiply, 0.f,  .5f},   // value = INI impact scale
    {"wh_cs_PhysHitReaction_ImpactCoef",       Kind::Multiply, 0.f,  .5f},
    {"wh_cs_PhysWeaponHitReaction_ImpactCoef", Kind::Multiply, 0.f,  .5f},
    {"wh_HitReaction_EnvironmentCollisionScale",Kind::Multiply,0.f,  .5f},
    {"es_ImpulseScale",                        Kind::Multiply, 0.f,  .5f},
    {"ca_DeathBlendTime",                      Kind::AtMost,   .15f, 0.f},   // never slower than retail (the 2026-10-11 log: retail 0)
    {"p_joint_damage_accum",                   Kind::Absolute, 2.f,  0.f},   // the mod's value
    {"es_MinImpulseVel",                       Kind::Absolute, .25f, 0.f},
    // R59.14: the character-physics switches the KCD1 "Semi Functional Ragdoll Physics" mod turns on
    // through its engine physics spec. All three exist in this exact WHGame build (string table), and
    // These names are now observed only; the old write targets below are historical.
    //  ca_physicsProcessImpact    : physical impacts on LIVING, animated characters (the flinch path;
    //                               KCD1 mod uses 3, and 4 at its highest spec)
    //  ca_ApplyJointVelocitiesMode: the body keeps its joint velocities when it becomes a ragdoll
    //                               (momentum carry-over instead of a dead drop)
    //  ca_UseRecoil               : per-joint physical recoil on characters
    {"ca_physicsProcessImpact",                Kind::Absolute, 3.f,  0.f},
    {"ca_ApplyJointVelocitiesMode",            Kind::Absolute, 1.f,  0.f},
    {"ca_UseRecoil",                           Kind::Absolute, 1.f,  0.f},   // R59.6: the es_ImpulseScale branch in the entity impulse path (2366E11) is only reached when this is > 0
    // R59.16C10R7: the rest of the KCD1 mod's physics spec that still exists in KCD2 (135 of its 142 CVars do; KCD2
    // ships NO sys_spec_Physics group, so these run at code defaults). Only switches that make hits/impulses reach
    // bodies; nothing that costs frame time (p_max_MC_iters, e_PhysProxyTriLimit) or changes the world (p_gravity_z).
    //  es_HitCharacters / es_HitDeadBodies : entity hits apply physics to living characters / corpses
    //  es_UpdateCollisionScript            : collision events reach entity scripts
    //  e_DeferredPhysicsEvents             : physics events are queued instead of dropped under load
    //  ca_UsePhysics                       : character (skeleton) physics on
    {"es_HitCharacters",                       Kind::Absolute, 1.f,  0.f},
    {"es_HitDeadBodies",                       Kind::Absolute, 1.f,  0.f},
    {"es_UpdateCollisionScript",               Kind::Absolute, 1.f,  0.f},
    {"e_DeferredPhysicsEvents",                Kind::Absolute, 1.f,  0.f},
    {"ca_UsePhysics",                          Kind::Absolute, 1.f,  0.f},
};
// R59.16C10R7: read-only startup baseline (never written): what this game actually runs with, so tuning starts
// from facts. Includes the KCD1 values deliberately NOT applied, for comparison.
// The KCD1 author forbids porting his files/values to KCD2: only engine-generic switch names are used here, never
// his tuned numbers. es_MaxImpulseAdjMass is logged, not set, until our own measurement picks a value.
inline constexpr const char* kBaselineOnly[]{"es_MaxImpulseAdjMass","p_max_MC_iters","p_max_substeps","p_gravity_z","e_PhysProxyTriLimit",
    "e_OnDemandPhysics","ca_UseJointMasking","es_MaxPhysDist","es_MaxPhysDistInvisible","es_UsePhysVisibilityChecks","pl_serialisePhysVel"};
inline constexpr std::size_t kCount=sizeof(kEntries)/sizeof(kEntries[0]);

// D51: C43T's separate global impact/recoil/fall threshold lease is retired.
// An old selection cannot acquire or reassert it. Only an actually owned value
// can be restored, with a fresh comparison and verification. Uncertain IO keeps
// restoration debt but has a finite budget; foreign writers are never fought.
enum class RetireResultD51 : unsigned char {
    Inactive, AlreadyOriginal, Restored, ForeignValue, ReadUnavailable,
    WriteRefused, VerifyUnavailable, RetryLimit, InvalidSavedValue
};
struct RetiredGlobalLeaseD51 {
    const char* name;bool owned=false;float original=0,wrote=0;unsigned attempts=0;
    static constexpr unsigned kAttempts=3;
    template<class IO> RetireResultD51 Retire(IO& io) noexcept {
        if(!owned)return RetireResultD51::Inactive;
        if(!name||!std::isfinite(original)||!std::isfinite(wrote)){
            owned=false;return RetireResultD51::InvalidSavedValue;
        }
        if(attempts>=kAttempts)return RetireResultD51::RetryLimit;
        ++attempts;float current=0;
        if(!io.Read(name,current)||!std::isfinite(current))return RetireResultD51::ReadUnavailable;
        if(current==original){owned=false;return RetireResultD51::AlreadyOriginal;}
        if(current!=wrote){owned=false;return RetireResultD51::ForeignValue;}
        if(!io.Write(name,original))return RetireResultD51::WriteRefused;
        float after=0;
        if(!io.Read(name,after)||!std::isfinite(after))return RetireResultD51::VerifyUnavailable;
        if(after==original){owned=false;return RetireResultD51::Restored;}
        if(after!=wrote){owned=false;return RetireResultD51::ForeignValue;}
        return RetireResultD51::WriteRefused;
    }
};

struct Slot { bool present=false,owned=false; float original=0,wanted=0; };
class Lease {
    bool on_=false; Slot slots_[kCount]{};
public:
    bool On() const noexcept { return on_; }
    const Slot& slot(std::size_t i) const noexcept { return slots_[i]; }
    static float target(const Entry& e,float live,float impactScale) noexcept {
        if(e.kind==Kind::Absolute)return e.value;
        if(e.kind==Kind::AtMost)return std::isfinite(live)&&live<e.value?live:e.value;
        if(e.kind==Kind::AtLeast)return std::isfinite(live)&&live>e.value?live:e.value;
        const float scale=std::isfinite(impactScale)&&impactScale>0?impactScale:1.f;
        const float base=std::isfinite(live)&&live>e.floor?live:e.floor;
        return base*scale;
    }
    // A legacy owned slot may only be restored while its value is still ours.
    template<class IO> static bool Restore(IO& io,const Entry& e,Slot& s) noexcept {
        if(!s.owned)return false;float live=0;
        if(!io.Read(e.name,live)||!std::isfinite(live)||std::fabs(live-s.wanted)>1e-4f){s.owned=false;return false;}
        const bool wrote=io.Write(e.name,s.original);s.owned=false;return wrote;
    }
    // IO keeps the historical interface, but ON now captures a read-only
    // baseline. impactScale is deliberately ignored, including old INI x3.
    template<class IO> unsigned Apply(IO& io,bool want,float) noexcept {
        unsigned restored=0;
        if(want&&!on_){
            for(std::size_t i=0;i<kCount;++i){auto& s=slots_[i];
                if(Restore(io,kEntries[i],s))++restored;
                s={};float live=0;if(!io.Read(kEntries[i].name,live)||!std::isfinite(live))continue;
                s.present=true;s.original=live;s.wanted=live;
            }
            on_=true;return restored;
        }
        if(!want&&on_){for(std::size_t i=0;i<kCount;++i)if(Restore(io,kEntries[i],slots_[i]))++restored;on_=false;}
        return restored;
    }
    template<class IO> unsigned Reassert(IO&) noexcept {return 0;}
};
}
