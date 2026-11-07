#include "upscaler_live_switch_d85.h"
#include <cstdio>
#include <cwchar>
#include <string>
#include <vector>
using namespace kcd2vr::upscaler_live_switch_d85;
int checks=0,failures=0;
#define CHECK(x) do{++checks;if(!(x)){++failures;std::fprintf(stderr,"FAIL %d: %s\n",__LINE__,#x);}}while(0)
struct Harness{
 Switch sw;bool busy=true,writable=true,ready=true,wanted=true;int stops=0,writes=0;
 Publication publication{100,7,7,0};std::vector<std::string>logs;Selection applied{};
 void Step(std::uint64_t now){sw.Step(now,wanted,[&]{return busy;},[&]{++stops;},[&](Selection s){++writes;applied=s;return writable;},[&](int){return ready;},[&]{return publication;},[&](const char*s){logs.push_back(s);});}
 void Request(int tech=2){sw.Request({tech,1,0});CHECK(sw.HoldsStereo());Step(100);CHECK(sw.CurrentPhase()==Phase::Stopping);CHECK(stops==1&&writes==0);busy=false;Step(200);CHECK(writes==1&&sw.CurrentPhase()==Phase::Waiting);}
 void Start(){Step(949);CHECK(sw.HoldsStereo());Step(950);CHECK(sw.CurrentPhase()==Phase::StartingStereo);CHECK(!sw.HoldsStereo());CHECK(logs.empty());}
};
int main(){
 for(int tech:{0,1,2,4}){
  Harness h;h.Request(tech);h.Start();CHECK(h.applied.tech==tech);h.Step(1000);CHECK(h.sw.CurrentPhase()==Phase::StartingStereo);
  // SDK contexts, read-back and old-epoch publications do not prove new output.
  h.publication={101,7,7,tech};h.Step(1100);CHECK(h.sw.CurrentPhase()==Phase::StartingStereo);
  h.publication={102,8,8,tech==1?2:1};h.Step(1200);CHECK(h.sw.CurrentPhase()==Phase::StartingStereo);
  h.publication={103,8,9,tech};h.Step(1300);CHECK(h.sw.CurrentPhase()==Phase::StartingStereo);
  h.publication={104,9,9,tech};h.Step(1400);CHECK(h.sw.CurrentPhase()==Phase::Done);CHECK(!h.sw.HoldsStereo());
  CHECK(h.logs.size()==1&&h.logs.back().find("STEREO_PUBLICATION_CONFIRMED")!=std::string::npos);
 }
 Harness timeout;timeout.Request();timeout.Start();timeout.Step(8950);CHECK(timeout.sw.CurrentPhase()==Phase::Failed);
 CHECK(timeout.sw.LastWhy()==Why::OutputNotConfirmed);CHECK(!timeout.sw.HoldsStereo());
 CHECK(std::wcsstr(timeout.sw.MenuNote(),L"UNCONFIRMED")!=nullptr);CHECK(timeout.logs.back().find("setting-applied-stereo-output-unconfirmed")!=std::string::npos);
 Harness noContext;noContext.ready=false;noContext.Request();noContext.Step(8200);CHECK(noContext.sw.LastWhy()==Why::PairNotCreated);CHECK(!noContext.sw.HoldsStereo());
 Harness refused;refused.writable=false;refused.sw.Request({2,1,0});refused.Step(100);refused.busy=false;refused.Step(200);CHECK(refused.sw.LastWhy()==Why::WriteRefused);CHECK(refused.writes==1);CHECK(!refused.sw.HoldsStereo());
 Harness stop;stop.sw.Request({2,1,0});stop.Step(100);stop.Step(5100);CHECK(stop.sw.LastWhy()==Why::StopTimeout);CHECK(stop.stops==1&&stop.writes==0);
 Harness mono;mono.wanted=false;mono.busy=false;mono.sw.Request({1,1,0});mono.Step(100);CHECK(mono.sw.CurrentPhase()==Phase::Done);CHECK(mono.logs.back().find("SETTING_APPLIED")!=std::string::npos);CHECK(std::wcscmp(mono.sw.MenuNote(),L"SETTING APPLIED")==0);
 std::printf("Live upscaler production switch checks=%d failures=%d\n",checks,failures);return failures?1:0;
}
