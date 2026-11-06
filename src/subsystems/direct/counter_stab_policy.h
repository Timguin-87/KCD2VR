#pragma once
// C40 COUNTER STAB (Miki 17 Oct: "fake masterstrikes" in DIRECT - a strong stab from the opposite side into the enemy's attack start;
// he reacts like after a stab, pushed away, bleeding). Pure policy + receipt helpers, no native calls. DIRECT experiment, default OFF.
// Native facts (WHGame 6A350E20 static + C37C2 log), see README_C40.md:
// - reaction kind of ONE hit = C52DF4(attacker,hitInfo+0x44 tier,strong=C52E0C): strong&&tier<5 ? [5320F18] : [5320F1C]. Log C37C2:
//   tier 0/3 (stamina only) -> kind 1, tier 5 (health) -> kind 2, 35/25 GEOM_OUTER = combat_hit_type 1 minor(hitTypeLow) / 2 major(hitTypeNormal).
// - tier = C_RPGUtils vfunc+0x10 (7261DC): health damage -> 5 (6 from 50 hp, 7 lethal), stamina only -> 2/3/4 (10/50).
// - the victim clip comes from combat_action_hit by tags: major stab rows (CombatHitGen ..oppStab+dZ5+hitTypeNormal) move the victim
//   0.7 m back (max_point 0;-0.7;0), minor = CombatHitTorso (flinch, 0 m). chargedAttack comes from HENRY's soul charged RPG state
//   (687CC4 = soul+0x1D0 vfunc+0x90, cvar wh_cs_ChargedAttackOverride), not from the hit: never written.
//   => RPG damage stays in the original strength submission. A qualified completed counter may additionally request
//      the existing MAJOR reaction row by C52DF4 flagA=false, without writing a tier/property or submitting another hit.
// - bleeding: the RPG hit itself adds heavy_bleeding (buff ae48b141, 30 s, ibi*0.75) as C_AddBuffEffect (9DC874, C626DC->C627E8) when
//   HENRY's soul has ability 40/75/81/85/90 (+ the hit's own conditions). No direct native AddBuff entry / thread contract is proven:
//   receipt only (bleeding=NOT_ADDED + which of those native abilities Henry has).
#include "../../vrik/direct_counter39.h"
#include "../../vrik/master_gate47.h" // R120: INSTANT master strike grant (taken by counter40::Decide)
#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstddef>
#include <cstdint>
namespace kcd2vr::counter_stab40 {
inline std::atomic<bool> enabled{true};                        // INI VrDirectCounterStab (COMBAT FLOW > COUNTER STAB); C44: default ON, perk-gated
// C44 (Miki: "masterstrikes only after Henry learns them (native perk)"): VrDirectCounterStabNeedsPerk - the counter stab needs
// the game's own "Master strike" perk (perk.xml 2993585c-40c9-42e3-ac45-b837f3bc50f7 -> perk_soul_ability -> soul_ability 73
// "MasterStrike"), read from Henry's soul ability table the way the heavy-bleed receipt reads it. Unreadable table = not learned.
inline std::atomic<bool> needsPerk{true};
inline constexpr std::uint32_t kMasterStrikeAbility=73,kMasterStrikeUnarmedAbility=9;
inline std::atomic<float> minEnergy{.6f};                      // INI VrDirectCounterStabEnergy: stroke energy / your reference stroke
inline std::atomic<float> strengthMul{1.5f};                   // INI VrDirectCounterStabStrength: x the stroke power, capped by the power rule
inline std::atomic<unsigned> windowMs{500};                    // INI VrDirectCounterStabWindowMs: after his attack START (phase 4/8 entered)
inline float clampEnergy(float v) noexcept {return std::isfinite(v)?std::clamp(v,.2f,2.f):.6f;}
inline float clampMul(float v) noexcept {return std::isfinite(v)?std::clamp(v,1.f,2.f):1.5f;}
inline unsigned clampWindow(int v) noexcept {return unsigned(std::clamp(v,150,1000));}
enum class Why : unsigned char {Qualified,Disabled,NotThrust,NoWindow,OtherTarget,NotAttacking,Late,Side,LowEnergy,NoPerk};
inline const char* name(Why w) noexcept {
    constexpr const char* n[]{"QUALIFIED","DISABLED","NOT_A_THRUST","NO_ATTACK_WINDOW","WINDOW_OF_ANOTHER_ENEMY","ENEMY_NOT_ATTACKING","LATE_IN_HIS_ATTACK","NOT_OPPOSITE_SIDE","LOW_ENERGY","MASTER_STRIKE_NOT_LEARNED"};
    return n[unsigned(w)<10?unsigned(w):0];
}
struct In {
    bool enabled=false,thrust=false,windowOpen=false,sameTarget=false;
    int npcPhase=-1,origin=-1,offered=-1;float ratio=0.f,minRatio=.6f;long long sinceStartMs=-1;unsigned windowMs=500;
    bool needsPerk=false,perk=false; // C44: perk gate (perk = Henry's soul has MasterStrike 73, or the native all-abilities flag)
};
// Order = what Miki fixes first: a stab, into HIS attack start, from the opposite side (C39 counterSide = native master-strike stance), hard enough.
inline Why qualify(const In& in) noexcept {
    if(!in.enabled)return Why::Disabled;
    if(in.needsPerk&&!in.perk)return Why::NoPerk;
    if(!in.thrust)return Why::NotThrust;
    if(!in.windowOpen)return Why::NoWindow;
    if(!in.sameTarget)return Why::OtherTarget;
    if(in.npcPhase!=4&&in.npcPhase!=8)return Why::NotAttacking;
    if(in.sinceStartMs<0||static_cast<unsigned long long>(in.sinceStartMs)>in.windowMs)return Why::Late;
    if(!direct_counter39::counterSide(in.origin,in.offered))return Why::Side;
    if(!std::isfinite(in.ratio)||in.ratio<in.minRatio)return Why::LowEnergy;
    return Why::Qualified;
}
// A receipt is worth a line only when the contact went into this enemy's attack (window or attack phase).
inline bool candidate(const In& in) noexcept {return in.thrust&&((in.windowOpen&&in.sameTarget)||in.npcPhase==4||in.npcPhase==8);}
// DS-F81 (Claude combat plan item 1): the master "OUR OUTCOME". When the
// native master window is CLOSED but a real opposite-side stab landed on an
// attacking enemy, request the MAJOR reaction row (heavy stagger, no paired
// clip) through reactionSelectorFlag - never a fabricated masterstrike, no
// window is invented. The native window open => let native master run
// (qualify() above), not this fallback.
inline Why counterOutcome(const In& in) noexcept {
    if(!in.enabled)return Why::Disabled;
    if(in.needsPerk&&!in.perk)return Why::NoPerk;
    if(!in.thrust)return Why::NotThrust;
    if(in.windowOpen)return Why::NoWindow; // the native window IS open: qualify() owns it
    if(!in.sameTarget)return Why::OtherTarget;
    if(in.npcPhase!=4&&in.npcPhase!=8)return Why::NotAttacking;
    if(!direct_counter39::counterSide(in.origin,in.offered))return Why::Side;
    if(!std::isfinite(in.ratio)||in.ratio<in.minRatio)return Why::LowEnergy;
    return Why::Qualified;
}
// Never lower than the stroke power, never above the power rule's final ceiling.
inline float boosted(float power,float mul,float cap) noexcept {
    if(!std::isfinite(power)||power<=0.f)return power;
    const float m=clampMul(mul);const float c=std::isfinite(cap)&&cap>0.f?cap:power;
    return std::max(power,std::min(power*m,c));
}
// A contact counter requests the native major stab reaction only AFTER its
// ONE native hit result is known. This changes reaction selection, not RPG
// damage/tier, a native master-strike window, recipe state or Henry's animation.
// The caller already owns the exact contact, target and accepted RPG transaction.
// 'kind' here is the CONTACT/STROKE kind (1 = thrust), NOT the separate native
// reaction-kind output of C52DF4 (1=minor/2=major, hitTypeName below).
template<class NativeResult> inline bool majorReaction(Why qualification,int kind,bool bodyContact,bool apply,
    std::uint32_t expectedTarget,const NativeResult& r) noexcept {
    return qualification==Why::Qualified&&kind==1&&bodyContact&&apply&&expectedTarget&&
        r.seen&&r.ok&&r.fields&&r.target==expectedTarget&&r.death==0&&
        r.blocked==0&&r.perfect==0&&r.parry==0&&r.master==0&&
        std::isfinite(r.dmg)&&std::isfinite(r.stam)&&r.dmg>=0&&r.stam>=0&&(r.dmg>0||r.stam>0);
}
// C52DF4's flagA=false selects its native major row. Ordinary hits retain the
// original C52E0C result. No BA0 property/header, damage or tier field is written.
inline bool reactionSelectorFlag(bool nativeFlagA,bool qualifiedContact) noexcept {
    return nativeFlagA&&!qualifiedContact;
}
inline const char* hitTypeName(int kind) noexcept {return kind==1?"MINOR(hitTypeLow)":kind==2?"MAJOR(hitTypeNormal)":"NONE";}
// Soul ability table (soul+0x320 {data,count}, 16-byte {id,value} rows sorted by id), read the way 9DCB78->9DCC70 reads it.
struct AbilityRow {std::uint32_t id;std::uint32_t pad;std::uint64_t value;};
static_assert(sizeof(AbilityRow)==16,"native ability row");
inline bool hasAbility(const AbilityRow* rows,std::size_t n,std::uint32_t id) noexcept {
    if(!rows||!n)return false;
    const auto e=rows+n;
    const auto it=std::lower_bound(rows,e,id,[](const AbilityRow& r,std::uint32_t v){return r.id<v;});
    return it!=e&&it->id==id;
}
// soul_ability.xml ids that make the native hit add heavy_bleeding (C627E8): HorrendousWounds, AimToMaim, CuttingEdge,
// CauseAlwaysHeavyBleeding, ForbiddenWeapon.
inline constexpr std::uint32_t kHeavyBleedAbility[5]{40,75,81,85,90};
inline unsigned heavyBleedBits(const AbilityRow* rows,std::size_t n) noexcept {
    unsigned b=0;for(unsigned i=0;i<5;++i)if(hasAbility(rows,n,kHeavyBleedAbility[i]))b|=1u<<i;return b;
}
inline constexpr const char* kBleedRefusal="NO_PROVEN_NATIVE_ADDBUFF_ENTRY(heavy_bleeding ae48b141 is added only inside the RPG hit, C627E8->9DC874)";
}
