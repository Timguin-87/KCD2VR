#pragma once
// C32 GORE (Miki 16 Oct): "I need only decapitations and impalements".
//  - a killing STAB always impales: the dead body hangs on the blade until you pull the blade out along its line,
//    kick him (R3) or push him off with the free hand. blood when the blade goes in and when it comes out.
//  - a STRONG killing CUT at the neck (energy >= ~90 %, armour ignored) takes the head off: the game's own head /
//    hair / beard / helmet attachments are hidden and the game's own blood effect plays at the neck.
// Game rules stay native: the native hit already killed him; this only decides what the dead body looks like.
// No hook, no timer thread, nothing per frame unless a body is on the blade or a head just came off.
// Pure logic (host tested). The native calls live in vrik/gore32.inl.
#include "../../vrik/gore_visibility59.h"
#include "../../vrik/gore_cut_d56.h"
#include "../../vrik/gorepak50.h" // DS-F8: limb event switch (file scope; no cycle, both host TUs see it)
#include <algorithm>
#include <atomic>
#include <cctype>
#include <cmath>
#include <cstdint>
#include <cstring>
#include "../../core/snapshot_mailbox.h"
#include "../physics/corpse_follow_r101.h" // R101: whole-body follow + shaper for the hang and the hand drag (pure)

namespace kcd2vr::gore32 {
// ---------------------------------------------------------------- settings (menu + INI)
inline std::atomic<bool> impaleHang{true};      // VrImpaleHang: the dead body hangs on the blade
inline std::atomic<float> impaleWeightKg{3.f};  // (C32 legacy, unused since C44: the load now comes from hangMassKg)
// C44 (Miki): "their weight should be like 40 kg, not full 100 kg" - VrImpaleHangMassKg 20..80, default 40. Two effects on OUR
// weapon sim only (the dead body itself is the game's ragdoll, driven to the blade by one per-frame part velocity):
//  - a point mass at the stuck point (a quarter of it: the ground carries most of a hanging body) = inertia/drag, and
//  - a sag of the weapon target: the blade pivot drops and the tip droops by his weight until Henry lifts/tilts the hand up.
inline std::atomic<float> hangMassKg{40.f};
// R128 (Miki 6 Nov: "yes, let it impale them on that fully, only plate armor makes this"): VrImpaleFullDepthR128,
// Experiments > GEAR page 3, default ON. ON = a killing stab that hangs the body draws it onto the blade until the
// guard is kGuardMarginR128 from the wound (plate armour on the struck zone keeps the old shallow point), and our own
// blade queries pass through the corpse that hangs on it (else the blade is pushed back out of him).
inline std::atomic<bool> impaleFullDepthR128{true};
inline constexpr double kGuardMarginR128=.08,kMaxFullDepthR128=1.0,kMaxHangDepthR128=1.20;
// current: the stuck point's depth behind the tip (m). gripToTipM: the blade line length grip->tip (m).
inline double fullDepthR128(double current,double gripToTipM,bool plate,bool on) noexcept {
    if(!std::isfinite(current))current=.15;
    if(!on||plate||!std::isfinite(gripToTipM)||gripToTipM<=kGuardMarginR128)return current;
    return std::max(current,std::min(gripToTipM-kGuardMarginR128,kMaxFullDepthR128));}
inline std::atomic<bool> decap{false};          // VrDecapitation (experiment, off until it looks right in the headset)
inline std::atomic<float> decapMinPower{.9f};   // strike energy ratio needed (1.0 = proper full stroke)
inline std::atomic<float> decapCorpseMinSpeed{2.5f}; // C41 VrDecapitationCorpseMinSpeed: blade speed (m/s) of a cut on a DEAD body (C43T corpse cut, dead-by-game list only)
inline std::atomic<bool> decapAllowBand{false}; // H18 VrDecapitationAllowBand: the living-kill neck band (z1.38..2.05, r0.40, ANY material) is opt-in. C41/C48 evidence: heads flew off shoulder/chest kills. Joint+head-surface tests remain unconditional.
inline std::atomic<float> limbCorpseMinSpeed{5.0f}; // C60 VrLimbCorpseMinSpeed: minimum speed for the legacy raw-part observation only; D56 dispatch requires full immutable cut evidence + positive male-human material.
// C60 receipt budget: saturating and keyed to the F3 capture session, so a
// finished recording does not exhaust all later sessions. Count before any
// expensive lookup; past exhaustion nothing more is admitted.
struct ReceiptBudget60 {std::uint64_t session=0;unsigned count=0;unsigned cap=64;
    bool admit(std::uint64_t currentSession) noexcept {if(currentSession!=session){session=currentSession;count=0;}if(count>=cap)return false;++count;return true;}};
// DS-F10: per-target limb-cut dedupe (Claude review 08.4). A cut kind that
// already fired on this entity within 2 s is refused; 16 slots. Pure logic.
struct LimbCutLedger60 {
    struct Entry {std::uint32_t id=0;std::uintptr_t identity=0;std::uint64_t tick[4]{};}; // per-kind ticks: one kind's cooldown never refreshes another's
    Entry e[16]{};unsigned next=0;
    static constexpr std::uint64_t kCooldownMs=2000;
    static constexpr unsigned maskOf(int cut) noexcept {
        using C=kcd2vr::gorepak50::Cut;
        return cut>=int(C::LeftHand)&&cut<=int(C::RightFoot)?1u<<unsigned(cut-int(C::LeftHand)):0u;
    }
    // Native game-thread only. A readiness check never consumes a cut; failed
    // pins, missing attachments or Lua-queue refusal can be retried immediately.
    bool available(std::uint32_t id,std::uintptr_t identity,std::uint64_t now,int cut) const noexcept {
        if(!id||!identity||!now||!maskOf(cut))return false;
        const unsigned k=unsigned(cut-int(kcd2vr::gorepak50::Cut::LeftHand));
        for(const auto& x:e)if(x.id==id&&x.identity==identity){
            const auto tick=x.tick[k];
            return !tick||(now>=tick&&now-tick>kCooldownMs);
        }
        return true;
    }
    bool admit(std::uint32_t id,std::uintptr_t identity,std::uint64_t now,int cut) noexcept {
        if(!available(id,identity,now,cut))return false;
        const unsigned k=unsigned(cut-int(kcd2vr::gorepak50::Cut::LeftHand));
        for(auto& x:e)if(x.id==id&&x.identity==identity){x.tick[k]=now;return true;}
        auto& x=e[next++%16];x={};x.identity=identity;x.tick[k]=now;x.id=id;return true;
    }
};
// DS-F23 (Codex 1540 material route): classify a limb cut from the ORIGINAL
// collision material (Receipt.bodyMaterial = idmatOrg) against the mounted
// body_material2subpart.xml, race 0 humans: 16/17=right forearm, 18=right
// hand, 23/24=left forearm, 25=left hand, 28/29=right lower leg, 30/31=right
// foot, 34/35=left lower leg, 36/37=left foot. Fail closed on unknown/0/-1.
// BOUNDARY: the same material ids mean OTHER body parts for non-human races
// (horse rows exist for the same ids); this classifier is therefore only a
// VISUAL fallback for the human combat lane (deduped, no damage), never a
// joint/impulse authority and never used when the part classifier resolved.
inline kcd2vr::gorepak50::Cut LimbFromMaterial60(int material) noexcept {
    using C=kcd2vr::gorepak50::Cut;
    switch(material){
    case 16:case 17:case 18:return C::RightHand;   // right forearm + hand
    case 23:case 24:case 25:return C::LeftHand;    // left forearm + hand
    case 28:case 29:case 30:case 31:return C::RightFoot; // right lower leg + foot
    case 34:case 35:case 36:case 37:return C::LeftFoot;  // left lower leg + foot
    default:return C::None;}
}
// DS-F36 (Codex F31 review): EXACT canonical family prefixes, normalized.
// Arbitrary segments before 'humans' must not match (a horse path like
// objects/characters/horses/humans/male/body/... is refused); the /body
// segment must immediately follow the human family. Attachment paths may be
// rooted with 'objects/' or not; both known variants are supported.
inline bool humanRacePath61(const char* norm) noexcept {
    if(!norm||!*norm)return false;
    return std::strncmp(norm,"objects/characters/humans/male/",31)==0||
           std::strncmp(norm,"objects/characters/humans/female/",33)==0||
           std::strncmp(norm,"characters/humans/male/",23)==0||
           std::strncmp(norm,"characters/humans/female/",25)==0;
}
inline bool humanBodyPath61(const char* norm) noexcept {
    if(!norm||!*norm)return false;
    return std::strncmp(norm,"objects/characters/humans/male/body/",36)==0||
           std::strncmp(norm,"objects/characters/humans/female/body/",38)==0||
           std::strncmp(norm,"characters/humans/male/body/",28)==0||
           std::strncmp(norm,"characters/humans/female/body/",30)==0;
}
// C43T: proof of death for the corpse cut. Miki's 17 oct run: 10 'corpse' decaps on LIVING enemies - a living npc's limbs
// are articulated hit shapes too, so the body type said 'corpse' while he kept fighting headless (codex C42 was right to
// switch that path off). Now only an npc the GAME itself reported killed (native hit receipt death=1, same entity
// identity) counts as a corpse. 16 slots, one hour, no guessing from physics.
struct DeadEntry {std::atomic<std::uint32_t> id{0};std::atomic<std::uintptr_t> identity{0};std::atomic<std::uint64_t> tick{0};};
inline DeadEntry knownDead[16];inline std::atomic<unsigned> knownDeadNext{0};
inline void NoteDead(std::uint32_t id,std::uintptr_t identity,std::uint64_t now) noexcept {if(!id||!identity)return;
    for(auto& e:knownDead)if(e.id.load()==id&&e.identity.load()==identity){e.tick.store(now);return;}
    auto& e=knownDead[knownDeadNext.fetch_add(1)%16];e.id.store(0);e.identity.store(identity);e.tick.store(now);e.id.store(id);}
inline bool KnownDead(std::uint32_t id,std::uintptr_t identity,std::uint64_t now) noexcept {if(!id||!identity)return false;
    for(auto& e:knownDead){const auto t=e.tick.load();if(e.id.load()==id&&e.identity.load()==identity&&now>=t&&now-t<=3600000ull)return true;}return false;}
// C44/C46 VrDecapitationStyle: 0 = HIDE HEAD: the head parts are hidden (C32), 1 = LOOSE NECK: the head stays, the death
// ragdoll's head joint is opened (no limits, no spring) and the head gets a kick, so it flops on a stretched neck (C44),
// 2 = STUMP + FLYING HEAD: the head parts are hidden, then a meat stump + blood on the Neck bone and the victim's own
// head skin on a rigid carrier pushed up/away, through deferred console Lua (C43D). Only on a native kill receipt
// (or a C43T dead-by-game corpse). Default 2 (Miki: "see the head flying off"); the Lua route is the least proven one.
inline std::atomic<int> decapStyle44{2};
constexpr int kDecapStyleCount46=3;
inline int clampDecapStyle46(int v) noexcept {return v<0?0:v>=kDecapStyleCount46?kDecapStyleCount46-1:v;}
inline std::atomic<float> decapHeadImpulse44{20.f};     // VrDecapitationHeadImpulse: N*s on the ragdoll head part (0 = none)
inline std::atomic<int> decapLooseNeckWaitMs44{3000};   // VrDecapitationLooseNeckWaitMs: how long we wait for the death ragdoll

struct V3 {double x=0,y=0,z=0;};
inline V3 operator+(V3 a,V3 b) noexcept {return {a.x+b.x,a.y+b.y,a.z+b.z};}
inline V3 operator-(V3 a,V3 b) noexcept {return {a.x-b.x,a.y-b.y,a.z-b.z};}
inline V3 operator*(V3 a,double s) noexcept {return {a.x*s,a.y*s,a.z*s};}
inline double dot(V3 a,V3 b) noexcept {return a.x*b.x+a.y*b.y+a.z*b.z;}
inline double len(V3 a) noexcept {return std::sqrt(dot(a,a));}
inline bool finite(V3 a) noexcept {return std::isfinite(a.x)&&std::isfinite(a.y)&&std::isfinite(a.z);}
inline V3 clampLen(V3 a,double m) noexcept {const double l=len(a);return l>m&&l>1e-9?a*(m/l):a;}

// ---------------------------------------------------------------- decapitation
// Lethal cut candidates come from the native hit receipt (other translation unit) through this mailbox.
struct CutCandidate {std::uint64_t serial=0,tick=0;std::uint32_t target=0;std::uintptr_t identity=0;float point[3]{};float power=0;bool twoHands=false;
    int bodyMaterial=-1;bool corpse=false;bool nativeLethal42=false;bool nonLethalLimb60=false;std::uint64_t bodyEpoch42=0,request42=0;int part44=-1;int rawPart47=-1;gore_cut_d56::Evidence cutD56{};}; // C41: struck body material (body_material2subpart) and dead-body test // C33: identity = the struck entity token of the receipt // DS-F8: nonLethalLimb60 = a strong cut on a limb that did NOT kill (limb event, no head/neck path)
inline core::SnapshotMailbox<CutCandidate> lethalCut;
// Killing-contact claims stop the same gesture being retried through the corpse lane.
inline core::SnapshotMailbox<gore_cut_d56::Claim> lethalGesturesD56[32];
inline std::atomic<unsigned> lethalNextD56{0};
inline void noteLethalGestureD56(const gore_cut_d56::Claim& c) noexcept {
    if(c.target&&c.identity&&c.gesture)(void)lethalGesturesD56[lethalNextD56.fetch_add(1)%32].publish(c);
}
inline bool wasLethalGestureD56(const gore_cut_d56::Claim& c) noexcept {
    for(const auto& slot:lethalGesturesD56){gore_cut_d56::Claim x{};if(slot.read(x)&&gore_cut_d56::same(x,c))return true;}return false;
}

// Hit point in the victim's MODEL space (feet at 0, z up). Neck/lower head of a standing adult, close to the body
// axis, so a cut into the shoulder or the arm never counts. Bounds are deliberately simple; the log shows misses.
// C41 (Miki): "everything +/- 15 cm around the neck" - the old neck band 1.36..1.86 widened by 15 cm both ways and the
// radius by 10 cm, so a strong cut into the head or the top of the shoulders counts too.
struct NeckBounds {double zMin=1.21,zMax=2.01,radius=.30;};
// The game's own hit-surface ids (Tables item/body_material2subpart.xml, human race 0): 1 head_up, 2 head_back,
// 3 head_front, 4 head_right, 5 head_left, 38 head_neck. A struck head/neck surface counts whatever the body's pose -
// the only test that also works on a corpse lying on the ground.
inline bool headMaterial(int m) noexcept {return (m>=1&&m<=5)||m==38;}
inline bool neckHit(V3 local,double scale=1.0,const NeckBounds& b={}) noexcept {
    if(!finite(local)||!(scale>.3&&scale<3))return false;
    const V3 l=local*(1.0/scale);
    return l.z>=b.zMin&&l.z<=b.zMax&&std::sqrt(l.x*l.x+l.y*l.y)<=b.radius;
}
// C47 (C46 log, 2 killing cuts, 0 heads): 00:21:34 target 45104, a 16.5 m/s cut on physics part 458 (a head part: the same
// part carries head_back/face materials 2/3 in the 17/18 Oct logs) with body material 0 and hitModel (-0.18,0.30,1.41).
// C41 refused it twice: material 0 was taken as "known, not head" (the band only ran for -1), and radial 0.35 > 0.30.
// Now: 0 (and anything outside the human table 1..38) = unknown; the KILL band is z 1.15..2.05, r 0.40 for every material
// (Miki: +/-15 cm around the neck, top of the shoulders counts); the struck part naming a head/neck joint always counts.
// C49 (older C48 log 01:39:24 / 01:39:59 / 01:43:07: killing cuts on the CHEST, material 6, hitModel z 1.26..1.30 r .02..0.21 -
// a 1.15 floor would have taken those heads off; the one real head kill was z 1.48 material 2): floor raised to 1.38 (neck base).
inline constexpr NeckBounds kKillNeck47{1.38,2.05,.40};
inline bool knownHumanMaterial47(int m) noexcept {return m>=1&&m<=38;}
enum class DecapBy47 : int {None=0,Joint=1,Material=2,Band=3};
inline const char* name(DecapBy47 b) noexcept {constexpr const char* n[]{"MISS","HEAD_NECK_JOINT","HEAD_SURFACE","NECK_BAND"};return n[unsigned(b)<4?unsigned(b):0];}
// corpse: joint first, head/neck material second (a lying body's model frame says nothing). living kill: joint, material, band.
inline DecapBy47 decapZone47(bool corpse,int material,bool headNeckJoint,V3 local,double scale=1.0) noexcept {
    if(headNeckJoint)return DecapBy47::Joint;
    if(headMaterial(material))return DecapBy47::Material;
    if(!corpse&&decapAllowBand.load(std::memory_order_relaxed)&&neckHit(local,scale,kKillNeck47))return DecapBy47::Band;
    return DecapBy47::None;
}
// skeleton joint names tried for the neck (exact CDefaultSkeleton name-hash lookup, case-insensitive CRC). "head" is the
// name the C44 loose neck already resolves; the head's parent joint counts as the neck whatever its name.
inline constexpr const char* kNeckJointNames47[]{"neck","neck1","neck2","neck3","neck_01","neck_02","Bip01 Neck","Bip01 Neck1","Bip01 Neck2","Bip01 Head"};
// pure classification of a struck part against resolved joint ids: head, the head's parent (neck), named neck joints,
// or a joint whose parent chain reaches the head within 4 steps (head children). parents[i] = parent of chain[i].
struct HeadNeckIds47 {int head=-1,headParent=-1;int named[10]{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};};
inline const char* classifyPart47(const HeadNeckIds47& ids,int part,const int* chain,int chainLen) noexcept {
    if(part<0)return nullptr;
    if(ids.head>=0&&part==ids.head)return "head";
    if(ids.headParent>=0&&part==ids.headParent)return "head_parent(neck)";
    for(int i=0;i<10;++i)if(ids.named[i]>=0&&part==ids.named[i])return kNeckJointNames47[i];
    for(int i=0;i<chainLen&&i<4;++i)if(ids.head>=0&&chain[i]==ids.head)return "head_child";
    return nullptr;
}
// Which game attachments belong to the head. Base skin "head", hair/beard pieces and helmet/hood parts that the
// game names "<item>-head", "<item>-hood", ... Eyes are bone attachments with the eyeballs. The neck/collar piece
// ("-neck", "-collar", gorgets) stays: it covers the cut.
inline bool iequal(const char* a,const char* b) noexcept {
    for(;*a&&*b;++a,++b)if(std::tolower((unsigned char)*a)!=std::tolower((unsigned char)*b))return false;return !*a&&!*b;}
inline bool istarts(const char* s,const char* p) noexcept {
    for(;*p;++s,++p)if(!*s||std::tolower((unsigned char)*s)!=std::tolower((unsigned char)*p))return false;return true;}
inline bool iends(const char* s,const char* e) noexcept {
    const auto n=std::strlen(s),m=std::strlen(e);return n>=m&&iequal(s+n-m,e);}
inline bool headAttachment(const char* name) noexcept {
    if(!name||!*name||istarts(name,"kcd2vr_"))return false; // C59: never re-cut/hide our own replacement/cap.
    if(iequal(name,"head")||iequal(name,"hair")||iequal(name,"beard")||iequal(name,"eye_left")||iequal(name,"eye_right")||
       iequal(name,"helmet")||iequal(name,"hat")||iequal(name,"hood"))return true;
    for(const char* p:{"head_","hair_","beard_","m_hair","m_beard","f_hair","eyelash","teeth","tongue"})if(istarts(name,p))return true;
    for(const char* e:{"-head","-hood","-visor","-coif","-face","-helmet","-hat","-cap","-hair","-beard","-mask","-eyes","-eye"})if(iends(name,e))return true;
    // C41: hoods/caps/helmets are not always named "<item>-hood". Any name containing these words is head gear;
    // a cape (shoulders) never is, nor a collar/gorget (covers the cut).
    auto has=[&](const char* w){const auto n=std::strlen(name),m=std::strlen(w);for(std::size_t i=0;i+m<=n;++i){std::size_t k=0;
        for(;k<m&&std::tolower((unsigned char)name[i+k])==w[k];++k){}if(k==m)return true;}return false;};
    if(has("cape")||has("collar")||has("gorget")||has("neck"))return false;
    // an item part named for another body slot ("<item>-torso", "-legs", ...) is never head gear, whatever its item name
    if(const char* dash=std::strrchr(name,'-'))for(const char* slot:{"-torso","-legs","-leg","-arms","-arm","-hands","-hand","-feet","-foot","-belt","-body","-shoulders"})if(iequal(dash,slot))return false;
    for(const char* w:{"hood","coif","helmet","helm","hat","cap","visor","head","hair","beard","mustache","moustache","eye","teeth","tongue","face","mask"})if(has(w))return true;
    return false;
}

// D73: detached-head presentation is narrower than the anatomical attachment
// classifier above. Natural head parts, helmets and head-only cap assets move to
// the flying head; coif/hood/cowl/neck layers stay on the actor.
// This admission runs BEFORE the native visibility lease, so omitted garments
// are never hidden and do not depend on a later Lua rollback.
inline bool pathHasD73(const char* s,const char* text) noexcept {
    if(!s)return false;
    const auto fold=[](char c){return c=='\\'?'/':char(std::tolower((unsigned char)c));};
    for(;*s;++s){const char* a=s;const char* b=text;while(*a&&*b&&fold(*a)==*b){++a;++b;}if(!*b)return true;}
    return false;
}
inline bool detachedHeadPartD73(const char* name,const char* skin) noexcept {
    if(!name||!*name||istarts(name,"kcd2vr_"))return false;
    for(const char* word:{"hood","coif","cowl","neck","collar","gorget","cape"})
        if(pathHasD73(name,word)||pathHasD73(skin,word))return false;
    if(skin&&*skin){
        const bool human=istarts(skin,"objects/characters/humans/")||istarts(skin,"characters/humans/")||
            istarts(skin,"objects\\characters\\humans\\")||istarts(skin,"characters\\humans\\");
        if(!human||!iends(skin,".skin"))return false;
        if(pathHasD73(skin,"/clothing/"))return pathHasD73(skin,"/clothing/head/helmet/")||pathHasD73(skin,"/clothing/head/cap/");
        return pathHasD73(skin,"/head/")||pathHasD73(skin,"/hair/")||pathHasD73(skin,"/beard")||
            pathHasD73(skin,"/eye")||pathHasD73(skin,"/teeth/")||pathHasD73(skin,"/tongue/");
    }
    // Pathless native eye/head bone attachments can still hide with the head.
    // Unknown equipment names alone are not proof of a head-only helmet skin.
    for(const char* exact:{"head","head_face","hair","beard","eye_left","eye_right","teeth","tongue"})
        if(iequal(name,exact))return true;
    for(const char* prefix:{"hair_","beard_","m_hair","m_beard","f_hair","eyelash","teeth_","tongue_"})
        if(istarts(name,prefix))return true;
    return false;
}

// ---------------------------------------------------------------- impalement: the body hangs on the blade
enum class Release {None,PullOut,Kick,HandPush,Stuck,Timeout,Lost,SwingOff};
inline const char* name(Release r) noexcept {
    constexpr const char* n[]{"NONE","PULL_OUT","KICK","HAND_PUSH","STUCK","TIMEOUT","LOST","SWING_OFF"};return unsigned(r)<8?n[unsigned(r)]:"?";}
inline bool gush(Release r) noexcept {return r==Release::PullOut||r==Release::Kick||r==Release::HandPush||r==Release::SwingOff;}

struct HangTuning {
    double follow=10.0;       // 1/s: how fast the wound is pulled to the blade point
    double maxSpeed=3.0;      // m/s cap of the drive (R101: was 6 - one part pulled at 6 m/s stretched the hanging body like latex)
    double pullSpeed=.05;     // m/s backwards along the blade relative to the body (below = tracking jitter)
    double pushRadius=.35;    // m: free palm this close to the wound
    double pushSpeed=1.0;     // m/s palm speed toward the body
    double stuckDistance=.50; // m: wound this far from the blade point ...
    std::uint64_t stuckMs=400;// ... for this long = the body got caught on something
    std::uint64_t maxMs=30000;
    double swingSpeed=4.0;    // R128: m/s of the stuck point ACROSS the blade = a hard sideways swing throws him off ...
    std::uint64_t swingMs=60; // ... for this long (one tracking spike is not a swing)
    double holdKi=25.0;       // R128: 1/s^2 integral of the hold gap: a constant pull (the hanging ragdoll's weight) is
    double holdIntegralMax=1.5; // ... cancelled instead of leaving a sag; its share of the drive is capped (m/s)
};
struct HangIn {
    std::uint64_t now=0;
    V3 bladePoint{},bladeAxis{};  // world: the stuck point on the blade, unit axis grip->tip
    V3 wound{};                   // world: the wound point on the impaled body part
    bool freePalm=false;V3 palm{},palmVelocity{};
    bool kick=false;              // the native kick reached him (kick26::ReleaseImpale)
    V3 kickDirection{};           // horizontal, Henry -> body
};
struct HangOut {bool drive=false;V3 velocity{};Release release=Release::None;V3 releaseVelocity{};double depth=0;};
struct Hang {
    bool active=false;std::uint64_t since=0,last=0,farSince=0,swingSince=0;double depth=0,initialDepth=0;
    V3 lastBlade{},lastWound{},integral{};bool haveLast=false;
    void start(std::uint64_t now,double penetration) noexcept { // R128: up to 1.2 m (full depth), was .60
        *this={};active=true;since=last=now;depth=initialDepth=std::clamp(std::isfinite(penetration)?penetration:.15,.05,kMaxHangDepthR128);}
    HangOut step(const HangIn& in,const HangTuning& t={}) noexcept {
        HangOut o{};if(!active)return o;
        if(!in.now||in.now<last||!finite(in.bladePoint)||!finite(in.wound)||!finite(in.bladeAxis)){active=false;o.release=Release::Lost;return o;}
        const double dt=haveLast?std::clamp(double(in.now-last)/1000.0,.001,.1):.011;
        const V3 bladeVel=haveLast?(in.bladePoint-lastBlade)*(1/dt):V3{};
        const V3 woundVel=haveLast?(in.wound-lastWound)*(1/dt):V3{};
        lastBlade=in.bladePoint;lastWound=in.wound;haveLast=true;last=in.now;
        const auto end=[&](Release r,V3 v){active=false;o.release=r;o.releaseVelocity=clampLen(v,8);o.depth=depth;return o;};
        if(in.kick)return end(Release::Kick,in.kickDirection*3.5+V3{0,0,.6});
        if(in.freePalm&&finite(in.palm)&&finite(in.palmVelocity)){
            const V3 to=in.wound-in.palm;const double d=len(to);
            if(d<=t.pushRadius&&len(in.palmVelocity)>=t.pushSpeed&&dot(in.palmVelocity,to)>0)return end(Release::HandPush,in.palmVelocity*1.3);
        }
        // pulling straight back slides the blade out instead of dragging the body
        // C35: integrate SIGNED relative axial travel. Rectifying only the
        // backward half-wave converted zero-net tracking jitter into a pullout.
        // Forward recovery can restore only the original proven penetration;
        // it cannot invent deeper contact or a new impalement.
        // R128: only the BLADE's own travel along its axis counts (and never more than the relative travel). Before,
        // the body sliding toward the tip under gravity (blade still) counted as a pull-out and let him go.
        const double rel=dot(bladeVel-woundVel,in.bladeAxis),own=dot(bladeVel,in.bladeAxis);
        const double along=std::clamp(rel,std::min(own,0.0),std::max(own,0.0));
        if(std::fabs(along)>t.pullSpeed)depth=std::clamp(depth+along*dt,0.0,initialDepth);
        if(depth<=0)return end(Release::PullOut,woundVel);
        // R128 (Miki 6 Nov: "the npc slided down from the blade with gravity"): the hold point is where the wound sits
        // on the blade now (the stuck point moved toward the tip by what was pulled out). The wound is pulled to it in
        // ALL directions, along the blade too. Before R128 the along-blade part of the gap was dropped, so a body on a
        // blade that pointed a bit down slid off toward the tip.
        const V3 hold=in.bladePoint+in.bladeAxis*(initialDepth-depth);
        const V3 gapAll=hold-in.wound;const V3 gap=gapAll-in.bladeAxis*dot(gapAll,in.bladeAxis);
        if(len(gap)>t.stuckDistance){if(!farSince)farSince=in.now;else if(in.now-farSince>=t.stuckMs)return end(Release::Stuck,woundVel);}
        else farSince=0;
        // R128: a hard swing across the blade throws him off (velocity = the swing)
        const V3 sideVel=bladeVel-in.bladeAxis*dot(bladeVel,in.bladeAxis);
        if(len(sideVel)>=t.swingSpeed){if(!swingSince)swingSince=in.now;else if(in.now-swingSince>=t.swingMs)return end(Release::SwingOff,sideVel*.8+V3{0,0,.3});}
        else swingSince=0;
        if(in.now-since>t.maxMs)return end(Release::Timeout,woundVel);
        // the blade's own motion is passed on, except backward along it (pulling back slides the blade out, never drags him)
        V3 ff=bladeVel*.9;const double ax=dot(ff,in.bladeAxis);if(ax<0)ff=ff-in.bladeAxis*ax;
        // integrate only while holding (gap < 10 cm); during the full-depth draw it would wind up and overshoot the guard
        if(t.holdKi>0)integral=len(gapAll)<.10?clampLen(integral+gapAll*dt,t.holdIntegralMax/t.holdKi):integral*.8;
        const V3 v=gapAll*t.follow+integral*t.holdKi+ff;
        o.drive=true;o.velocity=clampLen(v,t.maxSpeed);o.depth=depth;return o;
    }
};
// Extra weapon weight while a body hangs on it: felt more with less strength (the weapon feel already scales by
// strength/skill), capped so a 30 kg feel never comes from a dagger.
// C44: a quarter of VrImpaleHangMassKg (40 kg -> 10 kg at the stuck point), 2..20 kg, at most 8 x the weapon's own weight.
inline double clampHangMass44(double kg) noexcept {return std::isfinite(kg)?std::clamp(kg,20.,80.):40.;}
inline double hangLoadKg(double weaponKg,bool hanging) noexcept {
    if(!hanging)return 0;const double kg=std::clamp(clampHangMass44(hangMassKg.load(std::memory_order_relaxed))*.25,2.,20.);
    return std::min(kg,std::max(2.0,(std::isfinite(weaponKg)?weaponKg:1.5)*8.0));
}
// C44 sag of the weapon TARGET while he hangs (pure): the pivot (grip) drops and the tip droops toward "down" by his weight,
// less with a strong arm (force authority above the 180 N design profile). Henry holds him up by lifting / tilting the hand.
// 40 kg, 180 N: 6 cm drop, 10 deg droop; 80 kg: 12 cm, 20 deg. Ramped in/out over ~300 ms by the caller (no jump at the latch).
struct HangSag {double drop=0,droop=0,pullN=0;};
inline HangSag hangSag(double kg,double forceAuthority) noexcept {
    HangSag s{};const double m=clampHangMass44(kg);
    const double k=std::isfinite(forceAuthority)&&forceAuthority>1?std::clamp(180.0/forceAuthority,.5,1.5):1.;
    const double f=(m/80.)*k;s.drop=.12*f;s.droop=.35*f;s.pullN=m*9.81*.35;return s;
}
inline double sagRamp(double current,bool hanging,double dt) noexcept {
    const double step=std::clamp(std::isfinite(dt)?dt:0.,0.,.1)/.3;
    return std::clamp(hanging?current+step:current-step,0.,1.);
}
}
