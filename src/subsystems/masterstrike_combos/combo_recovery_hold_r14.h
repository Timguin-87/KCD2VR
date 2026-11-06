#pragma once
// R14 lane CB (Claude): combo recovery hold. pure rules + the read-only recovery view.
// why: in VR the next swing comes ~0.9-1.0 s after the release. the native attack clock ends Henry's attack
// ~0.65 s after the hit (00:39:57.831 phase16 total=0.396 -> 58.477 total 0 -> phase 2). the press then landed
// AFTER the action ended: D4AD94 still saw the combo slot payload (+868) and routed it to C5AC2C, which only sets
// "combo active" (+360) and fires the combo-input event - with no live attack to chain from, the press was eaten
// and nothing played (4/4 slot-open presses in the R14 run, release=UNSEEN, no attack anim after).
// fix: while the native combo slot is open and the player has not pressed yet, Henry's own attack clock stops in
// its LAST frames of phase 16 (dt nets to 0, the same proven writes R50C2/R11 use). the action stays the live
// main action, so the slot, 909A90's re-check and D4AD94 stay native. a press (combo route), a guard, the slot
// closing or the time limit releases it and the native clock runs on. no phase/result/damage write.
#include "../../vrik/attack_clock50c2.h"
#include "../../core/snapshot_mailbox.h"
#include <atomic>
#include <cmath>
#include <cstdint>

namespace kcd2vr::combo_hold_r14 {
inline std::atomic<bool> enabled{false};                // VrComboRecoveryHold
inline std::atomic<std::uint32_t> maxHoldMs{1000};     // VrComboRecoveryHoldMs (0 = off, max 1500)
// N2 (Codex N1 review): the stop is no longer a global pre-call timestamp. It is published AFTER the original D4AD94 returned
// "consumed" on the same stack for an owned prepare whose route really was the combo slot (ms<=0, or the C47 gate redirected
// a denied masterstrike to D4ADCC = the retail closed-window path), and it carries the held binding's complete identity.
inline std::atomic<std::uint64_t> windows{0},frames{0},endPress{0},endGuard{0},endSlot{0},endTimeout{0},endOther{0},faults{0};
inline std::atomic<int> lastWhy{0};
// R94: the actor whose attack tail is held right now (+ tick of the last held frame). The automatic posture guard
// reads it and stays down while the slot is open, so our own guard cannot end the hold. Manual LT still ends it.
inline std::atomic<std::uintptr_t> holdingActor{0};
inline std::atomic<std::uint64_t> holdingTick{0};
inline std::atomic<std::uint64_t> autoGuardDeferred{0};
inline bool holdingNow(std::uintptr_t actor,std::uint64_t now) noexcept {
    const auto t=holdingTick.load(std::memory_order_acquire);
    return actor&&holdingActor.load(std::memory_order_acquire)==actor&&t&&now>=t&&now-t<=100;
}

enum class Why:int {Hold=0,Off=1,BadDt=2,Phase=3,NoSlot=4,Pressed=5,Guard=6,Timeout=7,NotEnd=8};
inline const char* name(Why w) noexcept {
    switch(w){case Why::Hold:return "HOLD";case Why::Off:return "OFF";case Why::BadDt:return "BAD_DT";case Why::Phase:return "PHASE";
        case Why::NoSlot:return "SLOT_CLOSED";case Why::Pressed:return "PRESSED";case Why::Guard:return "GUARD";
        case Why::Timeout:return "TIMEOUT";default:return "NOT_END";}
}
// complete identity of one held stroke (the attackclock50 binding chain + its intent). a same-address action/clock reused by
// a new attack has another intent tick/gesture/sample, so it is a different key.
struct Key {
    std::uintptr_t actor=0,state=0,action=0,clock=0,manager=0,row=0;
    std::uint64_t serial=0,tick=0,sample=0,gesture=0,epoch=0,request=0,key=0,generation=0;
};
inline Key keyOf(const attackclock50::Binding& b) noexcept {
    Key k{};k.actor=b.intent.actor;k.state=b.intent.state;k.action=b.action;k.clock=b.clock;k.manager=b.manager;k.row=b.row;
    k.serial=b.intent.serial;k.tick=b.intent.tick;k.sample=b.intent.sample;k.gesture=b.intent.gesture59c3;k.epoch=b.intent.epoch;k.request=b.intent.request;
    k.key=b.intent.key;k.generation=b.intent.generation;return k;
}
inline bool valid(const Key& k) noexcept {return k.actor&&k.state&&k.action&&k.clock&&k.manager&&k.tick;}
inline bool same(const Key& a,const Key& b) noexcept {
    return valid(a)&&a.actor==b.actor&&a.state==b.state&&a.action==b.action&&a.clock==b.clock&&a.manager==b.manager&&a.row==b.row&&
        a.serial==b.serial&&a.tick==b.tick&&a.sample==b.sample&&a.gesture==b.gesture&&a.epoch==b.epoch&&a.request==b.request&&a.key==b.key&&a.generation==b.generation;
}
struct Stop {Key key{};std::uint64_t serial=0,tick=0;bool redirected=false;};
inline core::SnapshotMailbox<Stop> stop;
inline std::atomic<std::uint64_t> stopSerial{0},stopsPublished{0},stopsUnbound{0},stopsNotConsumed{0};
// the actual-route rule, evaluated after the original D4AD94 returned. inputs are D4AD94's own branch payload read before
// the call (+838 && heavy, +7D8, +868) plus the C47 redirect flag of THIS call and the native result.
struct Route {bool owned=false,prepare=false,known=false,r830Heavy=false,slot=false,redirected=false,consumed=false;int ms=-1;};
enum class RouteWhy:int {Combo=0,NotOwned=1,Release=2,Unknown=3,HeavyBranch=4,NoSlot=5,MasterStrike=6,NotConsumed=7};
inline RouteWhy route(const Route& r) noexcept {
    if(!r.owned)return RouteWhy::NotOwned;
    if(!r.prepare)return RouteWhy::Release;          // release edges have earlier retail exits (D4A9FC..D4AA5B): never a stop
    if(!r.known)return RouteWhy::Unknown;
    if(r.r830Heavy)return RouteWhy::HeavyBranch;
    if(!r.slot)return RouteWhy::NoSlot;
    if(r.ms>0&&!r.redirected)return RouteWhy::MasterStrike; // allowed kind-5 master strike: the action is replaced natively
    if(!r.consumed)return RouteWhy::NotConsumed;     // D4AD94 returned 0: nothing took the input
    return RouteWhy::Combo;
}
// per-stroke hold memory on the native game thread (the 9E3C16 hook). any key change = native replacement/new stroke: reset.
struct Tracker {Key key{};double heldMs=0;std::uint64_t started=0;bool holding=false,ended=false;int phase=0; /* R94: 16 or 32 where the hold began */};
// A refusal retires this occurrence. Only a different admitted binding may reset it.
inline void revoke(Tracker& t) noexcept {if(valid(t.key)){t.ended=true;t.holding=false;}}
inline double elapsed(const Tracker& t,std::uint64_t now) noexcept {
    if(!t.started)return t.heldMs;
    return now<t.started?double(UINT64_MAX):std::max(t.heldMs,double(now-t.started));
}
inline bool rebind(Tracker& t,const Key& k) noexcept {if(same(t.key,k))return false;t=Tracker{};t.key=k;return true;}
inline bool stopped(const Stop& s,const Key& k) noexcept {return s.serial&&same(s.key,k);}
struct Input {int phase=0;float total=-1,dt=0;bool slot=false,pressed=false,guard=false,on=true;double heldMs=0;std::uint32_t maxMs=0;};
// R94: the recovery tails that may be held. 16 = hit phase, 32 = recovery after a late VR hit (CB3 opens the slot there).
constexpr bool holdPhase(int phase) noexcept {return phase==16||phase==32;}
// only the last native frames of the hit phase are held; the native recovery itself plays at native speed.
inline Why decide(const Input& i) noexcept {
    if(!i.on||!i.maxMs)return Why::Off;
    if(!std::isfinite(i.dt)||i.dt<=0||i.dt>.08f)return Why::BadDt;
    if(!holdPhase(i.phase))return Why::Phase; // R94: phase 32 tail too
    if(!i.slot)return Why::NoSlot;
    if(i.pressed)return Why::Pressed;
    if(i.guard)return Why::Guard;
    if(!(i.heldMs<double(i.maxMs)))return Why::Timeout;
    if(!(i.total>=0.f)||i.total>i.dt+.03f)return Why::NotEnd;
    return Why::Hold;
}
// a held window ends (once) for this reason; NotEnd/Phase before any hold are not an end.
inline bool ends(Why w) noexcept {return w!=Why::Hold&&w!=Why::NotEnd;}

// precompensate the native dt advance so this update nets to zero (same writes as fist_clock_r11::holdPlan).
inline attackclock50::Plan holdPlan(std::uintptr_t clock,const attackclock50::View& v,float dt) noexcept {
    attackclock50::Plan p{};p.nativeDt=p.effectiveDt=dt;
    if(!clock||!v.records||!std::isfinite(dt)||dt<=0||!std::isfinite(v.rate)||v.rate<=0)return p;
    const float delta=-std::min(dt,std::min(v.elapsed,v.scaledElapsed/v.rate));
    if(!(delta<0))return p;
    p.writes[0]={clock+0x50,v.scaledElapsed,v.scaledElapsed+delta*v.rate};
    p.writes[1]={clock+0x54,v.elapsed,v.elapsed+delta};
    unsigned index=2;
    for(unsigned n=0;n<3;++n){const auto& row=v.rows[n];
        p.writes[index++]={v.records+n*sizeof(attackclock50::Record)+0xC,row.left,std::min(row.end,row.left-delta)};
        p.writes[index++]={v.records+n*sizeof(attackclock50::Record)+0x10,row.scaledLeft,std::min(row.scaledEnd,row.scaledLeft-delta*v.rate)};
    }
    p.effectiveDt=dt+delta;p.adjusted=true;return p;
}

// attackclock50::inspect for the SAME admitted action after its hit (phase 16). same identity chain incl. the actor+220 ->
// gate+28==0 term (retail D4D34C branches on it; kept until a reason to drop it is proven - Codex N1 review), read-only.
template<class Read> bool inspectRecovery(const attackclock50::Binding& b,std::uintptr_t base,Read read,attackclock50::View& v) noexcept {
    using attackclock50::Record;
    auto eq=[&](std::uintptr_t p,std::uintptr_t expected){std::uintptr_t x=0;return read(p,&x,8)&&x==expected;};
    auto integer=[&](std::uintptr_t p,int expected){int x=-1;return read(p,&x,4)&&x==expected;};
    auto byte=[&](std::uintptr_t p,unsigned char expected){unsigned char x=255;return read(p,&x,1)&&x==expected;};
    const auto a=b.intent.actor,s=b.intent.state,p=b.intent.player;
    v.check=1;
    if(!base||!a||!s||!p||!b.action||!b.helper||!b.clock||!b.manager||!b.row||!b.entity||
       !eq(a,base+0x3A80610)||!eq(a+0x1F8,p)||!eq(p,base+0x3AA4B98)||!eq(p+0x38,b.entity)||
       !eq(a+0x210,s)||!eq(s+0xCE0,a)||!eq(a+0x200,b.manager)||!eq(a+0x3C0,b.clock)||!eq(b.clock+8,a))return false;
    std::uintptr_t slots=0;
    v.check=2;
    if(!read(b.manager+0x38,&slots,8)||!slots||!eq(slots,b.action))return false; // still the live main action
    v.check=3;
    if(!eq(b.action,base+0x3A61C58)||!integer(b.action+0x3C,0)||!eq(b.action+0x40,b.manager)||
       !byte(b.action+0x28,1)||!eq(b.action+0x70,a)||!eq(b.action+0xA0,b.helper)||!eq(b.action+0x58,b.row)||
       !eq(b.helper,base+0x3A61040)||!eq(b.helper+8,a)||!eq(b.helper+0x20,b.action+0x88)||
       !eq(b.action+0x88,base+0x3A61440)||!byte(b.clock+0x14,1))return false;
    std::uintptr_t type=0,gate=0,end=0;int kind=-1;
    v.check=4;
    if(!read(b.row+0x148,&type,8)||!type||!read(type+8,&kind,4)||(kind!=0&&kind!=1)||
       !read(a+0x220,&gate,8)||!gate||!byte(gate+0x28,0)||
       !read(s+0x38,&v.phase,4)||!holdPhase(v.phase)|| // R94: 16 or 32
       !read(b.clock+0x58,&v.records,8)||!read(b.clock+0x60,&end,8)||!v.records||
       (v.records&3)||end<v.records||end-v.records!=3*sizeof(Record)||
       !read(b.clock+0x10,&v.rate,4)||!read(b.clock+0x50,&v.scaledElapsed,4)||!read(b.clock+0x54,&v.elapsed,4)||
       !read(v.records,v.rows.data(),sizeof(v.rows)))return false;
    v.check=5;
    if(!std::isfinite(v.rate)||v.rate<.1f||v.rate>8||!std::isfinite(v.elapsed)||v.elapsed<0||v.elapsed>30||
       !std::isfinite(v.scaledElapsed)||v.scaledElapsed<0||v.scaledElapsed>30)return false;
    unsigned mask=0;
    v.check=6;
    for(const auto& r:v.rows){
        const unsigned k=r.kind&255;
        if(k>2||(mask&(1u<<k)))return false;mask|=1u<<k;
        for(float x:{r.end,r.scaledEnd,r.left,r.scaledLeft})if(!std::isfinite(x)||x<0||x>30)return false;
        if(r.left>r.end+.001f||r.scaledLeft>r.scaledEnd+.001f)return false;
    }
    v.check=0;return mask==7;
}
}
