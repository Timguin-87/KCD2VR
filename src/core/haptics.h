#pragma once
// Only the XR action consumer calls OpenXR. Pose producer publishes small cues.
#include "snapshot_mailbox.h"
#include "../diagnostics/capture168.h"
#include <array>
#include <atomic>
#include <algorithm>
#include <cmath>
namespace kcd2vr::haptics101 {
struct Pulse {std::uint64_t epoch=0,sequence=0,tick=0,key=0;float amplitude=.12f;int durationMs=35;std::uint64_t captureSessionR95=0;};
struct Cue {std::uint64_t key=0,epoch=0,sequence=0,lastPulse=0,since=0;bool proximityLatched=false;};
inline std::atomic<bool> enabled{false},allowed{false},testPending{false};inline std::atomic<float> zoneRadius{.24f};
// V66.136 delivery telemetry (consumer side).
inline std::atomic<std::uint64_t> submitted{0},succeeded{0},rejected{0};inline std::atomic<std::uint64_t> epoch{1};
inline std::atomic<std::uint64_t> eventSequence{1};
inline std::array<core::SnapshotMailbox<Pulse>,2> queue{};
// The XR adapter installs a bounded central-log observer. Core-only consumers
// need no logger, and diagnostics cannot authorize or defer a pulse.
using TraceR95=void(*)(int,const Pulse&,const char*,const char*,std::uint64_t,int) noexcept;
inline std::atomic<TraceR95> traceR95{nullptr};
inline void tracePulseR95(int side,const Pulse& pulse,const char* stage,const char* reason,std::uint64_t now,int xrResult=0) noexcept {
    if(!pulse.captureSessionR95)return;
    if(const auto trace=traceR95.load(std::memory_order_acquire))trace(side,pulse,stage,reason,now,xrResult);
}
inline bool enqueueR95(int side,Pulse pulse) noexcept {
    namespace cap=diagnostics::capture168;
    const auto serial=cap::gate.session[unsigned(cap::Domain::Interaction)].load(std::memory_order_acquire);
    if(cap::active(cap::Domain::Interaction,pulse.tick)&&
       serial==cap::gate.session[unsigned(cap::Domain::Interaction)].load(std::memory_order_acquire))pulse.captureSessionR95=serial;
    else pulse.captureSessionR95=0;
    const bool accepted=queue[side].publish(pulse);
    tracePulseR95(side,pulse,accepted?"QUEUE_ACCEPTED":"QUEUE_REJECTED",accepted?"MAILBOX_PUBLISHED":"MAILBOX_BUSY",pulse.tick);
    return accepted; // exact mailbox result; no inference about XR or delivery
}

inline void select(bool on,std::uint64_t now=0){(void)now;enabled.store(false);epoch.fetch_add(1);testPending.store(on);enabled.store(on);}
inline void service_test(std::uint64_t now){
    if(!now||!enabled.load()||!allowed.load()||!testPending.exchange(false))return;
    const auto ep=epoch.load();for(int side=0;side<2;++side)(void)enqueueR95(side,Pulse{ep,now,now,1,.60f,80});
}
inline bool publish_event(int side,std::uint64_t key,float amplitude,int durationMs,std::uint64_t now){
    if(side<0||side>1||!now||!key||!enabled.load()||!allowed.load()||!std::isfinite(amplitude))return false;
    amplitude=std::clamp(amplitude,.01f,1.0f);durationMs=std::clamp(durationMs,1,100);
    const auto seq=(1ull<<63)|eventSequence.fetch_add(1,std::memory_order_relaxed);
    return enqueueR95(side,Pulse{epoch.load(),seq,now,key,amplitude,durationMs});
}
inline void observe(Cue& state,int side,std::uint64_t key,double gap,bool valid,std::uint64_t sample,std::uint64_t tick,double candidateRadius=0){
    const auto ep=epoch.load();if(side<0||side>1||!sample||!tick)return;
    if(state.epoch!=ep){state={};state.epoch=ep;}
    if(state.sequence==sample)return;state.sequence=sample;
    // V66.136: holster zone cue matches the ~30 cm gesture zone and is a CLEAR buzz
    // (was a 12 % / 35 ms tick inside 14 cm, which users could not feel).
    // The producer supplies the exact candidate radius (including shoulder
    // reach allowance). Other callers retain their configured default.
    const double selected=std::isfinite(candidateRadius)&&candidateRadius>0?candidateRadius:double(zoneRadius.load());
    const double R=std::clamp(selected,.10,.45);
    if(!enabled.load()||!allowed.load()||!valid||!key||!std::isfinite(gap)||gap>R+.08){state.proximityLatched=false;state.key=0;state.since=0;return;}
    if(state.key!=key){const bool wasIn=state.proximityLatched;state.key=key;if(!wasIn){state.proximityLatched=false;state.since=tick;}} // C22b: no re-buzz when the nearest zone swaps
    if(gap>=R&&!state.proximityLatched){state.since=tick;return;}
    if(!state.proximityLatched&&tick>=state.since&&tick-state.since>=30){state.proximityLatched=true;
        if(!state.lastPulse||(tick>=state.lastPulse&&tick-state.lastPulse>=1200)){
            if(enqueueR95(side,Pulse{ep,sample,tick,key,.55f,70}))state.lastPulse=tick;
        }
    }
}
inline bool eligible(const Pulse&p,std::uint64_t now){return p.key&&p.sequence&&p.epoch==epoch.load()&&enabled.load()&&allowed.load()&&now>=p.tick&&now-p.tick<=150&&std::isfinite(p.amplitude)&&p.amplitude>0&&p.amplitude<=1&&p.durationMs>=1&&p.durationMs<=100;}
}
