#pragma once
// C58 physics features. Pure logic, host-testable (tests/lab58_test.cpp); native side is lab58.inl
// (frik_pose_probe.cpp) and a few lines in geometry_hit178r27.inl / mod.cpp.
//
// D84: corpse grabbing is selected gameplay; it does not require a diagnostic key.
// Other legacy experiments retain their separately selected lab mode.
// Saved preferences are loaded by the host; atomic startup values are not INI defaults.
//   LivingHitImpact / LivingTouch          saved requests only; D51 living actuators are PAUSED
//   CorpseDrag       VrLabCorpseDrag58       either hand retains a local point on a qualified corpse part
//   PropWake         VrLabPropWake58         switch only here; lab_prop58.h/.inl + the lua pak are another lane
// Native gameplay rules stay native: no damage/stamina/hit writes or living
// ragdoll ownership. Corpse velocity is a native body operation, not a constraint.
#include "diagnostics/capture168.h"
#include "../subsystems/physics/corpse_follow_r101.h" // R101: whole-body follow + drive shaper (no latex stretch)
#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdint>

namespace kcd2vr::lab58 {
constexpr int kRungOff=0,kRungObserve=1,kRungCorpse=2,kRungLab=3;
enum class Sub : int {LivingHitImpact=0,CorpseDrag=1,PropWake=2,LivingTouch=3};

inline std::atomic<int> rung{0};                 // mirror of mod.cpp g_ragdoll_rung19c4r4 (stored by ApplyRagdollRung19c4r4)
inline std::atomic<bool> livingHitImpact{false}; // VrLabLivingHitImpact58
inline std::atomic<bool> corpseDrag{false};      // VrLabCorpseDrag58
// R90: corpse search route. false = always, R2 idle budget (10/s per empty hand);
// true = only from grip >= .15 until 700 ms later (R89). VrCorpseSearchSqueezeR89.
inline std::atomic<bool> corpseSqueezeSearchR90{false};
inline std::atomic<bool> propWake{false};        // VrLabPropWake58 (read by the prop-wake lane, nothing here uses it)
inline std::atomic<bool> livingTouch{false};     // VrLabLivingTouch58 (DS: the touch-driven push/poke on a LIVING npc)
inline std::atomic<float> livingHitK{6.f};       // VrLabLivingHitK58: J = min(400, sqrt(partMass) * speed * k)
inline std::atomic<bool> uiBlocked{false};       // mod.cpp: theater / menu plane presented (corpse drag refuses)
// D51 compatibility status, kept false by the host. The global C43T impact and
// recoil leases are retired; the mod does not force native living-arrow physics.
inline std::atomic<bool> impactLeaseOk{false};
// W5: the whole F9 ladder (0 off, 1 observe, 2 physical held weapon, 3 lab), mirrored by physics/host.inl
// ApplyPhysicalHeldF9T4. These are legacy gameplay modes, never recorder authority.
// D84 F9/Physics and F3/Combat have independent finite diagnostic leases.
inline std::atomic<int> f9Rung{0};
inline bool physicsDiagnostics(int f9) noexcept {return f9==1||f9==3;}
inline bool physicsDiagnostics() noexcept {return physicsDiagnostics(f9Rung.load(std::memory_order_relaxed));}
inline bool captureActiveD84(std::uint64_t now) noexcept {
    namespace c=kcd2vr::diagnostics::capture168;
    return c::active(c::Domain::Physics,now)||c::active(c::Domain::Combat,now);
}
inline kcd2vr::diagnostics::capture168::Domain captureDomainD84(std::uint64_t now) noexcept {
    namespace c=kcd2vr::diagnostics::capture168;
    return c::active(c::Domain::Physics,now)?c::Domain::Physics:c::Domain::Combat;
}

inline bool subSwitch(Sub s) noexcept {
    switch(s){case Sub::LivingHitImpact:return livingHitImpact.load(std::memory_order_relaxed);
              case Sub::CorpseDrag:return corpseDrag.load(std::memory_order_relaxed);
              case Sub::PropWake:return propWake.load(std::memory_order_relaxed);
              case Sub::LivingTouch:return livingTouch.load(std::memory_order_relaxed);}
    return false;
}
// Runtime lab selection is independent of diagnostic recording. A subfeature
// keeps rung 3 and its own switch, except corpse grabbing which is independently selected.
// active() also checks native feature availability.
inline bool activeWith(int currentRung,bool sub) noexcept {return currentRung==kRungLab&&sub;}
// D51: the reported living-NPC flailing is unresolved. A mapped joint or a
// successful Action/AddImpact return proves neither animation/physics ownership
// nor stable visible yielding. Suspend these extra actuators; native hits,
// arrows, death ragdolls, corpse drag and prop/world physics keep their owners.
inline constexpr bool livingReactionsAvailableD51() noexcept {return false;}
inline bool active(Sub s) noexcept {
    if(s==Sub::CorpseDrag)return subSwitch(s);
    if((s==Sub::LivingHitImpact||s==Sub::LivingTouch)&&!livingReactionsAvailableD51())return false;
    return activeWith(rung.load(std::memory_order_relaxed),subSwitch(s));
}
// This is a GLOBAL native threshold, not a local weapon response. D51 keeps
// native arrow/hit knockdown thresholds unchanged in every F9 mode.
inline constexpr bool fallAndPlayRequestedH17(bool consoleReady,bool combat,bool wanted,int labRung) noexcept {
    // No globally lowered arrow/hit knockdown threshold, including inside F9.
    (void)consoleReady;(void)combat;(void)wanted;(void)labRung;
    return false;
}

// ------------------------------------------------------------------ rung cycling (F9 key, menu button)
constexpr int clampRung(int r) noexcept {return r<kRungOff?kRungOff:r>kRungLab?kRungLab:r;}
constexpr int nextRung(int r) noexcept {return (clampRung(r)+1)%(kRungLab+1);}
constexpr bool corpseImpulse(int r) noexcept {return r==kRungCorpse||r==kRungLab;} // rung 3 includes rung 2
// menu RAGDOLL PHYSICS button: OFF -> F9 (rung 2) -> F9+LAB (rung 3) -> OFF. two menu-owned bools.
struct MenuRagdoll {bool wanted=false;bool lab=false;};
constexpr MenuRagdoll cycleMenu(MenuRagdoll m) noexcept {
    if(!m.wanted)return {true,false};
    if(!m.lab)return {true,true};
    return {false,false};
}
constexpr int rungFromMenu(MenuRagdoll m) noexcept {return !m.wanted?kRungOff:m.lab?kRungLab:kRungCorpse;}
constexpr const char* rungName(int r) noexcept {
    return r==kRungLab?"PHYSICS_LAB":r==kRungCorpse?"BOUNDED_NATIVE_CONTACT":r==kRungObserve?"OBSERVE_ONLY":"OFF";}

// ------------------------------------------------------------------ B. living hit impact (PLANCK-style distribution)
struct V3 {double x=0,y=0,z=0;};
inline V3 operator-(V3 a,V3 b) noexcept {return {a.x-b.x,a.y-b.y,a.z-b.z};}
inline V3 operator*(V3 a,double s) noexcept {return {a.x*s,a.y*s,a.z*s};}
inline double length(V3 a) noexcept {return std::sqrt(a.x*a.x+a.y*a.y+a.z*a.z);}
inline bool finite(V3 a) noexcept {return std::isfinite(a.x)&&std::isfinite(a.y)&&std::isfinite(a.z);}

constexpr double kImpactCapNs=400;
constexpr double kDefaultPartMass=4;
constexpr double kParentDecay[2]{.35,.25};
struct ImpactPlan {int part[3]{-1,-1,-1};double J[3]{};int count=0;};
// struck part gets J = min(400, sqrt(partMass or 4) * speed * k). up to two KNOWN parents get J*0.35 and J*0.25.
// unknown parents (id < 0 or same as the struck part) end the chain; nothing is guessed.
inline ImpactPlan planImpact(int struck,const int parents[2],int nParents,double speed,double partMass,double k,double cap=kImpactCapNs) noexcept {
    ImpactPlan p{};
    if(struck<0||!std::isfinite(speed)||speed<=0||!std::isfinite(k)||k<=0||!std::isfinite(cap)||cap<=0)return p;
    const double m=std::isfinite(partMass)&&partMass>0?partMass:kDefaultPartMass;
    const double J=std::min(cap,std::sqrt(m)*speed*k);
    p.part[0]=struck;p.J[0]=J;p.count=1;
    int last=struck;
    for(int i=0;i<std::min(nParents,2);++i){
        const int id=parents?parents[i]:-1;if(id<0||id==last||id==struck)break;
        p.part[p.count]=id;p.J[p.count]=J*kParentDecay[i];++p.count;last=id;}
    return p;
}
// per-npc cooldown 250 ms, max 4 impacts per second globally. one ADMITTED+ANSWERED hit = one plan (all its parts).
struct ImpactBudget {
    static constexpr std::uint64_t kNpcCooldownMs=250,kWindowMs=1000;static constexpr unsigned kPerWindow=4;
    struct Npc {std::uint32_t entity=0;std::uint64_t tick=0;};
    Npc npc[8]{};std::uint64_t global[kPerWindow]{};unsigned at=0;
    const char* refuse(std::uint32_t entity,std::uint64_t now) const noexcept {
        if(!entity)return "NO_ENTITY";
        for(const auto& n:npc)if(n.entity==entity&&n.tick&&now>=n.tick&&now-n.tick<kNpcCooldownMs)return "NPC_COOLDOWN";
        unsigned inWindow=0;for(auto t:global)if(t&&now>=t&&now-t<kWindowMs)++inWindow;
        return inWindow>=kPerWindow?"GLOBAL_BUDGET":nullptr;
    }
    bool admit(std::uint32_t entity,std::uint64_t now) noexcept {
        if(refuse(entity,now))return false;
        Npc* slot=nullptr;for(auto& n:npc){if(n.entity==entity){slot=&n;break;}if(!slot&&(!n.entity||!n.tick||now<n.tick||now-n.tick>=kNpcCooldownMs))slot=&n;}
        if(!slot)slot=&npc[0];
        *slot={entity,now};
        global[at++%kPerWindow]=now;return true;
    }
};

// ------------------------------------------------------------------ C. corpse limb drag (bounded velocity servo)
// W5: why a squeeze near a corpse did not grab - one short visible line (Miki: say why where I see it).
// nearM = nearest qualified corpse part to that hand (m, <0 none), unqualified = corpses the game killed that failed
// the identity/death/root proof this search, why = that proof's reason, otherHand = the other hand holds that part.
struct GrabWhyInput {bool handEmpty=true,uiOk=true,inputCurrent=true,fault=false,consumed=true,otherHand=false,bodyOwned=false;
    double nearM=-1,grabRadius=.25;unsigned qualified=0,unqualified=0;const char* why="";};
inline const char* grabWhy(const GrabWhyInput& g,char* out,unsigned n) noexcept {
    if(!out||!n)return "";out[0]=0;
    auto put=[&](const char* s){unsigned i=0;for(;s[i]&&i+1<n;++i)out[i]=s[i];out[i]=0;};
    if(g.fault){put("corpse grab off: physics fault - back after the next save load");return out;}
    if(!g.uiOk){put("corpse grab: menu or cutscene owns the hands");return out;}
    if(!g.handEmpty){put("corpse grab: that hand is not empty");return out;}
    if(!g.inputCurrent){put("corpse grab: hand tracking not current");return out;}
    if(g.bodyOwned){put("corpse grab: this body is already held by an impale or another grip");return out;}
    if(g.otherHand){put("corpse grab: your other hand already holds that part");return out;}
    if(!g.qualified&&g.unqualified){
        char t[96]{};unsigned i=0;const char* a="corpse grab: body not ready - ";for(;a[i]&&i<60;++i)t[i]=a[i];
        const char* w=g.why&&*g.why?g.why:"NO_PROOF";for(unsigned j=0;w[j]&&i<90;++j,++i)t[i]=(w[j]=='"'||w[j]=='\\')?'_':w[j];t[i]=0;put(t);return out;}
    if(!g.qualified){put("corpse grab: no body the game killed within reach");return out;}
    if(g.nearM>g.grabRadius){const int cm=int(g.nearM*100+.5),need=int(g.grabRadius*100+.5);
        char t[96]{};const char* a="corpse grab: hand ";unsigned i=0;for(;a[i];++i)t[i]=a[i];
        auto num=[&](int v){char d[12]{};int k=0;if(v<0)v=0;do{d[k++]=char('0'+v%10);v/=10;}while(v&&k<11);while(k)t[i++]=d[--k];};
        num(cm);const char* b=" cm from the body, needs ";for(unsigned j=0;b[j];++j)t[i++]=b[j];num(need);t[i++]=' ';t[i++]='c';t[i++]='m';t[i]=0;
        put(t);return out;}
    if(!g.consumed){put("corpse grab: open the hand near the body, then squeeze");return out;}
    return out;
}
struct DragTuning {
    double gain=6;          // 1/s: v = gain * (hand - retained local grip point)
    double maxSpeed=2.5;    // m/s clamp (R101: was 3; the drive is also rate limited by corpse_follow_r101::Shaper)
    double grabRadius=.25;  // hand within this of a part -> grab
    double leash=.5;        // hand-to-retained-grip error beyond this -> release
    std::uint64_t regrabMs=300,maxMs=0; // 0: hold while original input/body remain current
};
struct DragCandidate {bool valid=false;std::uint32_t entity=0;std::uintptr_t identity=0;int part=-1;V3 pos{};};
struct DragState {bool held=false;std::uint32_t entity=0;std::uintptr_t identity=0;int part=-1;std::uint64_t start=0,lastRelease=0;};
enum class DragEvent : int {None=0,Grab=1,Drive=2,Release=3};
struct DragInput {
    std::uint64_t now=0;bool gripHeld=false,handEmpty=false,uiOk=false,handValid=false;V3 hand{};
    DragCandidate candidate{};          // nearest qualified part while NOT held (native side searches only then)
    bool partValid=false;V3 part{};     // retained local grip point transformed by the CURRENT part pose
    bool entityQualified=false;         // while held: identity/death receipt/parent proof still hold
    bool fault=false;                   // a native fault latched (shared with the gore32 drive)
};
struct DragStep {DragEvent event=DragEvent::None;V3 v{};const char* reason="";};
// never writes anything on release (the body keeps its momentum like prop_drive); one grab at a time.
inline DragStep stepDrag(DragState& s,const DragInput& in,const DragTuning& t={}) noexcept {
    DragStep r{};
    auto release=[&](const char* why){s.held=false;s.entity=0;s.identity=0;s.part=-1;s.start=0;s.lastRelease=in.now;r.event=DragEvent::Release;r.reason=why;return r;};
    if(s.held){
        if(in.fault)return release("FAULT");
        if(!in.gripHeld)return release("GRIP_RELEASED");
        if(!in.uiOk)return release("UI_BLOCKED");
        if(!in.handEmpty)return release("HAND_NOT_EMPTY");
        if(!in.handValid||!finite(in.hand))return release("HAND_POSE_UNAVAILABLE");
        if(!in.entityQualified)return release("ENTITY_UNQUALIFIED");
        if(!in.partValid||!finite(in.part))return release("PART_POSE_UNAVAILABLE");
        if(in.now<s.start)return release("TIME_REVERSED");
        if(t.maxMs&&in.now-s.start>t.maxMs)return release("MAX_DURATION");
        const V3 e=in.hand-in.part;const double d=length(e);
        if(d>t.leash)return release("LEASH");
        V3 v=e*t.gain;const double l=length(v);if(l>t.maxSpeed&&l>0)v=v*(t.maxSpeed/l);
        r.event=DragEvent::Drive;r.v=v;r.reason="SERVO";return r;
    }
    // not held: refusals are silent (no receipt), only a grab is an edge
    if(in.fault){r.reason="FAULT";return r;}
    if(!in.gripHeld){r.reason="GRIP_NOT_HELD";return r;}
    if(!in.uiOk){r.reason="UI_BLOCKED";return r;}
    if(!in.handEmpty){r.reason="HAND_NOT_EMPTY";return r;}
    if(!in.handValid||!finite(in.hand)){r.reason="HAND_POSE_UNAVAILABLE";return r;}
    if(s.lastRelease&&(in.now<s.lastRelease||in.now-s.lastRelease<t.regrabMs)){r.reason="REGRAB_COOLDOWN";return r;}
    const auto& c=in.candidate;
    if(!c.valid||!c.entity||!c.identity||c.part<0||!finite(c.pos)){r.reason="NO_CANDIDATE";return r;}
    if(length(in.hand-c.pos)>t.grabRadius){r.reason="OUT_OF_REACH";return r;}
    s.held=true;s.entity=c.entity;s.identity=c.identity;s.part=c.part;s.start=in.now;
    r.event=DragEvent::Grab;r.reason="GRAB";return r;
}
}
