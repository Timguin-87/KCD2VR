// Actual pending-startup collector, graphics receipt producer and final capture filter.
#include "../src/diagnostics/startup_batch32.h"
#include "../src/diagnostics/graphics_receipts32.h"
#include "../src/diagnostics/capture168.h"
#include <cassert>
#include <cstdio>
#include <string>
#include <vector>
int main(){
 using namespace kcd2vr;unsigned checks=0;auto ck=[&](bool b){++checks;assert(b);};
 std::vector<std::string> lines;auto sink=[&](const char* s){lines.emplace_back(s);};
 diagnostics::StartupBatch32<2,96> batch;
 ck(batch.Add("C32 PROXY_STARTUP ASI result=LOADED name=a.asi"));
 ck(batch.Add("C32 PROXY_STARTUP ASI result=FAILED name=b.asi"));
 ck(!batch.Add("C32 PROXY_STARTUP ASI result=LOADED name=c.asi")&&batch.dropped==1);
 ck(!batch.Flush(false,sink)&&lines.empty()&&!batch.flushed);
 ck(batch.Flush(true,sink)&&lines.size()==3&&batch.flushed);
 ck(lines[0].find("name=a.asi")!=std::string::npos&&lines[1].find("name=b.asi")!=std::string::npos);
 ck(lines[2].find("omitted=1")!=std::string::npos);ck(!batch.Flush(true,sink)&&lines.size()==3);
 diagnostics::StartupBatch32<1,8> bounded;ck(bounded.Add("long_message"));ck(bounded.records[0][7]==0);
 namespace cap=diagnostics::capture168;cap::gate.mask=0;
 for(const auto& s:lines)ck(cap::essential(s.c_str())&&cap::accept(s.c_str(),100));
 lines.clear();diagnostics::GraphicsReceipts32 report;graphics_owner31::Shadows sh;graphics_owner31::Eye eye;
 report.Observe(false,true,false,sh,eye,sink);ck(lines.size()==1&&cap::essential(lines[0].c_str()));
 for(unsigned i=0;i<10000;++i)report.Observe(false,true,false,sh,eye,sink);
 ck(lines.size()==1);report.Observe(false,true,true,sh,eye,sink);ck(lines.size()==2&&!cap::essential(lines.back().c_str()));
 const auto state=lines.back();ck(cap::domain(state.c_str())==cap::Domain::Hud&&!cap::accept(state.c_str(),100));
 cap::gate.select(cap::Domain::Combat,true,100);ck(!cap::accept(state.c_str(),100));
 cap::gate.select(cap::Domain::Hud,true,100);ck(cap::accept(state.c_str(),100));
 ck(cap::domain("C26 EYE_ADAPT level=NATIVE")==cap::Domain::Hud);
 ck(cap::domain("C26 EYE_ADAPT level=PAUSED_HEAD_TURN")==cap::Domain::Hud);
 sh.pending=true;sh.retry=500;report.Observe(false,true,false,sh,eye,sink);
 ck(lines.back().find("C32 GRAPHICS_FAULT ")==0&&cap::essential(lines.back().c_str()));
 const auto n=lines.size();for(unsigned i=0;i<10000;++i)report.Observe(false,true,false,sh,eye,sink);ck(lines.size()==n);
 sh.pending=false;sh.retry=0;report.Observe(false,true,false,sh,eye,sink);
 ck(lines.back().find("C32 GRAPHICS_RECOVERED ")==0);report.Observe(true,true,false,sh,eye,sink);
 ck(lines.back().find("C32 GRAPHICS_OPTION ")==0);
 sh.gaveUp=true;report.Observe(true,true,false,sh,eye,sink);
 ck(lines.back().find("C32 GRAPHICS_FAULT ")==0&&report.faults==16);
 const auto gaveUpLines=lines.size();
 for(unsigned i=0;i<10000;++i)report.Observe(true,true,false,sh,eye,sink);
 ck(lines.size()==gaveUpLines); // stopped pending retries must not report recovered
 eye.gaveUp=true;report.Observe(true,true,false,sh,eye,sink);
 ck(lines.back().find("C32 GRAPHICS_FAULT ")==0&&report.faults==48);
 sh.gaveUp=eye.gaveUp=false;report.Observe(false,false,false,sh,eye,sink);
 ck(lines.back().find("C32 GRAPHICS_RECOVERED ")==0&&report.faults==0);
 report.Observe(1,false,false,sh,eye,sink); // C49 shadow quality edge names the level and the sun-map target
 ck(lines.back().find("C32 GRAPHICS_OPTION shadows=1 shadowQuality=MEDIUM sunResTarget=2048 ")==0&&cap::essential(lines.back().c_str()));
 report.Observe(2,false,false,sh,eye,sink);ck(lines.back().find("shadowQuality=HIGH sunResTarget=4096")!=std::string::npos);
 report.Observe(0,false,false,sh,eye,sink);ck(lines.back().find("shadows=0 shadowQuality=LOW sunResTarget=0")!=std::string::npos);
 {const auto k=lines.size();report.Observe(0,false,false,sh,eye,sink);ck(lines.size()==k);}
 diagnostics::StartupRetry32 retry;unsigned enables=0,flushes=0;bool ready=false;
 auto isReady=[&]{return ready;};auto enable=[&]{++enables;};auto flush=[&]{++flushes;};
 retry.Tick(100,true,isReady,enable,flush);ck(enables==1&&flushes==0);
 for(unsigned tick32=101;tick32<600;++tick32)retry.Tick(tick32,true,isReady,enable,flush);ck(enables==1);
 retry.Tick(600,true,isReady,enable,flush);ck(enables==2);ready=true;
 retry.Tick(1100,true,isReady,enable,flush);ck(enables==2&&flushes==1&&retry.done);
 retry.Tick(1600,true,isReady,enable,flush);ck(enables==2&&flushes==1);
 diagnostics::StartupRetry32 exhausted;ready=false;enables=flushes=0;
 for(unsigned tick32=1;tick32<5000;++tick32)exhausted.Tick(tick32,true,isReady,enable,flush);
 ck(enables==3&&flushes==0&&exhausted.done);
 std::printf("startup_graphics32: %u actual collector/producer/final-filter checks PASS; no native API used\n",checks);
}
