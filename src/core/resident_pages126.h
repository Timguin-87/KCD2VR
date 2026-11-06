#pragma once
#include <windows.h>
#include <psapi.h>
#include <cstdint>
#include <cstddef>

namespace kcd2vr::memory126 {
// Query only the addressed pages, not the rest of a potentially enormous heap
// region. Unresident/unknown pages fall back to VirtualQuery in the caller.
// This is permission evidence only; values and owner backlinks remain live.
inline bool residentReadable(const void* pointer,std::size_t need,
                             std::uintptr_t& first,std::uintptr_t& end) noexcept {
    static const std::size_t page=[](){SYSTEM_INFO info{};GetSystemInfo(&info);return std::size_t(info.dwPageSize);}();
    const auto address=reinterpret_cast<std::uintptr_t>(pointer);
    if(!address||!need||!page||address>UINTPTR_MAX-(need-1))return false;
    first=address-address%page;
    const auto last=address+need-1;
    const auto lastPage=last-last%page;
    if(lastPage>UINTPTR_MAX-page||lastPage-first>page)return false;
    PSAPI_WORKING_SET_EX_INFORMATION pages[2]{};
    const unsigned count=lastPage==first?1:2;
    pages[0].VirtualAddress=reinterpret_cast<void*>(first);
    pages[1].VirtualAddress=reinterpret_cast<void*>(lastPage);
    if(!QueryWorkingSetEx(GetCurrentProcess(),pages,DWORD(count*sizeof(pages[0]))))return false;
    for(unsigned i=0;i<count;++i){
        const auto& a=pages[i].VirtualAttributes;
        if(!a.Valid||a.Bad)return false;
        const auto protection=a.Win32Protection;
        if(protection&(PAGE_GUARD|PAGE_NOACCESS))return false;
        switch(protection&0xff){
            case PAGE_READONLY:case PAGE_READWRITE:case PAGE_WRITECOPY:
            case PAGE_EXECUTE_READ:case PAGE_EXECUTE_READWRITE:case PAGE_EXECUTE_WRITECOPY:break;
            default:return false;
        }
    }
    end=lastPage+page;return true;
}
}
