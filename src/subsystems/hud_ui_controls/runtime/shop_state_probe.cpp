#include "shop_state_probe.h"
#include "diagnostic_trace.h"
#include <DetourModKit/hook.hpp>
#include <windows.h>
#include <atomic>
#include <cstdint>
#include <cstring>
#include <optional>
#include <utility>

namespace kcd2vr::shop_state_probe { namespace {
constexpr std::uint32_t kTimestamp=0x6A350E20u;
constexpr std::uintptr_t kIsShopOpenedRttrRva=0x2EFE6B0ull;
constexpr std::uintptr_t kFindShopByTypeRva=0x2EFE54Cull;
constexpr unsigned char kEntry[]={0x40,0x53,0x48,0x83,0xEC,0x20,0x48,0x8B,0x49,0x70,0x48,0x8B,0xDA,0x41,0x8B,0xD0};
using WrapperFn=void(__fastcall*)(void*,void*,std::uint32_t);
using FindFn=void*(__fastcall*)(void*,std::uint32_t);
std::optional<DetourModKit::hook::Hook> g_hook; WrapperFn g_original=nullptr; FindFn g_find=nullptr;
std::atomic<bool> g_open{false}; std::atomic<std::uint64_t> g_samples{0};
bool readable(const void* p,std::size_t n=1) noexcept { if(!p)return false; MEMORY_BASIC_INFORMATION m{}; if(!VirtualQuery(p,&m,sizeof(m))||m.State!=MEM_COMMIT||(m.Protect&(PAGE_NOACCESS|PAGE_GUARD)))return false; auto a=(std::uintptr_t)p,e=(std::uintptr_t)m.BaseAddress+m.RegionSize; return a<=e&&n<=e-a; }
bool exact(std::uintptr_t b) noexcept { __try{auto*d=(IMAGE_DOS_HEADER*)b;if(!b||d->e_magic!=IMAGE_DOS_SIGNATURE)return false;auto*n=(IMAGE_NT_HEADERS64*)(b+d->e_lfanew);return n->Signature==IMAGE_NT_SIGNATURE&&n->FileHeader.TimeDateStamp==kTimestamp&&readable((void*)(b+kIsShopOpenedRttrRva),sizeof(kEntry))&&std::memcmp((void*)(b+kIsShopOpenedRttrRva),kEntry,sizeof(kEntry))==0;}__except(EXCEPTION_EXECUTE_HANDLER){return false;} }
void __fastcall Detour(void* wrapper,void* out,std::uint32_t typeId) noexcept {
    if(g_original) g_original(wrapper,out,typeId);
    bool open=false;
    __try {
        if(wrapper&&g_find&&readable((unsigned char*)wrapper+0x70,sizeof(void*))){
            void* owner=*(void**)((unsigned char*)wrapper+0x70);
            if(owner&&readable((unsigned char*)owner+0x10,sizeof(void*))){
                void* registry=*(void**)((unsigned char*)owner+0x10);
                void* shop=registry?g_find(registry,typeId):nullptr;
                if(shop&&readable((unsigned char*)shop+0x198,sizeof(void*))) open=(*(void**)((unsigned char*)shop+0x198)!=nullptr);
            }
        }
    } __except(EXCEPTION_EXECUTE_HANDLER){open=false;}
    g_open.store(open,std::memory_order_release);g_samples.fetch_add(1,std::memory_order_relaxed);
}
} // namespace
bool Install() noexcept {
    const auto b=(std::uintptr_t)GetModuleHandleW(L"WHGame.dll"); if(!exact(b)){diagnostic_trace::Write("V65.10-SHOP native IsShopOpened probe refused: exact build/entry mismatch");return false;}
    g_find=(FindFn)(b+kFindShopByTypeRva);
    DetourModKit::hook::InlineRequest req{"kcd2vr_v6510_native_shop_open",DetourModKit::hook::Target{DetourModKit::Address{b+kIsShopOpenedRttrRva}},DetourModKit::hook::Options{}};
    auto h=DetourModKit::hook::inline_at(std::move(req),&Detour);if(!h)return false;g_hook=std::move(*h);g_original=g_hook->original<WrapperFn>();if(!g_original||!g_hook->enable()){g_original=nullptr;g_hook.reset();return false;}
    diagnostic_trace::Write("V65.10-SHOP positive native owner installed: exact IsShopOpened RTTR wrapper +0x2EFE6B0, C_UIShop open field +0x198");return true;
}
void Uninstall() noexcept {g_open.store(false);if(g_hook){(void)g_hook->disable();g_hook.reset();}g_original=nullptr;g_find=nullptr;}
 void Clear() noexcept{g_open.store(false,std::memory_order_release);}
}
