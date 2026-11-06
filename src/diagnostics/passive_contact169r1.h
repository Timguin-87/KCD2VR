#pragma once
// Only fixed-size copies on producers. Formatted output is drained by the
// existing diagnostic service; no extra native query or material invocation.
#include "capture168.h"
#include "contact_capture_d84.h"
#include "../vrik/passive_contact169r1.h"
#include "../vrik/npc_contact172.h"
#include "../core/haptics.h"
#include <array>
#include <atomic>
#include <cstring>
namespace kcd2vr::diagnostics::passive169r1 {
namespace p=kcd2vr::passive169r1;
namespace captureD84=contact_capture_d84;
inline bool recording(std::uint64_t now) noexcept {return captureD84::select(now).session!=0;}
struct Record {
    unsigned kind=0; // 0 contact state, 1 exact raw world-ray witness
    p::Sample sample{};p::Decision decision{};captureD84::Ticket capture{};
    std::uint64_t combatSession=0,interactionSession=0,serial=0;
    bool hapticQueued=false;
    float distance=0;std::array<unsigned char,0x50> raw{};p::m::Vec3 rayOrigin{},rayMotion{};
    npc_contact172::Witness native172{};
};
inline std::array<Record,256> queue{};inline unsigned head=0,tail=0;
inline std::atomic_flag lock=ATOMIC_FLAG_INIT;
inline std::atomic<std::uint64_t> dropped{0},limited{0},nextSerial{0};
inline bool push(Record r) noexcept {
    if(!r.capture.session)r.capture=captureD84::select(r.sample.tick);
    if(!captureD84::current(r.capture,r.sample.tick))return false;
    r.combatSession=capture168::active(capture168::Domain::Combat,r.sample.tick)?capture168::gate.session[0].load():0;
    r.interactionSession=capture168::active(capture168::Domain::Interaction,r.sample.tick)?capture168::gate.session[1].load():0;
    if(lock.test_and_set(std::memory_order_acquire)){++dropped;return false;}
    const auto n=(head+1)%queue.size();if(n==tail){lock.clear(std::memory_order_release);++dropped;return false;}
    queue[head]=r;head=unsigned(n);lock.clear(std::memory_order_release);return true;
}
inline bool pop(Record& r) noexcept {
    if(lock.test_and_set(std::memory_order_acquire))return false;
    const bool ok=tail!=head;if(ok){r=queue[tail];tail=(tail+1)%queue.size();}lock.clear(std::memory_order_release);return ok;
}
struct RayContext {p::Sample sample{};bool active=false;};
inline thread_local RayContext rayContext{};
inline std::array<std::atomic<std::uint64_t>,4> lastRayTick{};
struct RayScope {
    RayContext previous;
    explicit RayScope(p::Sample s) noexcept:previous(rayContext){rayContext={s,recording(s.tick)};}
    ~RayScope(){rayContext=previous;}
    RayScope(const RayScope&)=delete;RayScope& operator=(const RayScope&)=delete;
};
inline std::uint64_t ray(const void* bytes,p::m::Vec3 origin,p::m::Vec3 motion,float distance,
                        npc_contact172::Witness native172={}) noexcept {
    const auto& c=rayContext;if(!c.active||!recording(c.sample.tick)||!bytes||c.sample.side<0||c.sample.side>1)return 0;
    const auto slot=unsigned(c.sample.source)*2+unsigned(c.sample.side);if(slot>=4)return 0;
    auto last=lastRayTick[slot].load(std::memory_order_relaxed);
    if(last&&c.sample.tick>=last&&c.sample.tick-last<100){++limited;return 0;}
    if(!lastRayTick[slot].compare_exchange_strong(last,c.sample.tick,std::memory_order_relaxed)){++limited;return 0;}
    const auto capture=captureD84::select(c.sample.tick);if(!capture.session)return 0;
    Record r{};r.capture=capture;r.kind=1;r.sample=c.sample;r.serial=++nextSerial;r.distance=distance;r.rayOrigin=origin;r.rayMotion=motion;
    r.native172=native172;
    std::memcpy(r.raw.data(),bytes,r.raw.size());
    return push(r)?r.serial:0;
}
struct Monitor {p::State state{};std::uint64_t loggedTick=0,combatSession=0,interactionSession=0;captureD84::Ticket capture{};};
// All calls occur on the serialized pose publication owner. No native pointers
// are stored/dereferenced here; actor is an identity token only.
inline std::array<Monitor,8> monitors{};
inline bool cue(int side,std::uint64_t key,float amplitude,int duration,std::uint64_t now) noexcept {
    namespace hp=kcd2vr::haptics101;
    hp::Pulse existing{};
    // A soft scrape must not replace an already queued native impact, holster,
    // calibration or stronger touch cue. Existing XR consumer remains sole API caller.
    if(side<0||side>1)return false;
    if(hp::queue[side].read(existing)&&hp::eligible(existing,now)&&existing.amplitude>=amplitude&&
       now>=existing.tick&&now-existing.tick<std::uint64_t(existing.durationMs+20))return false;
    return hp::publish_event(side,key,amplitude,duration,now);
}
inline void observe(const p::Sample& s,bool haptics=true,bool support=false) noexcept {
    if(s.side<0||s.side>1)return;
    const unsigned slot=unsigned(s.source)*2+unsigned(s.side);if(slot>=monitors.size())return;
    auto& monitor=monitors[slot];const auto previous=monitor.state;const auto d=p::update(monitor.state,s);
    bool sent=false;
    if(haptics&&d.amplitude>0){sent=cue(s.side,s.key,d.amplitude,d.durationMs,s.tick);
        if(support)(void)cue(1-s.side,s.key,d.amplitude*.6f,d.durationMs,s.tick);}
    const auto capture=captureD84::select(s.tick);if(!capture.session)return;
    const auto combat=capture168::active(capture168::Domain::Combat,s.tick)?capture168::gate.session[0].load():0;
    const auto interact=capture168::active(capture168::Domain::Interaction,s.tick)?capture168::gate.session[1].load():0;
    const bool newCapture=!captureD84::same(capture,monitor.capture)||combat!=monitor.combatSession||interact!=monitor.interactionSession;
    const bool due=!monitor.loggedTick||s.tick<monitor.loggedTick||s.tick-monitor.loggedTick>=500;
    if(!newCapture&&!due&&!d.changed&&!sent)return;
    monitor.loggedTick=s.tick;monitor.combatSession=combat;monitor.interactionSession=interact;monitor.capture=capture;
    Record r{};r.capture=capture;r.sample=s;r.decision=d;
    if(d.phase==p::Phase::End&&!s.eligible){r.sample.actor=previous.actor;r.sample.key=previous.key;
        r.sample.generation=previous.generation;r.sample.epoch=previous.epoch;r.sample.request=previous.request;}
    r.decision.phase=d.phase==p::Phase::None?monitor.state.phase:d.phase;
    r.decision.episode=monitor.state.episode;r.hapticQueued=sent;(void)push(r);
}
} // namespace kcd2vr::diagnostics::passive169r1
