#include "../src/vrik/physical_weapon.h"
#include <cstdio>
using namespace kcd2vr::physical;
namespace m=kcd2vr::pose_core;
static unsigned checks=0,failures=0;
static void check(bool ok,const char* name){++checks;if(!ok)++failures;std::printf("%s %s\n",ok?"PASS":"FAIL",name);}
static auto clear=[](const Profile&,Pose,Pose,bool){return 1.0;};
static Input input(){Input i{};i.allowed=true;i.epoch=i.bodyEpoch=1;i.sample=1;i.tick=1000;i.timeNs=1000000000ull;
 i.tracked[0]=i.tracked[1]=true;i.hands[0].p={.2,.3,1.3};i.hands[1].p={.05,.3,1.3};i.shoulders[0]={.2,0,1.3};i.shoulders[1]={-.2,0,1.3};
 i.reach[0]=i.reach[1]=.75;i.dynamics=true;i.controllerAnchor119=true;i.constraintDrive129=true;i.grip[0]=1;return i;}
static std::array<Profile,2> sword(double weight){std::array<Profile,2> p{};auto& a=p[0];a.key=1;a.generation=1;a.side=0;a.classId=4;a.primary=a.support=a.geometry=a.twoHanded=true;a.weight=weight;a.extent=1;a.tip={0,.9,0};a.axis={0,1,0};a.grip[1].p={0,-.15,0};p[1].side=1;return p;}
static void adv(Input& i,unsigned ms=10){++i.sample;i.tick+=ms;i.timeNs+=std::uint64_t(ms)*1000000ull;}
static double abrupt(double weight,bool support){auto p=sword(weight);auto i=input();State s{};step(s,i,p,clear);
 if(support){i.hands[1].p=compose(s.weapon[0].weapon,p[0].grip[1]).p;i.grip[1]=1;for(int n=0;n<8;++n){adv(i);step(s,i,p,clear);}}
 double maxErr=0;for(int n=0;n<12;++n){adv(i);i.hands[0].p.z+=.025;if(support)i.hands[1].p.z+=.025;auto o=step(s,i,p,clear);maxErr=std::max(maxErr,m::length(o.hands[0].p-i.hands[0].p));}return maxErr;}
int main(){
 const double light=abrupt(1.0,false),heavy=abrupt(6.0,false),two=abrupt(6.0,true);
 std::printf("errors light/heavy/heavy2 = %.1f/%.1f/%.1f mm\n",light*1000,heavy*1000,two*1000);
 check(light<.056&&heavy<.056,"free-space hand separation stays inside 55 mm hard bound");
 check(heavy>light+.001,"heavier weapon yields more under abrupt acceleration");
 check(two+1e-4<heavy,"two-handed constraint has greater authority");
 {auto p=sword(3);auto i=input();State s{};auto o=step(s,i,p,clear);for(int n=0;n<120;++n){adv(i);i.hands[0].p.z+=.0005;o=step(s,i,p,clear);}check(m::length(o.hands[0].p-i.hands[0].p)<.008,"slow deliberate motion remains nearly one-to-one");}
 {auto p=sword(3);auto i=input();State s{};step(s,i,p,clear);double worst=0;for(int n=0;n<30;++n){adv(i);i.hands[0].q=m::from_rotation_vector({0,0,.09*n});auto o=step(s,i,p,clear);worst=std::max(worst,m::angle(o.weapons[0].q,compose(i.hands[0],inverse(p[0].grip[0])).q));}check(worst<=5.01*3.141592653589793/180.0,"abrupt wrist rotation stays within five-degree free-space bound");}
 std::printf("%u checks, %u failures\n",checks,failures);return failures?1:0;
}
