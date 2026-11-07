#pragma once
// Production-TU host fixtures exercise the existing VirtualQuery fallback.
// The real Windows resident-page path has a separate OS-backed executable.
#include "windows.h"
struct SYSTEM_INFO { unsigned long dwPageSize=4096; };
inline void GetSystemInfo(SYSTEM_INFO* p){p->dwPageSize=4096;}
inline void* GetCurrentProcess(){return reinterpret_cast<void*>(-1);}
struct PSAPI_WORKING_SET_EX_INFORMATION {
    void* VirtualAddress=nullptr;
    struct {unsigned Valid=0,Bad=0,Win32Protection=0;} VirtualAttributes;
};
inline int QueryWorkingSetEx(void*,void*,unsigned long){return 0;}
#ifndef PAGE_READONLY
#define PAGE_READONLY 2
#define PAGE_READWRITE 4
#define PAGE_WRITECOPY 8
#define PAGE_EXECUTE_READ 32
#define PAGE_EXECUTE_WRITECOPY 128
#endif
