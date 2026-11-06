#pragma once
#include "../core/snapshot_mailbox.h"
#include "strike_feature_d41.h"
#include "impale_corpse_r15.h"
#include <cmath>
#include <array>
#include <atomic>
#include <cstdint>
#include <limits>
#include <mutex>

// Completed, same-stack native results. A gesture, ray, created reaction or
// requested animation is not an accepted hit. Pointer values are opaque identity
// tokens; only the native game thread may revalidate and dereference them.
namespace kcd2vr::contact58c1 {
struct Receipt {
    std::uint64_t serial=0,tick=0,impactTick=0,key=0,generation=0,epoch=0,request=0,sample=0;
    std::uint32_t target=0;
    int kind=-1,rawPart=-1,nativePart=-1,material=-1,origin=-1;
    int blocked=-1,perfect=-1,parry=-1,master=-1,death=-1,stabbed=-1,combo=-1,comboEnd=-1,dispatch=-1;
    bool seen=false,accepted=false,applied=false,fields=false,bodyContact=false;
    float damage=0,stamina=0,point[3]{};
    // C7G: same-stack contact attribution, including the direct VR backend.
    std::uint64_t attempt=0;std::uintptr_t targetIdentity=0,collider=0;
    int bodyMaterial=-1,feature=0;bool twoHands=false,continuation=false,direct=false;int weaponClass=-1;bool shield=false;
    float energy=-1.f; // C44: our stroke energy ratio of this contact (Pending powerRatio59c8, 1 = the reference stroke; < 0 unknown)
    float thrustSpeed96=-1.f; // R96: fastest blade contact speed (m/s) of THIS thrust gesture on THIS target, < 0 unknown
    float thrustTipDepth96=std::numeric_limits<float>::quiet_NaN(); // R97: deepest tip-under-surface (m) of THIS thrust on THIS target, NaN unknown
};
// R96: the queued hit carries the FIRST touch of a thrust (4 Nov 18:06:59 serial 11: 1.45 m/s queued, the 4.01 m/s push
// 12 ms later was EXACT_CONTACT_ALREADY_QUEUED). Damage keeps using the first touch; only the impale threshold reads the
// fastest contact of the same gesture+target, recorded here by the impact producer (frik_pose_probe GEOM_IMPACT).
// R97: the same row also keeps the deepest tip position under the touched surface (strike_feature_d41::tipDepth96).
struct ThrustPeak96 {std::uint64_t gesture=0,tick=0;std::uint32_t target=0;float speed=0;float depth=std::numeric_limits<float>::quiet_NaN();};
inline std::array<ThrustPeak96,8> thrustPeaks96{};inline std::mutex thrustPeaksMutex96;
inline void noteThrust96(std::uint64_t gesture,std::uint32_t target,double speed,std::uint64_t tick,double tipDepth=std::nan("")) noexcept {
    if(!gesture||!target||!std::isfinite(speed)||speed<=0)return;
    std::lock_guard<std::mutex> lock(thrustPeaksMutex96);ThrustPeak96* slot=nullptr;
    for(auto& p:thrustPeaks96)if(p.gesture==gesture&&p.target==target){slot=&p;break;}
    if(!slot){slot=&thrustPeaks96[0];for(auto& p:thrustPeaks96)if(p.tick<slot->tick)slot=&p;*slot={gesture,tick,target,0.f};}
    if(float(speed)>slot->speed)slot->speed=float(speed);if(tick>slot->tick)slot->tick=tick;
    if(std::isfinite(tipDepth)&&(!std::isfinite(slot->depth)||float(tipDepth)>slot->depth))slot->depth=float(tipDepth);
}
inline float thrustPeakSpeed96(std::uint64_t gesture,std::uint32_t target,std::uint64_t now) noexcept {
    if(!gesture||!target)return -1.f;std::lock_guard<std::mutex> lock(thrustPeaksMutex96);
    for(const auto& p:thrustPeaks96)if(p.gesture==gesture&&p.target==target&&now>=p.tick&&now-p.tick<=1000)return p.speed;
    return -1.f;
}
inline float thrustTipDepth96(std::uint64_t gesture,std::uint32_t target,std::uint64_t now) noexcept {
    if(!gesture||!target)return std::numeric_limits<float>::quiet_NaN();std::lock_guard<std::mutex> lock(thrustPeaksMutex96);
    for(const auto& p:thrustPeaks96)if(p.gesture==gesture&&p.target==target&&now>=p.tick&&now-p.tick<=1000)return p.depth;
    return std::numeric_limits<float>::quiet_NaN();
}
inline bool piercing97(const Receipt& r) noexcept {return strike_feature_d41::canPierce(r.feature,r.weaponClass,double(r.thrustTipDepth96));}
inline core::SnapshotMailbox<Receipt> latest;
inline std::atomic<bool> experiment{false};
// C44 IMPALE ON EVERY KILLING STAB (Miki): a native LETHAL thrust impales when the stroke had at least this energy
// (VrImpaleKillEnergy, 0 = every killing thrust). The C41 log had no lethal thrust at all (7 thrusts, 0..5 damage vs
// armour; the 10 wounding hits were cuts), so the kill itself stays the game's decision - this only filters pokes.
inline std::atomic<float> impaleKillEnergy44{.3f};
// Unknown energy (< 0: a receipt without the stroke ratio) is not filtered - the native kill already happened, this only trims pokes.
// R96: VrImpaleKillEnergy is measured like VrImpaleMinEnergy (corpse stab): thrust power = fastest contact speed / 6 m/s.
// The old E/Eref ratio is relative to Miki's full CUT (~75 J); a hard 4 m/s thrust is ~0.02..0.18 on that scale, so no
// killing stab could reach 0.30 (4 Nov 18:06:59: energy=0.02 need=0.30). Unknown speed keeps the old ratio test.
inline bool strongEnough44(const Receipt& r,float need) noexcept {
    if(!(need>0.f))return true;
    if(std::isfinite(r.thrustSpeed96)&&r.thrustSpeed96>0.f&&impale_corpse_r15::power(r.thrustSpeed96)>=double(need))return true;
    return !std::isfinite(r.energy)||r.energy<0.f||r.energy>=need;
}
inline float thrustPower96(const Receipt& r) noexcept {
    return std::isfinite(r.thrustSpeed96)&&r.thrustSpeed96>0.f?float(impale_corpse_r15::power(r.thrustSpeed96)):r.energy;}
inline bool strongEnough44(const Receipt& r) noexcept {return strongEnough44(r,impaleKillEnergy44.load(std::memory_order_relaxed));}
// C2 diagnostics only: first refusal reason for the EXISTING predicate below.
// This never makes a non-eligible receipt eligible and authorizes no physics write.
enum class LethalThrustRefusalC2 : unsigned { None,Identity,NativeResult,BodyContact,Continuation,
    NotThrust,Defended,NotLethal,Damage,ItemOwner,ReceiptAge,ImpactAge,ContactPoint,NotPiercingFeature };
inline const char* name(LethalThrustRefusalC2 r) noexcept {
    constexpr const char* n[]{"ELIGIBLE","IDENTITY_OR_PART","NATIVE_RESULT_UNACCEPTED","NOT_BODY_CONTACT","CONTINUATION_UNCONFIRMED",
        "NOT_THRUST","BLOCKED_OR_DEFENCE_UNKNOWN","NOT_LETHAL","NON_POSITIVE_OR_INVALID_DAMAGE","ITEM_OR_OWNER_CHANGED",
        "RECEIPT_OLDER_THAN_80MS_OR_FUTURE","IMPACT_RESULT_JOIN_OLDER_THAN_600MS_OR_FUTURE","INVALID_CONTACT_POINT","NO_PROVEN_PIERCING_TIP"};
    return n[unsigned(r)];
}
inline LethalThrustRefusalC2 lethalThrustReasonC2(const Receipt& r,std::uint64_t now,std::uint64_t key,
                         std::uint64_t generation,std::uint64_t epoch,std::uint64_t request) noexcept {
    using R=LethalThrustRefusalC2;
    if(!r.serial||!r.attempt||!r.target||!r.targetIdentity||!r.collider||r.rawPart<0)return R::Identity;
    if(!r.seen||!r.accepted||!r.applied||!r.fields)return R::NativeResult;
    if(!r.bodyContact)return R::BodyContact;
    if(!r.continuation)return R::Continuation;
    if(r.kind!=1)return R::NotThrust;
    if(!piercing97(r))return R::NotPiercingFeature;
    if(r.blocked!=0||r.perfect!=0||r.parry!=0)return R::Defended;
    if(r.death!=1)return R::NotLethal;
    if(!std::isfinite(r.damage)||r.damage<=0)return R::Damage;
    if(r.key!=key||r.generation!=generation||r.epoch!=epoch||r.request!=request)return R::ItemOwner;
    if(!r.tick||now<r.tick||now-r.tick>80)return R::ReceiptAge;
    if(!r.impactTick||r.tick<r.impactTick||r.tick-r.impactTick>600)return R::ImpactAge;
    if(!std::isfinite(r.point[0])||!std::isfinite(r.point[1])||!std::isfinite(r.point[2]))return R::ContactPoint;
    return R::None;
}
inline bool lethalThrust(const Receipt& r,std::uint64_t now,std::uint64_t key,
                         std::uint64_t generation,std::uint64_t epoch,std::uint64_t request) noexcept {
    return r.serial&&r.attempt&&r.target&&r.targetIdentity&&r.collider&&r.rawPart>=0&&r.continuation&&
        r.seen&&r.accepted&&r.applied&&r.fields&&r.bodyContact&&r.kind==1&&
        piercing97(r)&&
        r.blocked==0&&r.perfect==0&&r.parry==0&&r.death==1&&
        std::isfinite(r.damage)&&r.damage>0&&r.key==key&&r.generation==generation&&
        r.epoch==epoch&&r.request==request&&r.tick&&now>=r.tick&&now-r.tick<=80&&
        r.impactTick&&r.tick>=r.impactTick&&r.tick-r.impactTick<=600&&
        std::isfinite(r.point[0])&&std::isfinite(r.point[1])&&std::isfinite(r.point[2]);
}
}
