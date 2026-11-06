#pragma once
// DS-F94: the per-frame DRIVER for the crossbow/rifle physical reload stage
// policy. Mirrors the proven bow drive contract (ManualBowTick596 in
// staged_reload596c2.inl, pumped once per native frame from
// PhysicalNativeFrameTick6698 in src/frik_pose_probe.cpp):
//  1. one machine step per ORIGINAL source tick (Request.tick); same-tick
//     rereads (stereo/game-loop) do not dwell or advance the span clock;
//  2. a stale (>kMaxSourceAgeMs) or reversed source tick cancels the policy,
//     exactly like the bow's CONTEXT_CANCEL / SOURCE_REVERSED handling;
//  3. the Machine owns every stage transition; this driver only feeds host
//     evidence (Step + Request) and publishes the resulting Stage.
// PURE: no native reads or writes, no fabrication of a loaded weapon. The
// crossbow/rifle native reload call is UNPROVEN - see the seam below.
//
// INTEGRATION POINT (held): the exact bow-drive call site to extend is
// PhysicalNativeFrameTick6698, src/frik_pose_probe.cpp:3288-3289
//   {std::unique_lock<std::recursive_mutex> arrow(g_manual_arrow_mutex64,std::try_to_lock);
//    if(arrow.owns_lock())ManualBowTick596();}
// A CrossbowRifleDriveTick60() belongs next to it. It is deliberately NOT
// wired there yet: no producer of the crossbow/rifle Step evidence
// {valid,placed,released} exists in the tree, and the native reload action is
// unproven, so wiring would drive this policy on fabricated evidence.
#include "crossbow_rifle_reload60.h"
#include <cstdint>

// ---- UNPROVEN native reload seam (FAIL CLOSED: never call) -------------------
// The bow policy owns retail-pinned native leaves behind kReloadLeaves596
// (staged_leaf_pins596c2.inl). No equivalent pin exists for the crossbow or
// the rifle, so this driver executes NO native animation/ammo call of its
// own. The signature such a call would take is documented but never
// instantiated:
//
//   typedef void (__fastcall* CrossbowRifleNativeReloadFn)(
//       void* equipment, void* weaponComponent, void* ammoComponent,
//       unsigned actionTag); // UNPROVEN: span/winch on Empty->Working,
//                            // ramrod seat on Working->Loaded
//
// TODO(UNPROVEN): exact native reload call missing for the crossbow span/winch action and the rifle ramrod action; needs a retail pin (like kReloadLeaves596) before any call - never called here.
namespace kcd2vr::crossbow_rifle_reload_drive60 {
using Weapon=crossbow_rifle_reload60::Weapon;
using Stage=crossbow_rifle_reload60::Stage;
using Step=crossbow_rifle_reload60::Step;
using Request=crossbow_rifle_reload60::Request;
using Machine=crossbow_rifle_reload60::Machine;
// What the caller shows (the one visible line) and observes per frame.
struct Output {
    Stage stage=Stage::Idle;
    bool changed=false;      // the machine moved this drive
    bool faulted=false;      // stage==Fault (terminal until release())
    const char* reason=nullptr; // cancellation/fault text, or nullptr
};
struct Driver {
    // The bow drive refuses evidence older than 80 ms (ManualBowTick596's
    // tracking.tick freshness window).
    static constexpr std::uint64_t kMaxSourceAgeMs=80;
    Machine machine{};
    std::uint64_t lastSourceTick=0;
    bool armed() const noexcept {return machine.stage!=Stage::Idle&&machine.stage!=Stage::Fault;}
    // Arms the Empty stage. sourceTick is the machine-clock domain: the first
    // source tick the host will feed (the bow clocks its Loader with
    // tracking.tick, never the wall clock).
    void begin(Weapon w,std::uint64_t sourceTick) noexcept {machine.begin(w,sourceTick);lastSourceTick=0;}
    void release() noexcept {machine.release();lastSourceTick=0;}
    // Per-frame drive. `now` is the wall clock (GetTickCount64 at the drive,
    // used ONLY for source freshness); the machine itself is clocked by the
    // source tick r.tick, matching the bow's policy clock.
    Stage drive(const Request& r,const Step& s,std::uint64_t now,Output* out=nullptr) noexcept {
        const auto publish=[&](Stage st,bool changed,const char* reason) noexcept {
            if(out)*out=Output{st,changed,st==Stage::Fault,reason};
        };
        if(machine.stage==Stage::Idle||machine.stage==Stage::Fault){publish(machine.stage,false,nullptr);return machine.stage;}
        if(!r.tick||now<r.tick||now-r.tick>kMaxSourceAgeMs){ // stale/missing source: the bow's `valid` check
            release();publish(Stage::Idle,true,"SOURCE_STALE");return Stage::Idle;
        }
        // DS-F103 (Codex driver review): the CONTEXT validates BEFORE the
        // same-tick deduplication - a same-tick menu/context loss, tracking
        // invalidation or weapon replacement must release the ownership
        // immediately; only the ADVANCEMENT is deduplicated.
        if(machine.weapon!=Weapon::None&&(r.weapon!=machine.weapon||!r.weaponDrawn||!r.current||!r.handTracked)){
            release();publish(Stage::Idle,true,"IDENTITY_LOST");return Stage::Idle;
        }
        if(lastSourceTick&&r.tick<=lastSourceTick){ // once per original source tick
            if(r.tick<lastSourceTick){release();publish(Stage::Idle,true,"SOURCE_REVERSED");return Stage::Idle;}
            publish(machine.stage,false,nullptr);return machine.stage;
        }
        lastSourceTick=r.tick;
        const Stage before=machine.stage;
        const Stage after=machine.step(r,s,r.tick); // machine clock = source tick
        // UNPROVEN seam sites (never executed): a span/winch native call would
        // fire on Empty->Working and a ramrod seat native call on
        // Working->Loaded; both signatures live in the seam block above and are
        // deliberately not called until a retail pin proves them.
        const char* reason=nullptr;
        if(after==Stage::Fault){
            const std::uint64_t elapsed=r.tick>=machine.since?r.tick-machine.since:0;
            reason=elapsed>Machine::kMaxStageMs?"STAGE_TIMEOUT":
                   (before==Stage::Empty&&machine.spanMs>Machine::kMaxSpanMs)?"SPAN_TIMEOUT":
                   (before==Stage::Working&&s.placed&&!r.ammoAvailable)?"NO_AMMO":
                   (before==Stage::Empty&&s.stage==Machine::kSpan&&s.released)?"TAP_NOT_SPAN":"FAULT";
        }else if(after==Stage::Idle&&before!=Stage::Idle){
            lastSourceTick=0; // the machine released itself (identity loss or fire)
            reason=before==Stage::Ready?"READY_EXIT":"IDENTITY_LOST";
        }
        publish(after,after!=before,reason);
        return after;
    }
};
}
