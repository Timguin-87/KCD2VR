// R8b crafting recorder: START/SAMPLE/STOP per visit, 100 ms cadence, budgets, edges/speed/tilt.
#include "vrik/craft_recorder_r8b.h"
#include <cstdio>
#include <cstring>
namespace r=kcd2vr::craft_recorder_r8b;
static unsigned total=0,failed=0;static void check(bool v,const char* m){++total;if(!v)++failed;std::printf("%s %s\n",v?"PASS":"FAIL",m);}
int main(){
 r::Recorder rec;char line[2048]{};r::HandOut o[2]{};r::Sample s{};s.kind="alchemy";s.mode="FREE";s.instance=0x1C5EFA119A0;s.stage="UseBellows";
 check(!rec.sample(s,1000,line,sizeof(line),o),"no sample before START");
 check(rec.begin("alchemy",s.instance,"FREE",1000,line,sizeof(line))&&std::strstr(line,"START")&&std::strstr(line,"smithDepthFlipInspect=NOT_READ"),"START names the sources and what is not read");
 check(!rec.begin("alchemy",s.instance,"FREE",1001,line,sizeof(line)),"same station: one START per visit");
 s.hand[0].valid=true;s.hand[0].grip=1;s.hand[0].rot[3]=1;
 check(rec.sample(s,1000,line,sizeof(line),o)&&o[0].gripDown&&o[0].tiltDeg<1.f,"first sample: grip edge, level hand");
 check(!rec.sample(s,1050,line,sizeof(line),o),"50 ms later: not due");
 s.hand[0].pos[0]=.1f;s.hand[0].rot[0]=.70710678f;s.hand[0].rot[3]=.70710678f; // 90 deg roll about X
 check(rec.sample(s,1100,line,sizeof(line),o)&&o[0].speed>.99f&&o[0].speed<1.01f,"10 cm in 100 ms = 1 m/s");
 check(o[0].tiltDeg>89.f&&o[0].tiltDeg<91.f,"hand on its side: tilt 90");
 s.hand[0].grip=.5f;check(rec.sample(s,1200,line,sizeof(line),o)&&!o[0].gripUp,"hysteresis: 0.5 keeps the grip");
 s.hand[0].grip=.3f;check(rec.sample(s,1300,line,sizeof(line),o)&&o[0].gripUp,"grip release edge");
 s.stage="PlaceIngredient";s.admitted=true;(void)rec.sample(s,1400,line,sizeof(line),o);
 check(std::strstr(line,"stage=PlaceIngredient")&&std::strstr(line,"admitted=1"),"sample carries native stage and admission");
 check(rec.stop(1500,"LEFT_STATION",line,sizeof(line))&&std::strstr(line,"samples=5")&&std::strstr(line,"gripDowns=1,0")&&
  std::strstr(line,"admissions=1")&&std::strstr(line,"stageChanges=1"),"STOP counters");
 check(!rec.stop(1600,"x",line,sizeof(line)),"one STOP per visit");
 // budget
 r::Recorder b;b.begin("smithing",7,"FREE",0,nullptr,0);r::Sample t{};t.instance=7;unsigned wrote=0;
 for(unsigned i=0;i<r::MaxSamples+10;++i)wrote+=b.sample(t,1+std::uint64_t(i)*100,line,sizeof(line),o);
 check(wrote==r::MaxSamples&&b.dropped==10,"visit budget: 3000 then counted drops");
 check(!b.sample(r::Sample{},999999,line,sizeof(line),o),"other instance: no sample");
 std::printf("craft_recorder_r8b %u/%u\n",total-failed,total);return failed?1:0;}
