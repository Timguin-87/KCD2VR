#pragma once
#include <cstdint>
#include <cstddef>
#include <cstring>
namespace kcd2vr::native_parry55c2 {
// Read-only retail contract. Trigger open/close address actor->state + 0x770;
// their refcount payload (+8) is also consumed by the native block-input path.
// Never call these functions or modify their counter. See NATIVE_WINDOW.md.
inline constexpr std::size_t countOffset=0x778;
struct Pin {std::uintptr_t rva; const char* bytes; std::size_t size;};
inline constexpr Pin pins[]{
    {0x909F84,"\x48\x8b\x89\x10\x02\x00\x00\x48\x81\xc1\x70\x07\x00\x00\x48\x83\xc4\x28\xe9\x29\x00\x00\x00",23},
    {0x909F5C,"\x48\x8b\x89\x10\x02\x00\x00\x48\x81\xc1\x70\x07\x00\x00\x48\x83\xc4\x28\xe9\x81\x00\x00\x00",23},
    {0x909FC8,"\x44\x8b\x41\x08\x4c\x8b\xca\x48\x8b\xd1\x41\x8d\x40\x01\x89\x41\x08",17},
    {0x909FF8,"\x83\x69\x08\x01",4},
    {0x27C2A00,"\x4c\x8b\x80\x10\x02\x00\x00\x41\x83\xb8\x78\x07\x00\x00\x00",15},
};
template<class ReadBytes> bool contract(std::uintptr_t base,ReadBytes read) noexcept {
    if(!base)return false;
    for(const auto& pin:pins){unsigned char actual[24]{};
        if(!read(base+pin.rva,actual,pin.size)||std::memcmp(actual,pin.bytes,pin.size))return false;}
    return true;
}
inline bool validCount(int n) noexcept {return n>=0&&n<=16;} // defensive bound, not a claimed engine maximum
struct Window {
    std::uintptr_t opponent=0;std::uint32_t id=0;
    std::uint64_t serial=0;bool open=false,answered=false;
    bool observe(std::uintptr_t actor,std::uint32_t entityId,bool known,int count) noexcept {
        // An unreadable sample is not a closing edge and cannot rearm assistance.
        if(!known||!actor||!entityId||!validCount(count))return false;
        if(opponent!=actor||id!=entityId){opponent=actor;id=entityId;open=answered=false;}
        if(count==0){open=answered=false;return false;}
        if(!open){open=true;answered=false;++serial;}
        return !answered;
    }
    void consume() noexcept {if(open)answered=true;}
};
}
