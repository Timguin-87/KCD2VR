#include "vrik/melee_stroke118.h"
#include <cassert>
#include <cstdio>
using namespace kcd2vr::melee118;
int main(){
 Policy p{true,true,true,false,false,true};p.deliberateCut42=true;p.purposefulCuts52=true;
 Input in{};in.dt=.02;in.eligible=in.known=true;in.forward=in.modelForward={0,1,0};in.axis=in.modelAxis={1,0,0};
 State s{};s.ready=true;int prepares=0,commits=0;
 // The prior 2.0m/s onset + 1.2m/s fast-path floor let this slow sweep buy distance.
 in.tip=in.modelTip={0,.046,0};in.hand=in.modelHand={0,.022,0};
 for(int i=0;i<40;++i){auto r=update(s,in,p);prepares+=r.prepare;commits+=r.commit;}
 assert(!prepares&&!commits);
 // A deliberate translating cut still commits promptly in both independent frames.
 s={};s.ready=true;in.tip=in.modelTip={0,.08,0};in.hand=in.modelHand={0,.03,0};
 for(int i=0;i<5;++i){auto r=update(s,in,p);prepares+=r.prepare;commits+=r.commit;}
 assert(prepares==1&&commits==1);
 // An isolated spike cannot accumulate its following slow recovery as fast travel.
 s={};s.ready=true;update(s,in,p);in.tip=in.modelTip={0,.035,0};in.hand=in.modelHand={0,.018,0};
 for(int i=0;i<20;++i){auto r=update(s,in,p);assert(!r.commit);}
 // Thrusts retain their separate, lower axial hand-speed requirement.
 s={};s.ready=true;in.axis=in.modelAxis={0,1,0};in.tip=in.modelTip=in.hand=in.modelHand={0,.03,0};
 bool stab=false;for(int i=0;i<7;++i)stab|=update(s,in,p).commit;assert(stab);
 // A pose jump in one family alone cannot satisfy the live cut floor.
 s={};s.ready=true;in.axis=in.modelAxis={1,0,0};in.tip={0,.1,0};in.modelTip={0,.02,0};
 for(int i=0;i<10;++i)assert(!update(s,in,p).commit);
 std::puts("purposeful_cut52: slow sweep, deliberate cut, recovery, stab and two-frame agreement PASS");
}
