#include <cstdint>
static std::uint64_t fakeNs=1000000000ull,clockCalls=0;
static std::uint64_t testClock(){++clockCalls;return fakeNs;}
#define KCD2VR_PERF_D61_TEST_CLOCK testClock
#include "diagnostics/performance_metrics_d61.h"
#include "diagnostics/capture168.h"
#include <cassert>
#include <string>
#include <vector>
#include <cstdio>
namespace p=kcd2vr::diagnostics::perf_d61;
namespace c=kcd2vr::diagnostics::capture168;
int main(){
 std::vector<std::string> lines;
 auto write=[&](const char*s){assert(p::accept(s,p::nowMs()));lines.emplace_back(s);};
 c::gate.select(c::Domain::Combat,true,1000);const auto originalMask=c::gate.mask.load();
 assert(!c::active(c::Domain::Bug,1000));
 p::gate.select(false,1000);const auto before=clockCalls;
 for(int i=0;i<1000;++i){p::Scope a(0);p::Scope b(p::NativeStages);p::poseAge(900000000ull,true);}
 assert(clockCalls==before);for(auto&x:p::costs)assert(x.value.calls==0);
 assert(p::window.frames==0&&p::window.poses==0);
 assert(p::toggle(1000,write));assert(lines.size()==1&&c::gate.mask.load()==originalMask);
 assert(!c::active(c::Domain::Bug,1000)&&!c::active(c::Domain::Hud,1000));
 assert(!p::accept("F2_CAUSAL session=1 data=heavy",1000));
 assert(!p::accept("VRIK_F7 session=1 bone=heavy",1000));
 assert(!p::accept("D61_PERF UNKNOWN session=1",1000));
 assert(!p::accept("D61_PERF SUMMARY session=0",1000));
 assert(!p::accept("D61_PERF SUMMARY session=1oops",1000));
 p::External ext{};ext.xrFrames=10;ext.pairs=10;ext.presents=10;ext.fresh=10;ext.f2Rung=3;ext.otherCaptureMask=originalMask;
 p::service(1000,ext,write);assert(lines.size()==1);
 for(unsigned i=0;i<100;++i){
  fakeNs=1000000000ull+std::uint64_t(i)*10000000ull;
  {p::Scope frame(3);fakeNs+=5000000ull;}
  p::poseAge(fakeNs-500000ull,true);++ext.pairs;++ext.presents;++ext.fresh;++ext.xrFrames;
  p::service(p::nowMs(),ext,write);assert(lines.size()==1);
 }
 fakeNs=3000000000ull;p::service(3000,ext,write);assert(lines.size()==3);
 assert(lines[1].find("nativeFramesPerS=50.00")!=std::string::npos);
 assert(lines[1].find("poseSourceAgeAvgMs=0.500")!=std::string::npos);
 assert(lines[1].find("f2Policy=3")!=std::string::npos);
 assert(lines[2].find("NATIVE_FRAME={calls:100 avgUs:5000.00")!=std::string::npos);
 for(unsigned i=0;i<1999;++i){fakeNs=3000000000ull+i*1000000ull;p::service(p::nowMs(),ext,write);}assert(lines.size()==3);
 fakeNs=5000000000ull;p::service(5000,ext,write);assert(lines.size()==5);
 // OFF ends just this lease. Previously armed Combat remains; no global override.
 assert(!p::toggle(5000,write));assert(lines.size()==6);assert(c::gate.mask.load()==originalMask);
 assert(!p::accept(lines[1].c_str(),5000));assert(p::accept(lines.back().c_str(),5000));assert(!p::accept(lines.back().c_str(),10001));
 const auto disabledClocks=clockCalls;for(int i=0;i<1000;++i){p::Scope a(3);p::poseAge(1,false);}assert(clockCalls==disabledClocks+1000); // automatic map baseline: one existing-frame timestamp, no stage timings
 assert(!kcd2vr::diagnostics::map_d72::active.load());
 // Expired/replaced sessions cannot publish the in-flight stage.
 fakeNs=6000000000ull;assert(p::toggle(6000,write));p::service(6000,ext,write);
 const auto activeSession=p::gate.serial.load();
 {p::Scope a(1);p::gate.select(false,6000);p::gate.select(true,6001);fakeNs=6002000000ull;}
 assert(p::costs[1].value.calls==0);assert(p::gate.serial.load()!=activeSession);
 p::service(6002,ext,write);p::poseAge(fakeNs+1,true);assert(p::window.invalidAge==1);
 const auto until=p::gate.until.load();fakeNs=until*1000000ull;
 const auto count=lines.size();p::service(until,ext,write);assert(lines.size()==count+1);assert(lines.back().find("EXPIRED")!=std::string::npos);
 p::service(until+1,ext,write);assert(lines.size()==count+1);assert(!p::requested());
 assert(c::gate.mask.load()==originalMask);assert(p::gate.receipt(p::gate.serial.load(),until+5000));assert(!p::gate.receipt(p::gate.serial.load(),until+5001));
 assert(!p::gate.select(true,UINT64_MAX));assert(!p::requested());
 std::puts("PASS D61 performance-only actual helper: 2000 OFF scopes + pose samples 0 clocks/0 counter writes; 2 lines/2s; no Bug/Hud bits; other capture preserved; stage/frame/pose-age aggregation; stop/expiry/replacement and family whitelist");
}
