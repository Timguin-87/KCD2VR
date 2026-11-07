#include <cstdint>
static std::uint64_t clockNs=1000000000ull;
static unsigned clockCalls=0;
static std::uint64_t testClock(){++clockCalls;return clockNs;}
#define KCD2VR_PERF_D61_TEST_CLOCK testClock
static unsigned captureClockCalls=0;
static std::uint64_t captureClock();
static bool restartInCaptureClock=false;
#define KCD2VR_MAP_D84_CAPTURE_CLOCK captureClock
#include "../src/diagnostics/performance_metrics_d61.h"
#include <cassert>
#include <cstdio>
#include <string>
#include <vector>
namespace m=kcd2vr::diagnostics::map_d72;
namespace p=kcd2vr::diagnostics::perf_d61;
namespace c=kcd2vr::diagnostics::capture168;
static std::uint64_t captureClock(){++captureClockCalls;const auto tick=clockNs/1000000ull+9000000ull;
 if(restartInCaptureClock){restartInCaptureClock=false;c::gate.select(c::Domain::Bug,false,tick);c::gate.select(c::Domain::Bug,true,tick);}return tick;}

int main(){
 std::vector<std::string> lines;
 auto write=[&](const char* line){assert(m::accept(line,p::nowMs()));lines.emplace_back(line);};
 m::Map map{};m::External ext{};ext.f2Rung=2;ext.otherCaptureMask=4;
 auto service=[&](){m::service(p::nowMs(),map,ext,p::stageNames,write);};
 service();assert(lines.empty()&&!m::active.load()&&!p::requested());
 clockCalls=0;
 for(unsigned i=0;i<10000;++i){p::Scope frame{3};m::Scope leaf{m::Transform};m::nativeFrame(clockNs);}
 assert(clockCalls==0&&m::baseline.count==0); // both OFF: no timing or hidden baseline
 map.open=true;map.generation=1;service();assert(lines.empty()&&!m::active.load());
 map.open=false;c::gate.select(c::Domain::Bug,true,captureClock());service();
 for(unsigned i=0;i<130;++i){p::Scope frame{3};clockNs+=10000000ull;}
 assert(m::baseline.count==128);
 map.open=true;map.generation=1;service();assert(lines.size()==1&&m::active.load()&&!p::requested());
 assert(m::reporter.baselineSamples==128&&m::reporter.baselineMs==10.);
 const auto first=m::session.load();assert(m::current(first,p::nowMs()));
 assert(lines.back().find("policyChange=0")!=std::string::npos);
 for(unsigned i=0;i<100;++i){p::Scope frame{3};{p::Scope place{p::MapPlacement};{m::Scope leaf{m::Transform};clockNs+=1000000ull;}}clockNs+=19000000ull;}
 map.visible=true;map.placements=100;ext.pairs=100;ext.fresh=100;ext.reused=20;ext.xrFrames=120;
 service();assert(lines.size()==4&&lines[1].find("frameMs=20.000")!=std::string::npos);
 assert(lines[3].find("MAP_NATIVE_TRANSFORM={calls:100")!=std::string::npos);
 assert(lines[3].find("frameRegression=1")!=std::string::npos);
 assert(!p::requested()&&ext.f2Rung==2&&ext.otherCaptureMask==4);
 // A scope from a replaced map occurrence must never join its replacement.
 const auto oldCount=m::costs[m::Transform].value.calls;
 {m::Scope old{m::Transform};map.generation=2;clockNs+=1000000ull;service();assert(m::session.load()!=first);clockNs+=1000000ull;}
 assert(m::costs[m::Transform].value.calls==0&&oldCount==0&&m::reporter.baselineSamples==0);
 assert(!m::accept(lines.front().c_str(),p::nowMs()));
 map.open=false;map.visible=false;map.hides=1;service();assert(m::active.load());
 clockNs+=1999000000ull;service();assert(m::active.load());
 clockNs+=1000000ull;service();assert(!m::active.load());
 assert(lines.back().find("reason=CLOSED")!=std::string::npos);
 const auto closedLine=lines.back();const auto count=lines.size();service();assert(lines.size()==count);
 clockNs+=5001000000ull;assert(!m::accept(closedLine.c_str(),p::nowMs()));
 // The baseline after close must not include the entire open-map gap.
 {p::Scope f{3};}clockNs+=11000000ull;{p::Scope f{3};}
 assert(m::baseline.count==1&&m::baseline.dt[0]==11000000ull);
 map.open=true;map.generation=4;service();assert(m::active.load()&&m::reporter.baselineMs==11.);
 clockNs+=m::DurationMs*1000000ull;service();assert(!m::active.load());
 assert(lines.back().find("reason=FINITE_LIMIT")!=std::string::npos);
 const auto expired=m::session.load();clockNs+=10000000ull;service();assert(m::session.load()==expired&&!m::active.load());
 // An already open map does not continually rearm the finite capture.
 map.open=false;service();map.open=true;map.generation=6;service();assert(m::active.load()&&m::session.load()!=expired);
 assert(!m::accept("D61_PERF SUMMARY session=4",p::nowMs()));
 assert(!m::accept("D72_MAP_PERF WINDOW session=0",p::nowMs()));
 // Restart between producer entry and completion must not borrow the new F2 lease.
 const auto prior=m::session.load();const auto priorLine=lines.back();
 {m::Scope old{m::Transform};c::gate.select(c::Domain::Bug,false,captureClock());
  c::gate.select(c::Domain::Bug,true,captureClock());clockNs+=1000000ull;}
 assert(!m::current(prior,p::nowMs())&&!m::accept(priorLine.c_str(),p::nowMs()));
 service();assert(m::session.load()!=prior&&m::reporter.baselineSamples==0&&m::costs[m::Transform].value.calls==0);
 c::gate.select(c::Domain::Bug,false,captureClock());service();
 const auto stopped=lines.size();clockCalls=0;for(unsigned i=0;i<10000;++i){p::Scope f{3};m::Scope leaf{m::Transform};}
 assert(clockCalls==0&&lines.size()==stopped&&m::baseline.count==0);
 // Shift+F2 owns a finite independent route, including an already open map.
 p::gate.select(true,p::nowMs());service();assert(m::active.load()&&lines.size()==stopped+1);
 const auto perfMap=m::session.load();p::gate.select(false,p::nowMs());
 assert(!m::current(perfMap,p::nowMs()));service();assert(!m::active.load());
 // A closed-map baseline cannot span an OFF gap or capture restart.
 map.open=false;c::gate.select(c::Domain::Bug,true,captureClock());service();
 {p::Scope f{3};}clockNs+=10000000ull;{p::Scope f{3};}assert(m::baseline.count==1);
 c::gate.select(c::Domain::Bug,false,captureClock());clockNs+=1000000000ull;
 c::gate.select(c::Domain::Bug,true,captureClock());{p::Scope f{3};}assert(m::baseline.count==0);
 map.open=true;service();assert(m::reporter.baselineSamples==0);
 const auto beforeExpiry=lines.size();clockNs+=c::Duration*1000000ull;service();
 assert(!m::active.load()&&lines.size()==beforeExpiry&&!m::recordingD84(p::nowMs()));
 c::gate.poll(captureClock());clockCalls=0;{p::Scope f{3};}assert(clockCalls==0);
 unsigned samples=0;auto sample=[&](m::Map& v,m::External&){++samples;v.open=true;v.generation=123;};
 clockCalls=captureClockCalls=0;
 for(unsigned i=0;i<10000;++i)m::workerD84(p::stageNames,sample,write);
 assert(samples==0&&clockCalls==0&&captureClockCalls==0);
 c::gate.select(c::Domain::Bug,true,captureClock());m::workerD84(p::stageNames,sample,write);assert(samples==1);
 {m::Scope pending{m::Transform};c::gate.select(c::Domain::Bug,false,captureClock());clockCalls=captureClockCalls=0;}
 assert(clockCalls==0&&captureClockCalls==0); // OFF mid-scope does not time its destructor
 m::workerD84(p::stageNames,sample,write);assert(samples==1&&clockCalls==0&&captureClockCalls==0&&!m::active.load());
 c::gate.select(c::Domain::Bug,true,captureClock());m::workerD84(p::stageNames,sample,write);
 const auto beforeRace=m::session.load();restartInCaptureClock=true;
 assert(!m::current(beforeRace,p::nowMs())); // restart occurs between serial read and active qualification
 restartInCaptureClock=true;assert(m::recorderD84(p::nowMs()).bug==0); // no mixed-generation token
 c::gate.select(c::Domain::Bug,false,captureClock());m::workerD84(p::stageNames,sample,write);
 std::puts("PASS D84 map profile: zero OFF clock/baseline/format work; F2 or Shift+F2; real scopes; finite lease; recorder/map replacement; no stale baseline or inherited receipt");
}
