#include "core/readable_pages178r22c1.h"
#include <cassert>
#include <cstdio>
int main(){
    using kcd2vr::memory126::readableNow178r22c1;
    SYSTEM_INFO si{};GetSystemInfo(&si);const auto page=si.dwPageSize;
    auto* p=static_cast<unsigned char*>(VirtualAlloc(nullptr,page*4,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE));assert(p);
    // Untouched/unresident data is not spuriously rejected. Wide reads use the
    // region fallback; addressed resident and cross-page reads use page checks.
    assert(readableNow178r22c1(p,4));assert(readableNow178r22c1(p,3*page));
    for(unsigned n=0;n<4;++n)p[n*page]=1;
    assert(readableNow178r22c1(p+page-4,8));
    assert(!readableNow178r22c1(nullptr,4));assert(!readableNow178r22c1(p,0));
    assert(!readableNow178r22c1(reinterpret_cast<void*>(UINTPTR_MAX-1),8));
    DWORD old=0;assert(VirtualProtect(p+page,page,PAGE_NOACCESS,&old));
    assert(!readableNow178r22c1(p+page,4));assert(!readableNow178r22c1(p+page-4,8));
    assert(!readableNow178r22c1(p,3*page));
    assert(VirtualProtect(p+page,page,PAGE_READWRITE|PAGE_GUARD,&old));
    assert(!readableNow178r22c1(p+page,4));
    MEMORY_BASIC_INFORMATION region{};assert(VirtualQuery(p+page,&region,sizeof(region))&&(region.Protect&PAGE_GUARD));
    assert(VirtualProtect(p+page,page,PAGE_READONLY,&old));assert(readableNow178r22c1(p+page,4));
    assert(VirtualProtect(p+page,page,PAGE_EXECUTE,&old));assert(!readableNow178r22c1(p+page,4));
    assert(VirtualFree(p,0,MEM_RELEASE));assert(!readableNow178r22c1(p,4));
    std::puts("PASS current resident/fallback/cross-page/protection-change/guard/freed permission checks");
}
