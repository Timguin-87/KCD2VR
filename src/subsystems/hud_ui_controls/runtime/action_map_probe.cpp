#include "action_map_probe.h"
#include "ui_back_maps135r2.h"
#include "retail_build112.h"
#include <array>
#include "diagnostic_trace.h"

#include <DetourModKit/hook.hpp>
#include <windows.h>
#include <atomic>
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <optional>
#include <utility>

namespace kcd2vr::action_map_probe { namespace {
constexpr std::uint32_t kTimestamp=0x6A350E20u;
constexpr std::uintptr_t kActionMapEnableRva=0x85ADF4ull;
constexpr unsigned char kEnableEntry[]={0x48,0x89,0x5C,0x24,0x10,0x48,0x89,0x74,0x24,0x18,0x57,0x48,0x83,0xEC,0x40,0x40,0x8A,0xF2};
using EnableFn=void(__fastcall*)(void*,bool);
std::optional<DetourModKit::hook::Hook> g_hook;
EnableFn g_original=nullptr;
std::atomic<bool> g_shop_root{false},g_shop_inventory{false},g_shop_trader{false};
std::atomic<bool> g_shop_known{false};
std::atomic<bool> g_apse_root{false},g_apse_inventory{false},g_apse_player{false},g_apse_map{false},g_apse_questlog{false},g_apse_crafting{false};
std::atomic<bool> g_apse_known{false};
std::atomic<bool> g_stone_throwing{false},g_alchemy{false},g_alchemy_reading{false};
std::atomic<std::uint64_t> g_transitions{0};
std::array<std::atomic<bool>,ui_back_maps135r2::BackMaps.size()> g_back_maps{};
std::array<std::atomic<bool>,ui_back_maps135r2::Minigames.size()> g_minigame_maps162r1{};
constexpr std::size_t QamIndex171(std::string_view name){
    for(std::size_t i=0;i<ui_back_maps135r2::BackMaps.size();++i)
        if(ui_back_maps135r2::BackMaps[i]==name)return i;
    return ui_back_maps135r2::BackMaps.size();
}
static_assert(QamIndex171("qam_interaction_weapon")<ui_back_maps135r2::BackMaps.size());
static_assert(QamIndex171("qam_interaction_food")<ui_back_maps135r2::BackMaps.size());
static_assert(QamIndex171("chat_interaction")<ui_back_maps135r2::BackMaps.size()); // R59.16C5

bool Readable(const void* p,std::size_t n=1) noexcept {
    if(!p) return false; MEMORY_BASIC_INFORMATION m{};
    if(!VirtualQuery(p,&m,sizeof(m)) || m.State!=MEM_COMMIT || (m.Protect&(PAGE_NOACCESS|PAGE_GUARD))) return false;
    const auto a=reinterpret_cast<std::uintptr_t>(p), e=reinterpret_cast<std::uintptr_t>(m.BaseAddress)+m.RegionSize;
    return a<=e && n<=e-a;
}
bool Exact(std::uintptr_t b) noexcept {
    __try {
        if(!b) return false; auto* d=reinterpret_cast<IMAGE_DOS_HEADER*>(b); if(d->e_magic!=IMAGE_DOS_SIGNATURE)return false;
        auto* n=reinterpret_cast<IMAGE_NT_HEADERS64*>(b+d->e_lfanew);
        return n->Signature==IMAGE_NT_SIGNATURE && n->FileHeader.TimeDateStamp==kTimestamp &&
            Readable(reinterpret_cast<void*>(b+kActionMapEnableRva),sizeof(kEnableEntry)) &&
            std::memcmp(reinterpret_cast<void*>(b+kActionMapEnableRva),kEnableEntry,sizeof(kEnableEntry))==0;
    } __except(EXCEPTION_EXECUTE_HANDLER){ return false; }
}



const char* ReadName(void* self) noexcept {
    // libKCD2 1.5.6 CActionMap: CryString data pointer at +0x58.
    __try {
        if(!self || !Readable(static_cast<unsigned char*>(self)+0x58,sizeof(const char*))) return nullptr;
        const char* p=*reinterpret_cast<const char**>(static_cast<unsigned char*>(self)+0x58);
        if(!p || !Readable(p,1)) return nullptr;
        // Refuse corrupt/non-terminated names instead of walking arbitrary memory.
        for(std::size_t i=0;i<96;++i){ if(!Readable(p+i,1)) return nullptr; if(p[i]=='\0') return i? p:nullptr; }
    } __except(EXCEPTION_EXECUTE_HANDLER){}
    return nullptr;
}
void SetKnown(const char* name,bool enabled) noexcept {
    if(!name) return;
    for(std::size_t i=0;i<g_minigame_maps162r1.size();++i)if(ui_back_maps135r2::Minigames[i]==name){
        if(g_minigame_maps162r1[i].exchange(enabled)!=enabled){g_transitions.fetch_add(1);char m[200]{};
            std::snprintf(m,sizeof(m),"V66.162R1-MINIGAME map=%s active=%d controls=NATIVE_XI",name,enabled);diagnostic_trace::Write(m);}
        break;
    }
    for(std::size_t i=0;i<g_back_maps.size();++i)if(ui_back_maps135r2::BackMaps[i]==name){
        if(g_back_maps[i].exchange(enabled)!=enabled){
            g_transitions.fetch_add(1);
            char m[192]{};std::snprintf(m,sizeof(m),"V66.135R2-BACK-MAP %s=%s input=NATIVE_XI_B nativeExitRules=PRESERVED",name,enabled?"ON":"OFF");
            diagnostic_trace::Write(m);
        }
        break;
    }
    std::atomic<bool>* slot=nullptr;
    if(std::strcmp(name,"shop")==0){slot=&g_shop_root;g_shop_known.store(true,std::memory_order_release);}
    else if(std::strcmp(name,"shop_inventory")==0){slot=&g_shop_inventory;g_shop_known.store(true,std::memory_order_release);}
    else if(std::strcmp(name,"shop_trader")==0){slot=&g_shop_trader;g_shop_known.store(true,std::memory_order_release);}
    else if(std::strcmp(name,"apse")==0){slot=&g_apse_root;g_apse_known.store(true,std::memory_order_release);}
    else if(std::strcmp(name,"apse_inventory")==0){slot=&g_apse_inventory;g_apse_known.store(true,std::memory_order_release);}
    else if(std::strcmp(name,"apse_player")==0){slot=&g_apse_player;g_apse_known.store(true,std::memory_order_release);}
    else if(std::strncmp(name,"apse_map_",9)==0){slot=&g_apse_map;g_apse_known.store(true,std::memory_order_release);}
    else if(std::strcmp(name,"apse_questlog")==0 || std::strcmp(name,"apse_questlog_map")==0){slot=&g_apse_questlog;g_apse_known.store(true,std::memory_order_release);}
    else if(std::strcmp(name,"apse_crafting")==0){slot=&g_apse_crafting;g_apse_known.store(true,std::memory_order_release);}
    else if(std::strcmp(name,"stone_throwing")==0) slot=&g_stone_throwing;
    else if(std::strcmp(name,"alchemy")==0) slot=&g_alchemy;
    else if(std::strcmp(name,"alchemy_reading")==0) slot=&g_alchemy_reading;
    if(!slot) return;
    const bool old=slot->exchange(enabled,std::memory_order_acq_rel);
    if(old==enabled) return;
    const auto seq=g_transitions.fetch_add(1,std::memory_order_relaxed)+1;
    char m[256]{}; std::snprintf(m,sizeof(m),"V65.83-ACTIONMAP %s=%s transition=%llu authority=KCD2-CActionMap::Enable",name,enabled?"ON":"OFF",(unsigned long long)seq);
    diagnostic_trace::Write(m);
}
void __fastcall Detour(void* self,bool enabled) noexcept {
    if(g_original) g_original(self,enabled);
    const char* n=ReadName(self); if(n) SetKnown(n,enabled);
}
void Reset() noexcept {
    g_shop_root.store(false);g_shop_inventory.store(false);g_shop_trader.store(false);g_shop_known.store(false);
    g_apse_root.store(false);g_apse_inventory.store(false);g_apse_player.store(false);g_apse_map.store(false);g_apse_questlog.store(false);g_apse_crafting.store(false);g_apse_known.store(false);
    for(auto& map:g_back_maps)map.store(false);
    for(auto& map:g_minigame_maps162r1)map.store(false);
    g_stone_throwing.store(false);g_alchemy.store(false);g_alchemy_reading.store(false);g_transitions.store(0);
}
} // namespace

bool Install() noexcept {
    Reset(); const auto b=reinterpret_cast<std::uintptr_t>(GetModuleHandleW(L"WHGame.dll"));
    if(!Exact(b)||!retail_build112::Verify(b)){diagnostic_trace::Write("V65.83-ACTIONMAP refused: exact WHGame/CActionMap::Enable mismatch");return false;}
    DetourModKit::hook::InlineRequest req{"kcd2vr_v6583_actionmap_enable",DetourModKit::hook::Target{DetourModKit::Address{b+kActionMapEnableRva}},DetourModKit::hook::Options{}};
    auto h=DetourModKit::hook::inline_at(std::move(req),&Detour); if(!h)return false;
    g_hook=std::move(*h);g_original=g_hook->original<EnableFn>();
    if(!g_original||!g_hook->enable()){g_original=nullptr;g_hook.reset();return false;}
    diagnostic_trace::Write("V66.69-ACTIONMAP passive CActionMap::Enable observer READY; Shop + APSE/Inventory + Stone/Alchemy; NO Poll/GetActionMap worker calls");return true;
}

void Uninstall() noexcept { if(g_hook){(void)g_hook->disable();g_hook.reset();}g_original=nullptr;Reset(); }
bool ApseRoot() noexcept{return g_apse_root.load(std::memory_order_acquire);}
bool ApseMap() noexcept{return g_apse_map.load(std::memory_order_acquire);}
bool WeaponQamOpen171() noexcept{return g_back_maps[QamIndex171("qam_interaction_weapon")].load(std::memory_order_acquire);}
bool FoodQamOpen171() noexcept{return g_back_maps[QamIndex171("qam_interaction_food")].load(std::memory_order_acquire);}
bool Alchemy() noexcept{return g_alchemy.load(std::memory_order_acquire);} 
bool ChatOpen59c5() noexcept{return g_back_maps[QamIndex171("chat_interaction")].load(std::memory_order_acquire);} // R59.16C5: native quick-talk menu
bool BackContext() noexcept{for(auto& map:g_back_maps)if(map.load(std::memory_order_acquire))return true;return false;}
bool NativeMinigame() noexcept{for(auto& map:g_minigame_maps162r1)if(map.load(std::memory_order_acquire))return true;return false;}
std::uint64_t Transitions() noexcept{return g_transitions.load(std::memory_order_relaxed);} 
}
