// C29 release mode (KCD2VR_PROXY_DINPUT8=ON): KCD2VR is itself dinput8.dll. WHGame.dll imports DINPUT8.dll, windows
// loads ours from the game folder first, we forward the 6 real exports to the system dinput8.dll and start the mod
// like the asi version does. no asi loader needed. other *.asi files in the game folder (and scripts\, plugins\) are
// loaded by us so people with other asi mods lose nothing. our own old KCD2VR*.asi is skipped (no double mod).
#include <windows.h>
#include <unknwn.h>
#include <cwchar>
#include <cstdio>
#include "diagnostic_trace.h"
#include "proxy_dinput8_29.h"
#include "diagnostics/startup_batch32.h"

namespace {
HMODULE g_real29 = nullptr;
INIT_ONCE g_once29 = INIT_ONCE_STATIC_INIT;
BOOL CALLBACK LoadReal29(PINIT_ONCE, PVOID, PVOID*) {
    wchar_t path[MAX_PATH]{};
    const UINT n = GetSystemDirectoryW(path, MAX_PATH);
    if (n == 0 || n >= MAX_PATH - 16) return FALSE;
    wcscat_s(path, L"\\dinput8.dll");
    g_real29 = LoadLibraryW(path); // lazy system export resolution; our DllMain does not call it
    return g_real29 != nullptr; // InitOnce may retry a failed load; retain successful module for process life
}
FARPROC Real29(const char* name) {
    InitOnceExecuteOnce(&g_once29, LoadReal29, nullptr, nullptr);
    return g_real29 ? GetProcAddress(g_real29, name) : nullptr;
}
} // namespace

extern "C" HRESULT WINAPI Proxy_DirectInput8Create(HINSTANCE inst, DWORD version, REFIID riid, LPVOID* out, LPUNKNOWN outer) {
    using Fn = HRESULT(WINAPI*)(HINSTANCE, DWORD, REFIID, LPVOID*, LPUNKNOWN);
    const auto f = reinterpret_cast<Fn>(Real29("DirectInput8Create"));
    if (!f) { if (out) *out = nullptr; return E_FAIL; }
    return f(inst, version, riid, out, outer);
}
// Exact named System32 export returns its constant joystick data-format pointer.
// Keep the format opaque; the proxy does not construct or interpret it.
extern "C" const void* WINAPI Proxy_GetdfDIJoystick() {
    using Fn = const void*(WINAPI*)();
    const auto f = reinterpret_cast<Fn>(Real29("GetdfDIJoystick"));
    return f ? f() : nullptr;
}
extern "C" HRESULT WINAPI Proxy_DllCanUnloadNow() { return S_FALSE; } // the mod is never unloaded
extern "C" HRESULT WINAPI Proxy_DllGetClassObject(REFCLSID clsid, REFIID riid, LPVOID* out) {
    using Fn = HRESULT(WINAPI*)(REFCLSID, REFIID, LPVOID*);
    const auto f = reinterpret_cast<Fn>(Real29("DllGetClassObject"));
    if (!f) { if (out) *out = nullptr; return CLASS_E_CLASSNOTAVAILABLE; }
    return f(clsid, riid, out);
}
extern "C" HRESULT WINAPI Proxy_DllRegisterServer() {
    using Fn = HRESULT(WINAPI*)(); const auto f = reinterpret_cast<Fn>(Real29("DllRegisterServer")); return f ? f() : E_FAIL;
}
extern "C" HRESULT WINAPI Proxy_DllUnregisterServer() {
    using Fn = HRESULT(WINAPI*)(); const auto f = reinterpret_cast<Fn>(Real29("DllUnregisterServer")); return f ? f() : E_FAIL;
}

namespace kcd2vr::proxy29 {
namespace {
diagnostics::StartupBatch32<> startup32;
bool scanned32=false;
unsigned loaded32=0,skipped32=0,failed32=0;
void Record32(const char* message) noexcept {(void)startup32.Add(message);}
}
// Called once on the existing startup worker. Results wait for logger admission;
// no trace-driven rescan and no plugin enumeration/file IO during gameplay.
void LoadAsiPlugins() noexcept {
    if(scanned32)return;scanned32=true;
    wchar_t self[MAX_PATH]{};HMODULE me=nullptr;
    if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                            reinterpret_cast<LPCWSTR>(&LoadAsiPlugins), &me)){
        ++failed32;Record32("C32 PROXY_STARTUP ERROR stage=MODULE_HANDLE");return;}
    const DWORD length32=GetModuleFileNameW(me,self,MAX_PATH);
    if(!length32||length32>=MAX_PATH){++failed32;Record32("C32 PROXY_STARTUP ERROR stage=MODULE_PATH_TRUNCATED_OR_UNAVAILABLE");return;}
    wchar_t* slash=wcsrchr(self,L'\\');if(!slash){++failed32;Record32("C32 PROXY_STARTUP ERROR stage=MODULE_DIRECTORY");return;}*(slash+1)=0;
    const wchar_t* subdirs[]={L"",L"scripts\\",L"plugins\\"};
    for(const auto* sub:subdirs){
        wchar_t pattern[MAX_PATH]{};
        if(_snwprintf_s(pattern,MAX_PATH,_TRUNCATE,L"%ls%ls*.asi",self,sub)<0){
            ++failed32;Record32("C32 PROXY_STARTUP ERROR stage=ENUM_PATH_TOO_LONG");continue;}
        WIN32_FIND_DATAW fd{};HANDLE h=FindFirstFileW(pattern,&fd);
        if(h==INVALID_HANDLE_VALUE){
            const DWORD error=GetLastError();
            if(error!=ERROR_FILE_NOT_FOUND&&error!=ERROR_PATH_NOT_FOUND&&error!=ERROR_SUCCESS){
                ++failed32;char line[160]{};std::snprintf(line,sizeof(line),"C32 PROXY_STARTUP ERROR stage=ENUMERATE error=%lu",error);Record32(line);}
            continue;}

        do{
            if(fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)continue;
            char name8[MAX_PATH]{};
            if(!WideCharToMultiByte(CP_UTF8,0,fd.cFileName,-1,name8,MAX_PATH,nullptr,nullptr))
                std::snprintf(name8,sizeof(name8),"<name_not_representable>");
            char message[512]{};
            // Retained compatibility policy, not a claim of PE/module identity.
            if(_wcsnicmp(fd.cFileName,L"KCD2VR",6)==0){
                ++skipped32;std::snprintf(message,sizeof(message),"C32 PROXY_STARTUP ASI result=SKIPPED name=%s policy=KCD2VR_PREFIX",name8);
                Record32(message);continue;}
            wchar_t full[MAX_PATH]{};
            if(_snwprintf_s(full,MAX_PATH,_TRUNCATE,L"%ls%ls%ls",self,sub,fd.cFileName)<0){
                ++failed32;std::snprintf(message,sizeof(message),"C32 PROXY_STARTUP ASI result=REFUSED name=%s reason=PATH_TOO_LONG",name8);Record32(message);continue;}
            const HMODULE module=LoadLibraryW(full);const DWORD error=module?0:GetLastError();
            if(module)++loaded32;else++failed32;
            std::snprintf(message,sizeof(message),"C32 PROXY_STARTUP ASI result=%s name=%s error=%lu",module?"LOADED":"FAILED",name8,error);Record32(message);
        }while(FindNextFileW(h,&fd));
        FindClose(h);
    }
}
bool StartupRecordsPending32() noexcept {return scanned32&&!startup32.flushed;}
void FlushStartupRecords32() noexcept {
    if(!scanned32||!startup32.Flush(diagnostic_trace::Enabled(),[](const char* line){diagnostic_trace::Write(line);}))return;
    char message[224]{};std::snprintf(message,sizeof(message),
        "C32 PROXY_STARTUP SUMMARY loaded=%u failed=%u skipped=%u retainedModuleRefs=PROCESS_LIFETIME source=ONE_STARTUP_SCAN",
        loaded32,failed32,skipped32);diagnostic_trace::Write(message);
}
}
