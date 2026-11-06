#pragma once
// Exact-retail, read-only fader ownership recorder. Installed only on F2.
// Never edits registers, durations, callbacks or native state.
#include "diagnostics/capture_control168.h"
#include "diagnostic_trace.h"
#include "retail_build112.h"
#include <DetourModKit/hook.hpp>
#include <windows.h>
#include <atomic>
#include <optional>
#include <cstdio>
#include <cstring>
namespace kcd2vr::activity_fader47c1 {
inline std::optional<DetourModKit::hook::Hook> beginHook,endHook;
inline std::uintptr_t base=0;
inline std::atomic<unsigned> count{0};
inline bool Copy(std::uintptr_t p,void* out,std::size_t size) noexcept {
    MEMORY_BASIC_INFORMATION m{};
    if(!p||!VirtualQuery(reinterpret_cast<void*>(p),&m,sizeof(m))||m.State!=MEM_COMMIT||
       (m.Protect&(PAGE_NOACCESS|PAGE_GUARD))||p<std::uintptr_t(m.BaseAddress)||
       size>m.RegionSize-(p-std::uintptr_t(m.BaseAddress)))return false;
    __try {std::memcpy(out,reinterpret_cast<void*>(p),size);return true;}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
inline void Observe(DetourModKit::hook::MidContext& ctx,const char* edge) noexcept {
    using namespace DetourModKit::hook;
    const auto now=std::uint64_t(GetTickCount64());
    const bool capture=diagnostics::capture168::active(diagnostics::capture168::Domain::Bug,now)||
        diagnostics::capture168::active(diagnostics::capture168::Domain::Hud,now);
    if(!capture||count.load()>=128)return;
    const auto owner=gpr(ctx,Gpr::Rcx),fader=gpr(ctx,Gpr::Rdx);
    std::uintptr_t ov=0,fv=0,stored=0,namePtr=0;
    if(!Copy(owner,&ov,8)||ov!=base+0x3A97220||!Copy(fader,&fv,8)||fv!=base+0x3A5CFE8||
       !Copy(fader+8,&stored,8)||stored!=owner||!Copy(fader+0x10,&namePtr,8))return;
    char name[80]{};bool terminated=false;
    for(unsigned i=0;i<sizeof(name)-1;++i){
        if(!Copy(namePtr+i,name+i,1))return;
        if(!name[i]){terminated=true;break;}
        if(static_cast<unsigned char>(name[i])<32||static_cast<unsigned char>(name[i])>126)return;
    }
    if(!terminated||count.fetch_add(1)>=128)return;
    const unsigned bits=unsigned(gpr(ctx,Gpr::R8));float seconds=0;std::memcpy(&seconds,&bits,4);
    std::uintptr_t caller=0;(void)Copy(stack_pointer(ctx),&caller,8);
    char line[240]{};std::snprintf(line,sizeof(line),"R47C1 ACTIVITY_FADER edge=%s name=%s duration=%.4f callerRva=0x%llX observedOnly=1",edge,name,double(seconds),static_cast<unsigned long long>(caller>=base?caller-base:0));
    diagnostic_trace::Write(line);
}
inline void Begin(DetourModKit::hook::MidContext& c) noexcept {Observe(c,"BEGIN");}
inline void End(DetourModKit::hook::MidContext& c) noexcept {Observe(c,"END");}
inline bool Install() noexcept {
    count.store(0);
    if(beginHook&&endHook)return true;
    base=reinterpret_cast<std::uintptr_t>(GetModuleHandleW(L"WHGame.dll"));
    if(!retail_build112::Verify(base))return false;
    constexpr unsigned char a[]{0x48,0x89,0x5C,0x24,0x18,0x55,0x56,0x57,0x48,0x83,0xEC,0x50};
    constexpr unsigned char b[]{0x48,0x8B,0xC4,0x48,0x89,0x58,0x08,0x48,0x89,0x70,0x18,0x48,0x89,0x50,0x10,0x57};
    unsigned char bytes[16]{};
    if(!Copy(base+0x9AF2D4,bytes,sizeof(a))||std::memcmp(bytes,a,sizeof(a))||
       !Copy(base+0x8DD068,bytes,sizeof(b))||std::memcmp(bytes,b,sizeof(b)))return false;
    using namespace DetourModKit::hook;Options o{};o.fail_if_already_hooked=true;
    auto h=mid_at(MidRequest{"kcd2vr_activity_fader_begin47c1",Target{DetourModKit::Address{base+0x9AF2D4}},o},Begin);
    if(!h)return false;beginHook=std::move(*h);
    auto e=mid_at(MidRequest{"kcd2vr_activity_fader_end47c1",Target{DetourModKit::Address{base+0x8DD068}},o},End);
    if(!e){beginHook.reset();return false;}endHook=std::move(*e);
    if(!beginHook->enable()||!endHook->enable()){
        (void)beginHook->disable();(void)endHook->disable();beginHook.reset();endHook.reset();return false;
    }
    diagnostic_trace::Write("R47C1 ACTIVITY_FADER recorder installed lazily; F2 only, 128 events maximum, no native changes");return true;
}
inline void Uninstall() noexcept {
    if(beginHook){(void)beginHook->disable();beginHook.reset();}
    if(endHook){(void)endHook->disable();endHook.reset();}
}
}
