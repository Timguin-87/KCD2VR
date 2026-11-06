#pragma once
// Current native punch timing advances normally until actual committed fist contact.
// Contact catch-up uses the existing pinned attack-clock transaction. The engine owns strike entry,
// target selection, block/parry, costs and damage. No delayed collision replay.
#include "attack_clock50c2.h"
#include "pose_math.h"
#include <atomic>
#include <cmath>
#include <cstdint>

namespace kcd2vr::fist_clock_r11 {
namespace m=pose_core;
inline std::atomic<bool> enabled{true};          // VrFistClock
inline std::atomic<unsigned> maxHoldMs{500};      // legacy VrFistHoldMs is read for INI compatibility; no travelling hold
inline constexpr std::uint64_t kContactAgeMs=160; // same as the weapon contact (contact59c3)
inline constexpr std::uint64_t kAdvanceMs=45;     // a forward fist sample this recent = the punch is still travelling
inline constexpr std::uint64_t kStartMs=150;      // before the first forward sample, the fresh intent counts as travelling
inline std::atomic<unsigned long long> holds{0},catchups{0},released{0},rejected{0};
inline std::atomic<int> lastWhy{0};

// first real contact of a punch with an NPC body, copied after the whole pose committed.
struct Contact {
    std::uint64_t gesture=0,epoch=0,request=0,sample=0,tick=0;std::uintptr_t character=0,player=0;int side=-1;
    m::Vec3 point{},from{},to{};std::uint32_t entity=0;bool living=false;
};
inline bool finite3(m::Vec3 v) noexcept {return std::isfinite(v.x)&&std::isfinite(v.y)&&std::isfinite(v.z);}
inline double segmentDistance(m::Vec3 p,m::Vec3 a,m::Vec3 b) noexcept {
    const auto d=b-a;const double l=m::dot(d,d);
    const double t=l>1e-10?std::clamp(m::dot(p-a,d)/l,0.,1.):0.;return m::length(p-(a+d*t));
}
// The touch must sit on the current VR fist segment (12 cm, the H31 palm rule).
// The native admitted punch owns the strike window; curved hand travel need not
// keep the direction measured at gesture onset.
inline bool qualifies(const Contact& c,m::Vec3 dir) noexcept {
    if(c.side<0||c.side>1||!c.gesture||!c.epoch||!c.request||!c.sample||!c.tick||!c.player||!c.character||!c.entity||!c.living||
       !finite3(c.point)||!finite3(c.from)||!finite3(c.to)||!finite3(dir)||m::length(dir)<.5||m::length(c.to-c.from)>.25)return false;
    return segmentDistance(c.point,c.from,c.to)<=.12;
}
// one per punch and side: the first contact wins, later ones of the same punch are ignored.
struct Producer {std::uint64_t gesture=0,epoch=0,request=0;std::uintptr_t player=0,character=0;bool published=false;Contact pending{};};
inline bool firstOf(Producer& p,const Contact& c) noexcept {
    if(p.gesture!=c.gesture||p.epoch!=c.epoch||p.request!=c.request||p.player!=c.player||p.character!=c.character){p={};p.gesture=c.gesture;p.epoch=c.epoch;p.request=c.request;p.player=c.player;p.character=c.character;}
    if(p.published)return false;if(!p.pending.gesture)p.pending=c;p.published=true;return true;
}
inline bool touches(const Contact& c,std::uint64_t gesture,int side,std::uint64_t epoch,std::uint64_t request,
                    std::uintptr_t player,std::uintptr_t character,std::uint64_t now) noexcept {
    return c.gesture&&c.gesture==gesture&&c.side==side&&c.epoch==epoch&&c.request==request&&
        c.player==player&&c.character==character&&c.tick&&now>=c.tick&&now-c.tick<=kContactAgeMs;
}
// still travelling: a newer farthest point of this punch was seen recently (R10 peak), or the punch just started.
inline bool advancing(bool peakValid,std::uint64_t peakGesture,std::uint64_t peakTick,std::uint64_t gesture,
                      std::uint64_t intentTick,std::uint64_t now) noexcept {
    if(peakValid&&peakGesture==gesture)return now>=peakTick&&now-peakTick<=kAdvanceMs;
    return intentTick&&now>=intentTick&&now-intentTick<=kStartMs;
}

enum class Act:int {Native=0,Hold=1,CatchUp=2};
struct Decision {Act act=Act::Native;int why=0;};
// why: 1 touched, 2 travelling, 3 budget spent, 4 not travelling (back / stopped), 5 hold disabled, 6 not in wind-up,
//      7 window too short to catch up, 8 bad dt, 9 native progress (R27)
inline Decision decide(int phase,float prepare,float peak,float total,float dt,bool touched,bool travelling,
                       double heldMs,unsigned budgetMs) noexcept {
    if(!std::isfinite(dt)||dt<=0||dt>.08f)return {Act::Native,8};
    if(touched){
        if((phase==4||phase==8)&&peak>dt+.004f&&peak<=.9f&&total>peak)return {Act::CatchUp,1};
        return {Act::Native,7};
    }
    // R27: the gesture has already been admitted. Holding its native prepare
    // clock while the tracked fist advances delayed the engine sweep until the
    // fist had turned back. Let native time advance; only real contact above
    // may catch it up. Old INI hold values must not reinstate this delay.
    (void)prepare;(void)travelling;(void)heldMs;(void)budgetMs;
    return {Act::Native,9};
}
// Hold = this native update advances 0 s: elapsed and the three remaining rows get the native step back. the original
// then adds dt, nothing else changes (same addresses the weapon clock writes).
inline attackclock50::Plan holdPlan(std::uintptr_t clock,const attackclock50::View& v,float dt) noexcept {
    attackclock50::Plan p{};p.nativeDt=dt;p.effectiveDt=dt;
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
inline attackclock50::Plan catchUpPlan(std::uintptr_t clock,const attackclock50::View& v,float dt) noexcept {
    attackclock50::Binding b{};b.clock=clock;b.intent.kind=attack_timing178r22c2::Kind::Punch;
    return attackclock50::contactPlan59c3(b,v,dt,1.0,true);
}

// current native punch: actor/state/manager slot/action/helper/row and the coordinator rows, like ac50::inspect but for
// an unarmed row (type 5/6/7) and the fist binding. read-only.
struct Ids {std::uintptr_t actor=0,state=0,action=0,helper=0,row=0;int type=-1,hand=-1;std::uintptr_t player=0,entity=0;};
template<class Read> bool inspect(const Ids& id,std::uintptr_t clock,std::uintptr_t base,Read read,attackclock50::View& v) noexcept {
    auto eq=[&](std::uintptr_t p,std::uintptr_t expected){std::uintptr_t x=0;return read(p,&x,8)&&x==expected;};
    auto integer=[&](std::uintptr_t p,int expected){int x=-99;return read(p,&x,4)&&x==expected;};
    auto byte=[&](std::uintptr_t p,unsigned char expected){unsigned char x=255;return read(p,&x,1)&&x==expected;};
    v.check=1;
    if(!base||!id.actor||!id.state||!id.action||!id.helper||!id.row||!clock||!id.player||!id.entity||
       !eq(id.actor+0x1F8,id.player)||!eq(id.player,base+0x3AA4B98)||!eq(id.player+0x38,id.entity)||
       !eq(id.actor,base+0x3A80610)||!eq(id.actor+0x210,id.state)||!eq(id.state+0xCE0,id.actor)||
       !eq(id.actor+0x3C0,clock)||!eq(clock+8,id.actor))return false;
    std::uintptr_t manager=0,slots=0,type=0,end=0,gate=0;
    v.check=2;
    if(!read(id.actor+0x200,&manager,8)||!manager||!read(manager+0x38,&slots,8)||!slots||!eq(slots,id.action))return false;
    v.check=3;
    if(!eq(id.action,base+0x3A61C58)||!eq(id.action+0x40,manager)||!eq(id.action+0x70,id.actor)||
       !eq(id.action+0xA0,id.helper)||!eq(id.action+0x58,id.row)||!integer(id.action+0x3C,0)||!byte(id.action+0x28,1)||
       !eq(id.helper,base+0x3A61040)||!eq(id.helper+8,id.actor)||!eq(id.helper+0x20,id.action+0x88)||
       !eq(id.action+0x88,base+0x3A61440)||!byte(clock+0x14,1))return false;
    v.check=4;
    if(!read(id.row+0x148,&type,8)||!type||!integer(type+8,id.type)||(id.type<5||id.type>7)||
       !integer(id.row+0x224,id.hand)||!integer(id.state+0x1B8,id.hand)||
       !read(id.actor+0x220,&gate,8)||!gate||!byte(gate+0x28,0)||!integer(id.state+0x218,id.type)||!read(id.state+0x38,&v.phase,4)||(v.phase!=4&&v.phase!=8)||
       !read(clock+0x58,&v.records,8)||!read(clock+0x60,&end,8)||!v.records||(v.records&3)||end<v.records||
       end-v.records!=3*sizeof(attackclock50::Record)||
       !read(clock+0x10,&v.rate,4)||!read(clock+0x50,&v.scaledElapsed,4)||!read(clock+0x54,&v.elapsed,4)||
       !read(v.records,v.rows.data(),sizeof(v.rows)))return false;
    v.check=5;
    if(!std::isfinite(v.rate)||v.rate<.1f||v.rate>8||!std::isfinite(v.elapsed)||v.elapsed<0||v.elapsed>30||
       !std::isfinite(v.scaledElapsed)||v.scaledElapsed<0||v.scaledElapsed>30)return false;
    unsigned mask=0;v.check=6;
    for(const auto& r:v.rows){const unsigned k=r.kind&255;if(k>2||(mask&(1u<<k)))return false;mask|=1u<<k;
        for(float x:{r.end,r.scaledEnd,r.left,r.scaledLeft})if(!std::isfinite(x)||x<0||x>30)return false;
        if(r.left>r.end+.001f||r.scaledLeft>r.scaledEnd+.001f)return false;}
    v.check=0;return mask==7;
}
}
