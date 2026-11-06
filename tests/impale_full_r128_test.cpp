// R128: impale hold (the body stays on the blade against gravity) + full depth + plate rule (pure logic).
// g++ -std=c++20 -I src tests/impale_full_r128_test.cpp
#include "../src/vrik/gore32.h"
#include "../src/subsystems/gore/impale_policy.h"
#include <cmath>
#include <cstdio>
#include <string_view>
using namespace kcd2vr::gore32;
namespace i40=kcd2vr::impale40;
static int checks=0,fails=0;
static void ck(bool c,const char* n){++checks;if(!c){++fails;std::printf("FAIL %s\n",n);}}
static bool near(double a,double b,double e=1e-9){return std::fabs(a-b)<=e;}
int main(){
    // ---- full depth: grip->tip minus the 8 cm guard margin, at most 1 m; plate / OFF / unknown blade keep the pick depth
    ck(near(fullDepthR128(.12,.95,false,true),.87),"sword .95 m: to the guard (.87)");
    ck(near(fullDepthR128(.12,.95,true,true),.12),"plate keeps the shallow pick");
    ck(near(fullDepthR128(.12,.95,false,false),.12),"option OFF keeps the shallow pick");
    ck(near(fullDepthR128(.12,2.0,false,true),1.0),"long polearm capped at 1 m");
    ck(near(fullDepthR128(.10,.25,false,true),.17),"dagger .25 m -> .17");
    ck(near(fullDepthR128(.30,.30,false,true),.30),"never shallower than the pick");
    ck(near(fullDepthR128(.12,.05,false,true),.12)&&near(fullDepthR128(.12,std::nan(""),false,true),.12),"blade unknown/too short: pick depth");
    ck(near(fullDepthR128(std::nan(""),.95,true,true),.15),"invalid pick = .15");
    {Hang h;h.start(1,.87);ck(near(h.depth,.87)&&near(h.initialDepth,.87),"hang keeps a full depth (was clamped to .60)");
     h.start(1,5);ck(near(h.depth,kMaxHangDepthR128),"hang depth capped 1.2 m");}
    // ---- plate rule: the game's per-zone stab defence of THIS hit >= VrImpaleHardArmour
    {i40::Verdict v{};v.serial=7;v.target=45105;v.armour=i40::ArmourStatus::Read;v.stab[0]=10;v.stab[1]=20;v.stab[2]=72;v.stab[3]=30;
     auto p=i40::plateR128(v,7,45105,8,60);ck(p.known&&p.plate&&p.zone==i40::Zone::Body&&near(p.stab,72),"body material 8, stab 72 >= 60: plate");
     p=i40::plateR128(v,7,45105,30,60);ck(p.known&&!p.plate&&p.zone==i40::Zone::Legs,"legs 30: not plate");
     p=i40::plateR128(v,7,45105,3,60);ck(p.known&&!p.plate&&p.zone==i40::Zone::Head,"head 10: not plate");
     p=i40::plateR128(v,7,45105,-1,60);ck(p.plate&&p.zone==i40::Zone::Unknown,"unknown zone reads the body");
     p=i40::plateR128(v,8,45105,8,60);ck(!p.known&&!p.plate,"verdict of another hit: unknown, not plate");
     p=i40::plateR128(v,7,1,8,60);ck(!p.known&&!p.plate,"other target: not plate");
     v.armour=i40::ArmourStatus::PinMismatch;p=i40::plateR128(v,7,45105,8,60);ck(!p.known&&!p.plate,"armour not read: not plate");
     v.armour=i40::ArmourStatus::Read;v.stab[2]=std::nanf("");p=i40::plateR128(v,7,45105,8,60);ck(!p.known&&!p.plate,"bad value: not plate");}
    // ---- the hold against gravity (Miki 6 Nov: "the npc slided down from the blade with gravity")
    // blade held still, pointing 25 deg down; the wound gets the servo velocity (80 % follow) plus a constant pull down
    // of 5 frames of gravity per frame (the rest of the ragdoll hanging on the driven part).
    {const double a=25*3.14159265358979/180;const V3 axis{0,std::cos(a),-std::sin(a)};
     Hang h;h.start(1000,.80);HangIn in{};in.now=1000;in.bladeAxis=axis;in.bladePoint={0,1,1.2};in.wound=in.bladePoint;
     HangOut o{};double worst=0;bool held=true;
     for(int i=0;i<450;++i){in.now+=11;o=h.step(in);if(o.release!=Release::None){held=false;break;}
      const double dt=.011;in.wound=in.wound+o.velocity*(.8*dt)+V3{0,0,-9.81*dt*dt*5};
      worst=std::max(worst,len(in.wound-in.bladePoint));}
     ck(held&&h.active,"5 s on a still, down-tilted blade: still on it (no PULL_OUT from gravity)");
     std::printf("gravity hold worst=%.3f m\n",worst);ck(worst<.05,"the wound stays within 5 cm of the hold point");
     ck(h.depth>.79,"gravity slide does not eat the depth");}
    // body that already slid 20 cm toward the tip is pulled back onto the blade (along the axis)
    {Hang h;h.start(1000,.50);HangIn in{};in.now=1000;in.bladeAxis={0,1,0};in.bladePoint={0,1,1};in.wound={0,1.2,1};
     HangOut o=h.step(in);ck(o.drive&&o.velocity.y<-1.5,"axial gap is corrected toward the guard");
     for(int i=0;i<100;++i){in.now+=11;o=h.step(in);in.wound=in.wound+o.velocity*.011;}
     ck(h.active&&len(in.wound-in.bladePoint)<.01,"back on the hold point");}
    // full-depth draw: the stuck point moved 70 cm back (to the guard), the body slides onto the blade, no STUCK release
    {Hang h;h.start(1000,.87);HangIn in{};in.now=1000;in.bladeAxis={0,1,0};in.bladePoint={0,1,1};in.wound={0,1.7,1};
     HangOut o{};unsigned n=0;for(;n<200&&h.active;++n){in.now+=11;o=h.step(in);in.wound=in.wound+o.velocity*(.7*.011);if(len(in.wound-in.bladePoint)<.01)break;}
     ck(h.active&&len(in.wound-in.bladePoint)<.01&&n<100,"drawn onto the blade to the guard in < 1.1 s");}
    {Hang h;h.start(1000,.87);HangIn in{};in.now=1000;in.bladeAxis={0,1,0};in.bladePoint={0,1,1};in.wound={0,1.7,1};double over=0;
     for(int n=0;n<300&&h.active;++n){in.now+=11;auto o=h.step(in);in.wound=in.wound+o.velocity*(.7*.011);if(in.wound.y<1)over=std::max(over,1-in.wound.y);}
     ck(h.active&&over<.03&&std::fabs(in.wound.y-1)<.005,"no overshoot past the guard point (anti-windup), settles on it");}
    // ---- releases: pull out along the axis (player's blade travel), swing hard sideways, time limit
    {Hang h;h.start(1000,.30);HangIn in{};in.now=1000;in.bladeAxis={0,1,0};in.bladePoint={0,1,1};in.wound=in.bladePoint;(void)h.step(in);
     HangOut o{};unsigned n=0;while(h.active&&n<400){in.now+=11;in.bladePoint.y-=.011;o=h.step(in);in.wound=in.wound+o.velocity*.011;++n;}
     ck(o.release==Release::PullOut&&n<40,"pulling the blade back 30 cm along it slides it out");}
    {Hang h;h.start(1000,.30);HangIn in{};in.now=1000;in.bladeAxis={0,1,0};in.bladePoint={0,1,1};in.wound=in.bladePoint;(void)h.step(in);
     HangOut o{};for(int i=0;i<20&&h.active;++i){in.now+=11;in.bladePoint.x+=5*.011;o=h.step(in);}
     ck(o.release==Release::SwingOff&&gush(o.release)&&o.releaseVelocity.x>3,"hard sideways swing (5 m/s) throws him off");}
    {Hang h;h.start(1000,.30);HangIn in{};in.now=1000;in.bladeAxis={0,1,0};in.bladePoint={0,1,1};in.wound=in.bladePoint;(void)h.step(in);
     in.now+=11;in.bladePoint.x+=6*.011;HangOut o=h.step(in);in.now+=11;in.bladePoint.x+=0;o=h.step(in);
     ck(h.active&&o.release==Release::None,"one 6 m/s tracking spike is not a swing");}
    {Hang h;h.start(1000,.30);HangIn in{};in.now=1000;in.bladeAxis={0,1,0};in.bladePoint={0,1,1};in.wound=in.bladePoint;HangOut o{};
     for(int i=0;i<200;++i){in.now+=11;in.bladePoint.x+=2*.011;o=h.step(in);in.wound=in.wound+o.velocity*.011;}
     ck(h.active&&o.release==Release::None,"carrying him sideways at 2 m/s keeps him on");}
    {Hang h;h.start(1000,.30);HangIn in{};in.now=1000;in.bladeAxis={0,1,0};in.bladePoint={0,1,1};in.wound=in.bladePoint;(void)h.step(in);
     in.now=1000+30001;HangOut o=h.step(in);ck(o.release==Release::Timeout,"30 s time limit");
     h.start(40000,.30);ck(h.active,"a new impale starts again (no latch)");}
    ck(std::string_view(name(Release::SwingOff))=="SWING_OFF","release name");
    std::printf("impale_full_r128: %d/%d\n",checks-fails,checks);return fails?1:0;
}
