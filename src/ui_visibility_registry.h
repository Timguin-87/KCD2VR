#pragma once
#include "ui_native_state.h"
#include <array>
#include <atomic>
#include <cstdint>
namespace kcd2vr::ui_visibility_registry {
struct Event { std::uintptr_t owner=0;unsigned kind=0;bool visible=false,destroy=false; };
// Bounded MPSC queue. Native callbacks never wait for the consumer or for another
// producer; an overflow is counted and invalidates only that kind's certainty.
class Queue {
    static constexpr std::uint64_t Capacity=512;
    struct Cell { std::atomic<std::uint64_t> sequence{0}; Event event{}; };
    std::array<Cell,Capacity> cells_{};
    std::atomic<std::uint64_t> write_{0};
    std::uint64_t read_=0;
public:
    Queue() noexcept { for(std::uint64_t i=0;i<Capacity;++i)cells_[i].sequence.store(i); }
    bool Push(const Event& e) noexcept {
        auto pos=write_.load(std::memory_order_relaxed);
        for(unsigned attempt=0;attempt<8;++attempt){
            Cell& c=cells_[pos%Capacity];const auto s=c.sequence.load(std::memory_order_acquire);
            const auto delta=static_cast<std::int64_t>(s-pos);
            if(delta<0)return false;
            if(delta==0 && write_.compare_exchange_weak(pos,pos+1,std::memory_order_relaxed)){
                c.event=e;c.sequence.store(pos+1,std::memory_order_release);return true;
            }
            if(delta>0)pos=write_.load(std::memory_order_relaxed);
        }
        return false;
    }
    bool Pop(Event& e) noexcept {
        Cell& c=cells_[read_%Capacity];
        if(c.sequence.load(std::memory_order_acquire)!=read_+1)return false;
        e=c.event;c.sequence.store(read_+Capacity,std::memory_order_release);++read_;return true;
    }
};
struct State {std::uint32_t known=0,visible=0,epoch=0;};
class Registry {
    struct Entry {Event event{};bool used=false;};
    std::array<Entry,96> entries_{};
    State state_{};
public:
    bool Apply(const Event& event,std::uint32_t inputMask) noexcept {
        if(event.kind>=ui_native_state::Count||!event.owner)return false;
        Entry *found=nullptr,*empty=nullptr;
        for(auto& e:entries_){if(e.used&&e.event.owner==event.owner&&e.event.kind==event.kind){found=&e;break;}if(!e.used&&!empty)empty=&e;}
        if(!found&&!event.destroy)found=empty;
        const auto bit=1u<<event.kind;
        const bool wasVisible=found&&found->used&&found->event.visible;
        bool changed=false;
        if(found){changed=!found->used||found->event.visible!=event.visible||event.destroy;
            if(event.destroy)*found=Entry{};else{found->event=event;found->used=true;}
        }else if(!event.destroy)return false;
        state_.known|=bit;state_.visible=0;
        for(const auto& e:entries_)if(e.used&&e.event.visible)state_.visible|=1u<<e.event.kind;
        // A newly registered hidden component does not change focus. New visible
        // instances, actual hides and destruction of a visible instance do.
        if(changed&&(bit&inputMask)&&(wasVisible||event.visible))++state_.epoch;
        return true;
    }
    State Read()const noexcept{return state_;}
};
} // namespace
