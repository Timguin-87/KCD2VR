// R133 free targeting mode / FREE PLUS hold: host test of the pure policy (no game, no headset).
// g++ -std=c++20 -I src tests/free_targeting_plus_r133_test.cpp -o ft133 && ./ft133
#include "subsystems/experimental/free_targeting_d42.h"
#include <cstdio>
#include <cmath>
namespace r=kcd2vr::free_targeting_r133;namespace ft=kcd2vr::free_targeting_d42;
static unsigned checks=0,failures=0;
static void ck(bool x,const char* what){++checks;if(!x){++failures;std::printf("FAIL %s\n",what);}}
static bool near(double a,double b,double e=1e-6){return std::fabs(a-b)<=e;}
static const double D=3.141592653589793/180;
int main(){
    // INI: new key wins, old bool read as before, default 1
    ck(r::resolveIni(2,true,false)==2&&r::resolveIni(0,true,true)==0,"new VrFreeTargetingModeR133 wins over the old bool");
    ck(r::resolveIni(-1,true,true)==1,"old INI VrFreeTargeting=true -> 1 FREE");
    ck(r::resolveIni(-1,true,false)==0,"old INI VrFreeTargeting=false -> 0 NATIVE");
    ck(r::resolveIni(-1,false,true)==r::kDefaultMode&&r::kDefaultMode==1,"no key -> default 1 (FREE PLUS not headset proven)");
    ck(r::resolveIni(7,true,false)==0,"garbage new value falls back to the old bool");
    ck(r::cycle(0)==1&&r::cycle(1)==2&&r::cycle(2)==0&&r::cycle(9)==0,"Experiments switch cycles 0 -> 1 -> 2 -> 0");
    // mode owner + old bool API
    ft::setMode(0);ck(!ft::enabled&&ft::mode()==0,"mode 0 = free targeting off (camera lease + veto off)");
    auto g=ft::generation.load();ft::setMode(2);ck(ft::enabled&&ft::mode()==2&&ft::generation.load()==g+1,"mode 2 enables the D42 owners once");
    g=ft::generation.load();ft::setMode(1);ck(ft::enabled&&ft::generation.load()==g,"1 <-> 2 keeps the D42 veto generation (no re-arm needed)");
    ft::setMode(2);ft::set(true);ck(ft::mode()==2,"old set(true) keeps a chosen FREE PLUS");
    ft::set(false);ck(ft::mode()==0&&!ft::enabled,"old set(false) = NATIVE");
    ft::set(true);ck(ft::mode()==1&&ft::enabled,"old set(true) from NATIVE = FREE");
    // cone
    {const auto a=r::cone(0),b=r::cone(1),c=r::cone(2);
     ck(near(a.lockAngle,1.5708f)&&near(a.unlockAngle,2.4f)&&near(a.lockArea,.6f)&&!a.hyst,"mode 0 = the exact R52/R57 lease values");
     ck(b.lockArea>a.lockArea&&b.hyst&&c.lockArea>b.lockArea&&c.unlockHyst>b.unlockHyst,"FREE and FREE PLUS widen the selection area step by step");
     ck(c.unlockAngle>=c.lockAngle,"FREE PLUS unlock angle never below lock angle (no lock/unlock flicker)");
     ck(c.unlockAngle<a.unlockAngle,"FREE PLUS loses a target behind you (100 deg) instead of 137 deg");}
    // hold: the game turns Henry 1 deg per frame toward the target while engaged
    {r::Hold h{};r::Input in{};in.modePlus=true;in.context=true;in.engaged=true;in.haveRoot=true;
     double cam=.3,root=.25;std::uint64_t t=1000;
     for(int i=0;i<=60;++i){in.cameraYaw=cam;in.rootYaw=root;in.now=t;auto s=r::step(h,in);if(i)ck(s==r::Step::Absorb,"engaged frame absorbs");cam+=D;root+=D;t+=11;}
     ck(near(h.view,-60*D,1e-9)&&near(h.body,-60*D,1e-9),"60 deg of game turn absorbed into view and body offsets");
     ck(near(r::wrap(cam-D+h.view),.3,1e-9),"world view yaw unchanged while the game turned");
     ck(near(h.absorbedDeg,60,1e-6)&&near(h.maxOffsetDeg,60,1e-6),"receipt counts absorbed degrees");
     // the player's own turn passes
     in.userTurn=true;in.cameraYaw=cam+10*D;in.rootYaw=root+10*D;ck(r::step(h,in)==r::Step::Pass&&near(h.view,-60*D,1e-9),"stick/snap turn passes, view turns with it");
     in.userTurn=false;cam+=10*D;root+=10*D;
     // teleport-sized jump passes
     in.cameraYaw=cam+40*D;in.rootYaw=root+40*D;ck(r::step(h,in)==r::Step::Pass&&near(h.view,-60*D,1e-9),"> 20 deg in one frame is never absorbed");
     cam+=40*D;root+=40*D;
     // menu/clinch in the fight: kept, not absorbed
     in.context=false;in.cameraYaw=cam+5*D;ck(r::step(h,in)==r::Step::Pass&&near(h.view,-60*D,1e-9),"no absorb outside plain play, offset kept");
     cam+=5*D;in.context=true;
     // fight ends: bleed. the game answers our rx: N += -rx * 2 deg/frame
     in.engaged=false;int bleeds=0;r::Step s=r::Step::Pass;
     for(int i=0;i<600;++i){in.cameraYaw=cam;in.rootYaw=cam-.05;in.now=(t+=11);s=r::step(h,in);if(s!=r::Step::Bleed)break;++bleeds;
        const float rx=r::bleedRx(h.view,h.bleedSign);ck(i||rx>0,"negative view offset -> stick right first (turn right)");cam+=-rx*2*D;}
     ck(bleeds>5&&h.view==0&&h.body==0&&s==r::Step::Pass,"bleed turns Henry back under the view, offset runs out with no snap");
     ck(h.clears==0,"converging bleed needs no clear");}
    // bleed with the wrong sign on this machine: flips once and still converges
    {r::Hold h{};r::Input in{};in.modePlus=true;in.context=true;in.engaged=true;in.haveRoot=true;double cam=0;std::uint64_t t=5;
     for(int i=0;i<=30;++i){in.cameraYaw=cam;in.rootYaw=cam;in.now=t+=11;r::step(h,in);cam+=D;}
     in.engaged=false;r::Step s=r::Step::Pass;int n=0;bool flipped=false;
     for(;n<900;++n){in.cameraYaw=cam;in.rootYaw=cam;in.now=t+=11;s=r::step(h,in);flipped=flipped||h.flipped;if(s!=r::Step::Bleed)break;cam+=r::bleedRx(h.view,h.bleedSign)*2*D;}
     ck(flipped&&s==r::Step::Pass&&h.view==0,"inverted stick answer is detected once and the bleed still finishes");}
    // the game ignores our stick: one clear after 4 s, not a latch
    {r::Hold h{};r::Input in{};in.modePlus=true;in.context=true;in.engaged=true;in.haveRoot=true;double cam=0;std::uint64_t t=5;
     for(int i=0;i<=30;++i){in.cameraYaw=cam;in.rootYaw=cam;in.now=t+=11;r::step(h,in);cam+=D;}
     in.engaged=false;r::Step s=r::Step::Bleed;
     while(s==r::Step::Bleed){in.cameraYaw=cam;in.now=t+=11;s=r::step(h,in);}
     ck(s==r::Step::Cleared&&h.view==0&&h.clears==1,"no answer -> one clear after 4 s");
     in.engaged=true;in.cameraYaw=cam;r::step(h,in);in.cameraYaw=cam+D;ck(r::step(h,in)==r::Step::Absorb,"next fight holds again (no latch)");}
    // out of plain play long after the fight: cleared once
    {r::Hold h{};r::Input in{};in.modePlus=true;in.context=true;in.engaged=true;double cam=0;std::uint64_t t=5;
     for(int i=0;i<=10;++i){in.cameraYaw=cam;in.now=t+=11;r::step(h,in);cam+=D;}
     in.engaged=false;in.context=false;in.now=t+=1000;ck(r::step(h,in)==r::Step::Pass&&h.view!=0,"short dialogue/menu right after the fight keeps it");
     in.now=t+=5000;ck(r::step(h,in)==r::Step::Cleared&&h.view==0,"long non-play after the fight clears it");}
    // mode change / recenter reset
    {r::Hold h{};r::Input in{};in.modePlus=true;in.context=true;in.engaged=true;in.cameraYaw=0;r::step(h,in);in.cameraYaw=D;r::step(h,in);
     in.reset=true;ck(r::step(h,in)==r::Step::Cleared&&h.view==0,"recenter/load clears the offset");
     in.reset=false;in.modePlus=false;in.cameraYaw=2*D;ck(r::step(h,in)==r::Step::Off,"modes 0/1 never hold");}
    // stick: world direction = view direction
    {bool ok=true;for(double N=-3;N<3;N+=.7)for(double phi=-3;phi<3;phi+=.9){float x=.3f,y=.8f;r::rotateStick(x,y,phi);
        const double wx=std::cos(N)*x-std::sin(N)*y,wy=std::sin(N)*x+std::cos(N)*y;
        const double vx=std::cos(N+phi)*.3-std::sin(N+phi)*.8,vy=std::sin(N+phi)*.3+std::cos(N+phi)*.8;
        if(std::fabs(x)<1&&std::fabs(y)<1)ok=ok&&near(wx,vx,1e-5)&&near(wy,vy,1e-5);}
     ck(ok,"turned stick moves Henry where the player looks, for any game yaw/offset");
     float x=0,y=1;r::rotateStick(x,y,90*D);ck(near(x,-1,1e-6)&&near(y,0,1e-6),"view 90 deg left of the game camera: stick forward -> game left");
     x=0,y=1;r::rotateStick(x,y,.5*D);ck(x==0&&y==1,"under 1 deg the stick is untouched");}
    // body frame
    {const double a=.4,phi=-.7;float q[4]{0,0,float(std::sin(a/2)),float(std::cos(a/2))};r::yawFrame(q,phi);
     ck(near(2*std::atan2(q[2],q[3]),a+phi,1e-6)&&near(q[0],0,1e-7)&&near(q[1],0,1e-7),"body frame yaw = calibrated yaw + offset");
     float i[4]{0,0,.3f,.95393920f};r::yawFrame(i,0);ck(i[2]==.3f,"offset 0 leaves the calibrated frame bit-exact");}
    ck(std::fabs(r::bleedRx(1,-1))<=.55f&&std::fabs(r::bleedRx(.05,-1))>=.18f&&r::bleedRx(.01,-1)==0,"bleed stick bounded .18-.55, nothing under 1 deg");
    r::opponentMs=1000;ck(r::engaged(2400)&&!r::engaged(2600),"engaged = opponent seen in the last 1.5 s");
    r::userTurnMs=1000;ck(r::userTurning(1200)&&!r::userTurning(1300),"user turn window 250 ms");
    std::printf("R133 free targeting mode / FREE PLUS hold: %u checks, %u failures; game/headset NOT_RUN\n",checks,failures);
    return failures?1:0;
}
