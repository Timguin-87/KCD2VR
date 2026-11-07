// Actual production gesture input producer + actual native work ledger.
// Native callback scheduling/active-time integration are explicit host leaves.
#define KCD2VR_INPUT67_NO_MAIN
#include "input67_test.cpp"
#include "../../../src/vrik/alchemy_work_clock_d47.h"
using LedgerD49=kcd2vr::alchemy_work_clock_d47::Ledger;
struct MeterD49 {
 LedgerD49 clock;double active=0,cursor=0;std::uint64_t sequence=0;
 MeterD49(){clock.seed(0);}
 void tick(const s::Control& c,double dt){
  check(clock.observe(active,1));const bool fresh=c.sequence!=sequence;
  active+=double(clock.rate(fresh?c.totalSeconds-cursor:0,fresh,c.clockStop,dt,true))*dt;
  if(fresh){cursor=c.totalSeconds;sequence=c.sequence;}
 }
};
static void setupD49(Fixture& q,bool bellows){
 if(bellows){q.admit(18,1,true);auto& t=q.in.nativeFrame.table;t.actionKnown=true;t.action=999;
  t.handTypesKnown68=true;t.handType68[1]=18;q.in.action.parent=999;
  q.in.work={1,999,555,3,q.in.now,44,s::Bellows,2,2,true,false};
  auto& v=q.in.nativeFrame.offers[18];v.boundsValid=true;for(int j=0;j<3;++j){v.bounds.local[j]=-.02f;v.bounds.local[j+3]=.02f;v.bounds.world[j*4+j]=1;}
 }else{q.admit(2,1);q.advance();q.grip(0,.8f);q.in.palm[0].point={0,0,0};q.in.world[0].point[0]=0;
  q.in.action.parent=999;q.in.work={1,999,555,3,q.in.now,44,s::Mortar,3,3,true,false};
  for(int h=0;h<2;++h){q.tool(h,h?2:3);auto& t=q.in.tool[h];t.parent=999;
   t.binding={1,2,888,std::uintptr_t(700+h),t.entity,h,t.type,999,0x3F60770,std::uintptr_t(600+h),3};}
 }
 q.step();
}
static void gestureCadenceD49(bool bellows,int every,int phase){
 Fixture q;setupD49(q,bellows);MeterD49 m;decltype(q.step()) o{};
 for(int i=1;i<=60;++i){q.advance(11);
  if(bellows)q.f.hand[1].position[1]=q.in.world[1].point[2]=-i*.0008f;
  else q.f.hand[0].position[0]=i*.0008f;
  o=q.step();check(!o.semiControl.clockStop); // below quantum still moving
  if(i%every==phase)m.tick(o.semiControl,.011*every);
 }
 std::printf("gesture=%s every=%d phase=%d producer=%.6f active=%.6f discarded=%.6f\n",bellows?"bellows":"mortar",every,phase,o.semiControl.totalSeconds,m.active,m.clock.discarded);
 check(o.semiControl.totalSeconds>.29&&m.active>.28&&m.clock.discarded<.000001);
 // Same tracking sample is not another stroke or native charge.
 const auto total=o.semiControl.totalSeconds;o=q.step();check(o.semiControl.totalSeconds==total);
 // A genuine stop closes permission immediately despite fractional pending travel.
 q.advance(11);o=q.step();check(o.semiControl.clockStop);m.tick(o.semiControl,.011);check(m.clock.pending==0);
 q.advance(11);q.grip(bellows?1:0,0);o=q.step();check(o.semiControl.clockStop);
 if(!bellows)check(!o.control.held[0]&&o.control.held[1]);
}
static void semiCadenceD49(int every,int phase){
 s::Producer p;Frame f{};f.allowed=f.headValid=true;
 f.poseEpoch=4;f.sourceEpoch=5;f.sequence=1;f.tick=1000;f.timeNs=1000000000;
 for(auto& h:f.hand){h.valid=true;h.rotation[3]=1;h.grip=.8f;}
 s::WorkWitness w{1,999,555,3,f.tick,44,s::Mortar,3,3,true,false};
 auto c=p.step(f,true,1,3,w,false,false,0);MeterD49 m;
 for(int i=1;i<=80;++i){++f.sequence;f.tick+=11;f.timeNs+=11000000;w.tick=f.tick;
  f.hand[0].position[0]=i*.00048828125f;c=p.step(f,true,1,3,w,false,false,0);
  check(!c.clockStop);if(i%every==phase)m.tick(c,.011*every);
 }
 std::printf("semi every=%d phase=%d producer=%.6f active=%.6f discarded=%.6f\n",every,phase,c.totalSeconds,m.active,m.clock.discarded);
 check(c.totalSeconds>.24&&m.active>.23&&m.clock.discarded<.000001);
 ++f.sequence;f.tick+=11;f.timeNs+=11000000;w.tick=f.tick;c=p.step(f,true,1,3,w,false,false,0);
 check(c.clockStop);m.tick(c,.011);check(m.clock.pending==0);
 // Owner loss and invalid tracking do not inherit motion permission.
 ++f.sequence;f.tick+=11;f.timeNs+=11000000;w.tick=f.tick;f.hand[0].valid=false;
 c=p.step(f,true,1,3,w,false,false,0);check(!c.enabled&&c.clockStop);
}
int main(){
 for(auto mode:{std::pair{1,0},std::pair{2,0},std::pair{2,1},std::pair{3,0},std::pair{3,1},std::pair{3,2}}){
  gestureCadenceD49(false,mode.first,mode.second);gestureCadenceD49(true,mode.first,mode.second);semiCadenceD49(mode.first,mode.second);
 }
 LedgerD49 l;l.seed(0);check(l.rate(.05,true,false,.02,true)>0);const auto pending=l.pending;
 check(l.rate(0,false,false,.02,true)>0&&l.pending==pending);
 check(l.rate(0,false,true,.02,true)==0&&l.pending==0);
 check(l.rate(.05,true,false,.02,true)>0);check(l.rate(0,false,false,.02,false)==0&&l.pending==0);
 check(l.rate(.5,true,false,.02,true)<=2.5&&l.pending<=.12);
 check(l.observe(.05,1)&&l.pending<=.070001);const auto after=l.pending;check(l.observe(.05,1)&&l.pending==after);
 std::printf("D49 cadence: %u checks PASS; no retail execution or recipe completion inferred\n",checks);
}
