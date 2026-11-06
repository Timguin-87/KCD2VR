#pragma once
// Bounded passive recorder: plain F3/F5 capture or explicit reference presets. No file I/O, native calls, or heap allocation
// in a producer. The worker drains events; dropped records are counted explicitly.
#include "capture168.h"
#include <array>
#include <atomic>
#include <cstdint>
#include <cstring>
namespace kcd2vr::diagnostics::combat_capture {
enum class Kind {Input,Clip,Pose,State,Punch,Sweep,Contact,R3,Material,ContactPath,InteractionBody,CloseQuery,F11Native167,MeleeCommand174r2,PhysicalImpact177};
enum Source : unsigned {CombatReference=1,InteractionReference=2,FistReference=4,F11Hands167=8,F11Stabs167=16,CaptureOnly168R1=32,InteractionOnly168R1=64};
inline unsigned f11Rung167(unsigned source){return source&F11Stabs167?2:source&F11Hands167?1:0;}
inline unsigned sources(int f3,int f5,int f10) {
    return (f3==1?CombatReference:0u)|(f5==1?InteractionReference:0u)|(f10==1?FistReference:0u);
}
struct Event {
    Kind kind=Kind::State;std::uint64_t session=0,tick=0,sequence=0,poseEpoch=0,aliasHash=0,key[2]{};
    std::uintptr_t actor=0;int mode=0,id=-1,layer=-1;float value=0,phase=0;
    float grip[2]{},trigger[2]{},hand[6]{};unsigned flags=0,source=0,trackingFlags=0;char name[160]{};
    std::uint64_t parent164[3]{}; // causal predictor/sweep/sample identities, zero means no join
    float previous[7]{},current[7]{}; // sweep: world translation xyz then quaternion xyzw
    unsigned char contact114[0x88]{}; // exact borrowed payload copied by native +D51B38, fields not guessed
};
inline std::atomic<std::uint64_t> until{0},session{0},dropped{0};
inline std::atomic<unsigned> sourceMask{0};
inline std::atomic_flag lock=ATOMIC_FLAG_INIT;
inline std::array<Event,512> events{};inline unsigned head=0,tail=0;
inline std::atomic<std::uint64_t> captureOnlyUntil168r1{0};
inline bool captureOnly168r1(std::uint64_t now) noexcept {
    const auto end=captureOnlyUntil168r1.load(std::memory_order_acquire);
    return now&&now<end&&end-now<=capture168::Duration&&
        capture168::active(capture168::Domain::Combat,now);
}
inline std::atomic<std::uint64_t> interactionOnlyUntil168r1{0};
inline bool interactionOnly168r1(std::uint64_t now) noexcept {
    const auto end=interactionOnlyUntil168r1.load(std::memory_order_acquire);
    return now&&now<end&&end-now<=capture168::Duration&&capture168::active(capture168::Domain::Interaction,now);
}
inline bool active(std::uint64_t now) {
    // Recorder gates are the only logging authority; old comparison-source
    // flags and gameplay transitions cannot start a hidden capture.
    return capture168::active(capture168::Domain::Combat,now)||capture168::active(capture168::Domain::Interaction,now);
}
inline unsigned effectiveSources168r1(std::uint64_t now) noexcept {
    return sourceMask.load()|(captureOnly168r1(now)?CaptureOnly168R1:0u)|(interactionOnly168r1(now)?InteractionOnly168R1:0u);
}
inline void select(bool on,std::uint64_t now){session.fetch_add(1);until.store(on?now+60000:0,std::memory_order_release);}
inline void selectSources(unsigned mask,std::uint64_t now,bool renew=false) {
    const auto prior=sourceMask.exchange(mask);
    if(prior==mask&&!renew)return;
    session.fetch_add(1);
    until.store(mask?now+((mask&(CombatReference|InteractionReference|F11Hands167|F11Stabs167))?180000:60000):0,std::memory_order_release);
}
inline void name(Event& e,const char* s){if(!s)return;std::size_t n=0;while(n+1<sizeof(e.name)&&s[n]){e.name[n]=s[n];++n;}e.name[n]=0;}
inline bool push(Event e,bool force=false){
    if(!force&&!active(e.tick))return false;
    if(lock.test_and_set(std::memory_order_acquire)){dropped.fetch_add(1);return false;}
    const unsigned next=(head+1)%events.size();if(next==tail){lock.clear(std::memory_order_release);dropped.fetch_add(1);return false;}
    e.session=session.load();e.source=effectiveSources168r1(e.tick);events[head]=e;head=next;lock.clear(std::memory_order_release);return true;
}
inline bool pop(Event& e){if(lock.test_and_set(std::memory_order_acquire))return false;
    bool ok=tail!=head;if(ok){e=events[tail];tail=(tail+1)%events.size();}lock.clear(std::memory_order_release);return ok;}
} // namespace
