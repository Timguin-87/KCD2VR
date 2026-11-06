#include "vrik/defense_timing39.h"
#include "vrik/dirblock39.h"
#include <cassert>
#include <initializer_list>
int main(){namespace t=kcd2vr::defense_timing39;
 auto d=t::decide(true,.3f,true,true,256,30,2,3);assert(d.reason==t::Reason::Applied&&d.eta>=.099f&&d.eta<=.101f);
 for(int p:{4,8,16,32,64,512})assert(t::decide(true,.3f,true,true,p,30,2,3).eta==.3f);
 assert(t::decide(true,.3f,true,false,256,30,2,3).eta==.3f);
 assert(t::effectiveEta(.1f,3)==.08f);assert(t::decide(false,.3f,true,true,256,30,2,3).eta==.3f);
}
