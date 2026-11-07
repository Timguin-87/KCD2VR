#include "../src/vrik/arm_constraints.h"
#include "../src/vrik/physical_weapon.h"
#include "../src/vrik/fist_motion.h"
#include "../src/vrik/upper_body.h"
#include "../src/vrik/finger_surface.h"
#include <cstdio>
#include <limits>
using namespace kcd2vr::physical;
namespace pc=kcd2vr::pose_core;
static int checks=0,failures=0;
static void check(bool good,const char* text){++checks;failures+=!good;std::printf("%s %s\n",good?"PASS":"FAIL",text);}
static auto clear=[](const Profile&,Pose,Pose,bool){return 1.0;};
static Input input(){Input i{};i.allowed=true;i.epoch=i.bodyEpoch=1;i.tick=1008;i.timeNs=1000000000;i.sample=1;
 i.tracked[0]=i.tracked[1]=true;i.hands[0].p={.2,.3,1.3};i.hands[1].p={.2,.15,1.3};i.shoulders[0]={.2,0,1.3};i.shoulders[1]={-.2,0,1.3};i.reach[0]=i.reach[1]=1.2;i.contact=i.attacks=i.bowActions=false;i.autoSupport=true;return i;}
static std::array<Profile,2> sword(){std::array<Profile,2> p{};auto&a=p[0];a.key=a.generation=1;a.entity=42;a.side=0;a.classId=4;a.primary=a.geometry=a.support=a.twoHanded=true;a.tip={0,.9,0};a.axis={0,1,0};a.extent=1;a.weight=1.9;a.grip[1].p={0,-.15,0};p[1].side=1;return p;}
static void advance(Input&i,int ms=11){++i.sample;i.tick+=ms;i.timeNs+=std::uint64_t(ms)*1000000;}
static int freezes(bool highResolution){auto i=input();auto p=sword();i.autoSupport=false;State s{};int frozen=0;Quat prior{};
 for(int k=0;k<240;++k){i.sample=k+1;i.tick=1008+(std::uint64_t(k*1000.0/120)/16)*16;i.timeNs=highResolution?1000000000+std::uint64_t(k*1e9/120):0;i.hands[0].q=pc::from_rotation_vector({0,0,k*.003});auto o=step(s,i,p,clear);if(k&&pc::angle(prior,o.weapons[0].q)<1e-7)++frozen;prior=o.weapons[0].q;}return frozen;}
int main(){
 const int coarse=freezes(false),fine=freezes(true);std::printf("clock coarse_freezes=%d high_resolution_freezes=%d /239\n",coarse,fine);
 check(coarse>100&&fine==0,"producer nanoseconds remove equal-millisecond rotation holds without invented dt");
 {auto i=input();auto p=sword();State s{};step(s,i,p,clear);advance(i);i.hands[0].p.x+=.02;auto first=step(s,i,p,clear);const auto old=s.weapon[0].weapon;int queries=0;i.timeNs+=5000000;i.contact=true;
 auto repeat=step(s,i,p,[&](const Profile&,Pose,Pose,bool){++queries;return 1.0;});check(pc::length(repeat.weapons[0].p-old.p)<1e-12&&queries==0&&repeat.commandCount==0,"duplicate consumers do not integrate or emit queries/commands");}
 {auto i=input();auto p=sword();State s{};step(s,i,p,clear);i.hands[1].p.x+=.075;Output o{};for(int k=0;k<60;++k){advance(i);o=step(s,i,p,clear);}const double primary=pc::angle(o.hands[0].q,i.hands[0].q)*180/pc::armPi;
 std::printf("support primary_change=%.6f deg\n",primary);check(o.support[0]&&primary<=8.001,"support correction itself is bounded relative to primary controller");
 check(pc::angle(o.hands[1].q,compose(o.weapons[0],p[0].grip[1]).q)<1e-6,"support wrist remains exactly in its native handle frame");
 i.hands[1].q=pc::from_rotation_vector({0,pc::armPi,0});for(int k=0;k<30;++k){advance(i);o=step(s,i,p,clear);}check(!o.support[0]&&!o.owned[1],"sustained incompatible wrist rotation releases support instead of corkscrewing");
 i.hands[1].q={};for(int k=0;k<30;++k){advance(i);o=step(s,i,p,clear);}check(!o.support[0],"over-twist release cannot silently reacquire without leaving helper");
 i.hands[1].p.x+=.4;advance(i);step(s,i,p,clear);i.hands[1].p=point(s.weapon[0].weapon,p[0].grip[1].p);for(int k=0;k<12;++k){advance(i);o=step(s,i,p,clear);}check(o.support[0],"leaving and reapproaching the helper rearms proximity support");}
 {auto i=input();auto p=sword();i.autoSupport=false;i.haveWorld=true;i.contact=true;i.sweepGate=true;State s{};step(s,i,p,clear);advance(i);step(s,i,p,clear);int calls=0;advance(i);i.world.q=pc::from_rotation_vector({0,0,.1});step(s,i,p,[&](const Profile&,Pose,Pose,bool){++calls;return 1.0;});check(calls>0,"body rotation cannot be skipped by static-query reuse");}
 {auto i=input();auto p=sword();i.autoSupport=false;i.contact=true;State s{};step(s,i,p,clear);advance(i);i.hands[0].q=pc::from_rotation_vector({0,0,.5});auto o=step(s,i,p,[](const Profile&p,Pose a,Pose b,bool){Contact103 h{};h.fraction=.5;h.plane=true;h.feature=p.tip;h.point=point(blend(a,b,.5),p.tip);h.normal={1,0,0};return h;});
 check(o.blocked[0]&&o.impactSpeed[0]>.25&&pc::length(s.weapon[0].angularRecoilVelocity)>.01,"validated rotational tip contact creates bounded angular response");
 const auto impactTick=s.weapon[0].lastImpactTick;advance(i);o=step(s,i,p,[](const Profile&p,Pose a,Pose b,bool){Contact103 h{};h.fraction=0;h.plane=true;h.feature=p.tip;h.point=point(a,p.tip);h.normal={1,0,0};return h;});check(o.impactSpeed[0]==0&&s.weapon[0].lastImpactTick==impactTick,"stationary pressure does not manufacture repeated impact energy");}
 {auto i=input();auto p=sword();i.autoSupport=false;i.contact=true;State s{};step(s,i,p,clear);advance(i);i.hands[0].p.x+=.1;auto o=step(s,i,p,[](const Profile&,Pose,Pose,bool){Contact103 c{};c.known=false;c.fraction=0;c.plane=true;c.normal={1,0,0};return c;});check(o.blocked[0]&&o.impactSpeed[0]==0,"unknown query blocks safely without a fabricated normal or hit cue");}
 {auto i=input();std::array<Profile,2> p{};auto& b=p[1];b.key=b.generation=3;b.side=1;b.classId=9;b.primary=b.support=b.geometry=true;b.grip[0].p={0,-.15,0};b.axis={0,-1,0};b.tip={0,0,.6};i.hands[1].p={0,.4,1.3};i.hands[0].p={0,.25,1.3};i.bowActions=i.bowRetention=true;i.arrowFromShoulder=false;State s{};step(s,i,p,clear);
 advance(i);i.grip[0]=1;auto o=step(s,i,p,clear);check(o.bowHeld&&o.commandCount==1&&o.commands[0].kind==CommandKind::BowBegin,"direct bow nocking works without a shoulder-arrow Boolean");
 advance(i);i.hands[0].p.y-=.3;o=step(s,i,p,clear);advance(i);i.hands[0].p.x+=.31;o=step(s,i,p,clear);check(o.bowHeld,"brief modest off-axis motion retains draw");advance(i);i.hands[0].p.x-=.31;i.grip[0]=0;o=step(s,i,p,clear);check(o.commandCount==1&&o.commands[0].kind==CommandKind::BowFire,"deliberate tracked release dispatches exactly one bow fire request");advance(i);o=step(s,i,p,clear);check(o.commandCount==0,"released draw cannot fire twice");
 advance(i);i.hands[0].p={0,.25,1.3};i.grip[0]=1;step(s,i,p,clear);i.hands[0].p.y-=.3;i.hands[0].p.x+=.31;int cancels=0;for(int k=0;k<20;++k){advance(i);o=step(s,i,p,clear);for(unsigned j=0;j<o.commandCount;++j)cancels+=o.commands[j].kind==CommandKind::BowCancel;}check(cancels==1&&!o.bowHeld,"sustained invalid draw cancels once rather than silently keeping ownership");advance(i);i.grip[0]=0;o=step(s,i,p,clear);check(o.commandCount==0,"release after cancellation cannot shoot");}
 {const auto axes=pc::chest_axes103({1,0,0},{0,0,1});pc::ShoulderState103 a{},b{};bool mirror=true,bounded=true;for(int k=0;k<240;++k){const double t=double(k)/239;const pc::Vec3 r{.7-.9*t,.5*t,1.3+.6*std::sin(t*pc::armPi)};auto qr=pc::shoulder_delta103(a,*axes,0,{.2,0,1.3},r,.6,1,k+1,1008+k*8,1000000000ULL+k*8333333ULL);auto ql=pc::shoulder_delta103(b,*axes,1,{-.2,0,1.3},{-r.x,r.y,r.z},.6,1,k+1,1008+k*8,1000000000ULL+k*8333333ULL);mirror=mirror&&qr&&ql&&std::abs(a.angle.elevation-b.angle.elevation)<1e-12&&std::abs(a.angle.protraction-b.angle.protraction)<1e-12;bounded=bounded&&a.angle.elevation<=20*pc::armPi/180+1e-8&&a.angle.protraction<=16*pc::armPi/180+1e-8;}
 check(mirror,"left/right shoulder assistance uses symmetric chest axes, independent of solve order");check(bounded,"overhead and cross-body shoulder assistance remains within explicit limits");auto old=a;pc::shoulder_delta103(a,*axes,0,{.2,0,1.3},{0,1,2},.6,a.epoch,a.sample,a.tick+1,a.timeNs+1000000);check(a.angle.elevation==old.angle.elevation,"duplicate shoulder consumer cannot advance smoothing");
 const double d=pc::minimum_arm_reach103(.3,.27);const auto flex=std::acos(std::clamp((d*d-.3*.3-.27*.27)/(2*.3*.27),-1.0,1.0));check(std::abs(flex-155*pc::armPi/180)<1e-8,"close-reach radial limit corresponds to 155-degree flexion without changing bone lengths");}
 {pc::FingerCurve103 curve{};curve.count=4;curve.offset[0]={.04,0,0};for(int k=1;k<4;++k)curve.offset[k]={.04,0,0};for(int k=0;k<4;++k)curve.closed[k]=pc::from_rotation_vector({0,0,1.0});const auto original=curve;
 const pc::FingerSurface103 plane{{.16,0,0},{-1,0,0}};auto fit=pc::fit_finger_surface103(curve,0,{}, {.06,0,0},plane,.008);std::printf("surface fitted_curl=%.6f clearance=%.9f\n",fit.curl,fit.clearance);
 check(fit.valid&&fit.fits&&fit.curl>0,"open finger can curl at a wall without moving the wrist");check(pc::finger_clearance103(curve,fit.curl,{}, {.06,0,0},plane,.008).fits,"fitted phalange points remain outside verified plane");check(curve.closed[0].z==original.closed[0].z&&curve.open[0].w==original.open[0].w,"surface fitting cannot mutate stored relaxed/fist endpoints");auto impossible=pc::fit_finger_surface103(curve,0,{}, {.25,0,0},plane,.008);check(!impossible.fits,"impossible finger contact is not reported as a valid fit");auto invalid=pc::fit_finger_surface103(curve,0,{}, {},{{0,0,0},{0,0,0}},.008);check(!invalid.valid,"zero surface normal cannot create finger contact");}
 std::printf("merge103=%d/%d\n",checks-failures,checks);return failures?1:0;
}
