// R123 fixed melee grip + crossbow rest-frame skip: pure rules with the R122 log numbers.
#include "vrik/melee_grip_r123.h"
#include <cmath>
#include <cstdio>
static unsigned checks=0,failures=0;
static void check(bool ok,const char* what){++checks;if(!ok){++failures;std::printf("FAIL %s\n",what);}}
static double angleDeg(const double a[4],const double b[4]){double d=std::fabs(a[0]*b[0]+a[1]*b[1]+a[2]*b[2]+a[3]*b[3]);if(d>1)d=1;return 2*std::acos(d)*57.29577951308232;}
int main(){
    namespace mg=kcd2vr::melee_grip_r123;
    check(mg::enabled.load(),"R123 fixed grip is ON by default");
    check(mg::eligible(4,true,false),"longsword (class 4) uses the authored socket grip");
    check(mg::eligible(5,true,false),"hip weapon (class 5) uses the authored socket grip");
    check(!mg::eligible(7,true,false),"polearm keeps its R26 slt_0 grip");
    check(!mg::eligible(8,true,true)&&!mg::eligible(17,true,true),"shields keep their B4 reference path");
    check(!mg::eligible(14,false,false)&&!mg::eligible(9,false,false),"crossbow and bow are not melee");
    // R122 log 14:01:51 HAND_F10 ITEM key 18E25: native rest relation vs the applied learned grip.
    const double rest[4]{0.7071068,-0.7071068,0,0},learned[4]{0.910,-0.059,-0.386,0.141};
    check(angleDeg(rest,learned)>80,"the learned longsword grip is ~93 deg off its authored socket (the bug)");
    // crossbow: (-0.065,0.024,0.018) q~identity = bind/rest frame, (-0.081,0.142,-0.000) = the real hold, 12 cm apart
    const double restPos=0.0,holdPos=std::sqrt(0.016*0.016+0.118*0.118+0.018*0.018);
    check(mg::restFrame(restPos,0.0,0),"crossbow sample on the rest relation is skipped");
    check(!mg::restFrame(holdPos,1.3,0),"crossbow real hold (12 cm away) is learned");
    check(mg::restFrame(0.02,0.1,44)&&!mg::restFrame(0.02,0.1,45),"rest skip is bounded (no endless wait, no latch)");
    std::printf("R123 melee grip %u/%u\n",checks-failures,checks);
    return failures?1:0;
}
