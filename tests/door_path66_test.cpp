#include "../src/core/door_path66.h"
#include <cassert>
#include <cstdio>
#include <limits>
namespace d=kcd2vr::door_path66;
static unsigned checks=0;
static void check(bool b){++checks;assert(b);}
static bool near(double a,double b,double e=1e-5){return std::abs(a-b)<e;}
static d::Source source(){d::Source s{};s.operation=1;s.clip=2;s.topology=3;s.curve=4;s.press=5;s.poseEpoch=6;
 s.trackingEpoch=7;s.generation=8;s.settingsEpoch=9;s.experimentEpoch=10;s.player=11;s.entity=12;s.target=13;s.side=1;return s;}
static d::Curve curve(){d::Curve c{};c.source=source();
 for(unsigned i=0;i<33;++i){const double p=double(i)/32,a=p*1.5707963267948966;
 c.samples[i]={p,{{std::cos(a),std::sin(a),1},{0,0,std::sin(a*.5),std::cos(a*.5)}}};}return c;}
static d::Input input(const d::Curve& c,double phase,std::uint64_t now,d::Point offset={},d::Pose frame={}){
 const auto h=d::at(c,phase);return {c.source,frame,d::world(frame,d::add(h.p,d::rotate(h.q,offset))),now,now};}
int main(){
 auto c=curve();check(d::valid(c));d::State s{};d::Result r{};auto in=input(c,.25,1000,{0,.03,0});
 check(d::bind(s,c,in,1000,.25));check(near(s.offset.y,.03));
 // Exactly stationary, with actual fresh input, pauses instead of running a clock.
 in.tick=in.poseTick=1010;check(d::project(s,c,in,1010,{},r));check(r.stopped&&r.rate==0&&near(r.phase,.25));
 in=input(c,.375,1020,{0,.03,0});check(d::project(s,c,in,1020,{},r));check(near(r.phase,.375)&&r.rate>0&&!r.stopped);
 in=input(c,.3125,1030,{0,.03,0});check(d::project(s,c,in,1030,{},r));check(near(r.phase,.3125)&&r.rate<0);
 // A world translation/rotation moves the whole model and retained hand together.
 d::Pose frame{{100,-20,8},{0,0,std::sin(.4),std::cos(.4)}};
 in=input(c,.3125,1040,{0,.03,0},frame);check(d::project(s,c,in,1040,{},r));check(r.stopped&&near(r.phase,.3125));
 const auto expected=d::world(frame,d::at(c,r.phase).p);check(near(expected.x,r.handleWorld.p.x)&&near(expected.y,r.handleWorld.p.y));
 in=input(c,.5,1050,{0,.03,0},frame);check(d::project(s,c,in,1050,{},r));check(near(r.phase,.5)&&r.error<1e-6);
 // Direction belongs to the actual authored curve, independent of left/right or opening/closing.
 auto reversed=c;for(auto& x:reversed.samples){const double a=(1-x.phase)*1.5707963267948966;
 x.handle={{std::cos(a),std::sin(a),1},{0,0,std::sin(a*.5),std::cos(a*.5)}};}
 check(d::bind(s,reversed,input(reversed,.75,2000),2000,.75));
 check(d::project(s,reversed,input(reversed,.5,2010),2010,{},r));check(near(r.phase,.5)&&r.rate<0);
 // Nonlinear authored phase spacing and variable speed are retained.
 auto nonlinear=c;for(auto& x:nonlinear.samples){const double a=x.phase*x.phase*1.5707963267948966;
 x.handle={{std::cos(a),std::sin(a),1},{0,0,std::sin(a*.5),std::cos(a*.5)}};}
 check(d::bind(s,nonlinear,input(nonlinear,.25,2100),2100,.25));
 check(d::project(s,nonlinear,input(nonlinear,.5,2120),2120,{},r));check(near(r.phase,.5)&&near(r.rate,12.5));
 // Duplicate/flat portions cannot invent time advancement; nearest phase wins ties.
 auto flat=c;for(unsigned i=10;i<=14;++i)flat.samples[i].handle=flat.samples[10].handle;
 check(d::valid(flat));check(d::bind(s,flat,input(flat,.375,2200),2200,.375));
 check(d::project(s,flat,input(flat,.375,2210),2210,{},r));check(r.stopped&&near(r.phase,.375));
 // A crossing has two geometric solutions. A bounded local window keeps the current branch.
 auto crossing=c;for(auto& x:crossing.samples){const double a=x.phase*6.283185307179586;
 x.handle={{std::sin(a),std::sin(2*a),0},{}};}
 check(d::bind(s,crossing,input(crossing,.5,2300),2300,.5));
 check(d::project(s,crossing,input(crossing,.5,2310),2310,{.10,.20,.001},r));check(near(r.phase,.5));
 // Slow deliberate motion accumulates across frames below the noise distance.
 auto line=c;for(auto& x:line.samples)x.handle={{x.phase,0,0},{}};
 check(d::bind(s,line,input(line,.5,2400),2400,.5));
 for(unsigned i=1;i<=3;++i){check(d::project(s,line,input(line,.5+i*.0002,2400+i*10),2400+i*10,{},r));check(r.stopped);}
 check(d::project(s,line,input(line,.5012,2440),2440,{},r));check(!r.stopped&&near(r.phase,.5012));
 // Every identity component is part of the exact source; no rebind or cross-door adoption.
 for(unsigned fault=0;fault<14;++fault){check(d::bind(s,c,input(c,.25,3000),3000,.25));auto bad=input(c,.3,3010);
 switch(fault){case 0:++bad.source.operation;break;case 1:++bad.source.clip;break;case 2:++bad.source.topology;break;
 case 3:++bad.source.curve;break;case 4:++bad.source.press;break;case 5:++bad.source.poseEpoch;break;
 case 6:++bad.source.trackingEpoch;break;case 7:++bad.source.generation;break;case 8:++bad.source.settingsEpoch;break;
 case 9:++bad.source.experimentEpoch;break;case 10:++bad.source.player;break;case 11:++bad.source.entity;break;
 case 12:++bad.source.target;break;case 13:bad.source.side=0;break;}
 check(!d::project(s,c,bad,3010,{},r)&&!s.active);}
 for(unsigned fault=0;fault<12;++fault){auto bad=c;
 switch(fault){case 0:bad.source.entity=0;break;case 1:bad.source.side=2;break;case 2:bad.samples[0].phase=.01;break;
 case 3:bad.samples[32].phase=.99;break;case 4:bad.samples[5].phase=bad.samples[4].phase;break;
 case 5:bad.samples[6].phase=std::numeric_limits<double>::quiet_NaN();break;
 case 6:bad.samples[7].handle.p.x=std::numeric_limits<double>::infinity();break;
 case 7:bad.samples[8].handle.q.w=2;break;case 8:for(auto& x:bad.samples)x.handle={};break;
 case 9:bad.source.press=0;break;case 10:bad.source.trackingEpoch=0;break;case 11:bad.source.target=16777216;break;}
 check(!d::valid(bad));check(!d::bind(s,bad,input(bad,.25,4000),4000,.25));}
 // Retired/stale/future input, nonrigid model transform, detached hand and bad policy all fail closed.
 for(unsigned fault=0;fault<12;++fault){check(d::bind(s,c,input(c,.25,5000),5000,.25));auto bad=input(c,.3,5010);d::Limits l{};
 switch(fault){case 0:bad.poseTick=4929;break;case 1:bad.tick=5011;break;case 2:bad.modelWorld.q.w=2;break;
 case 3:bad.handWorld.x=std::numeric_limits<double>::quiet_NaN();break;case 4:bad.handWorld={99,99,99};break;
 case 5:l.maxPhaseStep=0;break;case 6:l.maxPhaseStep=2;break;case 7:l.maxDistance=.21;break;
 case 8:l.stopDistance=.20;break;case 9:l.stopDistance=-1;break;case 10:bad.poseTick=0;break;
 case 11:l.maxDistance=std::numeric_limits<double>::quiet_NaN();break;}
 check(!d::project(s,c,bad,5010,l,r)&&!s.active);}
 check(d::bind(s,c,input(c,.25,6000),6000,.25));check(!d::project(s,c,input(c,.3,6081),6081,{},r));
 check(!d::bind(s,c,input(c,.25,7000,{.201,0,0}),7000,.25));
 // Offset is retained through several phases, never snapped into a bind-pose or hand centre.
 check(d::bind(s,c,input(c,.1,8000,{.02,.03,-.01}),8000,.1));
 for(unsigned i=1;i<8;++i){const double phase=.1+i*.08;check(d::project(s,c,input(c,phase,8000+i*10,{.02,.03,-.01}),8000+i*10,{},r));
 check(near(r.phase,phase,2e-4)&&r.error<.001);}
 std::printf("door_path66: %u checks PASS (pure model-space inversion; no native acceptance claim)\n",checks);
}
