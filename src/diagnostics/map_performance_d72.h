#pragma once
// D84: finite map measurements require F2 or Shift+F2. OFF performs no
// timing, baseline collection, formatting or I/O; gameplay is independent.
#include "performance_gate_d61.h"
#include "capture168.h"
#include <algorithm>
#include <array>
#include <cstdio>
#if defined(_WIN32)
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

namespace kcd2vr::diagnostics::map_d72 {
inline constexpr unsigned SharedStages=38,Find=38,Model=39,Transform=40,Visibility=41,StageCount=42;
inline constexpr std::uint64_t DurationMs=120000,ReportMs=2000,AfterCloseMs=2000;
struct Aggregate {std::uint64_t session=0,calls=0,ns=0,maxNs=0;};
struct Counter {std::atomic_flag busy=ATOMIC_FLAG_INIT;Aggregate value{};};
inline std::array<Counter,StageCount> costs{};
inline std::atomic<std::uint64_t> session{0},until{0},receiptUntil{0},dropped{0};
inline std::atomic<bool> active{false},mapOpen{false};
struct Frames {std::uint64_t session=0,count=0,last=0,intervals=0,ns=0,maxNs=0;};
inline Frames frames;
inline std::atomic_flag framesBusy=ATOMIC_FLAG_INIT;
struct RecorderD84 {std::uint64_t bug=0,perf=0;};
inline std::atomic<std::uint64_t> bugLeaseD84{0},perfLeaseD84{0};
struct Baseline {std::array<std::uint64_t,128> dt{};unsigned cursor=0,count=0;std::uint64_t last=0;RecorderD84 recorder{};};
inline Baseline baseline;
inline std::atomic_flag baselineBusy=ATOMIC_FLAG_INIT;
inline bool selectedD84() noexcept {
 return (capture168::gate.mask.load(std::memory_order_relaxed)&capture168::members(capture168::Domain::Bug))!=0||perf_d61::requested();
}
inline std::uint64_t captureTickD84() noexcept {
#ifdef KCD2VR_MAP_D84_CAPTURE_CLOCK
 return KCD2VR_MAP_D84_CAPTURE_CLOCK();
#elif defined(_WIN32)
 return std::uint64_t(GetTickCount64()); // same clock as F2 select/poll
#else
 return perf_d61::nowMs(); // portable host; tests inject a distinct capture clock
#endif
}
inline bool bugActiveD84() noexcept {
 return (capture168::gate.mask.load(std::memory_order_relaxed)&capture168::members(capture168::Domain::Bug))&&
        capture168::active(capture168::Domain::Bug,captureTickD84());
}
inline bool recordingD84(std::uint64_t now) noexcept {
 return bugActiveD84()||perf_d61::gate.active(now);
}
inline RecorderD84 recorderD84(std::uint64_t now) noexcept {
 RecorderD84 r;
 const auto bug=capture168::gate.session[unsigned(capture168::Domain::Bug)].load(std::memory_order_acquire);
 if(bug&&bugActiveD84()&&bug==capture168::gate.session[unsigned(capture168::Domain::Bug)].load(std::memory_order_acquire))r.bug=bug;
 const auto perf=perf_d61::gate.serial.load(std::memory_order_acquire);
 if(perf&&perf_d61::gate.current(perf,now)&&perf==perf_d61::gate.serial.load(std::memory_order_acquire))r.perf=perf;
 return r;
}
inline bool currentRecorderD84(RecorderD84 r,std::uint64_t now) noexcept {
 return (r.bug&&r.bug==capture168::gate.session[unsigned(capture168::Domain::Bug)].load(std::memory_order_acquire)&&bugActiveD84()&&
              r.bug==capture168::gate.session[unsigned(capture168::Domain::Bug)].load(std::memory_order_acquire))||
        (r.perf&&perf_d61::gate.current(r.perf,now)&&r.perf==perf_d61::gate.serial.load(std::memory_order_acquire));
}
inline bool currentRecorderD84(std::uint64_t now) noexcept {
 return currentRecorderD84({bugLeaseD84.load(std::memory_order_acquire),perfLeaseD84.load(std::memory_order_acquire)},now);
}
inline bool current(std::uint64_t id,std::uint64_t now) noexcept {
 return id&&currentRecorderD84(now)&&active.load(std::memory_order_acquire)&&id==session.load(std::memory_order_acquire)&&now<until.load(std::memory_order_acquire);
}
inline void nativeFrame(std::uint64_t ns) noexcept {
 if(!selectedD84()||!recordingD84(ns/1000000ull))return;
 if(active.load(std::memory_order_relaxed)){
  const auto id=session.load(std::memory_order_acquire);
  if(!current(id,ns/1000000ull))return;
  if(framesBusy.test_and_set(std::memory_order_acquire)){++dropped;return;}
  if(frames.session!=id)frames={id};
  if(frames.last&&ns>frames.last){const auto dt=ns-frames.last;++frames.intervals;frames.ns+=dt;frames.maxNs=std::max(frames.maxNs,dt);}
  frames.last=ns;++frames.count;framesBusy.clear(std::memory_order_release);
 }else if(!mapOpen.load(std::memory_order_relaxed)){
  if(baselineBusy.test_and_set(std::memory_order_acquire))return;
  const auto recorder=recorderD84(ns/1000000ull);
  if(baseline.recorder.bug!=recorder.bug||baseline.recorder.perf!=recorder.perf){baseline={};baseline.recorder=recorder;}
  if(baseline.last&&ns>baseline.last){baseline.dt[baseline.cursor++%baseline.dt.size()]=ns-baseline.last;baseline.count=std::min(unsigned(baseline.dt.size()),baseline.count+1);}
  baseline.last=ns;baselineBusy.clear(std::memory_order_release);
 }
}
inline void cost(unsigned stage,std::uint64_t id,std::uint64_t ns) noexcept {
 if(stage>=StageCount)return;auto& c=costs[stage];
 if(c.busy.test_and_set(std::memory_order_acquire)){++dropped;return;}
 if(c.value.session!=id)c.value={id};++c.value.calls;c.value.ns+=ns;c.value.maxNs=std::max(c.value.maxNs,ns);c.busy.clear(std::memory_order_release);
}
struct Scope {
 unsigned stage;std::uint64_t id=0,start=0;
 explicit Scope(unsigned s) noexcept:stage(s){
  if(!selectedD84())return; // no clock call while both recorders are OFF
  // Only the existing native frame entry takes a baseline timestamp while idle.
  if(s==3||active.load(std::memory_order_relaxed)){
   const auto ns=perf_d61::nowNs();if(s==3)nativeFrame(ns);
   const auto candidate=session.load(std::memory_order_acquire);
   if(s<StageCount&&current(candidate,ns/1000000ull)){id=candidate;start=ns;}
  }
 }
 ~Scope(){if(!id||!selectedD84())return;const auto ns=perf_d61::nowNs();if(ns>=start&&current(id,ns/1000000ull))cost(stage,id,ns-start);}
 Scope(const Scope&)=delete;Scope& operator=(const Scope&)=delete;
};
struct Map {bool open=false,visible=false;std::uint64_t generation=0,placements=0,shows=0,hides=0,missing=0,stale=0;unsigned refusal=0,hideReason=0;};
struct External {std::uint64_t pairs=0,presents=0,xrFrames=0,fresh=0,reused=0,logLines=0;unsigned f2Rung=0,otherCaptureMask=0;};
struct Reporter {
 bool previousOpen=false;std::uint64_t generation=0,id=0,began=0,last=0,closeAt=0;
 double baselineMs=0;unsigned baselineSamples=0;External prior{};Map previousMap{};
};
inline Reporter reporter;
inline std::uint64_t delta(std::uint64_t a,std::uint64_t b) noexcept{return a>=b?a-b:0;}
inline void resetWindows(std::uint64_t id) noexcept {
 if(!framesBusy.test_and_set(std::memory_order_acquire)){frames={id};framesBusy.clear(std::memory_order_release);}
 for(auto& c:costs)if(!c.busy.test_and_set(std::memory_order_acquire)){c.value={id};c.busy.clear(std::memory_order_release);}
}
inline const char* stageName(unsigned i,const char* const* names) noexcept {
 if(i<SharedStages)return names[i];
 constexpr const char* leaf[]{"MAP_FIND","MAP_MODEL_READBACK","MAP_NATIVE_TRANSFORM","MAP_VISIBILITY"};return leaf[i-SharedStages];
}
inline bool accept(const char* line,std::uint64_t now) noexcept {
 if(!currentRecorderD84(now)||!line||std::strncmp(line,"D72_MAP_PERF ",13))return false;
 const auto* p=std::strstr(line,"session=");if(!p)return false;p+=8;
 if(*p<'0'||*p>'9')return false;char* end=nullptr;const auto id=std::strtoull(p,&end,10);
 return end&&(*end==' '||!*end)&&id&&id==session.load(std::memory_order_acquire)&&now<=receiptUntil.load(std::memory_order_acquire);
}
inline bool workerSelectedD84=false; // Mod worker only
inline void stopD84() noexcept {
 active.store(false,std::memory_order_release);until.store(0,std::memory_order_release);receiptUntil.store(0,std::memory_order_release);
 reporter={};bugLeaseD84.store(0,std::memory_order_release);perfLeaseD84.store(0,std::memory_order_release);
 if(!baselineBusy.test_and_set(std::memory_order_acquire)){baseline={};baselineBusy.clear(std::memory_order_release);}
 mapOpen.store(false,std::memory_order_release);workerSelectedD84=false;
}
// Mod worker only. Start from actual map intent and report its actual visibility.
// Existing counters only: no additional XR locate, native scene or GPU queries.
template<class Writer> inline void service(std::uint64_t now,const Map& map,const External& ext,const char* const* names,Writer&& write) noexcept {
 if(!recordingD84(now)){stopD84();return;}
 // A recorder restart cannot adopt a previous capture's map scopes or baseline.
 if(reporter.id&&!currentRecorderD84(now)){active.store(false,std::memory_order_release);reporter={};}
 const bool newOpen=map.open&&(!reporter.previousOpen||map.generation!=reporter.generation);
 const bool previouslyOpen=reporter.previousOpen;
 reporter.previousOpen=map.open;reporter.generation=map.generation;mapOpen.store(map.open,std::memory_order_release);
 char line[1900]{};
 if(newOpen){
  // A replacement occurrence invalidates every outstanding old scope.
  active.store(false,std::memory_order_release);auto id=session.load()+1;if(!id)id=1;
  const auto recorder=recorderD84(now);bugLeaseD84.store(recorder.bug,std::memory_order_release);perfLeaseD84.store(recorder.perf,std::memory_order_release);
  session.store(id,std::memory_order_release);until.store(now+DurationMs,std::memory_order_release);receiptUntil.store(now+DurationMs+5000,std::memory_order_release);
  reporter.id=id;reporter.began=reporter.last=now;reporter.closeAt=0;reporter.prior=ext;reporter.previousMap=map;reporter.baselineMs=0;reporter.baselineSamples=0;dropped.store(0);
  if(!baselineBusy.test_and_set(std::memory_order_acquire)){
   // A region change is not a closed-map baseline. Reject stale startup samples.
   if(!previouslyOpen&&currentRecorderD84(baseline.recorder,now)&&baseline.count&&baseline.last&&now*1000000ull>=baseline.last&&now*1000000ull-baseline.last<=2000000000ull){
    std::uint64_t sum=0;for(unsigned i=0;i<baseline.count;++i)sum+=baseline.dt[i];
    reporter.baselineSamples=baseline.count;reporter.baselineMs=double(sum)/double(baseline.count)/1e6;
   }
   baseline={};baselineBusy.clear(std::memory_order_release);
  }
  resetWindows(id);active.store(true,std::memory_order_release);
  std::snprintf(line,sizeof(line),"D72_MAP_PERF BEGIN session=%llu generation=%llu automatic=0 recorder=F2_OR_SHIFT_F2 baselineSamples=%u baselineFrameMs=%.3f durationMs=%llu afterCloseMs=%llu policyChange=0 extraNativeQueries=0 gpuMeasured=0",(unsigned long long)id,(unsigned long long)map.generation,reporter.baselineSamples,reporter.baselineMs,(unsigned long long)DurationMs,(unsigned long long)AfterCloseMs);write(line);return;
 }
 if(!active.load(std::memory_order_acquire))return;
 if(now<reporter.last){active.store(false,std::memory_order_release);reporter.id=0;return;}
 const bool justClosed=!map.open&&!reporter.closeAt;
 if(justClosed)reporter.closeAt=now;
 const bool closeEnd=reporter.closeAt&&now-reporter.closeAt>=AfterCloseMs;
 const bool expired=now>=until.load(std::memory_order_acquire),ending=closeEnd||expired;
 if(!ending&&!justClosed&&now-reporter.last<ReportMs)return;
 const auto id=reporter.id;const double sec=double(now-reporter.last)/1000.;if(sec<=0)return;
 Frames copy{};
 if(!framesBusy.test_and_set(std::memory_order_acquire)){
  if(frames.session==id)copy=frames;const auto last=frames.last;frames={id};frames.last=last;framesBusy.clear(std::memory_order_release);
 }
 std::array<Aggregate,StageCount> values{};
 for(unsigned i=0;i<StageCount;++i){auto& c=costs[i];if(c.busy.test_and_set(std::memory_order_acquire))continue;if(c.value.session==id)values[i]=c.value;c.value={id};c.busy.clear(std::memory_order_release);}
 const auto fresh=delta(ext.fresh,reporter.prior.fresh),reuse=delta(ext.reused,reporter.prior.reused);
 const double frameMs=copy.intervals?double(copy.ns)/double(copy.intervals)/1e6:0.;
 std::snprintf(line,sizeof(line),"D72_MAP_PERF WINDOW session=%llu phase=%s visible=%d windowS=%.3f nativeFps=%.2f stereoPairsPerS=%.2f xrPerS=%.2f reusedPct=%.2f frameMs=%.3f frameMaxMs=%.3f baselineFrameMs=%.3f deltaFrameMs=%.3f placements=%llu shows=%llu hides=%llu missing=%llu stale=%llu poseRefusal=%u hideReason=%u logLinesPerS=%.1f f2Policy=%u captureMask=%u dropped=%llu gpuMeasured=0",(unsigned long long)id,map.open?"OPEN":justClosed?"OPEN_END":"AFTER_CLOSE",map.visible?1:0,sec,double(copy.count)/sec,double(delta(ext.pairs,reporter.prior.pairs))/sec,double(delta(ext.xrFrames,reporter.prior.xrFrames))/sec,(fresh+reuse)?100.*double(reuse)/double(fresh+reuse):0.,frameMs,double(copy.maxNs)/1e6,reporter.baselineMs,reporter.baselineSamples&&copy.intervals?frameMs-reporter.baselineMs:0.,(unsigned long long)delta(map.placements,reporter.previousMap.placements),(unsigned long long)delta(map.shows,reporter.previousMap.shows),(unsigned long long)delta(map.hides,reporter.previousMap.hides),(unsigned long long)delta(map.missing,reporter.previousMap.missing),(unsigned long long)delta(map.stale,reporter.previousMap.stale),map.refusal,map.hideReason,double(delta(ext.logLines,reporter.prior.logLines))/sec,ext.f2Rung,ext.otherCaptureMask,(unsigned long long)dropped.load());write(line);
 unsigned order[StageCount]{};for(unsigned i=0;i<StageCount;++i)order[i]=i;
 std::sort(order,order+StageCount,[&](unsigned a,unsigned b){return values[a].ns>values[b].ns;});
 int n=std::snprintf(line,sizeof(line),"D72_MAP_PERF COSTS session=%llu timing=INCLUSIVE_CPU_WALL nestedDoNotSum=1",(unsigned long long)id);
 for(unsigned rank=0;rank<6&&values[order[rank]].calls;++rank){const auto i=order[rank];const auto& v=values[i];if(n>0&&n<int(sizeof(line))-180)n+=std::snprintf(line+n,sizeof(line)-n," %s={calls:%llu avgUs:%.2f maxUs:%.2f windowMs:%.2f}",stageName(i,names),(unsigned long long)v.calls,double(v.ns)/double(v.calls)/1000.,double(v.maxNs)/1000.,double(v.ns)/1e6);}
 write(line);
 n=std::snprintf(line,sizeof(line),"D72_MAP_PERF REVIEW session=%llu dominantMeasuredScope=%s baselineValid=%d frameRegression=%d attribution=CORRELATION_ONLY",(unsigned long long)id,values[order[0]].calls?stageName(order[0],names):"NO_SAMPLES",reporter.baselineSamples?1:0,reporter.baselineSamples&&copy.intervals&&frameMs>reporter.baselineMs*1.2?1:0);
 // Always print map leaves even when an inclusive frame/wait dominates top six.
 for(unsigned i=Find;i<StageCount;++i){const auto& v=values[i];if(n>0&&n<int(sizeof(line))-150)n+=std::snprintf(line+n,sizeof(line)-n," %s={calls:%llu avgUs:%.2f maxUs:%.2f}",stageName(i,names),(unsigned long long)v.calls,v.calls?double(v.ns)/double(v.calls)/1000.:0.,double(v.maxNs)/1000.);}
 write(line);reporter.last=now;reporter.prior=ext;reporter.previousMap=map;
 if(ending){
  active.store(false,std::memory_order_release);receiptUntil.store(now+5000,std::memory_order_release);
  std::snprintf(line,sizeof(line),"D72_MAP_PERF END session=%llu reason=%s durationMs=%llu nextCapture=NEXT_MAP_OPEN policyChange=0",(unsigned long long)id,closeEnd?"CLOSED":"FINITE_LIMIT",(unsigned long long)(now-reporter.began));write(line);
 }
}
// Own the caller gate too: OFF must not sample map state, aggregate counters,
// or read a performance clock before service can reject the work.
template<class Sampler,class Writer> inline void workerD84(const char* const* names,Sampler&& sample,Writer&& write) noexcept {
 if(!selectedD84()){if(workerSelectedD84)stopD84();return;}
 workerSelectedD84=true;
 const auto now=perf_d61::nowMs();
 if(!recordingD84(now)){stopD84();return;}
 Map map{};External ext{};sample(map,ext);service(now,map,ext,names,write);
}
}
