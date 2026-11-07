#include "subsystems/roomscale/tracked_step_d76.h"
#include <cstdio>
#include <initializer_list>
#include <limits>
using namespace kcd2vr::roomscale_d76;
static int checks=0,failures=0;
#define CHECK(x) do{++checks;if(!(x)){++failures;std::printf("FAIL line %d: %s\n",__LINE__,#x);}}while(0)
bool near(double a,double b){return std::abs(a-b)<1e-9;}
TrackedStepSample sample(){TrackedStepSample i{};i.actorLifetime=3;i.tracking=4;i.origin=5;i.sample=1;i.tick=i.now=100;i.y=i.standingY=1.7;i.allowed=i.live=true;return i;}
void advance(TrackedStepSample& i,unsigned ms=11){++i.sample;i.tick+=ms;i.now=i.tick;}
int main(){
 for(unsigned dt:{5u,11u,22u,50u}){
  TrackedStepState s{};auto i=sample();CHECK(!observe(s,i).submit);
  double x=0,z=0;
  for(int n=0;n<60;++n){advance(i,dt);i.x+=.001;i.z-=.003;auto d=observe(s,i);CHECK(d.submit);x+=d.x;z+=d.z;
   CHECK(!observe(s,i).submit);}
  CHECK(near(x,.060)&&near(z,-.180)); // total physical metres, independent of frame rate
  for(int n=0;n<20;++n){advance(i,dt);CHECK(!observe(s,i).submit);} // no offset-to-speed tail
 }
 TrackedStepState s{};auto i=sample();observe(s,i);
 advance(i);i.x=.02;i.z=.03;auto d=observe(s,i);CHECK(d.submit&&near(d.x,.02)&&near(d.z,.03));
 // Downward lean excludes displacement, including sideways movement; hysteresis
 // avoids alternating ownership near the configured height threshold.
 advance(i);i.y=1.59;i.x=.1;d=observe(s,i);CHECK(d.lean&&!d.submit&&d.retirePending);
 advance(i);i.y=1.63;i.x=.2;d=observe(s,i);CHECK(d.lean&&!d.submit);
 advance(i);i.y=1.7;i.x=.3;d=observe(s,i);CHECK(!d.lean&&!d.submit); // recovery never catches up
 advance(i);i.x=.301;d=observe(s,i);CHECK(d.submit&&near(d.x,.001));
 // Standing/walking height rises remain admitted. This is HMD classification,
 // not a claim of foot tracking or perfect classification of shallow leaning.
 advance(i);i.y=1.77;i.z=.05;CHECK(observe(s,i).submit);
 advance(i,70);i.x=.5;d=observe(s,i);CHECK(!d.submit&&d.retirePending);
 advance(i);i.x=.51;CHECK(observe(s,i).submit);
 advance(i);i.x=2;d=observe(s,i);CHECK(!d.submit&&d.retirePending);
 advance(i);i.live=false;CHECK(observe(s,i).retirePending);
 advance(i);i.live=true;i.x=4;CHECK(!observe(s,i).submit);
 advance(i);i.x=4.01;CHECK(observe(s,i).submit);
 advance(i);i.allowed=false;CHECK(observe(s,i).retirePending);
 advance(i);i.allowed=true;i.x=5;CHECK(!observe(s,i).submit);
 for(int field=0;field<3;++field){advance(i);i.x+=.1;if(field==0)++i.actorLifetime;if(field==1)++i.tracking;if(field==2)++i.origin;
  CHECK(!observe(s,i).submit);advance(i);i.x+=.01;CHECK(observe(s,i).submit);}
 auto bad=i;advance(bad);bad.now=bad.tick+61;CHECK(observe(s,bad).retirePending);
 observe(s,i);bad=i;advance(bad);bad.now=bad.tick-1;CHECK(observe(s,bad).retirePending);
 observe(s,i);bad=i;advance(bad);bad.x=std::numeric_limits<double>::quiet_NaN();CHECK(observe(s,bad).retirePending);
 observe(s,i);bad=i;--bad.sample;CHECK(observe(s,bad).retirePending);
 std::printf("D76 tracked displacement: %d/%d PASS\n",checks-failures,checks);
 return failures?1:0;
}
