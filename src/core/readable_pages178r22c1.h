#pragma once
#include "resident_pages126.h"

namespace kcd2vr::memory126 {
// Permission evidence only, refreshed on every call. No cached object values,
// identity bypass or TTL. Hot resident reads inspect only their addressed pages;
// unresident/wider ranges retain the ordinary VirtualQuery fallback. Callers
// still guard the actual access with SEH and validate the native owner/ABI.
inline bool readableNow178r22c1(const void* p,std::size_t n) noexcept {
    const auto address=reinterpret_cast<std::uintptr_t>(p);
    if(!address||!n||n>UINTPTR_MAX-address)return false;
    std::uintptr_t first=0,end=0;
    if(residentReadable(p,n,first,end))return true;
    MEMORY_BASIC_INFORMATION region{};
    if(!VirtualQuery(p,&region,sizeof(region))||region.State!=MEM_COMMIT||
       (region.Protect&(PAGE_GUARD|PAGE_NOACCESS)))return false;
    switch(region.Protect&0xff){
        case PAGE_READONLY:case PAGE_READWRITE:case PAGE_WRITECOPY:
        case PAGE_EXECUTE_READ:case PAGE_EXECUTE_READWRITE:case PAGE_EXECUTE_WRITECOPY:break;
        default:return false;
    }
    first=reinterpret_cast<std::uintptr_t>(region.BaseAddress);
    if(region.RegionSize>UINTPTR_MAX-first)return false;
    end=first+region.RegionSize;
    return address>=first&&address<end&&n<=end-address;
}
}
