#include "../src/vrik/physical_weapon.h"
#include <cstdio>
using namespace kcd2vr::physical; namespace m=kcd2vr::pose_core;
static unsigned checks=0,failures=0; static void check(bool b,const char*n){++checks;if(!b)++failures;std::printf("%s %s\n",b?"PASS":"FAIL",n);} static auto clear=[](const Profile&,Pose,Pose,bool){return 1.0;};
static Input input(){Input i{};i.allowed=true;i.epoch=i.bodyEpoch=1;i.sample=1;i.tick=1000;i.timeNs=1000000000ull;i.tracked[0]=i.tracked[1]=true;i.grip[0]=i.grip[1]=1;i.hands[0].p={.2,.25,1.3};i.shoulders[0]={.2,0,1.3};i.shoulders[1]={-.2,0,1.3};i.reach[0]=i.reach[1]=.8;i.dynamics=true;i.controllerAnchor119=true;i.constraintDrive129=true;i.supportConstraint130=true;i.supportMaxAngle=55*3.14159265358979323846/180.;return i;}
static std::array<Profile,2> sword(){std::array<Profile,2> p{};auto&a=p[0];a.key=1;a.generation=1;a.side=0;a.classId=4;a.primary=a.support=a.geometry=a.twoHanded=true;a.weight=2;a.extent=1;a.tip={0,.9,0};a.grip[1].p={0,-.25,0};p[1].side=1;return p;}
static void adv(Input&i,unsigned ms=10){++i.sample;i.tick+=ms;i.timeNs+=std::uint64_t(ms)*1000000ull;}
int main(){for(int cls:{4,7,10}){auto p=sword();p[0].classId=cls;
 if(cls==7){p[0].weight=9.6;p[0].extent=2;p[0].tip={0,1.8,0};p[0].grip[1].p={0,.35,0};}
 if(cls==10){p[0].weight=3;p[0].extent=.8;p[0].tip={0,.7,0};p[0].grip[1].p={0,.30,0};}
 auto i=input();State s{};auto o=step(s,i,p,clear);i.hands[1].p=compose(o.weapons[0],p[0].grip[1]).p;for(int n=0;n<8;++n){adv(i);o=step(s,i,p,clear);}check(o.support[0]&&o.owned[1],"explicit secondary Grip latches one rigid weapon");
 const auto primary=i.hands[0].p; i.hands[1].p=primary+m::rotate(m::from_rotation_vector({0,0,.55}),p[0].grip[1].p); double before=m::length(compose(o.weapons[0],p[0].grip[1]).p-i.hands[1].p);for(int n=0;n<12;++n){adv(i);o=step(s,i,p,clear);}double after=m::length(compose(o.weapons[0],p[0].grip[1]).p-i.hands[1].p);double primaryErr=m::length(o.hands[0].p-i.hands[0].p);double correction=m::angle(o.weapons[0].q,compose(i.hands[0],inverse(p[0].grip[0])).q);
 std::printf("support error %.1f -> %.1f mm; primary %.1f mm; correction %.1f deg\n",before*1000,after*1000,primaryErr*1000,correction*57.2958);
 check(after<before*.55,"secondary position materially steers the shared weapon");check(correction>8.0/57.2958&&correction<56.0/57.2958,"secondary authority can exceed retired eight-degree nudge but remains bounded");check(primaryErr<.056,"shared constraint keeps primary inside physical compliance bound");
 i.hands[1].p=compose(o.weapons[0],p[0].grip[1]).p+Vec3{0,0,.205};adv(i);o=step(s,i,p,clear);check(s.weapon[0].support,"latched socket survives a 20.5 cm residual while the bounded constraint catches up");
 adv(i);i.grip[1]=0;o=step(s,i,p,clear);check(!o.support[0]&&!o.owned[1]&&o.owned[0],"secondary release leaves primary ownership intact");}std::printf("%u checks, %u failures\n",checks,failures);return failures?1:0;}
