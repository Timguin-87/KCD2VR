#include "core/resident_pages126.h"
#include <cassert>
#include <chrono>
#include <cstdio>
int main(){
 SYSTEM_INFO si{};GetSystemInfo(&si);const std::size_t page=si.dwPageSize;
 auto* p=static_cast<unsigned char*>(VirtualAlloc(nullptr,page*4,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE));assert(p);
 for(unsigned i=0;i<4;++i)p[i*page]=1;
 std::uintptr_t first=0,end=0;
 auto read=[&](void* a,std::size_t n){return kcd2vr::memory126::residentReadable(a,n,first,end);};
 assert(read(p+4,8)&&first==reinterpret_cast<std::uintptr_t>(p)&&end==first+page);
 assert(read(p+page-4,8)&&end==reinterpret_cast<std::uintptr_t>(p)+2*page);
 assert(!read(p,3*page));assert(!read(nullptr,4));assert(!read(reinterpret_cast<void*>(UINTPTR_MAX-1),8));
 DWORD old=0;assert(VirtualProtect(p+page,page,PAGE_NOACCESS,&old));assert(!read(p+page,4));assert(!read(p+page-4,8));
 assert(VirtualProtect(p+page,page,PAGE_READWRITE|PAGE_GUARD,&old));assert(!read(p+page,4));
 MEMORY_BASIC_INFORMATION mbi{};assert(VirtualQuery(p+page,&mbi,sizeof(mbi))&&(mbi.Protect&PAGE_GUARD));
 assert(VirtualProtect(p+page,page,PAGE_READONLY,&old));
 const volatile unsigned char residentAgain=p[page];(void)residentAgain;assert(read(p+page,4));
 assert(VirtualFree(p,0,MEM_RELEASE));assert(!read(p,4));
 // Timings are a local Windows API benchmark, not a game FPS prediction.
 constexpr std::size_t size=256u*1024u*1024u;
 p=static_cast<unsigned char*>(VirtualAlloc(nullptr,size,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE));assert(p);
 for(std::size_t i=0;i<size;i+=page)p[i]=1;
 auto bench=[&](bool resident){const auto begin=std::chrono::steady_clock::now();for(unsigned i=0;i<200;++i){if(resident)assert(read(p,8));else assert(VirtualQuery(p,&mbi,sizeof(mbi)));}return std::chrono::duration<double,std::micro>(std::chrono::steady_clock::now()-begin).count()/200;};
 const auto regionUs=bench(false),pageUs=bench(true);
 std::printf("PASS Windows resident/cross-page/guard/noaccess/unmapped checks; 256MiB region VirtualQuery=%.3fus addressed-page query=%.3fus (200 each)\n",regionUs,pageUs);
 assert(VirtualFree(p,0,MEM_RELEASE));
}
