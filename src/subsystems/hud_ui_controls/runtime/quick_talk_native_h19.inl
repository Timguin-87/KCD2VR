// Compiled once by the existing native UI lifecycle TU. Dedicated native chat
// wrappers only; the broad retired CActionMap::Enable observer stays uninstalled.
#include "quick_talk_native_h19.h"
#include "retail_build112.h"
namespace kcd2vr::quick_talk_native_h19 { namespace {
using ToggleFn=void(__fastcall*)(void*,bool);
ToggleFn g_toggle=nullptr,g_input=nullptr;
std::optional<DetourModKit::hook::Hook> g_hooks[2];
std::atomic<bool> g_ready{false};
// revision and state share one atomic word: unknown0, disabled1, enabled3.
// A nested newer native transition cannot be overwritten by an outer return.
std::atomic<std::uint64_t> g_state{0};
std::uint64_t Begin() noexcept {
    auto old=g_state.load(std::memory_order_acquire);
    for(;;){const auto next=((old>>2)+1)<<2;
        if(g_state.compare_exchange_weak(old,next,std::memory_order_acq_rel))return next;}
}
void Complete(std::uint64_t token,bool enabled) noexcept {
    auto expected=token;(void)g_state.compare_exchange_strong(expected,token|(enabled?3ull:1ull),std::memory_order_release);
}
bool ChatInput(void* self,bool& enabled) noexcept {
    __try {if(!self)return false;const auto* p=static_cast<const unsigned char*>(self);
        // 61615A/616183 distinguish quick chat from full dialogue; 61613D/141
        // use this exact native boolean. No pointer survives the callback.
        if(p[0x35]!=1||p[0]>1)return false;enabled=p[0]!=0;return true;
    }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
void __fastcall Toggle(void* self,bool enabled) {
    const auto token=Begin();g_toggle(self,enabled);Complete(token,enabled);
}
void __fastcall Input(void* self,bool enabled) {
    bool before=false;
    const bool chat=ChatInput(self,before);
    const auto token=chat?Begin():0;
    g_input(self,enabled);
    if(chat){bool actual=false;if(ChatInput(self,actual))Complete(token,actual);}
}
bool Match(std::uintptr_t base,std::uintptr_t rva,const unsigned char* bytes,std::size_t size) noexcept {
    __try {return std::memcmp(reinterpret_cast<const void*>(base+rva),bytes,size)==0;}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
template<class F> bool Hook(unsigned index,std::uintptr_t base,std::uintptr_t rva,F detour,F& original,const char* name) {
    DetourModKit::hook::Options options{};options.fail_if_already_hooked=true;
    auto hook=DetourModKit::hook::inline_at(DetourModKit::hook::InlineRequest{name,DetourModKit::hook::Target{DetourModKit::Address{base+rva}},options},detour);
    if(!hook)return false;g_hooks[index]=std::move(*hook);original=g_hooks[index]->original<F>();
    return original&&g_hooks[index]->enable();
}
} // namespace
bool Install() noexcept {
    if(g_ready.load(std::memory_order_acquire))return true;
    const auto base=reinterpret_cast<std::uintptr_t>(GetModuleHandleW(L"WHGame.dll"));
    if(!base||!retail_build112::Verify(base))return false;
    constexpr unsigned char entry[]{0x40,0x53,0x48,0x83,0xEC,0x20,0x8A,0xDA,0xE8,0xCF,0xF4,0x2F,0x00};
    constexpr unsigned char input[]{0x48,0x89,0x5C,0x24,0x08,0x48,0x89,0x74,0x24,0x10,0x57,0x48,0x83,0xEC,0x20,0x40,0x8A,0xFA};
    constexpr unsigned char selector[]{0x80,0x7E,0x35,0x00};
    constexpr unsigned char map[]{'c','h','a','t','_','i','n','t','e','r','a','c','t','i','o','n',0};
    if(!Match(base,0x6160EC,entry,sizeof(entry))||!Match(base,0x616128,input,sizeof(input))||
       !Match(base,0x61615A,selector,sizeof(selector))||!Match(base,0x3A42518,map,sizeof(map)))return false;
    Begin();
    if(!Hook(0,base,0x6160EC,&Toggle,g_toggle,"h19_chat_map_toggle")||
       !Hook(1,base,0x616128,&Input,g_input,"h19_chat_input_toggle")){Uninstall();return false;}
    g_ready.store(true,std::memory_order_release);return true;
}
void Uninstall() noexcept {
    g_ready.store(false,std::memory_order_release);Begin();
    for(auto& h:g_hooks)if(h){(void)h->disable();h.reset();}
    // Original trampolines remain valid until callback teardown completes.
}
Snapshot Read() noexcept {
    const auto state=g_state.load(std::memory_order_acquire);
    return {g_ready.load(std::memory_order_acquire),(state&1)!=0,(state&3)==3,state>>2};
}
}
