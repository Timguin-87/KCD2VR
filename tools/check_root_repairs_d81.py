"""Exercise D81 map lookup, single-file logger and walking/dodge host contracts.

Uses production function bodies and explicit Windows/native leaves. This is
neither a Windows build nor a headset/rendering test. Outputs stay private.
"""
from pathlib import Path
import argparse
import json
import subprocess
import sys


def function(text, signature):
    start = text.index(signature)
    end = text.index("{", start) + 1
    depth = 1
    while depth:
        depth += (text[end] == "{") - (text[end] == "}")
        end += 1
    return text[start:end]


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("--source", type=Path, default=Path(__file__).resolve().parents[1])
    p.add_argument("--out", type=Path, required=True)
    args = p.parse_args()
    root, out = args.source.resolve(), args.out.resolve()
    out.mkdir(parents=True, exist_ok=True)
    results = {}

    def run(name, body, extra=(), expected=0):
        cpp = out / (name + ".cpp")
        cpp.write_text(body)
        exe = out / name
        command = ["g++", "-std=c++20", "-O2", "-pthread", "-I" + str(root / "src"),
                   *extra, str(cpp), "-o", str(exe)]
        built = subprocess.run(command, capture_output=True, text=True)
        (out / (name + ".build.txt")).write_text(built.stdout + built.stderr)
        if built.returncode:
            raise RuntimeError(name + " compile failed:\n" + built.stderr[-5000:])
        done = subprocess.run([str(exe)], capture_output=True, text=True)
        (out / (name + ".run.txt")).write_text(done.stdout + done.stderr)
        results[name] = {"exit": done.returncode, "expected_exit": expected,
                         "output": done.stdout, "actual_source": True}
        if done.returncode != expected:
            raise RuntimeError(name + " failed:\n" + done.stdout + done.stderr)
        print(name + ": " + done.stdout.strip())

    production = (root / "src/subsystems/vrmap/runtime/held_map_m1.cpp").read_text()
    actual_find = function(production, "void* Find(PrintedRegion")
    cache_prefix = r'''
#include <atomic>
#include <cstdint>
#include <cstring>
#include <cstdio>
#define __fastcall
#define __try try
#define __except(x) catch(...)
#define EXCEPTION_EXECUTE_HANDLER 1
namespace diagnostics::map_d72 {inline constexpr int Find=0;struct Scope{Scope(int){}};}
enum class PrintedRegion {Trosky,Kuttenberg};
static std::atomic<std::uint64_t> activation{1};
static std::uintptr_t base=0;
static int nameCalls=0,idCalls=0,checks=0,failures=0;
static unsigned char one[32]{},two[32]{};
static void* registry=one;
static void* named=one;
static void check(bool x,const char* why){++checks;if(!x){++failures;std::printf("FAIL %s\n",why);}}
static void* byId(void*,std::uint32_t){++idCalls;return registry;}
static bool Copy(std::uintptr_t from,void* to,std::size_t n){
 std::uintptr_t value=0;
 if(from==base+0x492D8A0)value=0x20000;
 else if(from==0x20000)value=base+0x4077048;
 else if(from==base+0x4077048+0x70)value=reinterpret_cast<std::uintptr_t>(&byId);
 else if((from==reinterpret_cast<std::uintptr_t>(one)||from==reinterpret_cast<std::uintptr_t>(two))&&n==8)value=base+0x4072C78;
 else if((from==reinterpret_cast<std::uintptr_t>(one)+12||from==reinterpret_cast<std::uintptr_t>(two)+12)&&n==4){std::uint32_t id=from==reinterpret_cast<std::uintptr_t>(one)+12?123:456;std::memcpy(to,&id,4);return true;}
 else return false;
 std::memcpy(to,&value,n);return true;
}
static void* FindRawD72(PrintedRegion,unsigned* refusal){++nameCalls;if(refusal)*refusal=named?0:3;return named;}
'''
    cache_test = r'''
int main(){
 base=reinterpret_cast<std::uintptr_t>(&byId)-0x4B4F98;
 unsigned why=99;
 check(Find(PrintedRegion::Trosky,&why)==one&&why==0&&nameCalls==1,"initial named lookup");
 for(int i=0;i<100;++i)check(Find(PrintedRegion::Trosky,&why)==one,"current registry identity");
 check(nameCalls==1&&idCalls==100,"steady placement performs no repeated name search");
 ++activation;check(Find(PrintedRegion::Trosky,&why)==one&&nameCalls==2,"activation invalidates cache");
 registry=nullptr;named=nullptr;check(!Find(PrintedRegion::Trosky,&why)&&why==3,"removed helper never reuses old pointer");
 registry=two;named=two;check(Find(PrintedRegion::Trosky,&why)==two,"replacement is resolved explicitly");
 const auto names=nameCalls;check(Find(PrintedRegion::Trosky,&why)==two&&nameCalls==names,"new ID joined through registry");
 registry=one;named=nullptr;check(!Find(PrintedRegion::Trosky,&why),"different registry object cannot use stale named lease");
 std::printf("%d checks, %d failures; native leaves substituted\n",checks,failures);return failures?1:0;
}
'''
    run("map_cache", cache_prefix + actual_find + cache_test)
    control = actual_find.replace("cache.entity&&cache.id&&cache.serial==serial", "false", 1)
    assert control != actual_find
    run("map_cache_negative", cache_prefix + control + cache_test, expected=1)

    shim = out / "log_shims"
    (shim / "DetourModKit").mkdir(parents=True, exist_ok=True)
    (shim / "windows.h").write_text(r'''
#pragma once
#include <cstdint>
#include <cstring>
#include <string>
using DWORD=std::uint32_t;using WORD=std::uint16_t;using HANDLE=void*;using HMODULE=void*;using LPCSTR=const char*;
constexpr unsigned MAX_PATH=260,GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS=4,GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT=2;
constexpr DWORD GENERIC_WRITE=0x40000000,FILE_SHARE_READ=1,FILE_SHARE_WRITE=2,FILE_SHARE_DELETE=4,CREATE_ALWAYS=2,FILE_ATTRIBUTE_NORMAL=128;
#define INVALID_HANDLE_VALUE reinterpret_cast<HANDLE>(std::intptr_t(-1))
inline unsigned created=0,closed=0;inline std::string createdPath;
inline int GetModuleHandleExA(DWORD,LPCSTR,HMODULE* x){*x=reinterpret_cast<void*>(1);return 1;}
inline DWORD GetModuleFileNameA(HMODULE,char* x,DWORD){std::strcpy(x,"C:\\game\\KCD2VR.asi");return DWORD(std::strlen(x));}
inline HANDLE CreateFileA(const char* path,DWORD,DWORD,void*,DWORD,DWORD,void*){++created;createdPath=path;return reinterpret_cast<void*>(2);}
inline int CloseHandle(HANDLE){++closed;return 1;}
inline std::uint64_t GetTickCount64(){return 10000;}
struct FILETIME{DWORD dwLowDateTime,dwHighDateTime;};struct SYSTEMTIME{WORD wYear,wMonth,wDayOfWeek,wDay,wHour,wMinute,wSecond,wMilliseconds;};
inline void GetSystemTimeAsFileTime(FILETIME*p){*p={};}
inline int FileTimeToLocalFileTime(const FILETIME*a,FILETIME*b){*b=*a;return 1;}
inline int FileTimeToSystemTime(const FILETIME*,SYSTEMTIME*b){*b={};return 1;}
''')
    (shim / "DetourModKit/async_logger_config.hpp").write_text(r'''
#pragma once
namespace DetourModKit {enum class OverflowPolicy{DropNewest};struct AsyncLoggerConfig{OverflowPolicy overflow_policy=OverflowPolicy::DropNewest;};}
''')
    (shim / "DetourModKit/logger.hpp").write_text(r'''
#pragma once
#include "async_logger_config.hpp"
#include <string>
#include <string_view>
#include <vector>
namespace DetourModKit {
enum class LogLevel{Trace,Debug,Info,Warning,Error};enum class LogOpenMode{Append,Truncate};
inline constexpr char DEFAULT_TIMESTAMP_FORMAT[]="test";
inline unsigned configurations=0;inline std::string path;inline LogOpenMode mode=LogOpenMode::Truncate;
struct Logger{
 bool async=false,admit=true;LogLevel level=LogLevel::Info;std::vector<std::string> records;
 static void configure(const char*,const char*p,const char*,LogOpenMode m){++configurations;path=p;mode=m;}
 void enable_async_mode(const AsyncLoggerConfig&){async=true;}
 bool is_async_mode_enabled()const{return async;}
 LogLevel get_log_level()const{return level;}
 void set_log_level(LogLevel l){level=l;}
 bool is_enabled(LogLevel l)const{return l>=level;}
 std::size_t dropped_count()const{return admit?0:1;}
 bool log_noexcept(LogLevel l,std::string_view s)noexcept{if(!admit||!is_enabled(l))return false;records.emplace_back(s);return true;}
};
inline Logger& log(){static Logger x;return x;}
}
''')
    log_body = r'''
#include "diagnostics/unified_log_d81.h"
#include "diagnostic_trace.cpp"
#include <cstdio>
int main(){
 unsigned checks=0,fail=0;auto check=[&](bool b,const char*s){++checks;if(!b){++fail;std::printf("FAIL %s\n",s);}};
 using namespace kcd2vr;
 check(diagnostics::unified_log_d81::Initialize(),"init");
 check(diagnostics::unified_log_d81::Initialize(),"repeat init");
 check(created==1&&closed==1&&DetourModKit::configurations==1,"one truncate/configuration only");
 check(DetourModKit::path==createdPath&&createdPath=="C:\\game\\KCD2VR.log","sole process path");
 check(DetourModKit::mode==DetourModKit::LogOpenMode::Append&&DetourModKit::log().async,"append shared async writer");
 check(diagnostic_trace::Output("CAPTURE",7),"capture reaches shared sink");
 diagnostic_trace::SetEnabled(true);diagnostic_trace::SetEnabled(false);diagnostic_trace::SetEnabled(true);
 check(created==1&&closed==1&&DetourModKit::configurations==1,"capture toggles never open or truncate a file");
 check(DetourModKit::log().records.size()==1&&DetourModKit::log().records[0]=="CAPTURE","raw content retained");
 DetourModKit::log().admit=false;check(!diagnostic_trace::Output("DROP",4)&&diagnostic_trace::g_sinkRejected==1,"rejected batch counted");
 DetourModKit::log().admit=true;DetourModKit::log().level=DetourModKit::LogLevel::Error;
 check(diagnostic_trace::Output("FILTER",6)&&DetourModKit::log().level==DetourModKit::LogLevel::Error,"explicit capture bypasses verbosity and restores it");
 std::printf("%u checks, %u failures; process logger/Windows leaves substituted\n",checks,fail);return fail?1:0;
}
'''
    run("unified_log", log_body, ["-I" + str(shim)])
    run("walking_dodge", '#include "' + str(root / "tests/roomscale58_test.cpp") + '"\n')
    run("prompt_policy", '#include "' + str(root / "tests/prompt_policy_d80_test.cpp") + '"\n')
    (out / "RESULT.json").write_text(json.dumps({"result": "PASS", "tests": results,
        "Windows_build": "NOT_RUN", "headset": "NOT_RUN"}, indent=2) + "\n")


if __name__ == "__main__":
    main()
