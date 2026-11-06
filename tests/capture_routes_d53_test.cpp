#include "diagnostics/interaction_cycle_h4.h"
#include <cstdio>
namespace c=kcd2vr::diagnostics::capture168;
namespace h=kcd2vr::diagnostics::interaction_cycle_h4;
int main(){
 unsigned checks=0,failures=0;
 auto check=[&](bool v,const char* m){++checks;if(!v){++failures;std::printf("FAIL %s\n",m);}};
 const char* prompt="D53 PROMPT_CONSUMER boundary=ACTION_HINT";
 const char* dodge="R59.16C5 ROOMSCALE_DODGE selected=left";
 c::gate.mask=0;check(!c::accept(prompt,1000),"quiet prompts off");
 check(!c::accept(dodge,1000),"quiet dodge off");
 h::Event e{};e.tick=1000;e.stage=h::Stage::Lifecycle;e.actor=123;e.token=45;e.code=0;
 h::copy(e.reason,sizeof(e.reason),"TRACKED_CAMERA_PACKET_JOIN_UNAVAILABLE");
 h::copy(e.label,sizeof(e.label),"D53_CAMERA_CORE_NOT_FINAL_RENDER");
 check(!h::observe(e,1000),"quiet camera off");
 c::gate.select(c::Domain::Interaction,true,1000);
 check(c::accept(prompt,1000),"F5 prompts captured");
 check(c::accept(dodge,1000),"F5 dodge remains captured");
 check(h::observe(e,1000),"F5 camera joins same capture");
 h::Event actual{};check(h::recorder.pop(actual)&&actual.token==45&&actual.actor==123,"camera owner identity preserved");
 e.tick=1001;check(!h::observe(e,1001),"identical camera reason coalesced");
 e.token=46;check(h::observe(e,1001),"new pickup token survives downstream coalescing");
 e.actor=124;check(h::observe(e,1001),"new camera actor survives downstream coalescing");
 e.tick=1002;e.code=1;h::copy(e.reason,sizeof(e.reason),"ACCEPTED_TRACKED_BODY_CAMERA_BASE");
 check(h::observe(e,1002),"camera recovery retained");
 c::gate.select(c::Domain::Interaction,false,1003);c::gate.select(c::Domain::Bug,true,1003);
 check(c::accept(prompt,1003),"F2 prompts captured");
 check(!c::accept(dodge,1003),"F2 does not enable combat dodge receipt");
 e.tick=1003;check(!h::observe(e,1003),"F2 does not open F5 camera queue");
 check(!c::accept(prompt,1003+c::Duration+1),"finite prompt capture expires");
 c::gate.mask=0;c::gate.select(c::Domain::Combat,true,1004);
 check(c::accept(dodge,1004),"F3 captures existing dodge decision");
 check(!c::accept(dodge,1004+c::Duration+1),"finite combat capture expires");
 std::printf("D53 capture boundaries %u/%u\n",checks-failures,checks);return failures?1:0;
}
