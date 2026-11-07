// W4: one squeeze -> first missing causal stage.
#include "vrik/craft_attempt_w4.h"
#include <cstdio>
#include <cstring>
namespace ca=kcd2vr::craft_attempt_w4;
static unsigned total=0,failed=0;static void check(bool b,const char* m){++total;if(!b)++failed;std::printf("%s %s\n",b?"PASS":"FAIL",m);}
int main(){
 ca::Tracker t;ca::Report r{};ca::Observation o{};
 o.grip=1;o.sample=true;check(!t.step(0,o,1000,r),"squeeze starts an attempt, nothing reported yet");
 o.grip=0;check(t.step(0,o,1100,r)&&r.firstMissing==ca::Stage::Nomination&&r.id==1,"no item near: NOMINATION missing");
 char why[112];ca::why(r,"alchemy","-",why,sizeof(why));check(std::strstr(why,"no item within reach")!=nullptr,"why text names the missing reach");
 o={};o.grip=1;o.sample=true;o.nominated=true;o.nominatedEntity=952233;o.nominatedSlot=18;o.nativeKnown=true;o.nativeEntity=952224;o.nativeSlot=6;
 (void)t.step(0,o,2000,r);o.grip=.2f;check(t.step(0,o,2100,r)&&r.firstMissing==ca::Stage::NativeSelection&&r.nativeEntity==952224,"25 Oct case: hand 952233, game 952224 -> NATIVE_SELECTION");
 ca::why(r,"alchemy","OUT_OF_REACH",why,sizeof(why));check(std::strstr(why,"game picked 952224")&&std::strlen(why)<=96,"why line short and names both objects");
 o.grip=1;o.nativeEntity=952233;o.nativeSlot=18;(void)t.step(1,o,3000,r);o.dispatched=true;(void)t.step(1,o,3010,r);
 o.dispatched=false;o.action=true;o.attached=true;o.work=true;(void)t.step(1,o,3100,r);o.grip=0;
 check(t.step(1,o,3500,r)&&r.firstMissing==ca::Stage::Count&&t.complete==1,"full chain incl. RELEASE -> complete");
 o={};o.grip=1;o.sample=true;o.action=true;o.attached=true;(void)t.step(0,o,4000,r);o.grip=0;
 check(t.step(0,o,4050,r)&&r.firstMissing==ca::Stage::Nomination&&!(r.reached&(1u<<unsigned(ca::Stage::Attach))),"stray native attach without our dispatch is not counted");
 o={};o.grip=1;o.sample=true;(void)t.step(0,o,5000,r);check(t.step(0,o,9100,r)&&r.timedOut,"held squeeze reports after 4 s");
 check(!t.step(0,o,9200,r),"still held after timeout: no new attempt until released");
 o.grip=0;(void)t.step(0,o,9300,r);o.grip=1;check(!t.step(0,o,9400,r)&&t.live[0].active,"release then squeeze: new attempt");
 std::printf("craft_attempt_w4 %u/%u\n",total-failed,total);return failed?1:0;}
