#pragma once
#include "../../native_script_t2.h"
#include <atomic>
#include <cstdint>
namespace kcd2vr::subsystems::alchemy::feedback_d80 {
// Exact authored audio from the shipped male.animevents. Playback does not
// re-enter animation events, item transfers, particles or recipe operations.
enum class Cue:unsigned {None,TakeHerb,ReturnHerb,HerbToBowl,HerbToMortar,HerbToPot,
    TakeBowl,ReturnBowl,TakeFlask,ReturnFlask,TakeSpecial,ReturnSpecial,
    SpecialToBowl,SpecialToMortar,SpecialToPot,TakeHourglass,ReturnHourglass};
struct Event {
    std::uintptr_t instance=0,player=0,controller=0;
    std::uint64_t lifetime=0,serial=0,tick=0;
    Cue cue=Cue::None;
};
class Queue {
    Event entries_[4]{};unsigned first_=0,count_=0;
    std::atomic_flag busy_=ATOMIC_FLAG_INIT;
    std::atomic<unsigned> pending_{0};
public:
    bool wanted()const noexcept{return pending_.load(std::memory_order_acquire)!=0;}
    bool push(const Event& e) noexcept {
        if(!e.serial||e.cue==Cue::None||busy_.test_and_set(std::memory_order_acquire))return false;
        bool ok=count_<4;
        if(ok){entries_[(first_+count_)%4]=e;++count_;pending_.store(count_,std::memory_order_release);}
        busy_.clear(std::memory_order_release);return ok;
    }
    bool pop(Event& e) noexcept {
        if(busy_.test_and_set(std::memory_order_acquire))return false;
        const bool ok=count_!=0;
        if(ok){e=entries_[first_];first_=(first_+1)%4;--count_;pending_.store(count_,std::memory_order_release);}
        busy_.clear(std::memory_order_release);return ok;
    }
};
inline Queue queue;
inline std::atomic<bool> transportReady{false};
using Consumer=void(*)(const native_script_t2::Context&,const Event&) noexcept;
inline std::atomic<Consumer> consumer{nullptr};
inline bool Wanted()noexcept{return queue.wanted();}
inline bool Offer(const Event& e)noexcept{return transportReady.load(std::memory_order_acquire)&&consumer.load(std::memory_order_acquire)&&queue.push(e);}
inline void Service(const native_script_t2::Context& ctx)noexcept {
    if(!Wanted()||!ctx.Current())return;
    Event e{};if(!queue.pop(e))return; // spent BEFORE any fallible native/Lua call
    if(auto fn=consumer.load(std::memory_order_acquire))fn(ctx,e);
}
}
