#pragma once
// Syntax-only host shim. NOT the Windows SDK; no Windows/native behavior is tested.
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <cwchar>
#include <cstdio>
#define __fastcall
#define __try try
#define __except(x) catch(...)
#define EXCEPTION_EXECUTE_HANDLER 1
using DWORD=std::uint32_t;using WORD=std::uint16_t;using BYTE=std::uint8_t;using UINT=unsigned;using ULONG_PTR=std::uintptr_t;using HMODULE=void*;using HANDLE=void*;using LPCWSTR=const wchar_t*;
constexpr DWORD MEM_COMMIT=0x1000,PAGE_NOACCESS=1,PAGE_GUARD=0x100,PAGE_EXECUTE=0x10,PAGE_EXECUTE_READ=0x20,PAGE_EXECUTE_READWRITE=0x40,PAGE_EXECUTE_WRITECOPY=0x80;
constexpr int MAX_PATH=260,GET_MODULE_HANDLE_EX_FLAG_PIN=1,GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS=4,GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT=2;
constexpr DWORD GENERIC_WRITE=0x40000000,FILE_SHARE_READ=1,FILE_SHARE_WRITE=2,CREATE_ALWAYS=2,FILE_ATTRIBUTE_NORMAL=128;
#define INVALID_HANDLE_VALUE reinterpret_cast<HANDLE>(std::intptr_t(-1))
struct MEMORY_BASIC_INFORMATION{void* BaseAddress;std::size_t RegionSize;DWORD State,Protect;};
struct IMAGE_DOS_HEADER{std::uint16_t e_magic;char unused[58];std::int32_t e_lfanew;};
struct IMAGE_NT_HEADERS64{DWORD Signature;struct{WORD Machine,NumberOfSections;DWORD TimeDateStamp;}FileHeader;};
constexpr DWORD IMAGE_DOS_SIGNATURE=0x5a4d,IMAGE_NT_SIGNATURE=0x4550;
inline std::uint64_t hostTickCount64=10000;
inline std::uint64_t GetTickCount64(){
#ifdef KCD2VR_POSE_FIXTURE_CLOCK
return KCD2VR_POSE_FIXTURE_CLOCK;
#else
return hostTickCount64;
#endif
}inline DWORD GetCurrentThreadId(){return 1;}inline HMODULE hostModuleHandleW=nullptr;inline HMODULE GetModuleHandleW(LPCWSTR){return hostModuleHandleW;}
inline std::size_t VirtualQuery(const void* p,MEMORY_BASIC_INFORMATION* m,std::size_t){m->BaseAddress=const_cast<void*>(p);m->RegionSize=1<<20;m->State=MEM_COMMIT;m->Protect=PAGE_EXECUTE_READWRITE;return sizeof(*m);}
inline void Sleep(DWORD){}
struct INPUT{DWORD type;struct{WORD wVk,wScan;DWORD dwFlags,time;ULONG_PTR dwExtraInfo;}ki;struct{long dx,dy;DWORD mouseData,dwFlags,time;ULONG_PTR dwExtraInfo;}mi;};
constexpr DWORD INPUT_KEYBOARD=1,INPUT_MOUSE=0,KEYEVENTF_KEYUP=2,KEYEVENTF_SCANCODE=8,KEYEVENTF_EXTENDEDKEY=1,MAPVK_VK_TO_VSC=0,MOUSEEVENTF_LEFTDOWN=2,MOUSEEVENTF_LEFTUP=4,MOUSEEVENTF_RIGHTDOWN=8,MOUSEEVENTF_RIGHTUP=16,MOUSEEVENTF_MOVE=1,MOUSEEVENTF_WHEEL=0x800;
inline UINT MapVirtualKeyW(UINT v,UINT){return v;}inline UINT SendInput(UINT n,INPUT*,int){return n;}
constexpr WORD VK_LBUTTON=1,VK_ESCAPE=27,VK_RETURN=13,VK_SPACE=32,VK_TAB=9,VK_LEFT=0x25,VK_UP=0x26,VK_RIGHT=0x27,VK_DOWN=0x28;
constexpr int WHEEL_DELTA=120;
inline DWORD GetLastError(){return 0;}
#ifndef _MSC_VER
inline int _stricmp(const char*a,const char*b){return std::strcmp(a,b);}inline int _strnicmp(const char*a,const char*b,std::size_t n){return std::strncmp(a,b,n);}
#endif
inline unsigned hostModulePinCalls=0;
#if defined(KCD2VR_NPC_LOOK_TEST63)
inline bool hostModulePinAllowed63=true;
inline LPCWSTR hostModulePinAddress63=nullptr;
#endif
inline int GetModuleHandleExW(DWORD flags,LPCWSTR address,HMODULE* out){
#if defined(KCD2VR_NPC_LOOK_TEST63)
 if(flags==(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_PIN)){
  ++hostModulePinCalls;hostModulePinAddress63=address;*out=hostModulePinAllowed63?hostModuleHandleW:nullptr;return *out!=nullptr;}
#endif
 if(flags==GET_MODULE_HANDLE_EX_FLAG_PIN){++hostModulePinCalls;*out=hostModuleHandleW;return *out!=nullptr;}return 0;}inline DWORD GetModuleFileNameW(HMODULE,wchar_t*,DWORD){return 0;}
struct FILETIME{DWORD dwLowDateTime,dwHighDateTime;};struct SYSTEMTIME{WORD wYear,wMonth,wDayOfWeek,wDay,wHour,wMinute,wSecond,wMilliseconds;};
inline void GetSystemTimeAsFileTime(FILETIME* p){*p={};}inline int FileTimeToLocalFileTime(const FILETIME*a,FILETIME*b){*b=*a;return 1;}inline int FileTimeToSystemTime(const FILETIME*,SYSTEMTIME*b){*b={};return 1;}
inline HANDLE CreateFileW(LPCWSTR,DWORD,DWORD,void*,DWORD,DWORD,HANDLE){return nullptr;}inline int WriteFile(HANDLE,const void*,DWORD n,DWORD*w,void*){*w=n;return 1;}inline int CloseHandle(HANDLE){return 1;}
#ifndef _MSC_VER
template<std::size_t N>int wcscat_s(wchar_t(&a)[N],const wchar_t*b){std::wcsncat(a,b,N-std::wcslen(a)-1);return 0;}
#endif

#include <algorithm>
#include <map>
#include <string>
inline std::map<std::string,std::string> hostIni;
inline bool hostIniFailWrites=false;
inline DWORD GetPrivateProfileSectionNamesA(char*out,DWORD capacity,const char*path){
 std::string joined,last;const std::string prefix=std::string(path?path:"")+"|";
 for(const auto& kv:hostIni){if(kv.first.compare(0,prefix.size(),prefix)!=0)continue;
  const auto end=kv.first.find('|',prefix.size());if(end==std::string::npos)continue;
  const auto name=kv.first.substr(prefix.size(),end-prefix.size());if(name==last)continue;
  joined+=name;joined.push_back('\0');last=name;
 }
 joined.push_back('\0');if(capacity<2)return 0;
 if(joined.size()>capacity){std::memcpy(out,joined.data(),capacity-2);out[capacity-2]=out[capacity-1]=0;return capacity-2;}
 std::memcpy(out,joined.data(),joined.size());return joined.size()-1;
}

inline DWORD GetPrivateProfileStringA(const char*s,const char*k,const char*d,char*out,DWORD n,const char*p){
 const std::string key=std::string(p?p:"")+"|"+(s?s:"")+"|"+(k?k:"");auto it=hostIni.find(key);std::string value=it==hostIni.end()?(d?d:""):it->second;
 if(!n)return 0;const auto len=std::min<std::size_t>(value.size(),n-1);std::memcpy(out,value.data(),len);out[len]=0;return DWORD(len);
}
inline int WritePrivateProfileStringA(const char*s,const char*k,const char*v,const char*p){
 if(!s&&!k&&!v)return 0; // Win32 flush convention (zero is not a copy failure here).
 if(hostIniFailWrites||!s||!k||!v)return 0;hostIni[std::string(p?p:"")+"|"+s+"|"+k]=v;return 1;
}
