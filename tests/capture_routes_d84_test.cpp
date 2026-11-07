#include "diagnostics/capture168.h"
#include "diagnostics/reload_capture_d84.h"
#include <cassert>
#include <cstdio>
namespace c=kcd2vr::diagnostics::capture168;
int main(){
 constexpr std::uint64_t now=10000;
 static_assert(unsigned(c::Domain::Bug)==7&&unsigned(c::Domain::Physics)==8);
 for(auto d:{c::Domain::Bug,c::Domain::Combat,c::Domain::Interaction,c::Domain::Vrik,c::Domain::Reload,c::Domain::Physics})
  c::gate.select(d,false,now);
 assert(!c::any(now)&&!c::accept("D84_F9 UPSTREAM reason=HUD_FAULT",now));
 c::gate.select(c::Domain::Physics,true,now);
 assert(c::any(now)&&c::active(c::Domain::Physics,now));
 assert(!c::active(c::Domain::Combat,now)&&!c::active(c::Domain::Bug,now));
 assert(c::domain("D84_F9 UPSTREAM reason=HUD_FAULT")==c::Domain::Physics);
 assert(c::accept("D84_F9 UPSTREAM reason=HUD_FAULT",now));
 assert(c::accept("C58 LAB RUNG_NOW=0",now));
 assert(!c::accept("C54R3 HIT_CHAIN reason=PHYSICS",now));
 assert(!c::accept("C54R3 F5_DOOR_CHAIN reason=PHYSICS",now));
 const auto f9=c::gate.session[unsigned(c::Domain::Physics)].load();
 for(auto d:{c::Domain::Bug,c::Domain::Combat,c::Domain::Interaction,c::Domain::Vrik})c::gate.select(d,true,now+1);
 c::gate.select(c::Domain::Physics,false,now+2);
 assert(!c::active(c::Domain::Physics,now+2)&&c::active(c::Domain::Interaction,now+2));
 assert(!c::accept("D84_F9 UPSTREAM reason=HUD",now+2));
 assert(c::gate.receipt(c::Domain::Physics,f9,now+3));
 assert(!c::gate.receipt(c::Domain::Physics,f9,now+5003));
 c::gate.select(c::Domain::Physics,true,now+4);
 assert(!c::gate.receipt(c::Domain::Physics,f9,now+4));
 assert(c::gate.poll(now+4+c::Duration)&(1u<<unsigned(c::Domain::Physics)));
 assert(!c::any(now+4+c::Duration));
 std::puts("PASS D84 F9 independent capture: OFF, combined keys, typed family, original session, stop/restart and finite expiry");
}
