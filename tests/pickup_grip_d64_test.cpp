#include "../src/core/native_grasp13.h"
#include "../src/diagnostics/capture168.h"
#include <cstdio>
int main(){int fails=0,n=0;auto ck=[&](bool b,const char* s){++n;if(!b)++fails;std::printf("%s %s\n",b?"PASS":"FAIL",s);};
 using namespace kcd2vr;pickup_grip_d64::Retirement state;grasp13::AcquisitionInput55 in{};
 in.character=10;in.epoch=3;in.trackingEpoch=4;in.sample=100;in.tick=1000;in.tracked[0]=in.tracked[1]=true;in.grip[0]=in.grip[1]=1;
 ck(!state.blocked(0,10,3),"ordinary held Grip without a pickup terminal remains available");
 ck(state.consume(10,3,17,0,in)&&state.blocked(0,10,3),"exact chest-stow terminal consumes its right Grip");
 ck(!state.blocked(1,10,3)&&!state.blocked(0,11,3)&&!state.blocked(0,10,4),"other hand character and body are unaffected");
 in.sample++;in.tick+=10;state.observe(in);ck(state.blocked(0,10,3),"continued squeeze cannot become holster Draw");
 in.grip[1]=0;state.observe(in);ck(state.blocked(0,10,3),"opening the opposite hand cannot retire pickup squeeze");
 in.tracked[0]=false;in.grip[0]=0;state.observe(in);ck(state.blocked(0,10,3),"tracking loss is not a physical opening");
 in.tracked[0]=true;state.observe(in);ck(!state.blocked(0,10,3),"observed physical opening immediately permits future holster acquisition");
 in.sample++;in.tick++;in.grip[0]=1;state.observe(in);ck(!state.blocked(0,10,3),"new squeeze needs no cooldown or fresh zone entry");
 ck(state.consume(10,3,18,0,in)&&state.blocked(0,10,3),"new terminal cannot reuse an older release");
 auto old=in;old.sample--;old.tick--;old.grip[0]=0;state.observe(old);ck(state.blocked(0,10,3),"out-of-date raw release cannot retire new terminal");
 ck(!state.consume(10,3,19,1,old),"open hand cannot publish consumed Grip");
 in.sample++;in.tick++;in.grip[0]=0;state.observe(in);state.observe(old);
 ck(!state.blocked(0,10,3),"late old source cannot undo a later observed opening");
 namespace cap=diagnostics::capture168;
 for(const auto* s:{"V66.120 HANDLING key=1","V66.124 WEAPON_ACQUISITION pose=1","R59.16C10 SUPPORT_LATCH reason=0","C39 SUPPORT_GRIP gap=0"})ck(cap::domain(s)==cap::Domain::Combat,"existing support producer routes to F3");
 std::printf("D64 pickup routing %d/%d\n",n-fails,n);return fails?1:0;}
