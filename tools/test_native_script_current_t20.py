"""Actual read-only Context capability with explicit VM-memory leaf outcomes."""
from pathlib import Path
import json
import subprocess
from source_text import read_source

ROOT=Path(__file__).resolve().parents[1]
OUT=ROOT.parent/'script_current_t20_host'


def block(text, start):
    pos=text.index(start);opening=text.index('{',pos);depth=1;end=opening+1
    while depth:
        depth+=(text[end]=='{')-(text[end]=='}');end+=1
    return text[pos:end]


def main():
    body=block(read_source(ROOT/'src/native_script_t2.cpp'),'bool Context::Current()')
    code=r'''
#include "native_script_t2.h"
#include <windows.h>
#include <atomic>
#include <cassert>
#include <thread>
#include <cstdio>
namespace kcd2vr::native_script_t2 {
const Context* current=nullptr;std::atomic<bool> enabled{true};DWORD ownerThread=0;bool callbackRevokedR96=false;
std::uintptr_t lastSystem=1,lastState=2;std::uint64_t generation=3;bool vmKnown=true;std::uintptr_t vmState=2;
bool Vm(std::uintptr_t,std::uintptr_t& state,std::uintptr_t& top,std::uintptr_t& frame){state=vmState;top=frame=16;return vmKnown;}
@BODY@
}
int main(){using namespace kcd2vr::native_script_t2;
 Context c{1,2,1000,3};current=&c;ownerThread=GetCurrentThreadId();assert(c.Current());
 Context copy=c;assert(!copy.Current());current=nullptr;assert(!c.Current());current=&c;
 enabled=false;assert(!c.Current());enabled=true;callbackRevokedR96=true;assert(!c.Current());callbackRevokedR96=false;
 bool foreign=true;std::thread other([&]{foreign=c.Current();});other.join();assert(!foreign);
 lastSystem=9;assert(!c.Current());lastSystem=1;lastState=9;assert(!c.Current());lastState=2;
 generation=4;assert(!c.Current());generation=3;
 vmKnown=false;assert(!c.Current());vmKnown=true;vmState=9;assert(!c.Current());vmState=2;assert(c.Current());
 std::puts("PASS actual Context::Current: same-stack/current original callback, enabled/owner-thread/system/state/generation, positive VM state; copied/foreign/replaced/unknown rejected");
}
'''.replace('@BODY@',body)
    OUT.mkdir(exist_ok=True);(OUT/'test.cpp').write_text(code)
    (OUT/'run.cmd').write_text(f'''@echo off
call "C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\VC\\Auxiliary\\Build\\vcvars64.bat" >nul
cd /d "{OUT}"
cl /nologo /std:c++latest /EHsc /fsanitize=address /Zi /I "{ROOT/'src'}" test.cpp /Fetest.exe
if errorlevel 1 exit /b %errorlevel%
test.exe
exit /b %errorlevel%
''')
    result=subprocess.run(['cmd','/c',str(OUT/'run.cmd')],capture_output=True,text=True)
    print(json.dumps({'suite':'actual same-stack script capability','exit':result.returncode,'stdout':result.stdout,
                     'stderr':result.stderr,'native_VM_memory_leaf':'SUBSTITUTED','native_game':False},indent=2))
    raise SystemExit(result.returncode)


if __name__=='__main__':main()
