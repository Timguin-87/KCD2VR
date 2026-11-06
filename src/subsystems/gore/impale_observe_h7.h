#pragma once
#include <array>
#include <cstdint>
namespace kcd2vr::impale_observe_h7 {
struct Identity {
    std::uint64_t serial=0,epoch=0,recovery=0,request=0;
    std::uint32_t target=0;
    std::uintptr_t entity=0,oldColliderValue=0; // Opaque comparison only, never dereferenced.
};
struct Schedule {
    Identity id{};std::uint64_t began=0;unsigned next=0;bool active=false;
    inline static constexpr std::array<std::uint64_t,5> dueMs{0,500,1500,3000,6000};
    void clear() noexcept {*this={};}
    void begin(Identity source,std::uint64_t now) noexcept {
        clear();if(!source.serial||!source.epoch||!source.recovery||!source.request||!source.target||!source.entity)return;
        id=source;began=now;active=true;
    }
    bool take(std::uint64_t now,std::uint64_t epoch,std::uint64_t recovery,std::uint64_t request,bool allowed,unsigned& index) noexcept {
        if(!active)return false;
        if(!allowed||epoch!=id.epoch||recovery!=id.recovery||request!=id.request||now<began||now-began>6500){clear();return false;}
        const auto elapsed=now-began;if(next>=dueMs.size()||elapsed<dueMs[next])return false;
        index=next++;
        // One current observation per native tick. Skip missed deadlines rather
        // than relabel several reads of today's state as historical samples.
        while(next<dueMs.size()&&elapsed>=dueMs[next])++next;
        if(next==dueMs.size())active=false;
        return true;
    }
};
}
