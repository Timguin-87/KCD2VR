// Executes actual proxy scanner/forwarders, diagnostic queue/filter/drain and
// extracted actual startup worker. Only Win32/module/game leaves are replaced.
// No product DLL or external ASI is loaded into this test process.
#include <windows.h>
#include "host_test_runtime.h"
#include <unknwn.h>
#include <cassert>
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <cwchar>
#include <map>
#include <string>
#include <vector>

namespace fixture32 {
struct Entry {std::wstring name;DWORD flags=0;};
struct Cursor {const std::vector<Entry>* files=nullptr;std::size_t index=0;};
static std::map<std::wstring,std::vector<Entry>> files;
static std::map<std::wstring,DWORD> enumerationErrors;
static std::map<HANDLE,Cursor> cursors;
static std::vector<std::wstring> loaded;
static std::vector<std::string> output,order;
static std::wstring modulePath=L"C:\\fixture\\dinput8.dll";
static std::wstring logPath;
static bool moduleOk=true,pathMissing=false,loggerOk=true,realOk=true,encodeOk=true;
static DWORD lastError=0;static unsigned scans=0,closes=0,systemLoads=0,procCalls=0,initCalls=0;
static bool once=false,createExport=true;static std::uintptr_t nextHandle=100;
static unsigned fileOpens=0;static std::uint64_t currentTick=10000;
static unsigned calls=0,checks=0;static void* token=reinterpret_cast<void*>(0x1234);
static void ck(bool b,const char* what){++checks;std::printf("%s %s\n",b?"PASS":"FAIL",what);std::fflush(stdout);assert(b);}
static bool has(const char* text){for(const auto& s:output)if(s.find(text)!=std::string::npos)return true;return false;}
static BOOL WINAPI Module(DWORD,LPCWSTR,HMODULE* out){if(!moduleOk)return FALSE;*out=reinterpret_cast<HMODULE>(1);return TRUE;}
static DWORD WINAPI Path(HMODULE,LPWSTR dst,DWORD size){
    if(pathMissing)return 0;const auto n=std::min<std::size_t>(modulePath.size(),size-1);
    std::wmemcpy(dst,modulePath.data(),n);dst[n]=0;return modulePath.size()>=size?size:DWORD(n);
}
static UINT WINAPI SystemDir(LPWSTR dst,UINT size){const wchar_t* p=L"C:\\Windows\\System32";wcscpy_s(dst,size,p);return UINT(wcslen(p));}
static HMODULE WINAPI Load(LPCWSTR name){
    const std::wstring p=name;
    if(p.find(L"System32\\dinput8.dll")!=std::wstring::npos){++systemLoads;return realOk?reinterpret_cast<HMODULE>(2):nullptr;}
    loaded.push_back(p);order.push_back("plugin");
    if(p.find(L"fail.asi")!=std::wstring::npos){lastError=ERROR_BAD_EXE_FORMAT;return nullptr;}
    return reinterpret_cast<HMODULE>(3);
}
static BOOL WINAPI Once(PINIT_ONCE,PINIT_ONCE_FN fn,PVOID value,LPVOID* context){
    ++initCalls;if(once)return TRUE;if(!fn(nullptr,value,context))return FALSE;once=true;return TRUE;
}
static HRESULT WINAPI Create(HINSTANCE,DWORD,REFIID,LPVOID* out,LPUNKNOWN){++calls;if(out)*out=token;return S_OK;}
static const void* WINAPI Format(){++calls;return token;}
static HRESULT WINAPI Class(REFCLSID,REFIID,LPVOID* out){++calls;if(out)*out=token;return S_OK;}
static HRESULT WINAPI Register(){++calls;return S_OK;}
static FARPROC WINAPI Proc(HMODULE,const char* name){
    ++procCalls;if(!std::strcmp(name,"DirectInput8Create"))return createExport?reinterpret_cast<FARPROC>(&Create):nullptr;
    if(!std::strcmp(name,"GetdfDIJoystick"))return reinterpret_cast<FARPROC>(&Format);
    if(!std::strcmp(name,"DllGetClassObject"))return reinterpret_cast<FARPROC>(&Class);
    return reinterpret_cast<FARPROC>(&Register);
}
static void Fill(WIN32_FIND_DATAW* fd,const Entry& e){*fd={};fd->dwFileAttributes=e.flags;wcscpy_s(fd->cFileName,e.name.c_str());}
static HANDLE WINAPI First(LPCWSTR pattern,WIN32_FIND_DATAW* fd){
    ++scans;const auto error=enumerationErrors.find(pattern);
    if(error!=enumerationErrors.end()){lastError=error->second;return INVALID_HANDLE_VALUE;}
    const auto it=files.find(pattern);if(it==files.end()||it->second.empty()){lastError=ERROR_FILE_NOT_FOUND;return INVALID_HANDLE_VALUE;}
    const auto h=reinterpret_cast<HANDLE>(++nextHandle);cursors[h]={&it->second,0};Fill(fd,it->second.front());return h;
}
static BOOL WINAPI Next(HANDLE h,WIN32_FIND_DATAW* fd){auto& c=cursors.at(h);if(++c.index>=c.files->size()){lastError=ERROR_NO_MORE_FILES;return FALSE;}Fill(fd,(*c.files)[c.index]);return TRUE;}
static BOOL WINAPI CloseFind(HANDLE h){++closes;cursors.erase(h);return TRUE;}
static int WINAPI Encode(UINT,DWORD,LPCWCH src,int,LPSTR dst,int size,LPCCH,LPBOOL){
    if(!encodeOk)return 0;const auto n=wcslen(src);if(n+1>std::size_t(size))return 0;
    for(std::size_t i=0;i<=n;++i)dst[i]=src[i]<128?char(src[i]):'?';return int(n+1);
}
static DWORD WINAPI Error(){return lastError;}
static HANDLE WINAPI File(LPCWSTR path,DWORD,DWORD,LPSECURITY_ATTRIBUTES,DWORD,DWORD,HANDLE){++fileOpens;logPath=path;return loggerOk?reinterpret_cast<HANDLE>(4):INVALID_HANDLE_VALUE;}
static BOOL WINAPI Write(HANDLE,LPCVOID data,DWORD n,LPDWORD written,LPOVERLAPPED){output.emplace_back(static_cast<const char*>(data),n);*written=n;return TRUE;}
static BOOL WINAPI Close(HANDLE){return TRUE;}
static void WINAPI Time(LPFILETIME ft){ft->dwLowDateTime=100;ft->dwHighDateTime=0;}
static BOOL WINAPI Local(const FILETIME* in,FILETIME* out){*out=*in;return TRUE;}
static BOOL WINAPI Calendar(const FILETIME*,LPSYSTEMTIME out){*out={};out->wYear=2026;out->wMonth=10;out->wDay=16;return TRUE;}
static ULONGLONG WINAPI Clock(){return currentTick;}
}

#define GetModuleHandleExW fixture32::Module
#define GetModuleFileNameW fixture32::Path
#define GetSystemDirectoryW fixture32::SystemDir
#define LoadLibraryW fixture32::Load
#define InitOnceExecuteOnce fixture32::Once
#define GetProcAddress fixture32::Proc
#define FindFirstFileW fixture32::First
#define FindNextFileW fixture32::Next
#define FindClose fixture32::CloseFind
#define WideCharToMultiByte fixture32::Encode
#define GetLastError fixture32::Error
#define CreateFileW fixture32::File
#define WriteFile fixture32::Write
#define CloseHandle fixture32::Close
#define GetSystemTimeAsFileTime fixture32::Time
#define FileTimeToLocalFileTime fixture32::Local
#define FileTimeToSystemTime fixture32::Calendar
#define GetTickCount64 fixture32::Clock
#include "../src/diagnostic_trace.cpp"
#include "../src/proxy_dinput8_29.cpp"
#define KCD2VR_PROXY_DINPUT8 1
namespace kcd2vr::companion_loader59c4 {static bool Install(){fixture32::order.push_back("companion");return true;}}
namespace kcd2vr {
struct Mod {
    static Mod& Instance(){static Mod instance;return instance;}
    void Run(){fixture32::order.push_back("Run");diagnostic_trace::SetEnabled(true);proxy29::FlushStartupRecords32();}
};
}
#include "generated_proxy_worker32.inl"

static void Reset32(){
    using namespace fixture32;
    kcd2vr::diagnostic_trace::SetEnabled(false);output.clear();files.clear();enumerationErrors.clear();cursors.clear();loaded.clear();order.clear();
    modulePath=L"C:\\fixture\\dinput8.dll";logPath.clear();moduleOk=true;pathMissing=false;loggerOk=true;realOk=true;encodeOk=true;
    lastError=0;scans=closes=0;
    fileOpens=0;currentTick=10000;
    kcd2vr::proxy29::startup32={};kcd2vr::proxy29::scanned32=false;
    kcd2vr::proxy29::loaded32=kcd2vr::proxy29::failed32=kcd2vr::proxy29::skipped32=0;
    kcd2vr::diagnostics::capture168::gate.mask=0;
}
static void RetryActual32(kcd2vr::diagnostics::StartupRetry32& startupRetry32,std::uint64_t now){
    using namespace kcd2vr;fixture32::currentTick=now;
#include "generated_proxy_retry32.inl"
}
int main(int argc,char**){
    using namespace fixture32;using namespace kcd2vr;
    if(argc>1){ // isolated reproducer; parent repairs the actual logger path
        Reset32();modulePath=L"C:\\"+std::wstring(243,L'a')+L"\\dinput8.dll";
        std::puts("PROBE valid near-MAX_PATH module + native diagnostic filename append");std::fflush(stdout);
        diagnostic_trace::SetEnabled(true);diagnostic_trace::SetEnabled(false);return 0;
    }
    Reset32();files[L"C:\\fixture\\*.asi"]={{L"first.asi"},{L"KCD2VR_old.asi"},{L"fail.asi"},{L"folder.asi",FILE_ATTRIBUTE_DIRECTORY}};
    files[L"C:\\fixture\\scripts\\*.asi"]={{L"second.asi"}};
    files[L"C:\\fixture\\plugins\\*.asi"]={{L"third.asi"}};
    ck(ModThreadMain(nullptr)==0,"actual worker returns normally");
    ck(scans==3&&closes==3&&loaded.size()==4,"actual scanner visits exactly three directories and closes handles");
    ck(order.size()==6&&order[4]=="companion"&&order[5]=="Run","actual worker scans before companion/Run logger admission");
    diagnostic_trace::Drain(true);
    ck(has("SUMMARY loaded=3 failed=1 skipped=1"),"actual quiet logger admits exact startup totals");
    ck(has("FAILED name=fail.asi error=193")&&has("SKIPPED name=KCD2VR_old.asi"),"native loader failure and duplicate-prefix skip retained");
    ck(has("BUFFER emitted=5 omitted=0"),"bounded receipt batch reaches actual queue/drain");
    const auto lines=output.size();proxy29::FlushStartupRecords32();proxy29::LoadAsiPlugins();diagnostic_trace::Drain(true);
    ck(scans==3&&output.size()==lines&&loaded.size()==4,"repeated flush/load cannot rescan or repeat summary");

    Reset32();files[L"C:\\fixture\\*.asi"]={{L"first.asi"}};loggerOk=false;
    proxy29::LoadAsiPlugins();proxy29::FlushStartupRecords32();
    ck(!proxy29::startup32.flushed&&output.empty(),"disabled logger does not consume scanned records");
    diagnostic_trace::SetEnabled(true);proxy29::FlushStartupRecords32();
    ck(!diagnostic_trace::Enabled()&&!proxy29::startup32.flushed,"failed file admission leaves records pending");
    loggerOk=true;diagnostic_trace::SetEnabled(true);proxy29::FlushStartupRecords32();diagnostic_trace::Drain(true);
    ck(has("SUMMARY loaded=1 failed=0 skipped=0")&&scans==3,"later successful admission flushes once without scan");
    diagnostic_trace::Write("C32 GRAPHICS_STATE shadows=1 eye=0");diagnostic_trace::Drain(true);
    ck(!has("GRAPHICS_STATE"),"actual quiet capture filter rejects periodic graphics state");
    diagnostic_trace::Write("C32 GRAPHICS_OPTION shadows=1 eyeSteady=0");diagnostic_trace::Drain(true);
    ck(has("GRAPHICS_OPTION"),"actual quiet capture filter retains option edges");
    Reset32();files[L"C:\\fixture\\*.asi"]={{L"first.asi"}};loggerOk=false;
    proxy29::LoadAsiPlugins();diagnostic_trace::SetEnabled(true);
    diagnostics::StartupRetry32 retry{};const auto initialOpens=fileOpens;
    RetryActual32(retry,10000);RetryActual32(retry,10499);
    ck(fileOpens==initialOpens+1&&retry.attempts==1&&proxy29::StartupRecordsPending32(),
        "actual Mod retry call waits 500ms after failed logger admission");
    loggerOk=true;RetryActual32(retry,10500);diagnostic_trace::Drain(true);
    ck(retry.done&&!proxy29::StartupRecordsPending32()&&has("SUMMARY loaded=1 failed=0 skipped=0")&&scans==3,
        "actual Mod retry call admits and flushes pending scan once");
    const auto retriedLines=output.size();const auto retriedOpens=fileOpens;
    RetryActual32(retry,100000);diagnostic_trace::Drain(true);
    ck(fileOpens==retriedOpens&&output.size()==retriedLines&&scans==3,"completed actual Mod retry becomes no-op");
    Reset32();proxy29::LoadAsiPlugins();loggerOk=false;diagnostics::StartupRetry32 exhausted{};
    for(auto t:{10000ull,10500ull,11000ull,11500ull,100000ull})RetryActual32(exhausted,t);
    ck(exhausted.done&&exhausted.attempts==3&&fileOpens==3&&proxy29::StartupRecordsPending32()&&scans==3,
        "actual Mod retry exhausts after three attempts retaining undispatched records");

    Reset32();auto& many=files[L"C:\\fixture\\*.asi"];
    for(unsigned i=0;i<40;++i)many.push_back({L"plugin"+std::to_wstring(i)+L".asi"});
    proxy29::LoadAsiPlugins();diagnostic_trace::SetEnabled(true);proxy29::FlushStartupRecords32();diagnostic_trace::Drain(true);
    ck(loaded.size()==40&&has("BUFFER emitted=32 omitted=8")&&has("SUMMARY loaded=40 failed=0 skipped=0"),
       "actual scanner counts all loads with bounded overflow receipts");

    Reset32();moduleOk=false;proxy29::LoadAsiPlugins();diagnostic_trace::SetEnabled(true);proxy29::FlushStartupRecords32();diagnostic_trace::Drain(true);
    ck(scans==0&&has("stage=MODULE_HANDLE")&&has("failed=1"),"module failure refuses scan and reports total");
    Reset32();modulePath=std::wstring(MAX_PATH+5,L'x');proxy29::LoadAsiPlugins();diagnostic_trace::SetEnabled(true);proxy29::FlushStartupRecords32();diagnostic_trace::Drain(true);
    ck(scans==0&&loaded.empty()&&has("MODULE_PATH_TRUNCATED_OR_UNAVAILABLE"),"truncated module path never loads partial path");
    Reset32();modulePath=L"dinput8.dll";proxy29::LoadAsiPlugins();diagnostic_trace::SetEnabled(true);proxy29::FlushStartupRecords32();diagnostic_trace::Drain(true);
    ck(scans==0&&has("stage=MODULE_DIRECTORY"),"no-directory path refuses scan");
    Reset32();enumerationErrors[L"C:\\fixture\\scripts\\*.asi"]=ERROR_ACCESS_DENIED;
    proxy29::LoadAsiPlugins();diagnostic_trace::SetEnabled(true);proxy29::FlushStartupRecords32();diagnostic_trace::Drain(true);
    ck(scans==3&&has("stage=ENUM")&&has("error=5")&&has("failed=1"),
        "actual scanner distinguishes denied directory from empty or missing directory");
    Reset32();modulePath=L"C:\\"+std::wstring(243,L'a')+L"\\dinput8.dll";
    const auto prefix=modulePath.substr(0,modulePath.find_last_of(L'\\')+1);files[prefix+L"*.asi"]={{L"very_long_plugin_name_that_does_not_fit.asi"}};
    proxy29::LoadAsiPlugins();modulePath=L"C:\\fixture\\dinput8.dll"; // isolate scanner from separately reported legacy log-path append overflow
    diagnostic_trace::SetEnabled(true);proxy29::FlushStartupRecords32();diagnostic_trace::Drain(true);
    ck(loaded.empty()&&has("reason=PATH_TOO_LONG")&&has("stage=ENUM_PATH_TOO_LONG"),"overlong plugin/directory paths are refused");
    Reset32();encodeOk=false;files[L"C:\\fixture\\*.asi"]={{L"unicode.asi"}};
    proxy29::LoadAsiPlugins();diagnostic_trace::SetEnabled(true);proxy29::FlushStartupRecords32();diagnostic_trace::Drain(true);
    ck(loaded.size()==1&&has("name=<name_not_representable>"),"diagnostic encoding failure does not alter wide native load path");
    Reset32();modulePath=L"C:\\"+std::wstring(243,L'a')+L"\\dinput8.dll";
    diagnostic_trace::SetEnabled(true);
    ck(diagnostic_trace::Enabled()&&logPath==L"KCD2VR-diagnostics.log", "actual logger uses safe relative fallback if log suffix cannot fit");
    Reset32();modulePath=L"C:\\"+std::wstring(MAX_PATH,L'a')+L"\\dinput8.dll";
    diagnostic_trace::SetEnabled(true);
    ck(diagnostic_trace::Enabled()&&logPath==L"KCD2VR-diagnostics.log", "truncated native module path cannot reach logger append");
    Reset32();modulePath=L"C:\\"+std::wstring(MAX_PATH-sizeof(L"KCD2VR-diagnostics.log")/sizeof(wchar_t)-4,L'a')+L"\\dinput8.dll";
    diagnostic_trace::SetEnabled(true);
    ck(diagnostic_trace::Enabled()&&logPath.size()==MAX_PATH-1&&logPath.find(L"C:\\")==0,
        "exact-fit native directory retains absolute log path with final terminator");

    g_real29=nullptr;once=false;realOk=false;systemLoads=0;void* out=token;const IID iid{};
    ck(Proxy_DirectInput8Create(nullptr,0,iid,&out,nullptr)==E_FAIL&&out==nullptr,"missing system module fails create with cleared output");
    realOk=true;ck(Proxy_DirectInput8Create(nullptr,0,iid,&out,nullptr)==S_OK&&out==token&&systemLoads==2,"failed lazy module initialization can retry");
    ck(Proxy_GetdfDIJoystick()==token&&Proxy_DllCanUnloadNow()==S_FALSE,"opaque joystick pointer forwarded and mod stays resident");
    ck(Proxy_DllGetClassObject(iid,iid,&out)==S_OK&&out==token&&Proxy_DllRegisterServer()==S_OK&&Proxy_DllUnregisterServer()==S_OK,
       "all remaining export entry points retain forwarding behavior");
    ck(systemLoads==2,"successful system module resolution is retained for process lifetime");
    createExport=false;out=token;
    ck(Proxy_DirectInput8Create(nullptr,0,iid,&out,nullptr)==E_FAIL&&out==nullptr,"missing named export refuses without stale output");
    diagnostic_trace::SetEnabled(false);
    std::printf("proxy_startup32=%u PASS actual_proxy_scanner_worker_trace=YES Win32_modules=MOCKS actual_ASI_load=NONE\n",checks);
}
