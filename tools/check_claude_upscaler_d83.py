#!/usr/bin/env python3
"""Reproduce supplied Claude D83 history defects and D82 source API incompatibility."""
from pathlib import Path
import argparse,hashlib,json,subprocess,shutil

def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--claude-source',type=Path,required=True)
    p.add_argument('--out',type=Path,required=True)
    p.add_argument('--cxx',default='g++')
    a=p.parse_args();root=Path(__file__).resolve().parents[1];out=a.out.resolve()
    out.mkdir(parents=True,exist_ok=True);shim=out/'shim';(shim/'DetourModKit').mkdir(parents=True,exist_ok=True)
    (shim/'windows.h').write_text('''#pragma once
#include <cstdint>
#include <cwchar>
#define __fastcall
#define __try try
#define __except(x) catch(...)
#define EXCEPTION_EXECUTE_HANDLER 1
#define PAGE_READWRITE 4
using DWORD=unsigned long;using HMODULE=void*;
inline std::uint64_t GetTickCount64(){return 1000;}
inline HMODULE GetModuleHandleW(const wchar_t*){return nullptr;}
inline void* GetProcAddress(HMODULE,const char*){return nullptr;}
inline int VirtualProtect(void*,std::size_t,DWORD,DWORD* previous){*previous=0;return 1;}
''')
    (shim/'diagnostic_trace.h').write_text('#pragma once\nnamespace diagnostic_trace{inline void Write(const char*){}}\n')
    (shim/'DetourModKit/hook.hpp').write_text('''#pragma once
#include <optional>
#include <cstdint>
namespace DetourModKit {struct Address{std::uintptr_t value;};namespace hook {
struct Target{Address address;};struct Options{};struct InlineRequest{const char* name;Target target;Options options;};
struct Hook{template<class Fn> Fn original(){return nullptr;}bool enable(){return true;}bool disable(){return true;}};
template<class Detour>std::optional<Hook> inline_at(InlineRequest,Detour){return std::nullopt;}
}}
''')
    source=a.claude_source.resolve();production=source/'src/native_upscaler_stereo_d82.cpp'
    shutil.copyfile(production,out/'claude_upscaler_actual.cpp')
    shutil.copyfile(source/'src/native_upscaler_stereo_d82.h',out/'native_upscaler_stereo_d82.h')
    fixture=root/'tests/claude_upscaler_review_d83.cpp';results=[]
    for label,flags in [('debug',['-O0','-g']),('optimized',['-O2']),('asan_ubsan',['-O1','-g','-no-pie','-fsanitize=address,undefined','-fno-omit-frame-pointer'])]:
        exe=out/label;cmd=[a.cxx,'-std=c++20','-pthread','-Wall','-Wextra','-Werror',*flags,'-I'+str(shim),'-I'+str(out),str(fixture),'-o',str(exe)]
        c=subprocess.run(cmd,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
        if c.returncode:raise RuntimeError(c.stdout)
        import os
        env=os.environ.copy();env['ASAN_OPTIONS']='detect_leaks=0'
        run=subprocess.run([str(exe)],text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,env=env)
        results.append({'name':label,'command':cmd,'exit':run.returncode,'output':run.stdout})
        if run.returncode:raise RuntimeError(run.stdout)
    probe=out/'overlay_api_probe.cpp'
    probe.write_text('''#include "native_upscaler_stereo_d82.h"
int main(){namespace n=kcd2vr::native_upscaler_stereo_d82;n::InstallEarly(true);n::SetMode(true,2);n::SetTechnologyReader(nullptr);n::BeginEpoch(1);(void)n::CanEnterStereo(2);(void)n::PairReady(1,2);(void)n::FrameReady(2);(void)n::TemporalActive();n::Stop();n::LogStatus("probe");}
''')
    # Include spelling deliberately uses angle brackets: do not pick the copied
    # Claude header beside the probe when checking the reviewed header.
    probe.write_text(probe.read_text().replace('#include "native_upscaler_stereo_d82.h"','#include <native_upscaler_stereo_d82.h>'))
    for label,include,expected in [('claude_header',source/'src',1),('reviewed_header',root/'src',0)]:
        cmd=[a.cxx,'-std=c++20','-fsyntax-only','-I'+str(include),str(probe)]
        c=subprocess.run(cmd,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
        if bool(c.returncode)!=bool(expected):raise RuntimeError(label+' unexpected result: '+c.stdout)
        results.append({'name':label,'command':cmd,'exit':c.returncode,'expected_failure':bool(expected),'output':c.stdout})
    report={'passed':True,'production_sha256':hashlib.sha256(production.read_bytes()).hexdigest(),
            'fixture_sha256':hashlib.sha256(fixture.read_bytes()).hexdigest(),'runs':results,
            'limits':'Full literal Claude upscaler TU compiled with controlled Win32, hook and vendor API leaves. Probes demonstrate history-routing behavior, not observed GPU pixels or native scheduling. No Windows/headset execution.'}
    (out/'RESULT.json').write_text(json.dumps(report,indent=2)+'\n');print(results[0]['output']);print('Overlay API break confirmed; reviewed D82 interface compiles.')
if __name__=='__main__':main()
