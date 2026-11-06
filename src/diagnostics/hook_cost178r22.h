#pragma once
// V66.178R22b: per-hook cost registry ("performance is bad after calibration"). Every detour the mod
// installs wraps its body in a Scope. F2 Bug or explicit HUD capture enables timing;
// the 5 s summary names completed calls from that same finite recorder lease.
#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include "capture168.h"
#if defined(_WIN32)
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif
namespace kcd2vr::hookcost178r22 {
enum Slot : unsigned {InputBegin,InputRelease,Alignment,Enrich,EntityCharacter,ComboStep,ComboReset,ProcessHit,SubmitHit,ActorUpdate,PhysCollision,CloseQuery,ContactPath,Damage,
    MaterialDispatch,MaterialLookup,MaterialExecute,SweepProbe,CombatInput,AttachmentUpdate,IkContext,IkBridge,IkPoseWriter,IkBlendWriter,InteractiveAction,InteractionBody,Prediction,PredictionMid,
    HudApply,HudVisibility,PalmGaze,FlashGetElement,HudLayerXr,AnimationBody,AlignmentNative,AlignmentPolicy,PoseTransaction,Count}; // R22C1 (ChatGPT): three more slots
inline constexpr const char* names[Count]{"INPUT_BEGIN","INPUT_RELEASE","ALIGNMENT_QUERY","COLLISION_ENRICH","PASSIVE_CHARACTER","COMBO_STEP","COMBO_RESET","PROCESS_HIT","SUBMIT_HIT","ACTOR_UPDATE","PHYS_COLLISION","CLOSE_QUERY","CONTACT_PATH","DAMAGE_RESULT",
    "MATERIAL_DISPATCH","MATERIAL_LOOKUP","MATERIAL_EXECUTE","WEAPON_SWEEP_PROBE","COMBAT_INPUT","ATTACHMENT_UPDATE","IK_CONTEXT","IK_BRIDGE","IK_POSE_WRITER","IK_BLEND_WRITER","INTERACTIVE_ACTION","INTERACTION_BODY","PREDICTION","PREDICTION_MID",
    "HUD_APPLY_LAYOUT","HUD_VISIBILITY","HUD_PALM_GAZE","FLASH_GET_ELEMENT","HUD_LAYER_XR","ANIMATION_BODY_POLICY","ALIGNMENT_NATIVE","ALIGNMENT_POLICY","POSE_TRANSACTION"};
struct Counter{std::atomic<std::uint64_t> calls{0},us{0},maxUs{0},origUs{0},origMaxUs{0},ownUs{0},ownMaxUs{0};}; // origUs: time inside the GAME's original function called from the hook
inline Counter counters[Count];
// D30: outermost measured hook time only (nested scopes not double counted); read by the F2 FRAME line.
inline std::atomic<std::uint64_t> topLevelUs{0},measuredOwnUs{0};
// Child scopes and an original-call interval can overlap. Subtract their union,
// not their sum; then count each child's own work exactly once in its own row.
inline std::uint64_t exclusive(std::uint64_t elapsed,std::uint64_t children,
    std::uint64_t originals,std::uint64_t originalChildren) noexcept {
    const auto overlap=std::min(originalChildren,std::min(children,originals));
    const auto removed=children+originals-overlap;
    return elapsed-std::min(elapsed,removed);
}
// Timing is an explicit finite F2 Bug / HUD recorder, never gameplay state.
// OFF uses the existing mask; no clock or shared counter write.
inline bool requested() noexcept {
    using namespace diagnostics::capture168;
    constexpr unsigned mask=(1u<<unsigned(Domain::Bug))|(1u<<unsigned(Domain::Hud));
    return (gate.mask.load(std::memory_order_relaxed)&mask)!=0;
}
inline std::uint64_t tickNow() noexcept {
#if defined(_WIN32)
    return GetTickCount64();
#else
    return std::uint64_t(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count());
#endif
}
struct Lease {
    diagnostics::capture168::Domain domain=diagnostics::capture168::Domain::Bug;
    std::uint64_t serial=0;
    bool current() const noexcept {
        if(!serial||!requested())return false;
        const auto& g=diagnostics::capture168::gate;
        return g.session[unsigned(domain)].load(std::memory_order_acquire)==serial&&g.active(domain,tickNow());
    }
};
inline Lease lease() noexcept {
    using namespace diagnostics::capture168;
    if(!requested())return {};
    const auto now=tickNow();
    for(auto d:{Domain::Bug,Domain::Hud}){
        const auto serial=gate.session[unsigned(d)].load(std::memory_order_acquire);
        if(serial&&gate.active(d,now)&&serial==gate.session[unsigned(d)].load(std::memory_order_acquire))return {d,serial};
    }
    return {};
}
inline bool enabled() noexcept {return lease().serial!=0;}
struct Scope;
inline thread_local Scope* currentScope=nullptr;
struct Scope {
    unsigned slot;Lease capture{};Scope* parent=nullptr;
    std::chrono::steady_clock::time_point t0{};
    std::uint64_t originalUs=0,originalMaxUs=0,childUs=0,originalChildUs=0;
    unsigned originalDepth=0;bool insideParentOriginal=false;
    explicit Scope(unsigned s) noexcept:slot(s),capture(s<Count?lease():Lease{}) {
        if(!capture.serial)return;
        parent=currentScope;insideParentOriginal=parent&&parent->originalDepth;currentScope=this;t0=std::chrono::steady_clock::now();
    }
    ~Scope(){
        if(!capture.serial)return;
        currentScope=parent;
        if(!capture.current())return; // no off interval / new recorder session joins this call
        const auto us=std::uint64_t(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now()-t0).count());
        if(parent){parent->childUs+=us;if(insideParentOriginal)parent->originalChildUs+=us;}
        const auto own=exclusive(us,childUs,originalUs,originalChildUs);
        const auto game=originalUs-std::min(originalUs,originalChildUs);
        auto& c=counters[slot];c.calls.fetch_add(1,std::memory_order_relaxed);c.us.fetch_add(us,std::memory_order_relaxed);
        if(!parent)topLevelUs.fetch_add(us,std::memory_order_relaxed);
        measuredOwnUs.fetch_add(own,std::memory_order_relaxed);c.ownUs.fetch_add(own,std::memory_order_relaxed);
        auto ownMax=c.ownMaxUs.load(std::memory_order_relaxed);while(own>ownMax&&!c.ownMaxUs.compare_exchange_weak(ownMax,own)){}
        c.origUs.fetch_add(game,std::memory_order_relaxed);
        auto m=c.maxUs.load(std::memory_order_relaxed);while(us>m&&!c.maxUs.compare_exchange_weak(m,us)){}
        m=c.origMaxUs.load(std::memory_order_relaxed);while(originalMaxUs>m&&!c.origMaxUs.compare_exchange_weak(m,originalMaxUs)){}
    }
    Scope(const Scope&)=delete;Scope& operator=(const Scope&)=delete;
};
inline Scope* owner(unsigned slot) noexcept {
    for(auto* s=currentScope;s;s=s->parent)if(s->slot==slot)return s->capture.current()?s:nullptr;
    return nullptr;
}
struct Original {
    Scope* measured=nullptr;std::chrono::steady_clock::time_point t0{};
    explicit Original(unsigned s) noexcept:measured(owner(s)){if(measured&&measured->originalDepth++==0)t0=std::chrono::steady_clock::now();}
    ~Original(){if(!measured||--measured->originalDepth)return;
        const auto us=std::uint64_t(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now()-t0).count());
        measured->originalUs+=us;measured->originalMaxUs=std::max(measured->originalMaxUs,us);
    }
    Original(const Original&)=delete;Original& operator=(const Original&)=delete;
};
inline void addOriginal(unsigned slot,std::uint64_t us) noexcept {
    if(auto* s=owner(slot)){s->originalUs+=us;s->originalMaxUs=std::max(s->originalMaxUs,us);}
}
inline void count(unsigned slot) noexcept {if(slot<Count&&enabled())counters[slot].calls.fetch_add(1,std::memory_order_relaxed);}
inline void add(unsigned slot,std::uint64_t us) noexcept {
    if(slot>=Count||!enabled())return;
    auto& c=counters[slot];c.calls.fetch_add(1,std::memory_order_relaxed);c.us.fetch_add(us,std::memory_order_relaxed);
    c.ownUs.fetch_add(us,std::memory_order_relaxed);measuredOwnUs.fetch_add(us,std::memory_order_relaxed);
    auto ownMax=c.ownMaxUs.load(std::memory_order_relaxed);while(us>ownMax&&!c.ownMaxUs.compare_exchange_weak(ownMax,us)){}
    auto m=c.maxUs.load(std::memory_order_relaxed);while(us>m&&!c.maxUs.compare_exchange_weak(m,us)){}
}
// Writes one line "HOOK_COST windowMs=... total=..ms  NAME calls= ms= maxUs= ..." (top entries by time, then resets).
inline int summary(char* out,std::size_t n,std::uint64_t windowMs) noexcept {
    struct Row{unsigned slot;std::uint64_t calls,us,mx,orig,origMx,inclusiveMx;} rows[Count];unsigned k=0;std::uint64_t total=0,totalOrig=0;
    for(unsigned i=0;i<Count;++i){auto& c=counters[i];
        const auto calls=c.calls.exchange(0),inclusive=c.us.exchange(0),inclusiveMx=c.maxUs.exchange(0),orig=c.origUs.exchange(0),origMx=c.origMaxUs.exchange(0);
        const auto own=c.ownUs.exchange(0),ownMx=c.ownMaxUs.exchange(0);(void)inclusive;
        if(!calls)continue;rows[k++]={i,calls,own,ownMx,orig,origMx,inclusiveMx};total+=own;totalOrig+=orig;
    }
    if(!enabled())return 0; // counters were drained; never print old off-period work
    for(unsigned a=0;a<k;++a)for(unsigned b=a+1;b<k;++b)if(rows[b].us>rows[a].us){const Row t=rows[a];rows[a]=rows[b];rows[b]=t;}
    int o=std::snprintf(out,n,"V66.178R22 HOOK_COST windowMs=%llu measuredHookOwnMs=%.1f measuredGameOriginalMs=%.1f scope=INSTRUMENTED_HOOKS_ONLY nested=EXCLUSIVE maxUs=OWN",(unsigned long long)windowMs,double(total)/1000.0,double(totalOrig)/1000.0);
    for(unsigned i=0;i<k&&i<10&&o<int(n)-72;++i)o+=std::snprintf(out+o,n-o," %s calls=%llu ms=%.1f maxUs=%llu game=%.1f gameMaxInclusiveUs=%llu inclusiveMaxUs=%llu",names[rows[i].slot],(unsigned long long)rows[i].calls,double(rows[i].us)/1000.0,(unsigned long long)rows[i].mx,double(rows[i].orig)/1000.0,(unsigned long long)rows[i].origMx,(unsigned long long)rows[i].inclusiveMx);
    return k?o:0;
}
}
