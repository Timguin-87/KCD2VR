#include "../src/diagnostics/capture168.h"
#include <cstdio>
int main(){
 namespace c=kcd2vr::diagnostics::capture168;unsigned checks=0,failed=0;
 auto check=[&](bool v){++checks;if(!v)++failed;};
 for(const char* s:{"D49 HOLSTER_DOOR_ROUTE token=8 ack=0","H40 F5_HAND_ONLY_DOOR ack=0 pending=8","V66.90R1-CURSOR cvarRejected=1"}){
  c::gate.mask=0;check(!c::accept(s,101));
  c::gate.select(c::Domain::Combat,true,100);check(!c::accept(s,101));
  c::gate.mask=0;c::gate.select(c::Domain::Interaction,true,100);check(c::accept(s,101));
  c::gate.mask=0;c::gate.select(c::Domain::Bug,true,100);check(c::accept(s,101));
  check(!c::accept(s,180101));
 }
 c::gate.mask=0;c::gate.select(c::Domain::Bug,true,100);
 check(!c::accept("D48_F11 event=RELOAD_ACCEPTED",101));
 c::gate.select(c::Domain::Reload,true,100);check(c::accept("D48_F11 event=RELOAD_ACCEPTED",101));
 std::printf("D49 production diagnostic routing %u/%u; quiet/expiry/domain isolation\n",checks-failed,checks);
 return failed?1:0;
}
