#include "diagnostics/capture168.h"
#include <cassert>
#include <cstdio>
int main(){
 namespace c=kcd2vr::diagnostics::capture168;
 constexpr auto now=1000ull;
 const char* lines[]{"D52 CONTACT_OCCUPANCY sample=1/96","D52 GORE_LIFECYCLE rollback=1",
   "D52 F5_DIALOGUE_PARTICIPANTS sourceTick=1000","D52 F5_DIALOGUE_GAZE wrote=1"};
 c::gate.mask=0;for(auto s:lines)assert(!c::accept(s,now));
 c::gate.select(c::Domain::Interaction,true,now);
 assert(c::accept(lines[0],now)&&!c::accept(lines[1],now)&&c::accept(lines[2],now)&&c::accept(lines[3],now));
 c::gate.select(c::Domain::Interaction,false,now);c::gate.select(c::Domain::Combat,true,now);
 assert(!c::accept(lines[0],now)&&c::accept(lines[1],now)&&!c::accept(lines[2],now)&&!c::accept(lines[3],now));
 c::gate.select(c::Domain::Combat,false,now);c::gate.select(c::Domain::Bug,true,now);
 assert(c::accept(lines[0],now)&&!c::accept(lines[1],now)&&!c::accept(lines[2],now)&&!c::accept(lines[3],now));
 for(auto s:lines)assert(!c::accept(s,now+c::Duration+1));
 puts("D52 capture routing: quiet, F5, F3, F2, expiry PASS");
}
