#pragma once
#include "attack_timing178r22c2.h"
#include "contact_authority176.h"
#include "../core/snapshot_mailbox.h"
#include <array>
#include <cstring>

namespace kcd2vr::attackclock50 {
struct Pin {std::uintptr_t rva;std::array<unsigned char,16> bytes;};
inline constexpr Pin pins[]{
    {0x9E3BFF,{0x39,0x43,0x70,0x74,0xC1,0x89,0x43,0x70,0x33,0xD2,0x48,0x8B,0x0D,0x70,0x9C,0xF4}},
    {0x9E3C16,{0x0F,0x28,0xC8,0x0F,0x28,0xE8,0xF3,0x0F,0x59,0x4B,0x10,0x0F,0x57,0xF6,0xF3,0x0F}},
    {0x9E3C24,{0xF3,0x0F,0x58,0x4B,0x50,0xF3,0x0F,0x11,0x4B,0x50,0x0F,0x28,0xC8,0xF3,0x0F,0x58}},
    {0x9E3C4E,{0xF3,0x0F,0x10,0x40,0x0C,0x0F,0x28,0xDD,0xF3,0x0F,0x59,0x5B,0x10,0xF3,0x0F,0x5C}},
    {0x9E3C70,{0xF3,0x0F,0x10,0x40,0x10,0xF3,0x0F,0x5C,0xC3,0x0F,0x28,0xC8,0xF3,0x0F,0x5F,0xCC}},
    {0x9E3CC8,{0x48,0x8B,0x43,0x08,0x48,0x8B,0x88,0x10,0x02,0x00,0x00,0x48,0x81,0xC1,0xF0,0x03}},
    {0x9E3CF7,{0x48,0x8B,0x43,0x08,0x48,0x8B,0x88,0x10,0x02,0x00,0x00,0x48,0x81,0xC1,0x50,0x04}},
    {0x9E3D0F,{0x48,0x8B,0x43,0x08,0x48,0x8B,0x88,0x10,0x02,0x00,0x00,0x48,0x81,0xC1,0x20,0x04}},
    {0xB27460,{0x48,0x8B,0x47,0x70,0xBE,0x04,0x00,0x00,0x00,0x8B,0xD6,0x48,0x8B,0x88,0x10,0x02}},
    {0xD4D15B,{0x48,0x8B,0x43,0x08,0x48,0x8B,0x88,0x10,0x02,0x00,0x00,0x48,0x81,0xC1,0xF0,0x03}},
    {0xD4CB82,{0x48,0x8D,0x05,0xD3,0x57,0xF0,0xFF,0x48,0x89,0x7C,0x24,0x40,0x48,0x8D,0x4D,0x28}},
    {0x9CC46A,{0x48,0x8B,0x46,0x38,0x48,0x8D,0x4D,0x48,0x4D,0x8B,0xFE,0x4A,0x8D,0x14,0xF0,0xE8}},
    {0x90D1B4,{0x4C,0x8B,0x49,0x40,0x44,0x8B,0x41,0x38,0x89,0x51,0x38,0x4D,0x85,0xC9,0x74,0x0E}},
    {0x1A72810,{0x8B,0x41,0x3C,0xC3,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC}},
};
// Mod-owned identity/evidence, never a retail object or an accepted-hit flag.
struct Binding {
    attack_timing178r22c2::Intent intent{};
    std::uintptr_t action=0,helper=0,clock=0,manager=0,row=0,entity=0;
};
struct Contact {std::uintptr_t actor=0;std::uint64_t serial=0,policy=0,key=0,generation=0,epoch=0,request=0,tick=0,sample=0,releaseTick=0;
    std::uint64_t gesture59c17=0;}; // R59.16C10R17: the stroke's gesture id (a second identity proof for the touched catch-up)
// R59.16C10R17: the R50C2 consumer required the release command's exact sample AND tick; the F4 press-1 release is deferred one
// native frame (PhysDeferRelease103) and the catch-up almost never saw a match. The same gesture of the same weapon/body/request
// is the same stroke.
inline bool touches(const Contact& c,const attack_timing178r22c2::Intent& i) noexcept {
    if(c.key!=i.key||c.generation!=i.generation||c.epoch!=i.epoch||c.request!=i.request)return false;
    return (c.sample==i.sample&&c.releaseTick==i.tick)||(c.gesture59c17&&c.gesture59c17==i.gesture59c3);
}
inline bool sameRelease(const Binding& b,const contact176::Ticket& t) noexcept {
    const auto& i=b.intent;
    return t.serial&&t.player==i.player&&t.entity==b.entity&&t.epoch==i.epoch&&t.request==i.request&&
        t.key==i.key&&t.generation==i.generation&&t.sample==i.sample&&t.tick==i.tick&&t.gesture59c3==i.gesture59c3;
}
inline core::SnapshotMailbox<Contact> contact;
inline std::atomic<bool> enabled{true};
struct Record {std::uint32_t kind=0;float end=0,scaledEnd=0,left=0,scaledLeft=0;};
static_assert(sizeof(Record)==0x14);
struct View {std::uintptr_t records=0;int phase=0,check=0;float rate=0,elapsed=0,scaledElapsed=0;std::array<Record,3> rows{};};

// Native callers prove these fields: B27308 / 9CC3D4 / D4D950 / 9E3B80.
// The current manager slot is checked BEFORE dereferencing the borrowed action.
template<class Read> bool inspect(const Binding& b,std::uintptr_t base,Read read,View& v) noexcept {
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
    if(!read(b.manager+0x38,&slots,8)||!slots||!eq(slots,b.action))return false; // main combat channel 0
    v.check=3;
    if(!eq(b.action,base+0x3A61C58)||!integer(b.action+0x3C,0)||!eq(b.action+0x40,b.manager)||
       !byte(b.action+0x28,1)||!eq(b.action+0x70,a)||!eq(b.action+0xA0,b.helper)||!eq(b.action+0x58,b.row)||
       !eq(b.helper,base+0x3A61040)||!eq(b.helper+8,a)||!eq(b.helper+0x20,b.action+0x88)||
       !eq(b.action+0x88,base+0x3A61440)||!byte(b.clock+0x14,1))return false;
    std::uintptr_t type=0,gate=0,end=0;int kind=-1;
    v.check=4;
    if(!read(b.row+0x148,&type,8)||!type||!read(type+8,&kind,4)||(kind!=0&&kind!=1)||
       !read(a+0x220,&gate,8)||!gate||!byte(gate+0x28,0)||
       !read(s+0x38,&v.phase,4)||(v.phase!=4&&v.phase!=8)||
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
        const unsigned k=r.kind&255; // upper bytes are native padding
        if(k>2||(mask&(1u<<k)))return false;mask|=1u<<k;
        for(float x:{r.end,r.scaledEnd,r.left,r.scaledLeft})if(!std::isfinite(x)||x<0||x>30)return false;
        if(r.left>r.end+.001f||r.scaledLeft>r.scaledEnd+.001f)return false;
    }
    v.check=0;return mask==7;
}
inline float remaining(const View& v,unsigned kind) noexcept {for(auto& r:v.rows)if((r.kind&255)==kind)return r.left;return -1;}
struct Patch {std::uintptr_t address=0;float before=0,after=0;};
struct Plan {std::array<Patch,8> writes{};float nativeDt=0,effectiveDt=0;bool adjusted=false,contactCatchup=false;};

// Precompensate the proven native dt update. The original then advances every
// deadline, publishes all three notifying properties and performs strike entry.
// No phase, result, damage, stamina, cooldown or opponent field is written here.
inline Plan plan(const Binding& b,const View& v,float dt,double speed,bool touched) noexcept {
    Plan p{};p.nativeDt=p.effectiveDt=dt;
    if((v.phase!=4&&v.phase!=8)||!std::isfinite(dt)||dt<=0||dt>.08f||
       !std::isfinite(speed)||speed<.35||speed>30)return p;
    const float reference=b.intent.kind==attack_timing178r22c2::Kind::Thrust?2.5f:4.5f;
    // R59.16C10R17: never SLOW the hidden attack below its native pace (the .75 floor delayed the NPC's hit sync for slow cuts).
    float effective=dt*float(std::clamp(std::sqrt(speed/reference),1.0,2.5));
    const float prepare=remaining(v,0),peak=remaining(v,1),total=remaining(v,2);
    if(prepare<0||peak<0||total<=0)return p;
    // A real, freshly qualified contact can catch up the SAME admitted stroke.
    // Limit to six native steps / 100 ms. Let the original consume the final
    // native step: pinning peak-.002 can converge to a positive float forever.
    if(touched&&peak>0){effective=std::max(effective,std::min(peak,dt*6));p.contactCatchup=true;}
    effective=std::min(effective,.1f);
    if(peak<=dt+.002f)return p; // native deadline crossing, never a near-zero step
    effective=std::min(effective,peak-.002f);
    if(!touched&&v.phase==4&&prepare>0)
        // The prepare-boundary limiter may reduce extra acceleration, never
        // rewind the original native step. A 40ms frame with 5ms preparation
        // left formerly became 15ms despite the R17 1.0x multiplier floor.
        effective=std::max(dt,std::min(effective,prepare+.01f));
    effective=std::min(effective,total);
    // Never freeze an expired window or speed up recovery after peak contact.
    if(peak<=0||effective<=0)return p;
    float delta=effective-dt;
    delta=std::max(delta,-std::min(v.elapsed,v.scaledElapsed/v.rate));
    if(std::fabs(delta)<.00001f)return p;
    p.writes[0]={b.clock+0x50,v.scaledElapsed,v.scaledElapsed+delta*v.rate};
    p.writes[1]={b.clock+0x54,v.elapsed,v.elapsed+delta};
    unsigned index=2;
    for(unsigned n=0;n<3;++n){const auto& row=v.rows[n];
        p.writes[index++]={v.records+n*sizeof(Record)+0xC,row.left,std::max(0.f,row.left-delta)};
        p.writes[index++]={v.records+n*sizeof(Record)+0x10,row.scaledLeft,std::max(0.f,row.scaledLeft-delta*v.rate)};
    }
    p.effectiveDt=dt+delta;p.adjusted=true;return p;
}
// R59C3: a qualified physical impact determines when wind-up finishes.
// Advance the SAME coordinator; run the original notification/entry path. Never call D4D34C out of order, spoof phase 8, or
// advance the NPC's clock/global frame time.
// R59.16C10R17 (Miki: "adapt the enemies' reactions to our speed"): R59C3 advanced only to PREPARE expiry, which left the hit sync
// ~0.16 s behind the real blade (the NPC's block, parry and hit reaction all key on Henry's +0x450). A touched stroke in phase 4
// OR 8 now advances to 2 ms before the HIT: phase 8 opens in this native update (prepare expired) and the hit sync is the next one;
// the NPC's aligned block (9E3D3D follower) re-reads +0x450 every frame and meets the blade. Strike/total stay positive.
inline Plan contactPlan59c3(const Binding& b,const View& v,float dt,double speed,bool touched) noexcept {
    auto p=plan(b,v,dt,speed,touched);
    if(!touched||(v.phase!=4&&v.phase!=8)||!std::isfinite(dt)||dt<=0||dt>.08f||
       !std::isfinite(speed)||speed<.35||speed>30)return p;
    const float prepare=remaining(v,0),peak=remaining(v,1),total=remaining(v,2);
    if(prepare<0||peak<=dt+.004f||peak>.9f||total<=peak)return p;
    const float effective=peak-.002f,delta=effective-dt;
    p.writes[0]={b.clock+0x50,v.scaledElapsed,v.scaledElapsed+delta*v.rate};
    p.writes[1]={b.clock+0x54,v.elapsed,v.elapsed+delta};
    unsigned index=2;
    for(unsigned n=0;n<3;++n){const auto& row=v.rows[n];
        p.writes[index++]={v.records+n*sizeof(Record)+0xC,row.left,std::max(0.f,row.left-delta)};
        p.writes[index++]={v.records+n*sizeof(Record)+0x10,row.scaledLeft,std::max(0.f,row.scaledLeft-delta*v.rate)};
    }
    p.nativeDt=dt;p.effectiveDt=effective;p.adjusted=true;p.contactCatchup=true;return p;
}

enum class Applied {Unchanged,Changed,Stale,Fault,RollbackFault};
template<class Read,class Write> Applied apply(const Plan& p,Read read,Write write) noexcept {
    if(!p.adjusted)return Applied::Unchanged;
    for(auto& w:p.writes){float current=0;if(!read(w.address,&current,4)||std::memcmp(&current,&w.before,4))return Applied::Stale;}
    for(unsigned i=0;i<p.writes.size();++i){auto& w=p.writes[i];
        if(!write(w.address,&w.after,4)){
            bool restored=true;for(unsigned j=0;j<=i;++j){auto& r=p.writes[j];restored=write(r.address,&r.before,4)&&restored;}
            return restored?Applied::Fault:Applied::RollbackFault;
        }
    }
    return Applied::Changed;
}
}
