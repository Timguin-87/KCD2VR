// Windows SDK and real SEH: unlike syntax-shim fixtures this executes memory
// access faults inside the actual extracted production native-read leaf.
#include <windows.h>
#include <cstring>
#include <cassert>
#include <cstdio>
#include "npc_look_read63.inc"
int main(){
    int original=63,value=0;assert(ReadTarget47c1(&original,value)&&value==63);
    assert(!ReadTarget47c1(nullptr,value));
    assert(!ReadTarget47c1(reinterpret_cast<void*>(1),value));
    SYSTEM_INFO system{};GetSystemInfo(&system);
    auto* page=static_cast<unsigned char*>(VirtualAlloc(nullptr,2*system.dwPageSize,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));
    assert(page);DWORD old=0;assert(VirtualProtect(page+system.dwPageSize,system.dwPageSize,PAGE_NOACCESS,&old));
    assert(!ReadTarget47c1(page+system.dwPageSize,value));
    assert(!ReadTarget47c1(page+system.dwPageSize-2,value));
    assert(VirtualFree(page,0,MEM_RELEASE));
    std::puts("npc_look63_fault: real Windows SEH valid/null/unmapped/protected/cross-page native reads PASS");
}
