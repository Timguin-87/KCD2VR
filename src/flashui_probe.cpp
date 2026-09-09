#include "flashui_probe.h"
#include "debug_trace.h"
#include <DetourModKit/hook.hpp>
#include <windows.h>
#include <atomic>
#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <optional>
#include <utility>

namespace kcd2vr::flashui_probe {
namespace {
constexpr std::uintptr_t kFlashUiGlobalRva = 0x492D938ull; // proven by HUD/cursor static xrefs
constexpr std::uintptr_t kUiHolderReleaseRva = 0x3968ACull;  // exact cleanup used after GetElement("hud") at WHGame+0x89B68C
constexpr std::uint32_t kSupportedWhTimestamp = 0x6A350E20u;
using GetElementFn = void*(__fastcall*)(void*, void*, const char*);
std::optional<DetourModKit::hook::Hook> g_hook;
GetElementFn g_original=nullptr;
std::atomic<std::uint64_t> g_calls{0};
std::atomic<std::uint64_t> g_hud_calls{0};
std::atomic<void*> g_last_hud{nullptr};
std::atomic<std::uint64_t> g_last_hud_tick{0};

std::uintptr_t WhBase() noexcept { return reinterpret_cast<std::uintptr_t>(GetModuleHandleW(L"WHGame.dll")); }
bool SupportedBuild(std::uintptr_t base) noexcept {
    if(!base) return false;
    __try {
        const auto* dos=reinterpret_cast<const IMAGE_DOS_HEADER*>(base);
        if(dos->e_magic!=IMAGE_DOS_SIGNATURE) return false;
        const auto* nt=reinterpret_cast<const IMAGE_NT_HEADERS64*>(base+dos->e_lfanew);
        return nt->Signature==IMAGE_NT_SIGNATURE && nt->FileHeader.TimeDateStamp==kSupportedWhTimestamp;
    } __except(EXCEPTION_EXECUTE_HANDLER) { return false; }
}
bool SafePtr(const void* p) noexcept {
    if(!p) return false; MEMORY_BASIC_INFORMATION mbi{};
    if(!VirtualQuery(p,&mbi,sizeof(mbi))) return false;
    if(mbi.State!=MEM_COMMIT || (mbi.Protect & (PAGE_NOACCESS|PAGE_GUARD))) return false;
    return true;
}
template<class T> bool Read(const void* p,T& out) noexcept { if(!SafePtr(p)) return false; __try{out=*reinterpret_cast<const T*>(p); return true;}__except(EXCEPTION_EXECUTE_HANDLER){return false;} }

bool ReadPointerSeh(const void* p, void*& out) noexcept {
    out = nullptr;
    if (!SafePtr(p)) return false;
    __try {
        out = *reinterpret_cast<void* const*>(p);
        return true;
    } __except(EXCEPTION_EXECUTE_HANDLER) {
        out = nullptr;
        return false;
    }
}

void* __fastcall Detour(void* self, void* out, const char* name) {
    auto* ret = g_original ? g_original(self,out,name) : nullptr;
    const auto n=g_calls.fetch_add(1,std::memory_order_relaxed)+1;
    bool hud=false; char local[64]{};
    if(name && SafePtr(name)) { __try{ std::strncpy(local,name,sizeof(local)-1); } __except(EXCEPTION_EXECUTE_HANDLER){local[0]=0;} }
    if(local[0] && (_stricmp(local,"hud")==0)) hud=true;
    if(hud) {
        const auto h=g_hud_calls.fetch_add(1,std::memory_order_relaxed)+1;
        void* out0=nullptr; if(out) Read(out,out0);
        if(out0 && SafePtr(out0)) {
            g_last_hud.store(out0,std::memory_order_release);
            g_last_hud_tick.store(GetTickCount64(),std::memory_order_release);
        }
        if(h<=8 || h%120==0) {
            char msg[512]{}; std::snprintf(msg,sizeof(msg),
                "V61.21-FLASHUI HUD-LOOKUP call=%llu hudCall=%llu iface=%p out=%p out0=%p ret=%p name='%s'",
                (unsigned long long)n,(unsigned long long)h,self,out,out0,ret,local); debug_trace::Write(msg);
        }
    }
    return ret;
}
}

bool Install() noexcept {
    const auto base=WhBase(); if(!SupportedBuild(base)){debug_trace::Write("V61.43-FLASHUI install skipped: unsupported WHGame build/timestamp"); return false;}
    void* iface=nullptr; ReadPointerSeh(reinterpret_cast<const void*>(base+kFlashUiGlobalRva), iface);
    if(!iface || !SafePtr(iface)){debug_trace::Write("V61.21-FLASHUI global 0x492D938 unresolved/null"); return false;}
    void** vt=nullptr; if(!Read(iface,vt) || !vt || !SafePtr(vt)){debug_trace::Write("V61.21-FLASHUI global resolved but vtable unreadable"); return false;}
    void* target=nullptr; if(!Read(reinterpret_cast<unsigned char*>(vt)+0x70,target) || !target){debug_trace::Write("V61.21-FLASHUI vslot+0x70 target unreadable"); return false;}
    char pre[384]{}; std::snprintf(pre,sizeof(pre),"V61.21-FLASHUI static-xref confirmed global_rva=0x492D938 iface=%p vt=%p slot70=%p strings={hud@0x3A55B7C,CombatCursorCross@0x3C33BE0,CursorCrossPBSlot@0x3C33BC8}",iface,vt,target); debug_trace::Write(pre);
    DetourModKit::hook::InlineRequest req{"kcd2vr_v6121_flashui_getelement",DetourModKit::hook::Target{DetourModKit::Address{reinterpret_cast<std::uintptr_t>(target)}},DetourModKit::hook::Options{}};
    auto result=DetourModKit::hook::inline_at(std::move(req),&Detour); if(!result){debug_trace::Write("V61.21-FLASHUI slot+0x70 passive hook failed"); return false;}
    g_hook=std::move(*result); g_original=g_hook->original<GetElementFn>(); auto e=g_hook->enable(); if(!e){g_original=nullptr;g_hook.reset();debug_trace::Write("V61.21-FLASHUI slot+0x70 enable failed");return false;}
    debug_trace::Write("V61.33-FLASHUI HUD observer enabled; F12 cursor proof retained; semantic MasterSwitch scale is user-triggered only"); return true;
}
bool HasRecentHud() noexcept {
    void* hud=g_last_hud.load(std::memory_order_acquire);
    const auto seen=g_last_hud_tick.load(std::memory_order_acquire);
    const auto now=GetTickCount64();
    return hud && SafePtr(hud) && seen && now>=seen && now-seen<=2000ull;
}


namespace {

bool ExecPtr(const void* p) noexcept {
    if(!p) return false;
    MEMORY_BASIC_INFORMATION mbi{};
    if(!VirtualQuery(p,&mbi,sizeof(mbi)) || mbi.State!=MEM_COMMIT || (mbi.Protect&(PAGE_NOACCESS|PAGE_GUARD))) return false;
    const DWORD prot=mbi.Protect & 0xFFu;
    return prot==PAGE_EXECUTE || prot==PAGE_EXECUTE_READ || prot==PAGE_EXECUTE_READWRITE || prot==PAGE_EXECUTE_WRITECOPY;
}

// Retail WHGame uses a tiny smart-pointer holder for IUIElement::GetElement("hud"):
//   rcx = FlashUI global, rdx = &holder, r8 = "hud", call vtbl+0x70
//   hud = *returnValue; cleanup(&holder) @ WHGame+0x3968AC.
// Mirror that exact sequence and keep the HUD reference alive only for this call.
struct HudHolder {
    alignas(16) unsigned char bytes[0x20]{};
    bool acquired=false;
};

void ReleaseHudHolder(HudHolder& h) noexcept {
    if(!h.acquired) return;
    const auto base=WhBase();
    if(!SupportedBuild(base)) return;
    const auto addr=base+kUiHolderReleaseRva;
    if(!ExecPtr(reinterpret_cast<void*>(addr))) return;
    using ReleaseFn=void(__fastcall*)(void*);
    auto fn=reinterpret_cast<ReleaseFn>(addr);
    __try { fn(h.bytes); }
    __except(EXCEPTION_EXECUTE_HANDLER) { }
    h.acquired=false;
}

bool AcquireHudDirect(HudHolder& holder, void*& hud) noexcept {
    hud=nullptr;
    const auto base=WhBase();
    if(!SupportedBuild(base)) return false;
    void* iface=nullptr;
    if(!ReadPointerSeh(reinterpret_cast<const void*>(base+kFlashUiGlobalRva),iface) || !iface || !SafePtr(iface)) return false;
    void** vt=nullptr;
    if(!Read(iface,vt) || !vt || !SafePtr(vt)) return false;
    void* target=nullptr;
    if(!Read(reinterpret_cast<unsigned char*>(vt)+0x70,target) || !ExecPtr(target)) return false;
    using DirectGetElementFn=void*(__fastcall*)(void*,void*,const char*);
    void* ret=nullptr;
    __try { ret=reinterpret_cast<DirectGetElementFn>(target)(iface,holder.bytes,"hud"); holder.acquired=true; }
    __except(EXCEPTION_EXECUTE_HANDLER) { ret=nullptr; holder.acquired=false; }
    if(!ret || !SafePtr(ret)) return false;
    if(!Read(ret,hud) || !hud || !SafePtr(hud)) return false;
    return true;
}

bool ApplyMovieClipDisplay(void* hud,const char* name,float x,float y,float scale) noexcept {
    if(!hud || !name || !*name || !SafePtr(hud)) return false;
    void** hvt=nullptr;
    if(!Read(hud,hvt)||!hvt||!SafePtr(hvt)) return false;
    void* findTarget=nullptr;
    if(!Read(reinterpret_cast<unsigned char*>(hvt)+0x238,findTarget)||!ExecPtr(findTarget)) return false;
    using FindChildFn=void*(__fastcall*)(void*,const char*,int);
    void* clip=nullptr;
    __try { clip=reinterpret_cast<FindChildFn>(findTarget)(hud,name,0); }
    __except(EXCEPTION_EXECUTE_HANDLER) { clip=nullptr; }
    if(!clip||!SafePtr(clip)) return false;

    void** cvt=nullptr;
    if(!Read(clip,cvt)||!cvt||!SafePtr(cvt)) return false;
    void* setTarget=nullptr; void* getTarget=nullptr;
    if(!Read(reinterpret_cast<unsigned char*>(cvt)+0xC8,setTarget) || !ExecPtr(setTarget)) return false;
    if(!Read(reinterpret_cast<unsigned char*>(cvt)+0xD0,getTarget) || !ExecPtr(getTarget)) return false;

    // Recovered from WHGame+0x89B4D0 and +0x9A85D4:
    // SFlashDisplayInfo: x @ +0x00, y @ +0x04,
    // xScale @ +0x0C, yScale @ +0x10, flags uint16 @ +0x34.
    // Flags: X=0x01, Y=0x02, XScale=0x08, YScale=0x10.
    // WHGame writes scale as a percentage (50..100), not a 0..1 fraction.
    alignas(16) unsigned char info[0x38]{};
    using DisplayFn=void(__fastcall*)(void*,void*);
    __try { reinterpret_cast<DisplayFn>(getTarget)(clip,info); }
    __except(EXCEPTION_EXECUTE_HANDLER) { return false; }

    const float pct=std::clamp(scale,0.25f,2.00f)*100.0f;
    std::memcpy(info+0x00,&x,sizeof(float));
    std::memcpy(info+0x04,&y,sizeof(float));
    std::memcpy(info+0x0C,&pct,sizeof(float));
    std::memcpy(info+0x10,&pct,sizeof(float));
    std::uint16_t flags=0;
    std::memcpy(&flags,info+0x34,sizeof(flags));
    flags=static_cast<std::uint16_t>(flags|0x01u|0x02u|0x08u|0x10u);
    std::memcpy(info+0x34,&flags,sizeof(flags));
    __try { reinterpret_cast<DisplayFn>(setTarget)(clip,info); }
    __except(EXCEPTION_EXECUTE_HANDLER) { return false; }
    return true;
}

} // namespace

bool ApplyVrLayout(bool enabled,
                   float subtitleScale,float subtitleY,
                   float dialogueScale,float dialogueY,float dialogueInsetPct,
                   float hintsScale,float hintsInsetPct,float hintsY) noexcept {
    subtitleScale=std::clamp(subtitleScale,0.30f,1.25f);
    subtitleY=std::clamp(subtitleY,-600.0f,600.0f);
    dialogueScale=std::clamp(dialogueScale,0.30f,1.50f);
    dialogueY=std::clamp(dialogueY,-720.0f,720.0f);
    dialogueInsetPct=std::clamp(dialogueInsetPct,0.0f,80.0f);
    hintsScale=std::clamp(hintsScale,0.50f,1.50f);
    hintsInsetPct=std::clamp(hintsInsetPct,0.0f,75.0f);
    hintsY=std::clamp(hintsY,-600.0f,600.0f);

    const float ss=enabled?subtitleScale:1.0f;
    const float ds=enabled?dialogueScale:1.0f;
    const float hs=enabled?hintsScale:1.0f;
    const float subY=-230.0f+(enabled?subtitleY:0.0f);
    const float dialogInset=enabled?(960.0f*dialogueInsetPct*0.01f):0.0f;
    const float dialogYPos=540.0f+(enabled?dialogueY:0.0f);
    const float hintInset=enabled?(960.0f*hintsInsetPct*0.01f):0.0f;
    const float hintYPos=-120.0f+(enabled?hintsY:0.0f);

    HudHolder holder{}; void* hud=nullptr;
    // Prefer a HUD object observed on the game's own GetElement call path (the
    // same technique that already proved CombatCursorCross SetVisible). Fall
    // back to the exact retail GetElement/holder sequence only when no live HUD
    // has been observed yet.
    // V62.7: prefer a freshly acquired HUD instance. Inventory/Apse transitions
    // can destroy and recreate MovieClips while an observed pointer still looks
    // address-valid. Direct reacquisition prevents reapplying to a stale object.
    if(!AcquireHudDirect(holder,hud)) {
        const auto seen=g_last_hud_tick.load(std::memory_order_acquire);
        const auto now=GetTickCount64();
        void* observed=g_last_hud.load(std::memory_order_acquire);
        if(observed && SafePtr(observed) && seen && now>=seen && now-seen<=30000ull) hud=observed;
    }
    if(!hud) { ReleaseHudHolder(holder); return false; }
    bool ok=true;
    ok=ApplyMovieClipDisplay(hud,"Subtitles",-480.0f,subY,ss)&&ok;
    ok=ApplyMovieClipDisplay(hud,"DialogLeft",dialogInset,dialogYPos,ds)&&ok;
    ok=ApplyMovieClipDisplay(hud,"DialogRight",1920.0f-dialogInset,dialogYPos,ds)&&ok;
    ok=ApplyMovieClipDisplay(hud,"Hints",-35.0f-hintInset,hintYPos,hs)&&ok;
    ReleaseHudHolder(holder);
    return ok;
}

bool SetHudMovieClipVisible(const char* name, bool visible) noexcept {
    if(!name || !*name) return false;
    HudHolder holder{}; void* hud=nullptr;
    // V62.7: prefer a freshly acquired HUD instance. Inventory/Apse transitions
    // can destroy and recreate MovieClips while an observed pointer still looks
    // address-valid. Direct reacquisition prevents reapplying to a stale object.
    if(!AcquireHudDirect(holder,hud)) {
        const auto seen=g_last_hud_tick.load(std::memory_order_acquire);
        const auto now=GetTickCount64();
        void* observed=g_last_hud.load(std::memory_order_acquire);
        if(observed && SafePtr(observed) && seen && now>=seen && now-seen<=30000ull) hud=observed;
    }
    if(!hud) { ReleaseHudHolder(holder); return false; }

    void** hvt=nullptr; if(!Read(hud,hvt)||!hvt||!SafePtr(hvt)){ ReleaseHudHolder(holder); return false; }
    void* findTarget=nullptr;
    if(!Read(reinterpret_cast<unsigned char*>(hvt)+0x238,findTarget)||!ExecPtr(findTarget)){ ReleaseHudHolder(holder); return false; }
    using FindChildFn=void*(__fastcall*)(void*,const char*,int);
    void* clip=nullptr;
    __try { clip=reinterpret_cast<FindChildFn>(findTarget)(hud,name,0); }
    __except(EXCEPTION_EXECUTE_HANDLER){ clip=nullptr; }
    if(!clip||!SafePtr(clip)){ ReleaseHudHolder(holder); return false; }

    void** cvt=nullptr; if(!Read(clip,cvt)||!cvt||!SafePtr(cvt)){ ReleaseHudHolder(holder); return false; }
    void* visTarget=nullptr;
    if(!Read(reinterpret_cast<unsigned char*>(cvt)+0x108,visTarget)||!ExecPtr(visTarget)){ ReleaseHudHolder(holder); return false; }
    using SetVisibleFn=void(__fastcall*)(void*,bool);
    bool ok=true;
    __try { reinterpret_cast<SetVisibleFn>(visTarget)(clip,visible); }
    __except(EXCEPTION_EXECUTE_HANDLER){ ok=false; }
    ReleaseHudHolder(holder);
    char msg[320]{}; std::snprintf(msg,sizeof(msg),
        "V62.4-FLASHUI visibility clip=%s visible=%s ok=%s",name,visible?"true":"false",ok?"true":"false");
    debug_trace::Write(msg);
    return ok;
}

bool SetCombatCursorVisible(bool visible) noexcept {
    return SetHudMovieClipVisible("CombatCursorCross", visible);
}

void Uninstall() noexcept {
    g_last_hud.store(nullptr,std::memory_order_release);
    g_last_hud_tick.store(0,std::memory_order_release);
    if(g_hook){ (void)g_hook->disable(); g_hook.reset(); }
    g_original=nullptr;
}
}
