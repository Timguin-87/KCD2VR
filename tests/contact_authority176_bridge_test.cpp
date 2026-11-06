// Exact production sweep/predictor authority helpers, synthetic native memory.
// No retail instruction executes and no Windows/headset result is claimed.
#include <intrin.h>
static void* caller176=nullptr;
static void* Caller176(){return caller176;}
#undef _ReturnAddress
#define _ReturnAddress Caller176
#include "../src/frik_pose_probe.cpp"
#undef _ReturnAddress
#include "../src/vrik/contact_contract176.h"
#include <cstdio>
#include <limits>
namespace kcd2vr::diagnostic_trace {void Write(const char*) noexcept {}}
namespace kcd2vr::retail_build112 {extern bool hostRetailVerified112;}
using namespace kcd2vr::frik_pose_probe;
namespace ca=kcd2vr::contact176;
static unsigned checks=0,failures=0,calls=0;
static ca::Payload observed{};
static const QuatT28 *seenPrevious=nullptr,*seenCurrent=nullptr;
static void ck(bool ok,const char* s){++checks;if(!ok)++failures;std::printf("%s %s\n",ok?"PASS":"FAIL",s);}
template<class T> static void put(void* p,std::size_t o,T v){std::memcpy(static_cast<unsigned char*>(p)+o,&v,sizeof(v));}
static void* __fastcall NativeSweep(void*,int,int,const QuatT28* p,const QuatT28* c,float){
    ++calls;seenPrevious=p;seenCurrent=c;observed=ca::current?*ca::current:ca::Payload{};return reinterpret_cast<void*>(0x42);
}
int main(){
    hostModuleHandleW=reinterpret_cast<void*>(0x180000000ull);hostTickCount64=1000;
    g_wh_pinned104=0;g_combat_experiment175=true;g_weapon_control_mode=7;
    kcd2vr::diagnostics::capture168::gate.mask=0;
    PhysFrame6698 f{};f.player=reinterpret_cast<void*>(0x100);g_last_player_entity=reinterpret_cast<void*>(0x200);
    ph::Command c{};c.kind=ph::CommandKind::Slash;c.side=0;c.tick=990;c.sample=4;c.bodyEpoch=8;c.epoch=9;c.key=10;c.generation=11;
    PublishPhysicalStrike176(c,f);ca::Ticket t{};ck(ca::released.read(t)&&t.serial&&t.tick==990&&t.sample==4,"actual release publisher preserves original physical intent age");
    ca::Live live{};live.identity={0x100,0x200,0x300,0x400,8,9,10,11,5,995};live.ticket=t;live.now=1000;
    ca::Payload a{};a.identity=live.identity;a.ticket=t;a.wrapper=0x500;a.action=0x600;a.hand=0;a.bone=13;a.dt=.016f;a.redirected=a.moving=true;
    ca::claimedSerial=0;
    ck(ca::admit(live,0x570,22)==ca::Decision::NoScope,"no cached or asynchronous authority outside native sweep");
    {ca::SweepScope scope(a);
     ck(ca::admit(live,0x570,0)==ca::Decision::Target,"unknown target cannot consume a release");
     ck(ca::admit(live,0x498,22)==ca::Decision::Details,"returned prediction +98 is not damaging publisher +170");
     auto changed=live;changed.identity.generation++;
     ck(ca::admit(changed,0x570,22)==ca::Decision::Identity,"replacement weapon generation cannot inherit scope");
     changed=live;changed.identity.sample++;
     ck(ca::admit(changed,0x570,22)==ca::Decision::Identity,"newer frame is not this borrowed sweep frame");
     changed=live;changed.now=1146;
     ck(ca::admit(changed,0x570,22)==ca::Decision::PoseAge,"original pose age remains bounded (150ms since V66.178R6)");
     changed=live;changed.ticket.serial++;
     ck(ca::admit(changed,0x570,22)==ca::Decision::Ticket,"different physical release cannot join by timing");
     {ca::SweepScope nested;ck(ca::admit(live,0x570,22)==ca::Decision::Identity,"unrelated nested sweep masks outer authority");}
     ck(ca::admit(live,0x570,22)==ca::Decision::Admit,"qualified exact native contact admitted with every recorder OFF");
     ck(ca::admit(live,0x570,23)==ca::Decision::Repeat,"same release cannot create another primary contact even on another target");}
    ck(ca::current==nullptr,"scope cannot escape synchronous original call");
    auto deny=[&](ca::Payload p,ca::Decision expected,const char* label){ca::SweepScope s(p);ck(ca::admit(live,0x570,22)==expected,label);};
    ca::claimedSerial=0;auto b=a;b.redirected=false;deny(b,ca::Decision::NoRedirect,"canned predictor has no tracked contact authority");
    b=a;b.moving=false;deny(b,ca::Decision::NoMotion,"duplicate sample or resting pressure cannot replay stroke");
    b=a;b.dt=std::numeric_limits<float>::quiet_NaN();deny(b,ca::Decision::NoRedirect,"nonfinite native dt cannot qualify");
    b=a;b.hand=1;deny(b,ca::Decision::NoRedirect,"offhand/native slot cannot borrow primary stroke");
    // V66.178R6: the native sweep runs 0.4-1.0 s after the release (animation-bound), so a release
    // stays valid for the attack it started; only a newer release/prepare/cancel (policy) or the
    // 1.5 s safety cap revokes it.
    b=a;b.ticket.tick=879;live.ticket.tick=879;{ca::SweepScope s(b);ck(ca::admit(live,0x570,22)==ca::Decision::Admit,"release 121 ms old still owns its own native attack sweep");}
    {ca::Identity late=live.identity;late.sample=t.sample+10;
     ck(ca::ticketMatches(t,late,t.tick+1499)&&!ca::ticketMatches(t,late,t.tick+1501),"release older than the 1.5 s safety cap is revoked");
     late.sample=t.sample+151;ck(ca::ticketMatches(t,late,t.tick+100),"fresh release is not revoked by higher pose sample density");
     ck(!ca::ticketMatches(t,late,t.tick+1501),"original release time cap remains unchanged at higher sample density");
     late.sample=t.sample-1;ck(!ca::ticketMatches(t,late,t.tick+100),"out-of-order source sequence still rejects");}
    live.ticket.tick=a.ticket.tick;ca::claimedSerial=0;
    g_combat_experiment175=false;PublishPhysicalStrike176(c,f);ca::released.read(t);
    ck(!t.serial,"V175 F12 weapon mode does not publish F4 authority");
    g_combat_experiment175=true;c.kind=ph::CommandKind::BowFire;PublishPhysicalStrike176(c,f);ca::released.read(t);
    ck(!t.serial,"bow release cannot publish melee authority");
    c.kind=ph::CommandKind::Pommel;PublishPhysicalStrike176(c,f);ca::released.read(t);
    ck(!t.serial,"unqualified pommel cannot publish ordinary slash/thrust authority");

    // Actual A7 detour forwards native arguments once and installs gameplay scope
    // only after the exact current predictor marked its borrowed inputs redirected.
    PredictionScope120 prediction{};prediction.redirected=true;prediction.collisions=reinterpret_cast<void*>(0x400);
    prediction.hand=0;prediction.bone=13;prediction.authority176=a;g_prediction_scope120=&prediction;
    g_phys_native_sweep_original110=&NativeSweep;QuatT28 previous{},current{};
    g_phys_collision_owner110=nullptr;caller176=reinterpret_cast<void*>(WhBase()+0xA7C9C1);
    const auto out=PhysNativeSweepProbe110(prediction.collisions,0,13,&previous,&current,.016f);
    ck(out==reinterpret_cast<void*>(0x42)&&calls==1&&seenPrevious==&previous&&seenCurrent==&current,"actual sweep preserves original native return and arguments exactly once");
    ck(observed.redirected&&observed.identity.sample==5&&observed.ticket.serial==a.ticket.serial&&!ca::current,"actual sweep publishes independent qualified F4 scope only inside original");
    {ca::SweepScope outer(a);caller176=reinterpret_cast<void*>(WhBase()+0xA7EB58);
     PhysNativeSweepProbe110(prediction.collisions,0,13,&previous,&current,.016f);
     ck(!observed.redirected&&ca::current==&outer.value,"actual unqualified live-bone sweep masks then restores enclosing authority");}
    caller176=reinterpret_cast<void*>(WhBase()+0xA7C9C1);prediction.redirected=false;
    PhysNativeSweepProbe110(prediction.collisions,0,13,&previous,&current,.016f);
    ck(!observed.redirected,"actual sweep refuses prediction inputs before midhook redirect");
    prediction.redirected=true;g_combat_experiment175=false;
    PhysNativeSweepProbe110(prediction.collisions,0,13,&previous,&current,.016f);
    ck(!observed.redirected,"actual V175 F12 sweep keeps F4 authority empty");
    g_prediction_scope120=nullptr;

    std::array<unsigned char,0xA00> player{};
    std::array<unsigned char,0x400> actor{},collisions{};
    std::array<unsigned char,0xD10> state{};
    std::array<unsigned char,0x40> entity{};
    std::array<unsigned char,0x80> manager{};
    std::array<unsigned char,0x280> slots{};
    const auto base=WhBase();
    put(player.data(),0,base+0x3AA4B98);put(actor.data(),0,base+0x3A80610);
    put(entity.data(),0,base+0x4072C78);put(collisions.data(),0,base+0x3B91300);
    put(manager.data(),0,base+0x3EC9958);put(player.data(),0x38,entity.data());
    put(player.data(),0x278,actor.data());put(actor.data(),0x1F8,player.data());
    put(actor.data(),0x210,state.data());put(state.data(),0xCE0,actor.data());
    put(actor.data(),0x218,collisions.data());put(collisions.data(),8,actor.data());
    put(player.data(),0x9D0,manager.data());put(manager.data(),0x38,player.data());
    put(player.data(),0x990,slots.data());put(slots.data(),0x260,std::uint64_t(10));
    put(state.data(),0x218,0);put(state.data(),0x1B8,0);put(state.data(),0x248,0);
    g_last_player_entity=entity.data();g_henry_item_attach_manager=manager.data();
    g_combat_experiment175=true;g_weapon_control_mode=7;g_census_mode=4;g_gameplay_allowed=true;
    g_head_valid=g_right_valid=g_left_valid=true;g_phys_armed6698=true;
    g_prediction_contract120=base;kcd2vr::retail_build112::hostRetailVerified112=true;
    g_weapon_request_epoch6694=9;g_pose_epoch6689=8;
    TrackingSnapshot tracking{};tracking.head_valid=tracking.left_valid=tracking.right_valid=tracking.allowed=true;
    tracking.tick=tracking.head_good_tick=tracking.left_good_tick=tracking.right_good_tick=1000;
    g_tracking_frames6695.publish(tracking);
    f.player=player.data();f.manager=manager.data();f.request=9;f.input.bodyEpoch=8;f.input.sample=5;f.input.tick=995;
    f.input.allowed=f.input.attacks=f.input.tracked[0]=f.input.tracked[1]=true;
    f.output.active[0]=f.output.owned[0]=true;f.profiles[0].primary=true;f.profiles[0].classId=1;
    f.profiles[0].key=10;f.profiles[0].generation=11;g_phys_frame6698.publish(f);g_phys_ready_sample6698=5;
    ca::Live actual{};
    ck(CurrentF4ContactLive176(actual)&&actual.identity.actor==reinterpret_cast<std::uintptr_t>(actor.data()),"actual current-live resolver qualifies reciprocal native identity and completed fresh melee frame");
    g_phys_ready_sample6698=4;
    ck(!CurrentF4ContactLive176(actual),"actual resolver rejects in-progress unpublished pose");g_phys_ready_sample6698=5;
    put(slots.data(),0x260,std::uint64_t(12));
    ck(!CurrentF4ContactLive176(actual),"actual resolver rejects held-item replacement");put(slots.data(),0x260,std::uint64_t(10));
    put(state.data(),0x308,std::uint8_t(1));
    ck(!CurrentF4ContactLive176(actual),"actual resolver rejects native blocking even with earlier release ticket");put(state.data(),0x308,std::uint8_t(0));
    g_guard_manual_owner112=player.data();g_guard_manual112=true;
    ck(!CurrentF4ContactLive176(actual),"actual resolver rejects held manual LT before native block state catches up");
    c.kind=ph::CommandKind::Slash;PublishPhysicalStrike176(c,f);ca::released.read(t);
    ck(!t.serial,"actual release publisher cannot grant strike while manual LT owns defense");
    g_guard_manual112=false;g_guard_manual_owner112=nullptr;
    g_guard_physical_owner112=player.data();g_guard_physical112=true;
    ck(!CurrentF4ContactLive176(actual),"actual resolver rejects physical guard ownership");
    g_guard_physical112=false;g_guard_physical_owner112=nullptr;
    tracking.right_held=true;g_tracking_frames6695.publish(tracking);
    ck(!CurrentF4ContactLive176(actual),"actual resolver rejects held tracking grace as new strike authority");
    tracking.right_held=false;g_tracking_frames6695.publish(tracking);
    PublishPhysicalStrike176(c,f);ca::released.read(t);CurrentF4ContactLive176(actual);
    ck(ca::ticketMatches(t,actual.identity,actual.now),"actual fresh physical release joins current exact item identity");
    PhysCancelAttack117();
    ck(!ca::ticketMatches(t,actual.identity,actual.now),"actual cancellation revokes earlier release despite unchanged item/frame and timestamps");

    unsigned pinReads=0;
    auto pins=[&](std::uintptr_t rva,void* out,std::size_t n){++pinReads;for(const auto& p:ca::retailPins)if(p.rva==rva&&n==16){std::memcpy(out,p.bytes.data(),16);return true;}return false;};
    ck(ca::retailContract(pins)&&pinReads==std::size(ca::retailPins),"retail contract validates every native publisher/borrow/false-return pin");
    ck(!ca::retailContract([&](auto r,void* p,auto n){if(!pins(r,p,n))return false;if(r==0xC52534)static_cast<unsigned char*>(p)[0]^=1;return true;}),"changed Process false-return branch fails contract");
    namespace cc=kcd2vr::diagnostics::combat_capture;
    namespace cap=kcd2vr::diagnostics::capture168;
    cc::Event event{};while(cc::pop(event)){}
    f.input.physicalCombat177=true;f.input.tick=1000;f.input.haveWorld=true;f.input.worldScale=1;
    f.output.owned[0]=true;f.output.contact119[0].known=f.output.contact119[0].plane=true;
    f.output.impact177[0].impulse=1;f.output.impact177[0].energy=.5;
    cap::gate.mask=0;ObservePhysicalImpact177(f);
    ck(!cc::pop(event),"rung2 impact recorder OFF cannot emit or format telemetry");
    cap::gate.select(cap::Domain::Interaction,true,1000);ObservePhysicalImpact177(f);
    ck(!cc::pop(event),"F5 alone cannot turn on F4 impact recording");
    cap::gate.select(cap::Domain::Combat,true,1000);ObservePhysicalImpact177(f);
    ck(cc::pop(event)&&event.kind==cc::Kind::PhysicalImpact177&&event.sequence==f.input.sample&&event.value==.5f,
       "F3 receives bounded copied mechanics with original sample identity");
    f.output.contact119[0].limited115=true;ObservePhysicalImpact177(f);
    ck(!cc::pop(event),"deferred contact cannot be logged as a qualified physical impact");
    f.output.contact119[0].limited115=false;f.input.physicalCombat177=false;ObservePhysicalImpact177(f);
    ck(!cc::pop(event),"rung1 cannot emit rung2 physical impact evidence");
    std::printf("contact_authority176_bridge %u/%u PASS\n",checks-failures,checks);return failures?1:0;
}
