// C39: DIRECT guard-out-of-stagger policy (one native block press per PB opening) + enemy attack-start / counter-side receipts.
#include "../src/vrik/direct_counter39.h"
#include <cstdio>
namespace dc=kcd2vr::direct_counter39;
static int fails=0;
#define CHECK(c) do{if(!(c)){std::printf("FAIL %s:%d %s\n",__FILE__,__LINE__,#c);++fails;}}while(0)
static dc::StaggerIn base(){dc::StaggerIn in{};in.enabled=true;in.posture=true;in.windowOpen=true;in.attacking=true;in.phase=64;in.type=1;in.window=1;in.now=1000;return in;}
int main(){
    // master-strike stance map (combat_zone.xml): blow from Henry's left -> strike from the right, overhead -> stab, stab -> overhead
    CHECK(dc::msZone(0)==5&&dc::msZone(1)==2&&dc::msZone(2)==1&&dc::msZone(5)==0&&dc::msZone(3)==4&&dc::msZone(4)==3&&dc::msZone(-1)==-1&&dc::msZone(6)==-1);
    CHECK(dc::counterSide(2,1)&&dc::counterSide(1,2)&&dc::counterSide(5,0)&&dc::counterSide(0,5));
    CHECK(!dc::counterSide(1,1)&&!dc::counterSide(-1,1)&&!dc::counterSide(4,3)&&!dc::counterSide(2,-1));
    CHECK(dc::staggered(64,1)&&dc::staggered(32,-1)&&dc::staggered(32,0)&&!dc::staggered(64,2)&&!dc::staggered(2,1)&&!dc::staggered(128,1)&&!dc::staggered(8,1));
    // opening serial: own monotonic count, unreadable samples never open/close, opponent change rebases
    {dc::Opening o{};CHECK(!o.observe(1,7,true,0));CHECK(o.observe(1,7,true,1)&&o.serial==1&&o.open);CHECK(!o.observe(1,7,true,1));
     CHECK(!o.observe(1,7,false,0)&&o.open);CHECK(!o.observe(1,7,true,-1)&&o.open);CHECK(!o.observe(1,7,true,0)&&!o.open);
     CHECK(o.observe(1,7,true,2)&&o.serial==2);CHECK(o.observe(2,8,true,1)&&o.serial==3);CHECK(!o.observe(0,8,true,1));}
    // one press per opening, only with posture/attacker/window/stagger, never with LT or a foreign native block
    {dc::Stagger s{};dc::Why w{};auto in=base();
     auto t=in;t.enabled=false;CHECK(dc::step(s,t,w)==dc::Act::None&&w==dc::Why::Disabled);
     t=in;t.posture=false;CHECK(dc::step(s,t,w)==dc::Act::None&&w==dc::Why::Posture);
     t=in;t.phase=2;CHECK(dc::step(s,t,w)==dc::Act::None&&w==dc::Why::Phase);
     t=in;t.windowOpen=false;CHECK(dc::step(s,t,w)==dc::Act::None&&w==dc::Why::Window);
     t=in;t.window=0;CHECK(dc::step(s,t,w)==dc::Act::None&&w==dc::Why::Window);
     t=in;t.attacking=false;CHECK(dc::step(s,t,w)==dc::Act::None&&w==dc::Why::Attacker);
     t=in;t.manual=true;CHECK(dc::step(s,t,w)==dc::Act::None&&w==dc::Why::Manual);
     t=in;t.blocking=true;CHECK(dc::step(s,t,w)==dc::Act::None&&w==dc::Why::Blocking);
     t=in;t.held=true;CHECK(dc::step(s,t,w)==dc::Act::None&&w==dc::Why::Held);
     CHECK(!s.pending&&!s.tried);
     CHECK(dc::step(s,in,w)==dc::Act::Press&&w==dc::Why::Pressed&&s.pending&&s.tried==1);
     auto h=in;h.held=true;h.now=1100;CHECK(dc::step(s,h,w)==dc::Act::Keep&&w==dc::Why::Holding);
     h.phase=32;h.now=1500;CHECK(dc::step(s,h,w)==dc::Act::Keep);
     h.phase=128;CHECK(dc::step(s,h,w)==dc::Act::Adopted&&w==dc::Why::Accepted&&!s.pending);
     // the same opening is spent even after the guard drops again
     auto again=in;again.now=1600;CHECK(dc::step(s,again,w)==dc::Act::None&&w==dc::Why::Spent);
     auto next=in;next.window=2;next.now=1700;CHECK(dc::step(s,next,w)==dc::Act::Press);
     // not adopted: phase leaves 32/64 -> release (ordinary posture press follows), timeout, posture lost, LT, foreign release
     auto l=next;l.held=true;l.phase=2;CHECK(dc::step(s,l,w)==dc::Act::Release&&w==dc::Why::PhaseLeft&&!s.pending);
     auto n3=in;n3.window=3;n3.now=2000;CHECK(dc::step(s,n3,w)==dc::Act::Press);
     auto to=n3;to.held=true;to.now=2000+dc::kStaggerHoldMs;CHECK(dc::step(s,to,w)==dc::Act::Keep);
     to.now=2001+dc::kStaggerHoldMs;CHECK(dc::step(s,to,w)==dc::Act::Release&&w==dc::Why::Timeout);
     auto n4=in;n4.window=4;CHECK(dc::step(s,n4,w)==dc::Act::Press);auto pl=n4;pl.held=true;pl.posture=false;CHECK(dc::step(s,pl,w)==dc::Act::Release&&w==dc::Why::PostureLost);
     auto n5=in;n5.window=5;CHECK(dc::step(s,n5,w)==dc::Act::Press);auto lt=n5;lt.held=true;lt.manual=true;CHECK(dc::step(s,lt,w)==dc::Act::Release);
     auto n6=in;n6.window=6;CHECK(dc::step(s,n6,w)==dc::Act::Press);auto fr=n6;fr.held=false;CHECK(dc::step(s,fr,w)==dc::Act::None&&w==dc::Why::Foreign&&!s.pending);
     auto n7=in;n7.window=7;CHECK(dc::step(s,n7,w)==dc::Act::Press);auto bk=n7;bk.held=true;bk.blocking=true;CHECK(dc::step(s,bk,w)==dc::Act::Adopted);
     auto ck=in;ck.window=8;ck.now=500;CHECK(dc::step(s,ck,w)==dc::Act::Press);auto back=ck;back.held=true;back.now=400;CHECK(dc::step(s,back,w)==dc::Act::Release&&w==dc::Why::Timeout);}
    // enemy attack start: never adopted mid-attack, opens on 4/8 from a known non-attack phase, closes on leaving, 4->8 stays one window
    {dc::Attack a{};using E=dc::Attack::Edge;
     CHECK(a.observe(10,11,5,true,4,1,100)==E::None&&!a.open);          // first sample already attacking: baseline only
     CHECK(a.observe(10,11,5,true,8,1,110)==E::None&&!a.open);
     CHECK(a.observe(10,11,5,true,2,-1,120)==E::None);
     CHECK(a.observe(10,11,5,true,4,1,130)==E::Open&&a.open&&a.serial==1&&a.openTick==130&&a.offered==1);
     CHECK(a.observe(10,11,5,true,8,1,200)==E::None&&a.open);
     CHECK(a.observe(10,11,5,false,-1,-1,210)==E::None&&a.open);       // unreadable: no edge
     CHECK(a.observe(10,11,5,true,16,-1,300)==E::Close&&!a.open);
     CHECK(a.observe(10,11,5,true,4,-1,400)==E::Open&&a.serial==2&&a.offered==-1);
     CHECK(a.observe(10,11,5,true,4,2,410)==E::None&&a.offered==2);    // offered zone filled in once readable
     CHECK(a.observe(20,21,6,true,4,0,420)==E::Lost&&!a.open);         // new opponent: rebase, no adoption
     CHECK(a.observe(20,21,6,true,1,-1,430)==E::None);CHECK(a.observe(20,21,6,true,4,0,440)==E::Open&&a.serial==3);
     CHECK(a.observe(0,0,0,false,-1,-1,450)==E::Lost&&!a.open);
     dc::publish(a);CHECK(dc::openState.load()==0);
     CHECK(a.observe(20,21,6,true,2,-1,460)==E::None);CHECK(a.observe(20,21,6,true,4,5,470)==E::Open);
     dc::publish(a);CHECK(dc::openState.load()==21&&dc::openTick.load()==470&&dc::openOffered.load()==5&&dc::openSerial.load()==a.serial);}
    CHECK(!dc::staggerGuard.load()); // C40: window-assist is opt-in, not an automatic perfect-block default
    // C44 VrDirectStaggerRule: 1 (default) = no DIRECT strike out of Henry's hit reaction, 0 = allowed, 2 = allowed at half strength
    CHECK(dc::staggerRule44.load()==1);
    CHECK(dc::clampStaggerRule44(-3)==0&&dc::clampStaggerRule44(7)==2&&dc::clampStaggerRule44(2)==2);
    CHECK(!dc::staggerStrikeAllowed44(1)&&dc::staggerStrikeAllowed44(0)&&dc::staggerStrikeAllowed44(2)&&dc::staggerStrikeAllowed44(9)); // 9 clamps to 2
    CHECK(dc::staggerStrength44(2,true,1.f)==.5f&&dc::staggerStrength44(2,false,1.f)==1.f&&dc::staggerStrength44(0,true,1.f)==1.f&&dc::staggerStrength44(1,true,.8f)==.8f);
    std::printf(fails?"direct_counter39: %d FAILED\n":"direct_counter39: OK\n",fails);return fails?1:0;
}
