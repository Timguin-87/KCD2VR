// J27 station torso placement + smith tool contact rules (src/vrik/station_ws_j27.h).
#include "vrik/station_ws_j27.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>
namespace ws=kcd2vr::station_ws_j27;
static unsigned checks=0;
static void check(bool ok,const char* what){++checks;if(!ok){std::fprintf(stderr,"FAIL %u %s\n",checks,what);std::exit(1);}}
static bool near(double a,double b,double e=1e-6){return std::fabs(a-b)<=e;}
static ws::TorsoIn base(){
    ws::TorsoIn in{};in.fx=0;in.fy=1;in.spine=.62;in.restHeadFwd=.02;in.restChestFwd=.03;in.chestFraction=.55;in.eyeFwd=.09;in.clear=.12;
    return in;
}
int main(){
    // upright, no motion: base clearance .02+.09-.03 = .08 < .12 -> hips go back so the chest is 12 cm behind the eye
    {auto in=base();const auto o=ws::torso(in);
     check(o.valid,"valid");check(near(o.chestClear,.12,1e-9),"clearance reached");
     check(o.py<0&&near(o.px,0),"pelvis moved back along forward");check(near(o.backShift,.04/.45,1e-9),"back shift = need/(1-t)");
     check(o.eyeAheadOfPelvis>.11,"eye ahead of pelvis");}
    // clearance already fine: no shift
    {auto in=base();in.clear=.05;const auto o=ws::torso(in);check(near(o.py,0)&&near(o.backShift,0),"no shift when clear");}
    // R38: a real 30 cm lean includes its geometric head drop; hips stay behind.
    {auto in=base();in.clear=0;in.follow=true;in.hy=.30;in.drop=in.spine-std::sqrt(in.spine*in.spine-.30*.30);const auto o=ws::torso(in);
     check(near(o.py,0)&&near(o.px,0),"lean inside radius keeps hips");check(o.eyeAheadOfPelvis>.40,"head over the table");}
    // R38: an upright 70 cm step -> hips follow after the 10 cm upright allowance
    {auto in=base();in.clear=0;in.follow=true;in.hy=.70;const auto o=ws::torso(in);check(near(o.py,.60,1e-9),"upright step follows after 10 cm");}
    // R38: upright sideways 25 cm -> 15 cm hip follow
    {auto in=base();in.clear=0;in.follow=true;in.hx=-.25;const auto o=ws::torso(in);
     check(near(o.px,-.15,1e-9)&&near(o.py,0,1e-9),"side follow past radius (right = forward x up)");}
    // FREE never follows XY
    {auto in=base();in.clear=0;in.follow=false;in.hy=.70;in.hx=.3;const auto o=ws::torso(in);check(near(o.px,0)&&near(o.py,0),"free mode hips fixed");}
    // drop explained by the lean is not crouch; extra drop is
    {auto in=base();in.clear=0;in.hy=.30;const double lean=.62-std::sqrt(.62*.62-.09);
     in.drop=lean*.5;check(near(ws::torso(in).pz,0),"lean drop not crouch");
     in.drop=lean+.10;check(near(ws::torso(in).pz,-.10,1e-9),"rest of the drop is crouch");}
    // straight squat still crouches 1:1
    {auto in=base();in.clear=0;in.drop=.2;check(near(ws::torso(in).pz,-.2,1e-9),"squat crouch");}
    // yaw follow: body forward +x, lean along +x stays inside radius, step past moves along +x
    {auto in=base();in.clear=0;in.follow=true;in.fx=1;in.fy=0;in.hx=.6;const auto o=ws::torso(in);
     check(near(o.px,.50,1e-9)&&near(o.py,0,1e-9),"yaw follow uses body forward");}
    // Reported failure: upright 40 cm walking must move the hips, while a
    // bent-over 40 cm reach can be performed without dragging them over the table.
    {auto in=base();in.clear=0;in.follow=true;in.hy=.40;
     check(near(ws::torso(in).py,.30),"40 cm upright step moves pelvis 30 cm");
     in.drop=in.spine-std::sqrt(in.spine*in.spine-.40*.40);
     check(near(ws::torso(in).py,0)&&near(ws::torso(in).pz,0),"same reach with measured lean keeps pelvis behind");
     in.hy=.70;in.drop=in.spine-std::sqrt(in.spine*in.spine-.45*.45);
     check(near(ws::torso(in).py,.25),"deep bend preserves 45 cm maximum table lean");}
    // No threshold jump when a slow step crosses 10 cm or a slow bend earns
    // more forward allowance. These are physical changes, not pose smoothing.
    {auto in=base();in.clear=0;in.follow=true;bool continuous=true;double previous=0;
     for(int k=0;k<=700;++k){in.hy=k*.001;const auto o=ws::torso(in);
       continuous=continuous&&o.valid&&std::abs(o.py-previous)<=.001000001;previous=o.py;}
     check(continuous,"upright hip translation is continuous at the lean boundary");
     in.hy=.40;previous=ws::torso(in).py;
     for(int k=0;k<=2000;++k){in.drop=k*.0001;const auto o=ws::torso(in);
       continuous=continuous&&o.valid&&std::abs(o.py-previous)<.001;previous=o.py;}
     check(continuous,"bending changes the lean allowance continuously");}
    // back shift capped
    {auto in=base();in.clear=.60;const auto o=ws::torso(in);check(near(o.backShift,.25,1e-9),"back shift cap");}
    // invalid input
    {auto in=base();in.fx=in.fy=0;check(!ws::torso(in).valid,"no forward");in=base();in.spine=0;check(!ws::torso(in).valid,"no spine");}
    // probes: hammer-like bounds, long axis z
    {const float b[6]{-.03f,-.06f,-.01f,.03f,.06f,.40f};const auto p=ws::probes(b);
     check(p.valid,"probes valid");check(near(p.p[0][2],-.01,1e-6)&&near(p.p[2][2],.40,1e-6)&&near(p.p[1][2],.195,1e-6),"probe ends on long axis");
     check(near(p.p[1][0],0,1e-6)&&near(p.radius,.03,1e-6),"probe centre + radius");}
    {const float tiny[6]{0,0,0,.01f,.01f,.01f};check(!ws::probes(tiny).valid,"tiny bounds refused");
     const float bad[6]{0,0,0,-1,1,1};check(!ws::probes(bad).valid,"inverted bounds refused");check(!ws::probes(nullptr).valid,"null bounds");}
    // hit fraction
    check(near(ws::hitFraction(.5,1.0,.1),.4),"fraction minus margin");check(near(ws::hitFraction(.05,1.0,.1),0),"inside margin = 0");
    check(near(ws::hitFraction(.5,0,.1),1),"no motion = clear");
    // admission
    check(ws::admitTool(true,true,true,true,true),"admit");check(!ws::admitTool(false,true,true,true,true),"toggle off = R26");
    check(!ws::admitTool(true,false,true,true,true),"smith gate closed");check(!ws::admitTool(true,true,false,true,true),"no socket");
    check(!ws::admitTool(true,true,true,false,true),"other tool");check(!ws::admitTool(true,true,true,true,false),"bad wrist");
    std::printf("PASS %u station_ws_j27 checks\n",checks);return 0;
}
