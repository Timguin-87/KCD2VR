#pragma once
#include "performance_gate_d61.h"
#include "map_performance_d72.h"
#include <algorithm>
#include <array>
#include <cstdio>
namespace kcd2vr::diagnostics::perf_d61 {
inline constexpr unsigned NativeStages=18,XrStages=17,HudApply=NativeStages+XrStages,MapTick=HudApply+1,MapPlacement=HudApply+2,StageCount=HudApply+3;
inline constexpr const char* stageNames[StageCount]{
 "POSE_SOLVE","NATIVE_SKIN","PLACEMENT","NATIVE_FRAME","EQUIPMENT_READ","NATIVE_STATE","EQUIPMENT_LIVE","EQUIPMENT_META","NATIVE_BEGIN","NATIVE_SPECIAL","NATIVE_FIST","NATIVE_GUARD","NATIVE_WEAPON","NATIVE_RHYTHM","ACTION_PREPARE","ACTION_RELEASE","ACTION_CANCEL","NATIVE_ZONE",
 "XR_FRAME","XR_WAIT","XR_PREDICTED_POSE","XR_SYNC_POSE","XR_LATE_POSE","XR_INPUT","XR_SWAPCHAIN_WAIT","XR_HANDOFF","XR_BRIDGE","XR_FENCE","XR_END","XR_BEGIN","XR_ACTION_SYNC","XR_GRIP_LOCATE","XR_PICKUP","XR_COMPLETE","XR_ACQUIRE","HUD_APPLY","MAP_TICK","MAP_PLACEMENT"};
struct Aggregate {std::uint64_t session=0,calls=0,ns=0,maxNs=0;};
struct Counter {std::atomic_flag busy=ATOMIC_FLAG_INIT;Aggregate value{};};
inline std::array<Counter,StageCount> costs{};
struct Window {
 std::uint64_t session=0,frames=0,lastFrameNs=0,intervals=0,frameNs=0,frameMaxNs=0,over20=0;
 std::uint64_t poses=0,committed=0,ageSamples=0,ageNs=0,ageMaxNs=0,invalidAge=0;
};
inline Window window;inline std::atomic_flag windowBusy=ATOMIC_FLAG_INIT;
inline void frame(std::uint64_t session,std::uint64_t ns) noexcept {
 if(windowBusy.test_and_set(std::memory_order_acquire))return;
 if(window.session!=session)window={session};
 if(window.lastFrameNs&&ns>window.lastFrameNs){const auto dt=ns-window.lastFrameNs;++window.intervals;window.frameNs+=dt;window.frameMaxNs=std::max(window.frameMaxNs,dt);window.over20+=dt>20000000ull;}
 window.lastFrameNs=ns;++window.frames;windowBusy.clear(std::memory_order_release);
}
inline void cost(unsigned stage,std::uint64_t session,std::uint64_t ns) noexcept {
 if(stage>=StageCount)return;auto&c=costs[stage];if(c.busy.test_and_set(std::memory_order_acquire))return;
 if(c.value.session!=session)c.value={session};++c.value.calls;c.value.ns+=ns;c.value.maxNs=std::max(c.value.maxNs,ns);c.busy.clear(std::memory_order_release);
}
static_assert(StageCount==map_d72::SharedStages);
struct Scope {
 map_d72::Scope automaticMapD72;
 unsigned stage;std::uint64_t session=0,start=0;
 explicit Scope(unsigned s) noexcept:automaticMapD72(s),stage(s){
  if(s>=StageCount||!requested())return;
  const auto id=gate.serial.load(std::memory_order_acquire),ns=nowNs();
  if(!gate.current(id,ns/1000000ull))return;session=id;start=ns;
  if(s==3)frame(session,ns); // existing native-frame stage, not XR cadence
 }
 ~Scope(){if(!session||!requested()||session!=gate.serial.load(std::memory_order_acquire))return;
  const auto ns=nowNs();if(ns>=start&&gate.current(session,ns/1000000ull))cost(stage,session,ns-start);}
 Scope(const Scope&)=delete;Scope& operator=(const Scope&)=delete;
};
inline void poseAge(std::uint64_t sourceNs,bool committed) noexcept {
 if(!requested())return;const auto session=gate.serial.load(std::memory_order_acquire),ns=nowNs();
 if(!gate.current(session,ns/1000000ull)||windowBusy.test_and_set(std::memory_order_acquire))return;
 if(window.session!=session)window={session};++window.poses;window.committed+=committed;
 if(sourceNs&&ns>=sourceNs){const auto age=ns-sourceNs;++window.ageSamples;window.ageNs+=age;window.ageMaxNs=std::max(window.ageMaxNs,age);}else ++window.invalidAge;
 windowBusy.clear(std::memory_order_release);
}
struct External {std::uint64_t pairs=0,presents=0,xrFrames=0,fresh=0,reused=0,logLines=0;unsigned f2Rung=0,otherCaptureMask=0;};
struct Reporter {std::uint64_t session=0,last=0;External prior{};};
inline Reporter reporter;
template<class Writer> inline void control(std::uint64_t now,const char* reason,Writer&& write) noexcept {
 char s[256]{};std::snprintf(s,sizeof(s),"D61_PERF CONTROL session=%llu active=%d reason=%s durationMs=%llu intervalMs=%llu policyChange=0 heavyCaptureArmed=0",
  (unsigned long long)gate.serial.load(),gate.active(now)?1:0,reason,(unsigned long long)DurationMs,(unsigned long long)ReportMs);write(s);
}
template<class Writer> inline bool toggle(std::uint64_t now,Writer&& write) noexcept {
 const bool on=gate.select(!gate.active(now),now);reporter={};control(now,on?"SHIFT_F2_BEGIN":"SHIFT_F2_END",write);return on;
}
inline std::uint64_t delta(std::uint64_t a,std::uint64_t b) noexcept {return a>=b?a-b:0;}
inline void clearWindows(std::uint64_t session) noexcept {
 if(!windowBusy.test_and_set(std::memory_order_acquire)){window={session};windowBusy.clear(std::memory_order_release);}
 for(auto&c:costs)if(!c.busy.test_and_set(std::memory_order_acquire)){c.value={session};c.busy.clear(std::memory_order_release);}
}
// Mod worker only: at most TWO lines / 2s plus explicit finite begin/end receipts.
// Copies existing counters; no native reads, bone walks, query/fence or policy writes.
template<class Writer> inline void service(std::uint64_t now,const External& ext,Writer&& write) noexcept {
 if(gate.poll(now)){control(now,"EXPIRED",write);reporter={};return;}
 if(!gate.active(now))return;const auto session=gate.serial.load(std::memory_order_acquire);
 if(reporter.session!=session){reporter={session,now,ext};clearWindows(session);return;}
 if(now<reporter.last){reporter={session,now,ext};clearWindows(session);return;}
 if(now-reporter.last<ReportMs)return;
 const double sec=double(now-reporter.last)/1000.;const auto prior=reporter.prior;reporter={session,now,ext};
 Window copy{};if(!windowBusy.test_and_set(std::memory_order_acquire)){
  if(window.session==session)copy=window;const auto last=window.lastFrameNs;window={session};window.lastFrameNs=last;windowBusy.clear(std::memory_order_release);}
 std::array<Aggregate,StageCount> values{};
 for(unsigned i=0;i<StageCount;++i){auto&c=costs[i];if(c.busy.test_and_set(std::memory_order_acquire))continue;if(c.value.session==session)values[i]=c.value;c.value={session};c.busy.clear(std::memory_order_release);}
 if(!gate.current(session,now))return;
 const auto fresh=delta(ext.fresh,prior.fresh),reuse=delta(ext.reused,prior.reused);
 char line[1400]{};std::snprintf(line,sizeof(line),"D61_PERF SUMMARY session=%llu windowS=%.2f nativeFramesPerS=%.2f pairsPerS=%.2f presentsPerS=%.2f xrPerS=%.2f freshPerS=%.2f reusePct=%.2f frameAvgMs=%.3f frameMaxMs=%.3f over20ms=%llu poses=%llu committed=%llu poseSourceAgeAvgMs=%.3f poseSourceAgeMaxMs=%.3f invalidAge=%llu logLinesPerS=%.1f f2Policy=%u otherCaptureMask=%u source=CPU_COUNTERS_ONLY gpuMeasured=0",
 (unsigned long long)session,sec,double(copy.frames)/sec,double(delta(ext.pairs,prior.pairs))/sec,double(delta(ext.presents,prior.presents))/sec,double(delta(ext.xrFrames,prior.xrFrames))/sec,double(fresh)/sec,(fresh+reuse)?100.*double(reuse)/double(fresh+reuse):0.,copy.intervals?double(copy.frameNs)/double(copy.intervals)/1e6:-1.,double(copy.frameMaxNs)/1e6,(unsigned long long)copy.over20,(unsigned long long)copy.poses,(unsigned long long)copy.committed,copy.ageSamples?double(copy.ageNs)/double(copy.ageSamples)/1e6:-1.,double(copy.ageMaxNs)/1e6,(unsigned long long)copy.invalidAge,double(delta(ext.logLines,prior.logLines))/sec,ext.f2Rung,ext.otherCaptureMask);write(line);
 unsigned order[StageCount]{};for(unsigned i=0;i<StageCount;++i)order[i]=i;
 std::sort(order,order+StageCount,[&](unsigned a,unsigned b){return values[a].ns>values[b].ns;});
 int n=std::snprintf(line,sizeof(line),"D61_PERF STAGES session=%llu windowS=%.2f timing=INCLUSIVE_CPU_WALL nestedDoNotSum=1 top=6",(unsigned long long)session,sec);
 for(unsigned rank=0;rank<6&&values[order[rank]].calls;++rank){const unsigned i=order[rank];const auto&v=values[i];if(n>0&&n<int(sizeof(line))-160)n+=std::snprintf(line+n,sizeof(line)-n," %s={calls:%llu avgUs:%.2f maxUs:%.2f}",stageNames[i],(unsigned long long)v.calls,double(v.ns)/double(v.calls)/1000.,double(v.maxNs)/1000.);}
 write(line);
}
}
