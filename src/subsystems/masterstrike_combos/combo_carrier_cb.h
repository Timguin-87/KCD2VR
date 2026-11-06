#pragma once
// D30-CMB (Claude): combo carrier keep-alive for COMBO HELP: HOLD. pure rules, host tested (tests/combo_carrier_cb_test.cpp).
// why (Codex N2 carrier review + Claude exit classification, 27 Oct): the N2 hold freezes Henry's attack CLOCK, but the
// ordinary attack's Mannequin fragment (C_CombatAnimAction vt 3A60C88, attack+0x80) still reaches its natural end through
// the controller (4B3DF8 -> vf+88 at 4B3FC2 -> 90CCEC -> 90CD98). 90CD98 fires the end delegate C54964 once (+98 guard),
// that retires the attack (B667AC -> 9CD780 reason 2) and the helper unregisters the combo listener 164A4A8 - so the next
// press inside the earned slot has nothing to chain into.
// fix: while N2 has this exact attack armed in the open slot, ONLY that natural arrival is deferred (+98 stays 0, the
// delegate is not called, 90CCEC's own post work still runs). the token is released once through the ORIGINAL 90CD98 on
// the game thread when the window ends (press consumed, slot closed/deadline, guard, hit, phase change, toggle off...)
// and only while the same attack still owns the manager main slot with the same fragment/functor. every other arrival
// (status-4 forced stop with the delegate unbound, Exit, timed replacement 4AE389) is native and drops the token: we
// never call a saved pointer after a native route touched it. no slot/timer/phase/latch/result write, NPCs untouched.
#include "../../core/snapshot_mailbox.h"
#include <atomic>
#include <cstdint>

namespace kcd2vr::combo_carrier_cb {
// retail WHGame bdf8f9e4 / 6A350E20 pins (rva)
inline constexpr std::uintptr_t kFunctorVt=0x3A99EC0,kEndDelegate=0xC54964,kEnd=0x90CD98,kNotifyCallRet=0x90CD2D,kNotifyRet=0x4B3FC8,kFragmentVt=0x3A60C88,kAttackVt=0x3A61C58;
inline constexpr unsigned char kEndEntry[]{0x48,0x89,0x5C,0x24,0x08,0x48,0x89,0x74,0x24,0x10,0x57,0x48,0x83,0xEC,0x20,0x80,0xB9,0x98,0x00,0x00,0x00,0x00};
inline constexpr unsigned char kNotifyCall[]{0x48,0x8B,0xCF,0xE8,0x6B,0x00,0x00,0x00,0x48,0x8B,0x0E};   // 90CD25..90CD30
inline constexpr unsigned char kControllerCall[]{0x48,0x8B,0xCB,0xFF,0x90,0x88,0x00,0x00,0x00};        // 4B3FBF..4B3FC7
inline constexpr std::uint64_t kStrandMs=600; // a token older than the window + this is reported, never force-called

// the armed window N2 publishes from its own native clock tick (same identity N2 already proved for the attack).
struct Window {
    std::uintptr_t actor=0,attack=0,manager=0,fragment=0;
    std::uint64_t key=0;      // N2 stroke identity (intent tick) - a new stroke is a new window
    std::uint64_t serial=0,tick=0;
    unsigned thread=0;
};
inline bool armed(const Window& w,std::uint64_t now,std::uint64_t maxAgeMs=120) noexcept {
    return w.serial&&w.actor&&w.attack&&w.manager&&w.fragment&&w.key&&w.thread&&now>=w.tick&&now-w.tick<=maxAgeMs;
}
// everything read at the 90CD98 entry (all reads fault-contained by the caller)
struct Arrival {
    std::uintptr_t fragment=0,fragmentVt=0,attackOfWindowFragmentSlot=0;
    std::uintptr_t ret=0,outerRet=0,functor=0,functorTarget=0,functorVt=0,functorFn=0; // functor=[frag+0x158]: vt, fn +08, raw target +20
    unsigned char once=1;     // [fragment+0x98]
    unsigned thread=0;
};
struct Token {
    std::uintptr_t actor=0,fragment=0,attack=0,manager=0,functor=0,functorTarget=0;
    std::uint64_t key=0,deferredAt=0,deadline=0;
    unsigned thread=0;bool live=false;
};
enum class Verdict:int {Native=0,Defer=1,DropNative=2};
enum class Why:int {Off=0,NotArmed=1,NotNatural=2,OtherFragment=3,Guarded=4,Identity=5,Thread=6,Defer=7,TokenNonNatural=8,Again=9};
inline const char* name(Why w) noexcept {
    switch(w){case Why::Off:return "OFF";case Why::NotArmed:return "NOT_ARMED";case Why::NotNatural:return "NOT_NATURAL";
        case Why::OtherFragment:return "OTHER_FRAGMENT";case Why::Guarded:return "ONCE_ALREADY_SET";case Why::Identity:return "IDENTITY";
        case Why::Thread:return "THREAD";case Why::Defer:return "DEFER";case Why::TokenNonNatural:return "TOKEN_NATIVE_ROUTE";default:return "AGAIN";}
}
struct Decision {Verdict verdict=Verdict::Native;Why why=Why::Off;};
inline bool natural(const Arrival& a,std::uintptr_t base) noexcept {
    return a.ret==base+kNotifyCallRet&&a.outerRet==base+kNotifyRet;
}
// the hook decision. `token` is the current deferred token (may be !live).
inline Decision decide(bool on,const Window& w,bool windowArmed,const Arrival& a,const Token& token,std::uintptr_t base) noexcept {
    const bool nat=natural(a,base);
    if(token.live&&a.fragment==token.fragment){
        if(nat&&a.once==0&&a.thread==token.thread)return {Verdict::Defer,Why::Again}; // repeated natural notification: still deferred
        return {Verdict::DropNative,Why::TokenNonNatural};                            // any native route owns it now
    }
    if(token.live)return {Verdict::Native,Why::NotArmed};                             // one deferred end at a time
    if(!on)return {Verdict::Native,Why::Off};
    if(!windowArmed)return {Verdict::Native,Why::NotArmed};
    if(a.fragment!=w.fragment)return {Verdict::Native,Why::OtherFragment};
    if(!nat)return {Verdict::Native,Why::NotNatural};
    if(a.once!=0)return {Verdict::Native,Why::Guarded};
    if(a.fragmentVt!=base+kFragmentVt||a.attackOfWindowFragmentSlot!=a.fragment||!a.functor||a.functorVt!=base+kFunctorVt||a.functorFn!=base+kEndDelegate||a.functorTarget!=w.attack)
        return {Verdict::Native,Why::Identity};
    if(a.thread!=w.thread)return {Verdict::Native,Why::Thread};
    return {Verdict::Defer,Why::Defer};
}
inline Token make(const Window& w,const Arrival& a,std::uint64_t now,std::uint64_t windowMs) noexcept {
    Token t{};t.actor=w.actor;t.fragment=a.fragment;t.attack=w.attack;t.manager=w.manager;t.functor=a.functor;t.functorTarget=a.functorTarget;
    t.key=w.key;t.deferredAt=now;t.deadline=now+windowMs;t.thread=a.thread;t.live=true;return t;
}
// D31 (DeepSeek review): manager+0x38 is the SLOTS pointer, the current main action is [slots+0] - the same two-step
// read as native_attack_clock50c2 / combo_recovery_hold_r14 / fist_action_h12. the old single read compared the
// slots pointer with the attack, so every release dropped (IDENTITY_GONE) and the deferred end was never called.
template<class Read> inline bool readManagerMain(Read&& read,std::uintptr_t manager,std::uintptr_t& main) noexcept {
    std::uintptr_t slots=0;main=0;
    return manager&&read(manager+0x38,slots)&&slots&&read(slots,main);
}
// what the release site re-reads right before calling the original 90CD98
struct Live {
    std::uintptr_t managerMain=0,attackVt=0,attackFragment=0,fragmentVt=0,functor=0,functorTarget=0;
    unsigned char once=1;unsigned thread=0;bool readOk=false;
};
enum class Release:int {Keep=0,Call=1,Drop=2,Strand=3};
enum class ReleaseWhy:int {Hold=0,Ended=1,Deadline=2,Replaced=3,Gone=4,Thread=5,Strand=6};
inline const char* name(ReleaseWhy w) noexcept {
    switch(w){case ReleaseWhy::Hold:return "HOLD";case ReleaseWhy::Ended:return "WINDOW_END";case ReleaseWhy::Deadline:return "DEADLINE";
        case ReleaseWhy::Replaced:return "SLOT_REPLACED";case ReleaseWhy::Gone:return "IDENTITY_GONE";case ReleaseWhy::Thread:return "OTHER_THREAD";default:return "STRANDED";}
}
struct ReleaseDecision {Release what=Release::Keep;ReleaseWhy why=ReleaseWhy::Hold;};
// windowStillArmed: N2 still has THIS stroke armed this tick. called on the native clock tick (game thread).
inline ReleaseDecision release(const Token& t,bool windowStillArmed,std::uint64_t windowKey,const Live& l,std::uint64_t now,std::uintptr_t base) noexcept {
    if(!t.live)return {Release::Keep,ReleaseWhy::Hold};
    const bool expired=now>=t.deadline;
    if(windowStillArmed&&windowKey==t.key&&!expired)return {Release::Keep,ReleaseWhy::Hold};
    if(l.thread!=t.thread)return now>=t.deadline+kStrandMs?ReleaseDecision{Release::Strand,ReleaseWhy::Strand}:ReleaseDecision{Release::Keep,ReleaseWhy::Thread};
    if(!l.readOk||l.attackVt!=base+kAttackVt||l.fragmentVt!=base+kFragmentVt||l.attackFragment!=t.fragment||
       l.functor!=t.functor||l.functorTarget!=t.functorTarget||l.once!=0)return {Release::Drop,ReleaseWhy::Gone};
    if(l.managerMain!=t.attack)return {Release::Drop,ReleaseWhy::Replaced}; // never let the delegate retire a NEW main action
    return {Release::Call,expired?ReleaseWhy::Deadline:ReleaseWhy::Ended};
}
inline std::atomic<bool> enabled{false}; // follows VrComboRecoveryHold (COMBO HELP: HOLD)
// runtime state shared by the N2 clock tick (publishes the window), the 90CD98 hook (defers) and Henry's actor update
// (releases before the native update). all three run on the game thread; the thread id is still checked on every use.
inline core::SnapshotMailbox<Window> window;
inline std::atomic<std::uint64_t> windowSerial{0};
inline std::atomic_flag tokenLock=ATOMIC_FLAG_INIT;
inline Token token{};                 // guarded by tokenLock
inline std::atomic<bool> tokenLive{false}; // cheap pre-check for the hooks
inline std::atomic<bool> retired{false};   // a fault retires the experiment for the process
inline std::uint64_t windowMs(std::uint32_t holdMs) noexcept {return std::uint64_t(holdMs?holdMs:1000)+200;} // hard cap past the N2 budget

inline std::atomic<std::uint64_t> deferred{0},released{0},dropped{0},stranded{0},natives{0},faults{0};
}
