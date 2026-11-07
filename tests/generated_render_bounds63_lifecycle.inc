// Generated verbatim production install/shutdown; do not edit.
static bool InstallHenryBounds63(std::uintptr_t base) noexcept {
    if(g_henry_bounds_retired63.load(std::memory_order_acquire))return false;
    if(g_henry_bounds_enabled63.load(std::memory_order_acquire))return true;
    const unsigned char getter[]{0x48,0x8D,0x81,0xE0,0x09,0x00,0x00,0xC3};
    const unsigned char caller[]{0x48,0x8B,0x02,0x48,0x8B,0xCA,0xFF,0x90,0xA8,0x00,0x00,0x00,
        0xF3,0x0F,0x10,0x00,0xF3,0x0F,0x5D,0x03,0xF3,0x0F,0x11,0x03};
    std::uintptr_t slot=0;
    if(!(retail_build112::Ready(base)||retail_build112::Verify(base))||!SupportedWhBuild(base)||!Read(reinterpret_cast<void*>(base+rb63::kCharacterVft+0xA8),slot)||slot!=base+rb63::kGetter||
       !Safe(reinterpret_cast<void*>(base+rb63::kGetter),sizeof(getter))||
       std::memcmp(reinterpret_cast<void*>(base+rb63::kGetter),getter,sizeof(getter))||
       !Safe(reinterpret_cast<void*>(base+0x63F902),sizeof(caller))||
       std::memcmp(reinterpret_cast<void*>(base+0x63F902),caller,sizeof(caller)))return false;
    HMODULE module63=nullptr;
    if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_PIN,
       reinterpret_cast<LPCWSTR>(&HenryBoundsDetour63),&module63)||!module63)return false;
    DetourModKit::hook::InlineRequest req{"kcd2vr_c63_henry_render_bounds",DetourModKit::hook::Target{DetourModKit::Address{base+rb63::kGetter}},DetourModKit::hook::Options{}};
    auto hook=DetourModKit::hook::inline_at(std::move(req),&HenryBoundsDetour63);
    if(!hook)return false;
    g_henry_bounds_hook63=std::move(*hook);
    g_henry_bounds_original63=g_henry_bounds_hook63->original<HenryBoundsGetter63>();
    if(g_henry_bounds_original63&&g_henry_bounds_hook63->enable()){
        g_henry_bounds_enabled63.store(true,std::memory_order_release);return true;}
    // Failed enable is not proof that no callback remains reachable. Retain
    // the pinned module, exact hook handle and immutable original for lifetime.
    g_henry_bounds_retired63.store(true,std::memory_order_release);
    g_henry_bounds_enabled63.store(false,std::memory_order_release);return false;
}
static void ShutdownHenryBounds63() noexcept {
    g_henry_bounds_enabled63.store(false,std::memory_order_release);
    (void)g_render_bounds63.publish({});
    if(g_henry_bounds_hook63){
        g_henry_bounds_retired63.store(true,std::memory_order_release);
        (void)g_henry_bounds_hook63->disable();
        // A successful disable also does not drain in-flight frames. Retain
        // the original/trampoline and prohibit a second installation either way.
    }
}
