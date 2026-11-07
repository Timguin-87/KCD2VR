#include "../src/vrik/impact_haptics133.h"
#include <cstdio>
#include <cmath>
using kcd2vr::impact_haptics133::map;
static int fails=0,checks=0;
static void ok(bool v,const char* n){++checks;if(!v){++fails;std::printf("FAIL %s\n",n);}else std::printf("PASS %s\n",n);}
int main(){
    const auto quiet=map(.20,2.0,false);
    const auto light=map(2.0,1.0,false);
    const auto heavy=map(2.0,5.0,false);
    const auto fast=map(5.0,5.0,true);
    ok(quiet.primary==0.0f&&quiet.support==0.0f,"sub-threshold contact stays silent");
    ok(heavy.primary>light.primary,"heavier weapon gives stronger cue at same impact speed");
    ok(fast.primary>heavy.primary,"faster impact gives stronger cue");
    ok(fast.support>0.0f&&fast.support<fast.primary,"secondary hand gets weaker shared impact cue");
    ok(fast.primary<=1.0f&&fast.support<=1.0f&&fast.durationMs>=1&&fast.durationMs<=100,"OpenXR pulse contract remains bounded");
    std::printf("impact133 checks=%d failures=%d primary=%.3f support=%.3f duration=%d\n",checks,fails,fast.primary,fast.support,fast.durationMs);
    return fails?1:0;
}
