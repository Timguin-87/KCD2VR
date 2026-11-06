#pragma once
#include <atomic>
#include <chrono>
#include <cstdint>
#include "swing_effort59c14.h"
// V66.178R8 ATTACK TRANSPORT = native XI gamepad events.
// The 2026-10-09 00:42 headset run (R7, new per-stage timers) attributed every stall:
//   NATIVE_ACTION_PREPARE 135-141 ms, NATIVE_ACTION_CANCEL 68-71 ms, NATIVE_GUARD 70 / 140 ms,
// i.e. ~69 ms for EVERY direct call into the player's combat action handler (WHGame+0x8E5324,
// prepare_to_attack / attack_primary / attack_abort / block) made from the mod's CCryAction
// frame hook. A real gamepad never calls that handler directly: it publishes xi_triggerr (>0.4 =
// prepare, release = attack_primary), xi_triggerl_btn (block) and xi_a (attack_abort) and the game
// dispatches them in its own input pass (defaultProfile.xml combat_base). INI VrAttackTransport
// (1 = XI, 0 = direct call, the old path). Zone selection still uses the direct zone setter.
//
// V66.178R13: the game's dispatch of our pad symbols lands in the mod's own OnAction detour
// (CombatInputDetour101), which in F4 suppresses every melee action not made by the direct path;
// the XI emitter marks OUR symbols with a thread-local provenance (ownedDispatch) that the detour
// honours, and the stroke technique is carried so a physical thrust selects the light input id.
//
// V66.178R16 (review F01): edges are ACKNOWLEDGED by the emitter, not timed.
//  * a prepare keeps RT down until the emitter has actually dispatched an owned RT-down
//    (rtDownAcked); a release requested before that stays pending (no lost prepare+release pair);
//  * an abort keeps RT down until the emitter has dispatched the owned A-down (aAcked), so the
//    game sees attack_abort BEFORE attack_primary's release (release after abort cannot fire);
//  * ownership rides WITH the published pad (ownedMask, set by apply()), not with timers at emit
//    time, so a delayed dispatch cannot lose its provenance;
//  * attack_special rides the same LT hold as the R10 kick request.
namespace kcd2vr::attack_xi178r8 {
inline std::atomic<bool> enabled{false}; // runtime default comes from INI VrAttackTransport (=1); host fixtures exercise the direct bridge
inline std::atomic<bool> rtHeld{false};            // prepare_to_attack held until release = attack_primary
inline std::atomic<bool> ltHeld{false};            // block held
inline std::atomic<bool> rtReleasePending{false};  // release requested before the down was acknowledged
inline std::atomic<bool> rtDownAcked{false};       // emitter dispatched an owned RT-down since the last prepare
inline std::atomic<bool> abortPending{false};      // A requested; RT stays down until the A-down was dispatched
inline std::atomic<bool> aAcked{false};
inline std::atomic<std::uint64_t> abortUntil{0};   // xi_a press window (attack_abort), extended until acked
inline std::atomic<std::uint64_t> specialUntil{0}; // LT held for attack_special (combat_locked holdTriggerDelay 0.25 s)
inline std::atomic<bool> specialViaLb{false};      // V66.178R17: attack_special as an LB press (needs the KCD2VR_Input defaultProfile mod: combat/attack_special = xi_shoulderl)
inline std::atomic<std::uint64_t> lbUntil{0};
inline constexpr std::uint64_t kLbPressMs=220; // R18: long enough for the input mod's onHold (holdTriggerDelay 0.1)
inline std::atomic<int> pendingStroke{0};          // 0 none, 1 slash, 2 thrust, 3 pommel (carried to the detour)
inline std::atomic<unsigned> ownedMask{0};         // bit0 RT, bit1 LT, bit2 A, bit3 LB: forced by apply() for the current pad (legacy view of the packet)
// V66.178R18 (review F05): one coherent packet per pad = values + ownership + prepare serial, published
// together by PublishGamepadState (same producer, same instant as the values) and read ONCE by the emitter.
// bits 0-15 rt*1000, 16-31 lt*1000, 32 a, 33 lb, 40-43 ownedMask, 48-55 prepare serial.
inline std::atomic<unsigned> prepareSerial{0};     // bumped on every prepare; the emitter acks only the matching down
inline std::atomic<bool> abortHoldsRt{false};      // abort keeps RT down only when a prepare was actually down (no RT-down introduced by an abort)
inline std::atomic<bool> ltGap{false};             // guard release+repress inside one pad: publish one LT-up pad first
inline std::atomic<bool> ltPublished{false};
inline std::atomic<bool> rawLtR122{false};         // R122: the physical LT is passed to the game this pad (out of combat, frik FistTransportTrigger101)
inline std::atomic<std::uint64_t> costEnvelope54{0}; // nonce16 | full prepareSerial32 | stroke2
inline unsigned packetNonce54(unsigned serial,unsigned stroke) noexcept {
    const auto e=costEnvelope54.load(std::memory_order_acquire);
    return unsigned((e>>16)&0xFFFFFFFFu)==serial&&unsigned((e>>48)&3u)==(stroke&3u)?unsigned(e&65535u):0;
}
inline std::uint64_t pack(float rt,float lt,bool a,bool lb,unsigned mask,unsigned serial,unsigned stroke=0,unsigned costNonce=0) noexcept {
    auto q=[](float v){v=v<0?0.f:(v>1?1.f:v);return std::uint64_t(v*1000.f+.5f);};
    return q(rt)|(q(lt)<<16)|(std::uint64_t(a?1:0)<<32)|(std::uint64_t(lb?1:0)<<33)|(std::uint64_t(mask&15u)<<40)|(std::uint64_t(serial&255u)<<48)|(std::uint64_t(stroke&3u)<<56)|
        (std::uint64_t(costNonce&63u)<<34)|(std::uint64_t((costNonce>>6)&15u)<<44)|(std::uint64_t((costNonce>>10)&63u)<<58);
}
struct Packet{float rt=0,lt=0;bool a=false,lb=false;unsigned mask=0,serial=0,stroke=0,costNonce54=0;};
template<class Emit> inline void emitTriggers51c3(const Packet& p,Emit&& emit) {
    emit("xi_triggerl",p.lt,false,false);
    // Only our combat packet changes ordering. UI/native pad packets retain it.
    if(p.mask&3u)emit("xi_triggerl_btn",0.f,true,p.lt>.20f);
    emit("xi_triggerr",p.rt,false,false);
    if(!(p.mask&3u))emit("xi_triggerl_btn",0.f,true,p.lt>.20f);
    emit("xi_triggerr_btn",0.f,true,p.rt>.20f);
}
inline Packet unpack(std::uint64_t v) noexcept {Packet p;p.rt=float(v&0xFFFF)*.001f;p.lt=float((v>>16)&0xFFFF)*.001f;p.a=(v>>32)&1;p.lb=(v>>33)&1;p.mask=unsigned((v>>40)&15);p.serial=unsigned((v>>48)&255);p.stroke=unsigned((v>>56)&3);p.costNonce54=unsigned((v>>34)&63u)|(unsigned((v>>44)&15u)<<6)|(unsigned((v>>58)&63u)<<10);return p;}
// Emitter-thread state per exact RT symbol. The release keeps the gesture that
// produced its down edge, even when the producer has already published a cut.
struct GestureLatch51 {
    bool held=false;unsigned stroke=0,serial=0,costNonce54=0;
    unsigned update(bool ours,bool active,unsigned kind,unsigned sequence,unsigned nonce=0) noexcept {
        if(active&&ours&&!held){held=true;stroke=kind;serial=sequence;costNonce54=nonce;}
        const auto result=held?stroke:0;
        if(!active)held=false;
        return result;
    }
};
inline thread_local int dispatchedStroke51=-1; // -1 = outside XI; zero = no physical weapon gesture
inline thread_local unsigned dispatchedCost54=0;
inline thread_local unsigned dispatchedSerial54=0;
inline std::atomic<bool> rtAwaitUp51{false};
// R55 behavior retained: an owned RT-up acknowledges release; a 400 ms bound
// permits recovery. The former R53 explanation (two unrelated serial counters)
// was incorrect: PublishGamepadState packs prepareSerial and the latch reads it.
// This timeout is not evidence that a new native attack was admitted.
inline std::atomic<std::uint64_t> rtAwaitSince53{0};
inline void ackRtUp51(unsigned) noexcept {rtAwaitUp51=false;rtAwaitSince53.store(0,std::memory_order_relaxed);
}
inline std::atomic<std::uint64_t> beginObserved49{0},releaseObserved49{0},classUnobserved49{0};
inline void receipt49(bool prepare,bool visited) noexcept {
    if(!visited)++classUnobserved49;else if(prepare)++beginObserved49;else ++releaseObserved49;
}
inline std::atomic<std::uint64_t> edges{0};        // diagnostics
inline constexpr std::uint64_t kSpecialHoldMs=420,kAbortMs=120;
// V66.178R22 stall attribution (review question "what is expensive?"): the combat-input detour records, per
// dispatch, the microseconds spent in the game's own handler vs. in the mod's code around it; the XI
// emitter prints both on a stall. Thread-local: the dispatch is synchronous on the input thread.
inline thread_local std::uint64_t detourUs=0,handlerUs=0,detourCalls=0,insideHooksUs=0; // insideHooksUs: mod hooks that run INSIDE the game handler (input class, combo probe, master factory)
inline thread_local std::uint64_t insideOriginalUs=0; // R22b: game code called from INSIDE a timed hook (subtracted from insideHooksUs)
struct HookTimer178r22{std::chrono::steady_clock::time_point t0=std::chrono::steady_clock::now();~HookTimer178r22(){insideHooksUs+=std::uint64_t(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now()-t0).count());}};
struct OriginalTimer178r22{std::chrono::steady_clock::time_point t0=std::chrono::steady_clock::now();~OriginalTimer178r22(){insideOriginalUs+=std::uint64_t(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now()-t0).count());}};
// R22b: per-action record of the last pad dispatch (which of the N actions bound to one symbol was slow).
struct DispatchRecord178r22{char name[28]{};int mode=0;std::uint32_t handlerUs=0;};
inline thread_local DispatchRecord178r22 dispatchRecords[8];inline thread_local unsigned dispatchRecordCount=0;
inline void recordDispatch(const char* name,int mode,std::uint64_t us) noexcept {if(dispatchRecordCount<8){auto& r=dispatchRecords[dispatchRecordCount++];unsigned i=0;for(;name&&name[i]&&i<27;++i)r.name[i]=name[i];r.name[i]=0;r.mode=mode;r.handlerUs=std::uint32_t(us>0xFFFFFFFFull?0xFFFFFFFFull:us);}}
// Thread-local provenance set by the XI emitter around the game-side dispatch of our symbols.
inline thread_local bool ownedDispatch=false;
inline bool specialHeld(std::uint64_t now) noexcept {return now<specialUntil.load(std::memory_order_relaxed);}
inline bool requestSpecial(std::uint64_t now) noexcept {
    if(specialViaLb.load(std::memory_order_relaxed)){const auto u=lbUntil.load();if(u&&now<u)return false;lbUntil.store(now+kLbPressMs);return true;}
    const auto until=specialUntil.load(std::memory_order_relaxed);
    if(until&&now<until)return false;
    if(rawLtR122.load(std::memory_order_acquire)&&!ltPublished.load())ltGap.store(true); // R122: a held physical LT first goes up, so the kick hold is a fresh LT press
    specialUntil.store(now+kSpecialHoldMs,std::memory_order_relaxed);return true;
}
// RT is down while a prepare is held, while a release waits for its down to be acknowledged, or
// while an abort waits for its A to be acknowledged.
inline bool rtDown(std::uint64_t) noexcept {
    return rtHeld.load(std::memory_order_relaxed)||(rtReleasePending.load(std::memory_order_relaxed)&&!rtDownAcked.load(std::memory_order_relaxed))||
           (abortPending.load(std::memory_order_relaxed)&&abortHoldsRt.load(std::memory_order_relaxed)&&!aAcked.load(std::memory_order_relaxed));
}
inline void reset() noexcept {rtAwaitUp51=false;rtAwaitSince53.store(0);rtHeld.store(false);ltHeld.store(false);rtReleasePending.store(false);rtDownAcked.store(false);abortPending.store(false);aAcked.store(false);abortHoldsRt.store(false);ltGap.store(false);ltPublished.store(false);abortUntil.store(0);specialUntil.store(0);lbUntil.store(0);pendingStroke.store(0);ownedMask.store(0);}
// V66.178R18 (review F05): leaving the owning mode must retire software input in order. A prepare that
// is still down becomes an abort (A before RT-up: no release-attack on exit); everything timed is dropped.
inline void cancel(std::uint64_t now) noexcept {
    const bool down=rtHeld.load()||rtReleasePending.load();
    specialUntil.store(0);lbUntil.store(0);ltHeld.store(false);ltGap.store(false);pendingStroke.store(0);
    if(down){abortPending.store(true);aAcked.store(false);abortHoldsRt.store(true);abortUntil.store(now+kAbortMs);rtHeld.store(false);rtReleasePending.store(false);}
}
// Pad builder (mod.cpp, once per pad): settle acknowledged transitions, force our symbols, publish the mask.
inline void apply(std::uint64_t now,float& rt,float& lt,bool& a,bool* lb=nullptr) noexcept {
    if(rtReleasePending.load()&&rtDownAcked.load()){rtReleasePending.store(false);rtHeld.store(false);rtDownAcked.store(false);}
    if(abortPending.load()&&aAcked.load()){abortPending.store(false);aAcked.store(false);rtDownAcked.store(false);}
    unsigned mask=0;
    if(rtDown(now)){rt=1.0f;mask|=1u;}
    if(ltGap.load(std::memory_order_acquire)){lt=0;/* retain the up across unpublished/overwritten producer frames */}
    else if(ltHeld.load()||specialHeld(now)){lt=1.0f;mask|=2u;}
    ltPublished.store((mask&2u)!=0);
    if(now<abortUntil.load()||abortPending.load()){a=true;mask|=4u;}
    if(lb&&now<lbUntil.load()){*lb=true;mask|=8u;} // R17: LB = attack_special with the input mod
    ownedMask.store(mask,std::memory_order_release);
}
// Emitter acknowledgements (input thread), called only for an owned dispatch of that symbol.
inline void ackRtDown(unsigned serial) noexcept {if((serial&255u)==(prepareSerial.load()&255u))rtDownAcked.store(true);}
inline void ackLtUp55c1() noexcept {ltGap.store(false,std::memory_order_release);}
inline void ackADown() noexcept {aAcked.store(true);}
// name: 0 prepare_to_attack, 1 attack_primary, 2 attack_abort, 3 block, 4 attack_special
// mode: 1 press, 2 release, 4 hold (retail OnAction activation modes used by the mod)
inline bool translate(int name,int mode,std::uint64_t now) noexcept {
    switch(name){
    case 0: if(mode==1){
        if(rtAwaitUp51.load()){const auto since=rtAwaitSince53.load(std::memory_order_relaxed);if(since&&now>=since&&now-since<400)return false;rtAwaitUp51=false;} // R53: expired await
        rtAwaitUp51=true;rtAwaitSince53.store(now,std::memory_order_relaxed);rtHeld.store(true);rtDownAcked.store(false);rtReleasePending.store(false);
        const auto serial=prepareSerial.fetch_add(1)+1;const auto* input=kcd2vr::swing_effort59c14::queuedInput54;
        const unsigned nonce=input?kcd2vr::swing_effort59c14::inputRegistry54.publish(*input,now,serial,unsigned(pendingStroke.load())):0;
        costEnvelope54.store(std::uint64_t(nonce)|(std::uint64_t(serial)<<16)|(std::uint64_t(pendingStroke.load()&3)<<48),std::memory_order_release);
    }return true; // a new down needs the preceding up to have been dispatched
    case 1: if(mode==2){ // attack_primary = RT release (after the down was seen)
        if(!rtHeld.load()&&!rtReleasePending.load())return true; // V66.178R20: nothing of ours is down - a release never INTRODUCES a down (the 2026-10-09 18:xx loop: drain release -> RT down -> new prepare -> drain ...)
        if(rtDownAcked.load()){rtHeld.store(false);rtDownAcked.store(false);}else rtReleasePending.store(true);}return true;
    case 2: if(mode==1){ // attack_abort
        // V66.178R21: an abort with nothing of ours down sends NOTHING. The retail abort press sets the
        // controller latch to CANCELLED(2) and only an attack_primary RELEASE resets it to 0 - a lone A press
        // left every later prepare rejected (2026-10-09 18:38 log: 22 prepares, all inputClassVisited=0).
        if(!rtHeld.load()&&!rtReleasePending.load())return true;
        abortHoldsRt.store(true);abortPending.store(true);aAcked.store(false);abortUntil.store(now+kAbortMs);rtHeld.store(false);rtReleasePending.store(false);}return true; // attack_abort: A first, RT release after (RT only if it was down)
    case 3: if(mode==1){if(!ltHeld.load()&&(ltPublished.load()||rawLtR122.load(std::memory_order_acquire)))ltGap.store(true);ltHeld.store(true);}else if(mode==2)ltHeld.store(false);return true; // hold(4): the pad keeps LT down by itself; release+repress = one LT-up pad first
    case 4: if(mode==1)return requestSpecial(now);return true;      // attack_special = LT hold (kick unarmed / back-off armed; needs lock)
    default: return false;
    }
}
}
