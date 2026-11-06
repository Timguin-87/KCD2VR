#pragma once
// REPAIR2: qualified cuts and stabs share the native window/opposite-origin rule.
// Native eligibility/outcomes remain authoritative. Stroke-matched victim reaction
// selection is a separate owner, not implemented by this admission correction.
// Native facts (WHGame 6A350E20, static; see MASTERSTRIKE_COMBO_RESEARCH_C47.md):
// - the window = Henry state+0x7D8, a counted property opened/closed by C_CombatTriggerMasterStrike (vtable 183B81528,
//   slots +0x70 1899CF4 -> 909FC4 count+1, +0x78 1899D1C -> 909FF4 count-1). Its slot +0x90 (90A59C) asks 908DA0(kind 5),
//   which for a sword-family weapon (909B4C; the R59 family gate adds 3/5/6/7 for Henry) asks the soul for ability 0x49 = 73
//   "MasterStrike" (perk 2993585c "Master strike", autolearnable=false): no perk -> the window never opens. The perk is NATIVE.
// - D4AD94 (attack input decision, prepare D4AB37 / release D4AA62): at D4ADBF cmp [state+7D8],0; jg 22B780B -> 1483540
//   (kind 5 -> 1483580 -> factory 1483A70 -> paired action 55/62 -> 90C850 dispatch) and then ALWAYS returns 1 (D4AE52), so
//   both callers skip the ordinary attack. A factory refusal (the old VrMasterStrikeVeto at 1483A90) therefore EATS the press.
// - the C47 deny point is the cold block 22B780B itself: execution resumes at D4ADCC, the instruction right after the jg, i.e.
//   exactly the retail path for a closed window (combo slot D4AE70 -> C5AC2C, special input D4AE88, else return 0 = the
//   ordinary attack). Nothing is written: no counter, no flag, no action, no damage.
#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdio>
namespace kcd2vr::master_gate47 {
// Legacy saved key VrMasterStrikeStabOnly: 0 native, 1 qualified CUT/STAB from
// the opposite origin (default), 2 paired off. Preserve saved modes; mode1 now
// includes cuts. Native window, perk and outcome are never forced.
// R120 (Miki 6 Nov: "the impale masterstrike was native, not instant/free"): mode 3 = INSTANT. the game still
// grants it (perk + its own open window, the same qualified cut/stab as mode 1), but the paired clip is NOT started:
// the press resumes at D4ADCC (the retail closed-window path = his ordinary attack, same as mode 2) and the
// granted hit is handed to his VR blade - the next blade contact on that same enemy within kInstantGrantMs lands
// at full strike power (counter_stab_native.inl Decide), a stab also asks the native MAJOR stab reaction.
// saved as VrMasterStrikeModeR120 (0..3); VrMasterStrikeStabOnly keeps 0..2 for older builds.
inline constexpr int kModeInstant=3;
inline std::atomic<int> mode{1};
inline std::atomic<float> minEnergy{.30f}; // Legacy saved value retained for roundtrip/telemetry; no additional energy gate.
inline std::atomic<bool> testForce{false}; // DS-F5 VrMasterStrikeTestForce: TEST receipts (C68 MS_TEST) at every attack press with the window/gate state. Receipts only - never forces a native window, stance or outcome.
inline constexpr int kModeCount=4; // R120: 0 native, 1 opposite cut/stab, 2 paired off, 3 INSTANT
inline int clampMode(int v) noexcept {return v<0?0:v>=kModeCount?kModeCount-1:v;}
inline float clampEnergy(float v) noexcept {return std::isfinite(v)?std::clamp(v,0.f,1.f):.30f;}
inline constexpr double kThrustReferenceMs=3.0; // V66.178R30: thrust/pommel energy reference speed (scale = speed / 3 m/s)
enum class Stroke : unsigned char {Unknown,Cut,Thrust,Pommel};
inline const char* name(Stroke s) noexcept {constexpr const char* n[]{"UNKNOWN","CUT","THRUST","POMMEL"};return n[unsigned(s)<4?unsigned(s):0];}
enum class Reason : unsigned char {Qualified,PairedOff,NoPerk,NotThrust,LowEnergy,SideUnknown,NotOpposite,WindowQualified,StabFamilyD70}; // I10: WindowQualified = side not checked (relaxed)
inline const char* name(Reason r) noexcept {
    constexpr const char* n[]{"OPPOSITE_CUT_OR_STAB","PAIRED_MASTER_STRIKE_OFF","MASTER_STRIKE_NOT_LEARNED","UNSUPPORTED_STROKE","LOW_ENERGY","SIDE_UNKNOWN","NOT_OPPOSITE_SIDE",
        "CUT_OR_STAB_IN_NATIVE_WINDOW_SIDE_UNCHECKED","EXPERIMENT_NON_STAB_REACTION_FAMILY"};
    return n[unsigned(r)<9?unsigned(r):0];
}
// combat_zone.xml master_strike_combat_zone_id: the zone a master strike answers an attack from zone z with.
inline int masterZone(int z) noexcept {switch(z){case 0:return 5;case 1:return 2;case 2:return 1;case 5:return 0;case 3:return 4;case 4:return 3;default:return -1;}}
inline bool attackZone(int z) noexcept {return z==0||z==1||z==2||z==5;}
inline bool opposite(int origin,int enemyZone) noexcept {return attackZone(origin)&&attackZone(enemyZone)&&masterZone(enemyZone)==origin;}
struct In {
    int mode=1;Stroke stroke=Stroke::Unknown;float energy=0.f,minEnergy=.30f;int origin=-1,enemyZone=-1;
    int perk=-1; // -1 unreadable (the native window opener still checks it), 0 not learned, 1 learned
    bool stabFamiliesD70=false; // appended: preserve the original aggregate prefix
};
// This stroke has already qualified. Do not impose a second energy threshold.
// The hook observes the actual native open-window branch. Unknown origin leaves
// an ordinary attack; no window is created and no native acceptance is forced.
inline Reason decide(const In& in) noexcept {
    if(in.mode==2)return Reason::PairedOff;
    if(in.perk==0)return Reason::NoPerk;
    if(in.stabFamiliesD70&&in.enemyZone==1&&in.mode!=kModeInstant)return Reason::StabFamilyD70; // R120: no paired clip in INSTANT, so no family swap
    if(in.stroke!=Stroke::Thrust&&in.stroke!=Stroke::Cut)return Reason::NotThrust;
    if(!attackZone(in.origin)||!attackZone(in.enemyZone))return Reason::SideUnknown;
    if(!opposite(in.origin,in.enemyZone))return Reason::NotOpposite;
    return Reason::Qualified;
}
inline bool allowed(Reason r) noexcept {return r==Reason::Qualified||r==Reason::WindowQualified;}
// R120: what the cold block does with this verdict. paired = the retail kind-5 request (paired clip);
// instant = the press goes the closed-window way AND the blade gets the granted master strike.
inline bool pairedClip(int mode,Reason r) noexcept {return mode!=kModeInstant&&allowed(r);}
inline bool instantGrant(int mode,Reason r) noexcept {return mode==kModeInstant&&allowed(r);}
// R120 INSTANT grant: written by the gate hook only when the native window was really open for this VR press
// (= the game admitted the master strike), read once by the blade contact on the same enemy. no latch: a grant
// that is not used expires after kInstantGrantMs, the next window makes a new one.
inline constexpr std::uint64_t kInstantGrantMs=1500;
struct InstantGrant {std::uint64_t serial=0,tick=0;std::uintptr_t targetActor=0,targetState=0;unsigned char stroke=0;};
inline std::atomic<std::uint64_t> grantSerial{0},grantTick{0},grantUsed{0};
inline std::atomic<std::uintptr_t> grantTargetState{0};
inline std::atomic<unsigned> grantStroke{0};
inline std::atomic<std::uint64_t> grantsMade{0},grantsUsed{0};
inline bool grantFits(std::uint64_t serial,std::uint64_t used,std::uint64_t tick,std::uintptr_t state,std::uintptr_t ts,std::uint64_t now) noexcept {
    return serial&&serial!=used&&tick&&ts&&state==ts&&now>=tick&&now-tick<=kInstantGrantMs;}
inline void publishGrant(std::uintptr_t targetState,Stroke s,std::uint64_t now) noexcept {
    if(!targetState)return;
    grantTargetState.store(targetState,std::memory_order_relaxed);grantStroke.store(unsigned(s),std::memory_order_relaxed);
    grantTick.store(now?now:1,std::memory_order_relaxed);grantSerial.store(grantsMade.fetch_add(1,std::memory_order_relaxed)+1,std::memory_order_release);}
// one blade contact takes the grant (compare-exchange on the used serial: two contacts never both get it)
inline bool takeGrant(std::uintptr_t ts,std::uint64_t now,std::uint64_t* taken=nullptr) noexcept {
    const auto serial=grantSerial.load(std::memory_order_acquire);auto used=grantUsed.load(std::memory_order_acquire);
    if(!grantFits(serial,used,grantTick.load(std::memory_order_relaxed),grantTargetState.load(std::memory_order_relaxed),ts,now))return false;
    if(!grantUsed.compare_exchange_strong(used,serial,std::memory_order_acq_rel))return false;
    grantsUsed.fetch_add(1,std::memory_order_relaxed);if(taken)*taken=serial;return true;}
// R120 full strike power for the granted hit: the power rule's own ceiling, never lower than the stroke.
inline float instantPower(float power,float cap) noexcept {
    if(!std::isfinite(power)||power<=0.f)return power;
    return std::isfinite(cap)&&cap>power?cap:power;}
// I10 (R14 lane CB, Miki: "masterstrike working, verbatim or not"): the native window (+7D8) only opens while Henry's stance already
// answers the attacker's master zone (C566D8), so the extra VR-origin side test denied the one real attempt of the R14 run
// (00:39:21.604 side=1/5 NOT_OPPOSITE) and MS never fired. Default: inside the open native window a qualified cut/stab is a
// master strike whatever the measured origin. VrMasterStrikeOppositeSide=true restores the strict opposite-origin rule.
inline std::atomic<bool> requireOpposite{false};
// D30-CMB VrMasterStrikeCue (default on): J27 run - the native window (+7D8) opened ONCE in a whole fight and closed after
// 180 ms (00:20:18.227 OPEN -> .407 CLOSE) with no VR press inside it; the 4 eligibility clusters gave no cue either. the
// gamepad game shows the master-strike shield; VR gets one strong pulse on the weapon hand the moment the native window
// opens, so the player can strike inside it. no window/stance/press write - the game still decides.
inline std::atomic<bool> cue{true};
inline bool cueOnOpen(bool enabled,bool openEdge,std::uint64_t serial,std::uint64_t lastCued) noexcept {return enabled&&openEdge&&serial&&serial!=lastCued;}
// the relaxed pass is labelled WindowQualified, so a receipt never claims a measured opposite-side strike. It only passes
// this extra mod gate; the native window, perk, kind-5 dispatch and outcome stay native.
inline Reason sideRule(Reason r,bool strict) noexcept {return !strict&&(r==Reason::NotOpposite||r==Reason::SideUnknown)?Reason::WindowQualified:r;}
// C49 review: the RELEASE edge of a gesture whose PREPARE edge was already allowed keeps that verdict. The native kind-5 request
// already ran at prepare; a release-edge denial (its own effort scale can be lower than the prepare speed) would send the retail
// release press down the closed-window / ordinary-attack path behind the paired master strike. Mode 2 (never) still refuses.
inline Reason commitReason(Reason fresh,bool preparedAllowedSameGesture) noexcept {
    return preparedAllowedSameGesture&&fresh!=Reason::PairedOff&&!allowed(fresh)?Reason::Qualified:fresh;}
// Energy of the attempt in one unit: the release edge carries the stroke's own energy scale (effortScale19); the prepare edge
// only has the hand velocity, so its forward speed is scaled the way R30 scales a thrust (speed / 3 m/s).
inline float prepareEnergy(double vx,double vy,double vz) noexcept {
    const double s=std::sqrt(vx*vx+vy*vy+vz*vz);return std::isfinite(s)?float(std::min(s/kThrustReferenceMs,1.25)):0.f;}
// Soul ability table rows (soul+0x320 {data,count}, 16-byte {id,value} sorted by id), as C44 reads them.
struct AbilityRow {std::uint32_t id;std::uint32_t pad;std::uint64_t value;};
static_assert(sizeof(AbilityRow)==16,"native ability row");
inline constexpr std::uint32_t kMasterStrikeAbility=73;
// Binary search with a caller-supplied row reader (bounded, sorted-table check per probe).
template<class ReadRow> int hasAbility(std::size_t n,std::uint32_t id,ReadRow readRow) noexcept {
    if(n>512)return -1;std::size_t lo=0,hi=n;
    while(lo<hi){const std::size_t mid=lo+(hi-lo)/2;AbilityRow r{};if(!readRow(mid,r))return -1;
        if(r.id==id)return 1;if(r.id<id)lo=mid+1;else hi=mid;}
    return 0;
}
struct Receipt {
    std::uint64_t attempt=0,tick=0,ageMs=0,denied=0,allowedN=0;Stroke stroke=Stroke::Unknown;float energy=0.f,minEnergy=0.f;
    int origin=-1,enemyZone=-1,window=-1,perk=-1,mode=1;bool prepare=true,xi=false;Reason reason=Reason::Qualified;
};
inline const char* perkName(int p) noexcept {return p==1?"LEARNED":p==0?"NOT_LEARNED":"UNKNOWN";}
inline int format(const Receipt& r,char* out,std::size_t cap) noexcept {
    if(!out||!cap)return 0;
    const bool ok=allowed(r.reason);
    return std::snprintf(out,cap,
        "C47 MS_GATE attempt=%llu edge=%s stroke=%s energy=%.2f min=%.2f side=%d/%d(%s) window=%d perk=%s mode=%d transport=%s ageMs=%llu "
        "verdict=%s reason=%s route=%s totals(allowed/denied)=%llu/%llu writes=NONE",
        (unsigned long long)r.attempt,r.prepare?"PREPARE":"COMMIT",name(r.stroke),double(r.energy),double(r.minEnergy),r.origin,r.enemyZone,
        !attackZone(r.origin)||!attackZone(r.enemyZone)?"UNKNOWN":opposite(r.origin,r.enemyZone)?"OPPOSITE":"NOT_OPPOSITE",
        r.window,perkName(r.perk),r.mode,r.xi?"XI":"DIRECT",(unsigned long long)r.ageMs,ok?"ALLOWED":"DENIED",name(r.reason),
        r.mode==kModeInstant&&ok?"INSTANT_D4ADCC_ORDINARY_ATTACK_BLADE_GRANTED":ok?"NATIVE_KIND5_PAIRED":"D4ADCC_ORDINARY_ATTACK_PATH",(unsigned long long)r.allowedN,(unsigned long long)r.denied);
}
// (b) KILL MOVE rule for the gore lane (C47 research, Miki: "a kill move with the swing for the neck decapitates instead of
// impaling"). Only a NATIVE death of this hit counts; the struck surface comes from the native hit (body_material2subpart:
// human 1..5 head, 38 head_neck; body_subpart 23..28 head_* / head_neck). The native attack zone 0 ("head", BONEID_HEAD) is a
// fallback only when the surface is unknown: a side swing at the neck is natively zone 1/2 (upper_left/right), not 0.
enum class KillMove : unsigned char {None,Impale,Decapitate};
inline const char* name(KillMove k) noexcept {constexpr const char* n[]{"NONE","IMPALE","DECAPITATE"};return n[unsigned(k)<3?unsigned(k):0];}
struct KillIn {bool nativeDeath=false;Stroke stroke=Stroke::Unknown;float energy=0.f,decapMin=.9f,impaleMin=0.f;int material=-1,subpart=-1,nativeZone=-1;};
inline bool headNeckSurface(int material,int subpart) noexcept {return (material>=1&&material<=5)||material==38||(subpart>=23&&subpart<=28);}
inline KillMove killMove(const KillIn& k) noexcept {
    if(!k.nativeDeath||!std::isfinite(k.energy))return KillMove::None;
    if(k.stroke==Stroke::Thrust)return k.energy>=k.impaleMin?KillMove::Impale:KillMove::None;
    if(k.stroke!=Stroke::Cut||k.energy<k.decapMin)return KillMove::None;
    const bool surfaceKnown=k.material>=0||k.subpart>=0;
    return (surfaceKnown?headNeckSurface(k.material,k.subpart):k.nativeZone==0)?KillMove::Decapitate:KillMove::None;
}
}
