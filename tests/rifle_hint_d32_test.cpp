#include "subsystems/ranged_reload/rifle_hint_d32.h"
#include "vrik/visible_failure.h"
#include <cstdio>
#include <cstring>
int main(){unsigned n=0,f=0;auto ck=[&](bool y,const char* m){++n;if(!y){++f;std::printf("FAIL %s\n",m);}};
 namespace rh=kcd2vr::rifle_hint_d32;
 ck(rh::kDeadlineMs==4000,"silent native fallback after 4 s (was 12 s)");
 ck(rh::hintFor(true,true,false,false)==rh::Hint::Grip,"empty stage, not held: grip");
 ck(rh::hintFor(true,true,true,false)==rh::Hint::Pull,"empty stage, held: pull back");
 ck(rh::hintFor(true,false,false,true)==rh::Hint::Place,"working stage: grip again");
 ck(rh::hintFor(true,false,false,false)==rh::Hint::None,"loaded/other: no hint");
 for(int h=1;h<=3;++h){const char* t=rh::text(rh::Hint(h));ck(t&&kcd2vr::visible_failure::safe(t),"hint text is visible-line safe");}
 ck(rh::text(rh::Hint::None)==nullptr,"no text for none");
 rh::State s{};
 auto t=rh::step(s,rh::Hint::Grip,false,false);ck(t.show&&std::strstr(t.show,"grip at the barrel")&&!t.haptic,"first tick shows grip");
 t=rh::step(s,rh::Hint::Grip,false,false);ck(!t.show&&!t.haptic,"same hint not repeated");
 t=rh::step(s,rh::Hint::Pull,true,false);ck(t.show&&std::strstr(t.show,"pull back")&&t.haptic,"grip taken: pull hint + tick");
 t=rh::step(s,rh::Hint::Pull,true,false);ck(!t.show&&!t.haptic,"holding: quiet");
 t=rh::step(s,rh::Hint::Place,false,true);ck(t.show&&std::strstr(t.show,"grip again")&&t.haptic&&s.stagesDone==1,"stage advanced: place hint + tick");
 t=rh::step(s,rh::Hint::None,false,true);ck(!t.show&&t.haptic&&s.stagesDone==2,"loaded: final tick, no text");
 std::printf("rifle_hint_d32 %u checks, %u failures\n",n,f);return f?1:0;}
