#pragma once
// D30 (Miki 27 Oct: "f2 for frames and jitter, f7 for vrik, all logs gated behind f binds"):
// one line per second that shows performance and jitter at a glance.
//  F2 (Bug lease):  "F2 FRAME"  game frame rate + frame time p50/p95/max, headset fps, fresh/reused images,
//                   our own measured CPU per game frame, log lines per second, raw/final hand and head acceleration.
//  F7 (Vrik lease): "F7 SUMMARY" solve time, goal-raw offset p95 per hand, contact plane frames, frame51 valid %,
//                   anchor source switches, elbow refusals, finger writes.
// Producers only copy numbers into fixed windows while the matching lease is on (one mask load when off).
// Formatting happens once per second on the mod worker. No native reads, no gameplay writes.
#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include "capture168.h"
namespace kcd2vr::diagnostics::frame_d30 {
namespace cap=capture168;
// fixed size sample window with exact order statistics on the copy (n <= 512)
struct Window {
    std::array<float,512> v{};unsigned n=0;std::uint64_t total=0;double sum=0;float mx=0;
    void add(float x) noexcept {if(!std::isfinite(x))return;++total;sum+=x;mx=std::max(mx,x);if(n<v.size())v[n++]=x;else v[std::size_t(total%v.size())]=x;}
    void clear() noexcept {n=0;total=0;sum=0;mx=0;}
    float pct(double p) const noexcept {
        if(!n)return 0.f;std::array<float,512> c{};std::copy_n(v.begin(),n,c.begin());
        const auto k=std::size_t(std::clamp(p,0.,1.)*double(n-1)+.5);
        std::nth_element(c.begin(),c.begin()+std::ptrdiff_t(k),c.begin()+std::ptrdiff_t(n));return c[k];
    }
    double mean() const noexcept {return total?sum/double(total):0.;}
};
// D45: the producer's sequence is an identity, not a frame counter. Captured
// sequences normally advance by four; the former seq == last+1 gate rejected
// every triple and printed zero. Use source-time intervals, never recorder time.
// This is acceleration magnitude (m/s^2), including intentional motion. It is
// neither physical jerk (third derivative) nor an isolated measure of jitter.
struct Acceleration {
    double p[3]{},velocity[3]{},lastDt=0;unsigned have=0;
    std::uint64_t lastSeq=0,lastNs=0;
    double push(double x,double y,double z,std::uint64_t seq,std::uint64_t ns) noexcept {
        if(!seq||!ns||!std::isfinite(x)||!std::isfinite(y)||!std::isfinite(z)){have=0;return -1;}
        // A second skinning/eye consumer does not provide another motion sample.
        // An old sample must not rewind the accepted stream either.
        if(have&&seq<=lastSeq)return -1;
        if(have&&ns<=lastNs){have=0;return -1;}
        const double dt=have?double(ns-lastNs)*1e-9:0;
        if(have&&(dt<1e-6||dt>.100000001))have=0;
        double result=-1;const double point[3]{x,y,z};
        if(have){
            double squared=0;
            for(unsigned i=0;i<3;++i){
                const double v=(point[i]-p[i])/dt;
                const double a=have>=2?2*(v-velocity[i])/(dt+lastDt):0;
                squared+=a*a;velocity[i]=v;
            }
            if(have>=2&&std::isfinite(squared))result=std::sqrt(squared);
        }
        std::copy_n(point,3,p);lastDt=dt;lastSeq=seq;lastNs=ns;have=std::min(have+1,2u);
        return result;
    }
};
struct MotionContext {
    std::uint64_t poseEpoch=0,sourceEpoch=0,character=0,policy=0,control=0,filterPolicy=0,elbowPolicy=0;
    bool operator==(const MotionContext& b) const noexcept {
        return poseEpoch==b.poseEpoch&&sourceEpoch==b.sourceEpoch&&character==b.character&&
            policy==b.policy&&control==b.control&&filterPolicy==b.filterPolicy&&elbowPolicy==b.elbowPolicy;
    }
};
struct State {
    std::atomic_flag lock=ATOMIC_FLAG_INIT;
    // F2
    Window frameMs{};std::uint64_t frames=0,lastFrameNs=0;
    Window rawAccel[2]{},finalAccel[2]{},headAccel{};
    Acceleration rawMotion[2]{},finalMotion[2]{},headMotion{};
    MotionContext motionContext{};std::uint64_t motionSession=0;
    // F7
    Window transactionUs{},lockWaitUs{},prepareUs{},goalOffset[2]{};std::uint64_t poseFrames=0,frameValid=0,anchorSwitches=0,planeFrames[2]{},elbowRefused[2]{},fingerOk[2]{};
    bool lastFrameValid=false,haveLastValid=false;
};
inline State state;
inline bool f2On(std::uint64_t now) noexcept {
    const auto m=cap::gate.mask.load(std::memory_order_relaxed);
    return (m&(1u<<unsigned(cap::Domain::Bug)))&&cap::active(cap::Domain::Bug,now);
}
inline bool f7On(std::uint64_t now) noexcept {
    const auto m=cap::gate.mask.load(std::memory_order_relaxed);
    return (m&(1u<<unsigned(cap::Domain::Vrik)))&&cap::active(cap::Domain::Vrik,now);
}
inline bool anyOn() noexcept {
    return (cap::gate.mask.load(std::memory_order_relaxed)&((1u<<unsigned(cap::Domain::Bug))|(1u<<unsigned(cap::Domain::Vrik))))!=0;
}
struct Guard {bool ok;explicit Guard() noexcept:ok(!state.lock.test_and_set(std::memory_order_acquire)){} ~Guard(){if(ok)state.lock.clear(std::memory_order_release);}};
// game thread, once per native gameplay frame (PhysicalNativeFrameTick6698)
inline void gameFrame(std::uint64_t nowNs,std::uint64_t tick) noexcept {
    if(!anyOn()||!f2On(tick)){state.lastFrameNs=0;return;}
    Guard g;if(!g.ok)return;
    if(state.lastFrameNs&&nowNs>state.lastFrameNs){const double ms=double(nowNs-state.lastFrameNs)/1e6;if(ms<1000.)state.frameMs.add(float(ms));}
    state.lastFrameNs=nowNs;++state.frames;
}
struct PoseSample {
    std::uint64_t tick=0,sequence=0,timeNs=0;bool committed=false;
    MotionContext motionContext{};
    bool rawValid[3]{};double raw[3][3]{}; // head, right, left (tracking space, m)
    bool finalValid[2]{};double finalWrist[2][3]{}; // model space, m (root motion removed)
    // F7 extras (only filled when the F7 trace ran)
    bool traced=false;double transactionUs=-1,lockWaitUs=-1,prepareUs=-1;bool goalValid[2]{};double goalOffsetMm[2]{};int planes[2]{};bool elbowOk[2]{true,true},fingerOk[2]{};
    bool frameValid=false;
};
inline void pose(const PoseSample& s) noexcept {
    const bool f2=f2On(s.tick),f7=f7On(s.tick);if(!f2&&!f7)return;
    Guard g;if(!g.ok)return;
    if(f2){
        const auto session=cap::gate.session[unsigned(cap::Domain::Bug)].load(std::memory_order_acquire);
        // D46: separate capture sessions must not inherit a prior rung's window.
        if(state.motionSession!=session){
            state.headAccel.clear();
            for(int h=0;h<2;++h){state.rawAccel[h].clear();state.finalAccel[h].clear();}
        }
        if(state.motionSession!=session||!(state.motionContext==s.motionContext)){
            state.headMotion.have=0;
            for(int h=0;h<2;++h){state.rawMotion[h].have=0;state.finalMotion[h].have=0;}
            state.motionSession=session;state.motionContext=s.motionContext;
        }
        if(s.rawValid[0]){const auto a=state.headMotion.push(s.raw[0][0],s.raw[0][1],s.raw[0][2],s.sequence,s.timeNs);if(a>=0)state.headAccel.add(float(a));}
        else state.headMotion.have=0;
        for(int h=0;h<2;++h){
            if(s.rawValid[1+h]){const auto a=state.rawMotion[h].push(s.raw[1+h][0],s.raw[1+h][1],s.raw[1+h][2],s.sequence,s.timeNs);if(a>=0)state.rawAccel[h].add(float(a));}
            else state.rawMotion[h].have=0;
            if(s.committed&&s.finalValid[h]){const auto a=state.finalMotion[h].push(s.finalWrist[h][0],s.finalWrist[h][1],s.finalWrist[h][2],s.sequence,s.timeNs);if(a>=0)state.finalAccel[h].add(float(a));}
            else state.finalMotion[h].have=0;
        }
    }
    if(f7){
        ++state.poseFrames;if(s.frameValid)++state.frameValid;
        if(state.haveLastValid&&state.lastFrameValid!=s.frameValid)++state.anchorSwitches;
        state.lastFrameValid=s.frameValid;state.haveLastValid=true;
        if(s.transactionUs>=0)state.transactionUs.add(float(s.transactionUs));
        if(s.lockWaitUs>=0)state.lockWaitUs.add(float(s.lockWaitUs));
        if(s.prepareUs>=0)state.prepareUs.add(float(s.prepareUs));
        if(s.traced)for(int h=0;h<2;++h){
            if(s.goalValid[h])state.goalOffset[h].add(float(s.goalOffsetMm[h]));
            if(s.planes[h]>0)++state.planeFrames[h];
            if(!s.elbowOk[h])++state.elbowRefused[h];
            if(s.fingerOk[h])++state.fingerOk[h];
        }
    }
}
// Counters the worker passes in (cumulative, the reporter takes deltas).
struct External {std::uint64_t xrFrames=0,fresh=0,reused=0,ourUs=0,inclusiveUs=0,logLines=0,handoffNs=0,handoffCalls=0;};
inline const char* motionPercentile(char (&out)[32],const Window& w) noexcept {
    if(w.n)std::snprintf(out,sizeof(out),"%.3f",double(w.pct(.95)));
    else std::snprintf(out,sizeof(out),"N/A");
    return out;
}
inline int formatF2(char* out,std::size_t n,double sec,const State& s,const External& d) noexcept {
    const double fps=sec>0?double(s.frames)/sec:0.;
    char raw[2][32]{},final[2][32]{},head[32]{};
    return std::snprintf(out,n,"F2 FRAME windowS=%.1f gameFps=%.1f frameMsP50=%.2f frameMsP95=%.2f frameMsMax=%.1f xrFps=%.1f freshPerS=%.1f reusePct=%.1f handoffMs=%.2f "
        "measuredHookOwnMsPerFrame=%.3f measuredHookInclusiveMsPerFrame=%.3f cpuScope=INSTRUMENTED_HOOKS_ONLY logLinesPerS=%.0f motionMetric=ACCELERATION_D45 handRawAccelP95Mps2=%s/%s handFinalAccelP95Mps2=%s/%s headAccelP95Mps2=%s rawSamplesRL=%u/%u finalSamplesRL=%u/%u headSamples=%u",
        sec,fps,double(s.frameMs.pct(.5)),double(s.frameMs.pct(.95)),double(s.frameMs.mx),sec>0?double(d.xrFrames)/sec:0.,sec>0?double(d.fresh)/sec:0.,
        (d.fresh+d.reused)?100.*double(d.reused)/double(d.fresh+d.reused):0.,d.handoffCalls?double(d.handoffNs)/double(d.handoffCalls)/1e6:-1.,
        s.frames?double(d.ourUs)/1000./double(s.frames):0.,s.frames?double(d.inclusiveUs)/1000./double(s.frames):0.,sec>0?double(d.logLines)/sec:0.,
        motionPercentile(raw[0],s.rawAccel[0]),motionPercentile(raw[1],s.rawAccel[1]),
        motionPercentile(final[0],s.finalAccel[0]),motionPercentile(final[1],s.finalAccel[1]),motionPercentile(head,s.headAccel),
        s.rawAccel[0].n,s.rawAccel[1].n,s.finalAccel[0].n,s.finalAccel[1].n,s.headAccel.n);
}
inline int formatF7(char* out,std::size_t n,double sec,const State& s) noexcept {
    const auto pf=s.poseFrames?double(s.poseFrames):1.;
    return std::snprintf(out,n,"F7 SUMMARY windowS=%.1f poseFrames=%llu transactionUsAvg=%.0f transactionUsP95=%.0f transactionUsMax=%.0f lockWaitUsP95=%.0f lockWaitUsMax=%.0f prepareUsAvg=%.0f prepareUsP95=%.0f prepareUsMax=%.0f goalRawOffsetP95mm=%.1f/%.1f contactPlaneFramesPct=%.1f/%.1f "
        "frame51ValidPct=%.1f anchorSourceSwitches=%llu elbowRefusedRL=%llu/%llu fingerWritesPct=%.1f/%.1f",
        sec,(unsigned long long)s.poseFrames,s.transactionUs.mean(),double(s.transactionUs.pct(.95)),double(s.transactionUs.mx),double(s.lockWaitUs.pct(.95)),double(s.lockWaitUs.mx),s.prepareUs.mean(),double(s.prepareUs.pct(.95)),double(s.prepareUs.mx),
        double(s.goalOffset[0].pct(.95)),double(s.goalOffset[1].pct(.95)),100.*double(s.planeFrames[0])/pf,100.*double(s.planeFrames[1])/pf,
        100.*double(s.frameValid)/pf,(unsigned long long)s.anchorSwitches,(unsigned long long)s.elbowRefused[0],(unsigned long long)s.elbowRefused[1],
        100.*double(s.fingerOk[0])/pf,100.*double(s.fingerOk[1])/pf);
}
// mod worker, called every loop; prints at most once per second per lease
template<class Write> inline void report(std::uint64_t now,const External& cumulative,Write&& write) noexcept {
    static std::uint64_t last=0;static External prior{};
    if(last&&now>=last&&now-last<1000)return;
    const bool f2=f2On(now),f7=f7On(now);
    const double sec=last&&now>last?double(now-last)/1000.:0.;last=now;
    External d{};d.xrFrames=cumulative.xrFrames-prior.xrFrames;d.fresh=cumulative.fresh-prior.fresh;d.reused=cumulative.reused-prior.reused;
    d.ourUs=cumulative.ourUs-prior.ourUs;d.inclusiveUs=cumulative.inclusiveUs-prior.inclusiveUs;d.logLines=cumulative.logLines-prior.logLines;
    d.handoffNs=cumulative.handoffNs-prior.handoffNs;d.handoffCalls=cumulative.handoffCalls-prior.handoffCalls;
    const bool primed=prior.xrFrames!=0||prior.fresh!=0||prior.logLines!=0;prior=cumulative;
    State copy{};
    {Guard g;if(!g.ok)return;
     copy.frameMs=state.frameMs;copy.frames=state.frames;for(int h=0;h<2;++h){copy.rawAccel[h]=state.rawAccel[h];copy.finalAccel[h]=state.finalAccel[h];copy.goalOffset[h]=state.goalOffset[h];
        copy.planeFrames[h]=state.planeFrames[h];copy.elbowRefused[h]=state.elbowRefused[h];copy.fingerOk[h]=state.fingerOk[h];}
     copy.headAccel=state.headAccel;copy.transactionUs=state.transactionUs;copy.lockWaitUs=state.lockWaitUs;copy.prepareUs=state.prepareUs;copy.poseFrames=state.poseFrames;copy.frameValid=state.frameValid;copy.anchorSwitches=state.anchorSwitches;
     state.frameMs.clear();state.frames=0;state.headAccel.clear();state.transactionUs.clear();state.lockWaitUs.clear();state.prepareUs.clear();state.poseFrames=0;state.frameValid=0;state.anchorSwitches=0;
     for(int h=0;h<2;++h){state.rawAccel[h].clear();state.finalAccel[h].clear();state.goalOffset[h].clear();state.planeFrames[h]=0;state.elbowRefused[h]=0;state.fingerOk[h]=0;}}
    if(!primed||sec<=0.)return; // first window only primes the cumulative counters
    char line[1200]{};
    if(f2&&formatF2(line,sizeof(line),sec,copy,d)>0)write(line);
    if(f7&&formatF7(line,sizeof(line),sec,copy)>0)write(line);
}
}
