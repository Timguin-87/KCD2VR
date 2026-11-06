#include "vrik/interaction_ownership111.h"
#include <cstdio>
#include <cstdlib>
using namespace kcd2vr::interaction_ownership111;
static int n=0;static void ck(bool v,const char*m){++n;if(!v){std::fprintf(stderr,"FAIL %s\n",m);std::exit(1);}}
int main(){
 ck(Live(0,0,false,true,false),"production gameplay enabled");
 ck(Live(1,0,false,true,false),"V135 F3 reference does not own F5 interactions");
 ck(Live(2,0,false,true,false),"V135 F3 live diagnostic does not own F5 interactions");
 ck(!Live(0,1,false,true,false),"F5 reference read only");
 ck(Live(0,2,false,true,false),"F5 live diagnostic");
 ck(Comparison114(1,3,false,true),"V135 F5 held comparison independent of F3 reference");
 ck(Comparison114(0,2,false,true),"F5 current rung 2 reaches held interaction");
 ck(!Comparison114(0,1,false,true),"F5 recorder remains read-only");
 ck(!Comparison114(0,2,true,true),"F5 held remains menu gated");
 ck(!Live(0,0,true,true,false),"UI blocks");
 ck(!Live(0,0,false,false,false),"uncalibrated blocks");
 ck(!Live(0,0,false,true,true),"physical combat ownership blocks contextual use");
 ck(HandEligible(true,0,true,false),"empty tracked neutral hand");
 ck(!HandEligible(true,1,true,false),"occupied blocks");
 ck(!HandEligible(true,-1,true,false),"unknown item blocks");
 ck(!HandEligible(true,0,true,true),"UI grip ownership blocks");
 ck(!HandEligible(false,0,true,false),"tracking blocks");
 ck(!HandEligible(true,0,false,false),"stick/modifier blocks");
 std::printf("PASS %d interaction ownership checks\n",n);return 0;
}