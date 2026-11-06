#include "../src/vrik/fist_motion.h"
#include <cstdio>
using namespace kcd2vr::fists;
int main(){ int failed=0; for(int hz:{60,90,120,144}){ State s{};Sample x{};x.epoch=1;x.actor=123;x.valid=x.empty[0]=x.empty[1]=true;x.head={0,1.6,0};x.controller[0]=x.allowed[0]={.2,1.3,-.2};x.controller[1]=x.allowed[1]={-.2,1.3,-.2}; int hits=0,resets=0,same=0;std::uint64_t last=0;
for(int i=0;i<hz*2;++i){double t=double(i)/hz;x.tick=1008+std::uint64_t(t*1000/16)*16;x.sequence=i+1;if(x.tick==last)++same;last=x.tick;x.grip[0]=x.trigger[0]=t>.2?1:0;double travel=t<1?0:std::min(.26,(t-1)*1.8);x.controller[0].z=x.allowed[0].z=-.2-travel;const bool was=s.hand[0].released;hits+=step(s,x).valid;if(was&&!s.hand[0].released)++resets;}
if(hits!=1||resets!=0)++failed;std::printf("hz=%d timer_quantum=16ms same_tick_samples=%d readiness_resets=%d punch_requests=%d\n",hz,same,resets,hits);} return failed?1:0;}
