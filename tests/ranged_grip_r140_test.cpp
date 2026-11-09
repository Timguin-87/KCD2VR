// R140 bow/crossbow first draw: the rest/bind-frame rules of melee_grip_r123.h plus a replica of the primary-hand
// learn order in physical_native.inl PhysCaptureGrip6698 (window countdown -> rest skip -> 6 stable -> finalize).
#include "vrik/melee_grip_r123.h"
#include <cmath>
#include <cstdio>
#include <vector>
static unsigned checks=0,failures=0;
static void check(bool ok,const char* what){++checks;if(!ok){++failures;std::printf("FAIL %s\n",what);}}
namespace mg=kcd2vr::melee_grip_r123;
// one sample = distance (m) / angle (rad) of the native grip candidate from the authored rest relation
struct S{double m,rad;};
struct Learn{bool ready=false;unsigned stable=0,skips=0,window=0;S cand{},hand{};int replaced=0,provisional=0,kept=0;};
static bool near(S a,S b){return std::fabs(a.m-b.m)<.018&&std::fabs(a.rad-b.rad)<.10;}
static void step(Learn& g,S c,int cls){
    if(g.ready&&!g.window)return;
    if(g.ready&&g.window&&!--g.window){++g.kept;return;}
    const bool ranged=mg::rangedRest(cls);
    const bool onRest=ranged&&mg::restRelation(c.m,c.rad);
    if(ranged&&mg::skipRest(c.m,c.rad,g.skips,g.window!=0)){if(!g.window)++g.skips;g.stable=0;return;}
    if(g.stable&&near(c,g.cand))++g.stable;else{g.cand=c;g.stable=1;}
    if(g.stable>=6){
        if(g.window){g.window=0;++g.replaced;}
        else if(onRest){g.window=mg::kRestWindow;++g.provisional;}
        g.hand=c;g.ready=true;
    }
}
static Learn run(int cls,unsigned bind,unsigned hold,S holdPose={.12,1.3}){
    Learn g{};for(unsigned i=0;i<bind;++i)step(g,{0,0},cls);for(unsigned i=0;i<hold;++i)step(g,holdPose,cls);return g;
}
int main(){
    check(mg::rangedRest(9)&&mg::rangedRest(10)&&mg::rangedRest(14)&&mg::rangedRest(15),"bow and the three crossbows use the rest skip");
    check(!mg::rangedRest(4)&&!mg::rangedRest(5)&&!mg::rangedRest(7)&&!mg::rangedRest(8)&&!mg::rangedRest(13),"melee, polearm, shield, rifle unchanged");
    check(mg::restRelation(0,0)&&!mg::restRelation(.12,1.3),"bind frame is rest, the crossbow hold 12 cm away is not");
    for(unsigned k=0;k<200;++k)check(mg::skipRest(0,0,k,false)==mg::restFrame(0,0,k),"crossbow skip identical to R123 restFrame when not provisional");
    check(mg::skipRest(0,0,999,true),"provisional window skips rest frames past the cap");
    // normal draw: 3 bind frames then the hold
    {auto g=run(9,3,20);check(g.ready&&near(g.hand,{.12,1.3})&&!g.provisional,"bow: short bind frame never learned (old code: learned only if >= 6 bind frames)");}
    // slow first draw after a load: 10 identical bind frames (old code: bow learned the bind frame = rotated)
    {auto g=run(9,10,20);check(g.ready&&near(g.hand,{.12,1.3}),"bow slow first draw: the real hold is learned, not the bind frame");}
    // very slow: bind longer than the 45-sample cap -> provisional rest learn, then the hold replaces it
    {auto g=run(10,60,20);check(g.ready&&g.provisional==1&&g.replaced==1&&near(g.hand,{.12,1.3}),"crossbow bind > cap: provisional rest learn replaced by the hold");}
    // a weapon whose hold IS the rest relation: learned after the cap, kept after the window (never left without a grip)
    {auto g=run(9,200,0);check(g.ready&&g.provisional==1&&g.kept==1&&!g.window&&near(g.hand,{0,0}),"hold on the rest relation: kept, window closes (no latch)");}
    // melee never skips
    {auto g=run(4,6,0);check(g.ready&&near(g.hand,{0,0})&&!g.provisional,"melee learn order unchanged (no skip, no window)");}
    std::printf("R140 ranged grip %u/%u\n",checks-failures,checks);
    return failures?1:0;
}
