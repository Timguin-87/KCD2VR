#pragma once
#include "pet_gesture_d59.h"
#include "../../core/snapshot_mailbox.h"
#include "../../native_script_t2.h"
#include <atomic>
#include <cstdio>
#include <cstring>

namespace kcd2vr::petting_d59 {
using CurrentReader=bool(*)(const Frame&,int,std::uint64_t) noexcept;
using Reporter=void(*)(const char*) noexcept;
struct Request {Frame frame{};std::uint64_t serial=0,generation=0;int side=-1;};
inline core::SnapshotMailbox<Request> requests;
inline std::atomic<bool> enabled{true};
inline std::atomic<std::uint64_t> generation{1},serials{0},pending{0};
inline std::atomic<CurrentReader> currentReader{nullptr};
inline std::atomic<Reporter> reporter{nullptr};
inline Recognizer recognizer;
inline void SetCurrentReader(CurrentReader read) noexcept {currentReader.store(read,std::memory_order_release);}
inline void SetReporter(Reporter report) noexcept {reporter.store(report,std::memory_order_release);}
inline bool Enabled() noexcept {return enabled.load(std::memory_order_acquire);}
inline void SetEnabled(bool on) noexcept {
    if(enabled.exchange(on,std::memory_order_acq_rel)!=on)generation.fetch_add(1,std::memory_order_acq_rel);
}
inline bool Wanted() noexcept {return pending.load(std::memory_order_acquire)!=0;}
inline void Publish(const Frame&f) noexcept {
    static std::uint64_t observedGeneration=0;
    const auto gen=generation.load(std::memory_order_acquire);
    if(gen!=observedGeneration){recognizer.reset();observedGeneration=gen;}
    if(!Enabled())return;
    const int side=recognizer.update(f);if(side<0)return;
    // Consumed before publication; an unknown dispatch never retries on the
    // next input frame. Failed transport needs a fresh withdrawal/caress.
    const auto serial=serials.fetch_add(1,std::memory_order_acq_rel)+1;
    if(serial>9007199254740991ull)return;
    std::uint64_t empty=0;if(!pending.compare_exchange_strong(empty,serial,std::memory_order_acq_rel))return;
    if(!requests.publish(Request{f,serial,gen,side})){
        auto own=serial;(void)pending.compare_exchange_strong(own,0,std::memory_order_acq_rel);
    }
}
inline void Service(const native_script_t2::Context&ctx) noexcept {
    Request r{};if(!requests.read(r)||!r.serial||pending.load(std::memory_order_acquire)!=r.serial)return;
    // Consume before native entry. A lost/exceptional return is UNKNOWN, never
    // permission to replay the native effect. No continuous VM polling.
    auto own=r.serial;if(!pending.compare_exchange_strong(own,0,std::memory_order_acq_rel))return;
    const auto read=currentReader.load(std::memory_order_acquire);
    char line[384]{},input[128]{},reply[160]{};
    const char*result="CANCELLED_BEFORE_NATIVE";
    if(Enabled()&&r.generation==generation.load(std::memory_order_acquire)&&
       Eligible(r.frame,r.side,ctx.now)&&read&&read(r.frame,r.side,ctx.now)&&ctx.Current()&&
       Enabled()&&r.generation==generation.load(std::memory_order_acquire)){
        std::snprintf(input,sizeof(input),"PET|%llu|%u|%u",(unsigned long long)r.serial,r.frame.player,r.frame.hand[r.side].target);
        const bool called=ctx.CallPacket("kcd2vr_pet_d59",input,reply,sizeof(reply));
        char prefix[96]{};std::snprintf(prefix,sizeof(prefix),"PET|%llu|%u|",(unsigned long long)r.serial,r.frame.hand[r.side].target);
        result=called&&!std::strncmp(reply,prefix,std::strlen(prefix))?reply+std::strlen(prefix):"TRANSPORT_UNKNOWN_NO_RETRY";
    }
    std::snprintf(line,sizeof(line),"D59 PET target=%u side=%d serial=%llu result=%s gesture=OPEN_HAND_STROKE dispatch=EXACT_NATIVE_COMPANION_BOND nativeAccept=UNCLAIMED visualOwner=NATIVE_PAIRED physics=NONE",
        r.frame.hand[r.side].target,r.side,(unsigned long long)r.serial,result);
    if(auto report=reporter.load(std::memory_order_acquire))report(line); // caller's F5 gate
}
}
