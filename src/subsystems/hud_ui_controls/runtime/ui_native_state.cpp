#include "ui_native_state.h"
#include "flashui_probe.h"
#include "diagnostic_trace.h"
#include "ui_visibility_registry.h"
#include "ui_native_catalogue.h"
#include <DetourModKit/hook.hpp>
#include <windows.h>
#include <array>
#include <atomic>
#include <cstring>
#include <optional>
#include <utility>
#include "quick_talk_native_h19.inl"

namespace kcd2vr::ui_native_state { namespace {
constexpr std::uint32_t kTimestamp=0x6A350E20u;
constexpr std::uintptr_t kElementVtable=0x4049BD0;
constexpr std::uintptr_t kVisible=0x568510, kDestroy=0x16E94B0;
constexpr std::uintptr_t kShopUpdate=0x56724C, kCraftOn=0x1813E00, kCraftOff=0x1813ED0;
std::uintptr_t g_base=0;
std::atomic<std::uint64_t> g_losses{0},g_observations{0};
std::atomic<std::uint32_t> g_installed{0};
ui_visibility_registry::Queue g_events;
ui_visibility_registry::Registry g_registry;
std::atomic<std::uint32_t> g_unknown{0};
std::atomic<std::uintptr_t> g_last_event[Count]{};
std::atomic<std::uint64_t> g_kind_samples[Count]{};
using VisibleFn=void(__fastcall*)(void*,bool);
using DestroyFn=void*(__fastcall*)(void*,unsigned);
using UpdateFn=void(__fastcall*)(void*,float);
using CraftOnFn=void(__fastcall*)(void*,bool);
using CraftOffFn=void(__fastcall*)(void*);
VisibleFn g_visible=nullptr; DestroyFn g_destroy=nullptr, g_shop_destroy=nullptr;
UpdateFn g_shop=nullptr; CraftOnFn g_craft_on=nullptr; CraftOffFn g_craft_off=nullptr;
std::optional<DetourModKit::hook::Hook> g_hooks[6];

bool Match(std::uintptr_t rva,const unsigned char* p,std::size_t n) noexcept {
    __try { return g_base && std::memcmp(reinterpret_cast<void*>(g_base+rva),p,n)==0; }
    __except(EXCEPTION_EXECUTE_HANDLER) { return false; }
}
bool Header(std::uintptr_t b) noexcept {
    __try { auto* d=reinterpret_cast<IMAGE_DOS_HEADER*>(b);
        if(!b||d->e_magic!=IMAGE_DOS_SIGNATURE)return false;
        auto* n=reinterpret_cast<IMAGE_NT_HEADERS64*>(b+d->e_lfanew);
        return n->Signature==IMAGE_NT_SIGNATURE&&n->FileHeader.TimeDateStamp==kTimestamp;
    } __except(EXCEPTION_EXECUTE_HANDLER) { return false; }
}
unsigned Classify(const char* n) noexcept {
    if(!n)return Count;
    const auto* entry=ui_native_catalogue::Find(n);
    return entry ? static_cast<unsigned>(entry->kind) : Count;
}
bool ElementFields(void* self,unsigned& kind,bool& visible) noexcept {
    char name[64]{};
    __try {
        auto* p=static_cast<unsigned char*>(self);
        if(!p||*reinterpret_cast<std::uintptr_t*>(p)!=g_base+kElementVtable)return false;
        const auto v=p[0x78]; if(v>1)return false;
        const char* s=*reinterpret_cast<const char**>(p+0x30); if(!s)return false;
        unsigned i=0;for(;i<63&&s[i];++i)name[i]=s[i]; if(i==63&&s[i])return false;
        visible=v!=0;
    } __except(EXCEPTION_EXECUTE_HANDLER) { return false; }
    kind=Classify(name);return kind<Count;
}
bool Byte(void* p,std::size_t offset,bool& value) noexcept {
    __try { const auto v=static_cast<unsigned char*>(p)[offset];if(v>1)return false;value=v!=0;return true; }
    __except(EXCEPTION_EXECUTE_HANDLER) { return false; }
}
void Publish(void* owner,unsigned kind,bool visible,bool destroy=false) noexcept {
    if(kind>=Count||!owner)return;
    g_observations.fetch_add(1,std::memory_order_relaxed);g_kind_samples[kind].fetch_add(1,std::memory_order_relaxed);
    const auto address=reinterpret_cast<std::uintptr_t>(owner);
    if((address&3)==0){const auto signature=address|(visible?1u:0u)|(destroy?2u:0u);
        if(g_last_event[kind].exchange(signature,std::memory_order_acq_rel)==signature)return;
    }
    if(!g_events.Push({reinterpret_cast<std::uintptr_t>(owner),kind,visible,destroy})){
        g_losses.fetch_add(1,std::memory_order_relaxed);g_unknown.fetch_or(1u<<kind,std::memory_order_release);
    }
}
void __fastcall Visible(void* p,bool request) {
    g_visible(p,request);
    unsigned k=Count;bool actual=false;
    if(ElementFields(p,k,actual))Publish(p,k,actual);
}
void* __fastcall Destroy(void* p,unsigned flags) {
    unsigned k=Count;bool actual=false;const bool identified=ElementFields(p,k,actual);
    if(identified)Publish(p,k,false,true);
    // Pause/death caches carry the same native movie lifetime contract. Notify
    // before destruction, including address reuse; no Flash calls are made here.
    if(identified&&(k==Hud||k==ItemSelection||k==ItemTransfer||k==Menu||k==GameOver||k==Overlay))flashui_probe::NotifyHudDestroyed(p);
    void* result=g_destroy(p,flags);
    // No dereference after destruction. Nested native hides cannot resurrect it.
    if(identified)Publish(p,k,false,true);
    return result;
}
void* __fastcall ShopDestroy(void* p,unsigned flags) {
    Publish(p,Shop,false,true);
    auto* result=g_shop_destroy(p,flags);
    Publish(p,Shop,false,true);return result;
}
void __fastcall ShopUpdate(void* p,float dt) {
    g_shop(p,dt);
    bool value=false,valid=false;
    __try {
        auto* b=static_cast<unsigned char*>(p);
        valid=*reinterpret_cast<std::uintptr_t*>(b)==g_base+0x3D2D628 &&
            (*reinterpret_cast<std::uintptr_t**>(b+0x28))[1]==g_base+0x1A7DE10;
    } __except(EXCEPTION_EXECUTE_HANDLER) { valid=false; }
    if(valid&&Byte(p,0x30,value))Publish(p,Shop,value);
}
void __fastcall CraftOn(void* p,bool mode) {g_craft_on(p,mode);bool v=false;if(Byte(p,0x2D1,v))Publish(p,Crafting,v);}
void __fastcall CraftOff(void* p) {g_craft_off(p);bool v=false;if(Byte(p,0x2D1,v))Publish(p,Crafting,v);}
template<class F> bool Hook(unsigned slot,std::uintptr_t rva,F detour,F& original,const char* name) {
    DetourModKit::hook::InlineRequest req{name,DetourModKit::hook::Target{DetourModKit::Address{g_base+rva}},DetourModKit::hook::Options{}};
    auto h=DetourModKit::hook::inline_at(std::move(req),detour);if(!h)return false;
    g_hooks[slot]=std::move(*h); original=g_hooks[slot]->original<F>();
    if(!original||!g_hooks[slot]->enable()){g_hooks[slot].reset();original=nullptr;return false;}return true;
}
} // namespace
bool Install() noexcept {
    if(g_installed.load())return true;
    g_base=reinterpret_cast<std::uintptr_t>(GetModuleHandleW(L"WHGame.dll"));
    if(!Header(g_base))return false;
    // Signatures generated from the supplied DLL and accompanied by static evidence.
    static constexpr unsigned char vis[]={0x48,0x89,0x5c,0x24,0x08,0x48,0x89,0x74,0x24,0x10,0x57,0x48,0x83,0xec,0x30};
    static constexpr unsigned char get[]={0x0f,0xb6,0x41,0x78,0xc3};
    static constexpr unsigned char name[]={0x48,0x8b,0x41,0x30,0xc3};
    bool shape=false;
    __try { auto* vt=reinterpret_cast<std::uintptr_t*>(g_base+kElementVtable);
        shape=vt[0xE0/8]==g_base+kVisible&&vt[0xE8/8]==g_base+0x1A7D900&&vt[0x50/8]==g_base+0x72DD20&&vt[0]==g_base+kDestroy;
    } __except(EXCEPTION_EXECUTE_HANDLER) { shape=false; }
    static constexpr unsigned char destroy[]={0x48,0x89,0x5c,0x24,0x08,0x57,0x48,0x83,0xec,0x20,0x8b,0xda,0x48,0x8b,0xf9};
    if(!Match(kDestroy,destroy,sizeof(destroy)))return false;
    if(!shape||!Match(kVisible,vis,sizeof(vis))||!Match(0x1A7D900,get,sizeof(get))||!Match(0x72DD20,name,sizeof(name)))return false;
    if(!Hook(0,kVisible,&Visible,g_visible,"ui88_flash_visible"))return false;
    if(!Hook(1,kDestroy,&Destroy,g_destroy,"ui88_flash_destroy")){Uninstall();return false;}
    std::uint32_t installed=1;
    static constexpr unsigned char shop[]={0x40,0x53,0x48,0x83,0xec,0x30,0x48,0x8b,0xd9,0x0f,0x29,0x74,0x24,0x20,0x48,0x83};
    if(Match(kShopUpdate,shop,sizeof(shop))&&Match(0x2BA5B98,destroy,sizeof(destroy))&&Hook(2,kShopUpdate,&ShopUpdate,g_shop,"ui88_shop_update")){
        if(Hook(5,0x2BA5B98,&ShopDestroy,g_shop_destroy,"ui88_shop_destroy"))installed|=2;
        else{(void)g_hooks[2]->disable();g_hooks[2].reset();g_shop=nullptr;}
    }
    static constexpr unsigned char con[]={0x48,0x89,0x5c,0x24,0x18,0x88,0x54,0x24,0x10,0x57,0x48,0x83,0xec,0x20,0x48,0x8d};
    static constexpr unsigned char coff[]={0x48,0x89,0x5c,0x24,0x10,0x48,0x89,0x74,0x24,0x18,0x57,0x48,0x83,0xec,0x30,0x48};
    if(Match(kCraftOn,con,sizeof(con))&&Match(kCraftOff,coff,sizeof(coff))&&Hook(3,kCraftOn,&CraftOn,g_craft_on,"ui88_craft_on")){
        if(Hook(4,kCraftOff,&CraftOff,g_craft_off,"ui88_craft_off"))installed|=4;
        else{(void)g_hooks[3]->disable();g_hooks[3].reset();g_craft_on=nullptr;}
    }
    g_installed.store(installed,std::memory_order_release);
    diagnostic_trace::Write(quick_talk_native_h19::Install()?"H19 QUICK_TALK observer=READY source=DEDICATED_NATIVE_CHAT_WRAPPERS polling=NONE":"H19 QUICK_TALK observer=UNAVAILABLE choicesRequireNativeWitness=1");
    diagnostic_trace::Write("V66.89-UI-LIFECYCLE 28 native definitions / 30 observed kinds; exact existing hooks; no manager polling; UNKNOWN until observed");
    return true;
}
void Uninstall() noexcept {
    quick_talk_native_h19::Uninstall();
    for(auto& h:g_hooks)if(h){(void)h->disable();h.reset();}
    g_installed.store(0);
    // Function pointers intentionally remain valid until hooks have been disabled.
}
Snapshot Read() noexcept {
    // One UI worker is the sole consumer. Only copies fixed-size native observations.
    ui_visibility_registry::Event e{};
    for(unsigned i=0;i<512&&g_events.Pop(e);++i){
        if(!g_registry.Apply(e,ui_native_catalogue::OwnershipMask)){
            g_losses.fetch_add(1,std::memory_order_relaxed);g_unknown.fetch_or(1u<<e.kind,std::memory_order_release);
        }
    }
    const auto state=g_registry.Read();Snapshot s{};
    const auto unknown=g_unknown.load(std::memory_order_acquire);
    s.known=state.known&~unknown;s.visible=state.visible&~unknown;s.epoch=state.epoch;
    s.installed=g_installed.load(std::memory_order_acquire);s.losses=g_losses.load(std::memory_order_relaxed);s.observations=g_observations.load(std::memory_order_relaxed);
    for(unsigned i=0;i<Count;++i)s.samples[i]=g_kind_samples[i].load(std::memory_order_relaxed);
    return s;
}
void ObserveElement(void* element) noexcept {
    if(!(g_installed.load(std::memory_order_acquire)&1u))return;
    unsigned k=Count;bool v=false;if(ElementFields(element,k,v))Publish(element,k,v);
}
} // namespace
