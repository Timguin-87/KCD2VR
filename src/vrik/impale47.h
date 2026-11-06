#pragma once
// C47 IMPALE + DECAP (Miki 18 Oct, C46 log: "impalements were not working, nothing happened, also no decapitations").
// Pure logic, host tested (tests/impale47_test.cpp). Native sides: vrik/physical_weapon.h (weapon admission),
// vrik/impale_part59c7g.inl + part_anchor59c7g.inl (ragdoll wait + anchor), vrik/gore32.inl (decap zone).
//  - the C46 log had exactly one killing thrust (00:22:38 serial 84, IMPALE_REQUESTED) and after it no candidate,
//    latch or hang line. the weapon side only latched when the CURRENT blade tip was within 12 cm of the native
//    contact point; the receipt comes 60..80 ms after the impact (hybrid waits for the native strike), by then the
//    blade had moved 16..26 cm (GEOM_IMPACT .500/.622), so the request was dropped without a word.
//    now: within 12 cm = the contact point (old), else (hang mode) the blade point nearest to the contact, 10..40 cm
//    behind the tip, as long as the contact is within 60 cm of the tip and 35 cm of the blade line.
//  - the game thread waits (20 Hz, up to4.5s in H18) for the death ragdoll. C52 and
//    later require the ORIGINAL hit collider to be the current articulated root
//    (vt 3A48B30) and retain its exact raw part. Same entity foreign data alone
//    does not prove a successor-body/part map; no nearest-part fallback is used.
//    H18 additionally admits a NEW contact on the CURRENT corpse's exact part;
//    it never transfers the killing hit's part to that replacement body.
#include <algorithm>
#include <cmath>
#include <atomic>
#include <cstdint>
#include <cstddef>
#include <cstdio>

namespace kcd2vr::impale47 {
// H18: original lethal-victim wait only. These do not extend the current pose,
// tracking, contact, native input, or completed-hit freshness contracts.
inline constexpr std::uint64_t kCorpseWaitH18=4500,kWeaponWaitH18=4700;
// I10 (R14-IM): physics foreign data of an NPC body. KCD2 tags the entity's own bodies with type 2 (not the CryEngine
// stock 1): H50 run 25 Oct 10:40:06 C47 ROOT_OBSERVE root vft 3A48B30 rootForeignKind=2 rootForeign=1C3F8BBD800
// = entity of target 45105 (F3-ENRICH entityCandidate, idReadback 45105); R25 accepts the same for F9 corpse drag.
// type 1 kept. data must be exactly this entity - never a guess. VrImpaleEntityRoot (default on) lets the normal
// impale use it too; off = R25 behaviour (type 1 only, the ragdoll root never qualifies on retail).
inline std::atomic<bool> entityRoot{true};
inline bool foreignIsEntity(int type,std::uintptr_t data,std::uintptr_t entity,bool allowType2) noexcept {
    return (type==1||(allowType2&&type==2))&&data!=0&&data==entity;}
// I10 (R14-IM): why an impale gave up, said once in game (empty = say nothing)
inline const char* gaveUpText(const char* reason) noexcept {
    if(!reason||!*reason)return "";
    const auto is=[&](const char* s){const char* a=reason;while(*a&&*s&&*a==*s){++a;++s;}return !*a&&!*s;};
    // D30-CMB: J27 00:22:10 killing stab - his ragdoll became the articulated root between 1.5 and 3.0 s, blade out at 3.4 s.
    if(is("BLADE_WITHDRAWN"))return "impale: blade pulled out before he went limp - keep it in him ~3 s";
    if(is("WEAPON_RELEASED_OR_TRACKING_LOST"))return "impale: weapon let go before he fell";
    if(is("ORIGINAL_VICTIM_WAIT_EXPIRED"))return "impale: his body never turned to a ragdoll in time";
    if(is("SUPERSEDED_BY_NEW_THRUST"))return "";
    return "impale: gave up";}
// R128: the 6 Nov 19:20:24 line said "see log C47 GAVE_UP" but that row is written only while F3 records, so the
// quiet log never said why. The status line (log only - the headset shows it only while F2 records) now carries the
// reason code itself: "impale: gave up (NO_EXACT_RAGDOLL_OR_NEW_CORPSE_CONTACT_IN_ORIGINAL_WAIT)".
inline const char* gaveUpLine(const char* reason,char* buf,std::size_t n) noexcept {
    const char* t=gaveUpText(reason);if(!buf||n<2)return t;
    const char* g="impale: gave up";const char* a=t;const char* b=g;while(*a&&*b&&*a==*b){++a;++b;}
    if(*a||*b){std::snprintf(buf,n,"%s",t);return buf;} // a specific text (or empty)
    std::snprintf(buf,n,"impale: gave up (%s)",reason);return buf;}
// ---------------------------------------------------------------- weapon side: where the body sits on the blade
enum class Pick : int {Contact=0,BladeLine=1,TipFar=2,OffBlade=3,Invalid=4};
inline const char* name(Pick p) noexcept {
    constexpr const char* n[]{"CONTACT_POINT","BLADE_LINE","TIP_FAR_OVER_60CM","OFF_BLADE_LINE_OVER_35CM","INVALID"};
    const auto i=unsigned(p);return i<5?n[i]:"?";}
struct StuckPick {Pick pick=Pick::Invalid;double tipDistance=-1,lateral=-1,depth=0;double point[3]{};bool ok() const noexcept {return pick==Pick::Contact||pick==Pick::BladeLine;}};
struct PickTuning {double exact=.12,maxTip=.60,maxLateral=.35,minDepth=.10,maxDepth=.40;};
// world metres. grip/tip: the blade line now; hit: the native contact point of the killing thrust.
// lenient = the hang mode (the game thread pulls the corpse onto the blade point anyway).
inline StuckPick pickStuckPoint(const double grip[3],const double tip[3],const double hit[3],bool lenient,const PickTuning& t={}) noexcept {
    StuckPick r{};double ax[3],d2=0,al2=0;
    for(int i=0;i<3;++i){if(!std::isfinite(grip[i])||!std::isfinite(tip[i])||!std::isfinite(hit[i]))return r;ax[i]=tip[i]-grip[i];al2+=ax[i]*ax[i];d2+=(hit[i]-tip[i])*(hit[i]-tip[i]);}
    r.tipDistance=std::sqrt(d2);const double al=std::sqrt(al2);
    if(r.tipDistance<=t.exact){r.pick=Pick::Contact;for(int i=0;i<3;++i)r.point[i]=hit[i];
        if(al>1e-6){double back=0;for(int i=0;i<3;++i)back+=(tip[i]-hit[i])*ax[i]/al;r.depth=std::max(0.0,back);}return r;}
    if(!lenient||!(al>.05)){r.pick=Pick::TipFar;return r;}
    if(r.tipDistance>t.maxTip){r.pick=Pick::TipFar;return r;}
    double u[3],back=0;for(int i=0;i<3;++i){u[i]=ax[i]/al;back+=(tip[i]-hit[i])*u[i];} // metres behind the tip along the blade
    double l2=0;for(int i=0;i<3;++i){const double q=tip[i]-u[i]*back;l2+=(hit[i]-q)*(hit[i]-q);}
    r.lateral=std::sqrt(l2);
    if(r.lateral>t.maxLateral){r.pick=Pick::OffBlade;return r;}
    r.depth=std::clamp(back,t.minDepth,std::max(t.minDepth,std::min(t.maxDepth,al-.10)));
    for(int i=0;i<3;++i)r.point[i]=tip[i]-u[i]*r.depth;
    r.pick=Pick::BladeLine;return r;
}

// ---------------------------------------------------------------- game thread: bounded wait for the death ragdoll
enum class Poll : int {NotYet=0,Ready=1,Fatal=2};
enum class WaitResult : int {Continue=0,Done=1,GaveUp=2};
struct RagdollWait {
    bool active=false;std::uint64_t since=0,until=0,next=0,periodMs=50;unsigned polls=0;
    void begin(std::uint64_t now,std::uint64_t windowMs=1500,std::uint64_t period=50) noexcept {
        *this={};active=true;since=next=now;periodMs=std::clamp<std::uint64_t>(period,10,500);
        until=now+std::clamp<std::uint64_t>(windowMs,100,5000);}
    bool due(std::uint64_t now) const noexcept {return active&&now>=next;}
    bool within(std::uint64_t now) const noexcept {return active&&now>=since&&now<until;}
    std::uint64_t waitedMs(std::uint64_t now) const noexcept {return now>=since?now-since:0;}
    // one poll result -> continue / done / gave up (window over, fatal, or the clock went backwards)
    WaitResult step(std::uint64_t now,Poll p) noexcept {
        if(!active)return WaitResult::GaveUp;
        ++polls;next=now+periodMs;
        // A late/rewound Ready result is not an in-window native handoff.
        if(now<since||now>=until||p==Poll::Fatal){active=false;return WaitResult::GaveUp;}
        if(p==Poll::Ready){active=false;return WaitResult::Done;}
        return WaitResult::Continue;
    }
};
// nearest ragdoll part to the contact point (index scan result), pure selection
struct PartPick {int part=-1;double distance=1e9;};
inline void considerPart(PartPick& best,int part,const double pos[3],const double hit[3]) noexcept {
    if(part<0)return;double d2=0;for(int i=0;i<3;++i){if(!std::isfinite(pos[i])||!std::isfinite(hit[i]))return;d2+=(pos[i]-hit[i])*(pos[i]-hit[i]);}
    const double d=std::sqrt(d2);if(d<best.distance){best.distance=d;best.part=part;}
}
// weapon-side refusal codes (WeaponOutput::impaleRefusal47): 1 + contact58c1::LethalThrustRefusalC2, 20.. our own
constexpr int kRefuseTooWeak=20,kRefuseTipFar=21,kRefuseOffBlade=22,kRefuseInvalid=23;
inline const char* refusalName(int code) noexcept {
    switch(code){case kRefuseTooWeak:return "TOO_WEAK_BELOW_VrImpaleKillEnergy";case kRefuseTipFar:return "TIP_FAR_FROM_CONTACT";
    case kRefuseOffBlade:return "CONTACT_OFF_BLADE_LINE";case kRefuseInvalid:return "INVALID_BLADE";default:return code>0&&code<20?"RECEIPT_CONTRACT":"NONE";}
}
}
