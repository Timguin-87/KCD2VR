namespace defense27 {
static std::optional<DetourModKit::hook::Hook> candidateHook;
static bool retired=false;
template<std::size_t N> static bool Pin(std::uintptr_t rva,const unsigned char(&want)[N]) noexcept {
    unsigned char got[N]{};return SehCopy146(reinterpret_cast<const void*>(g_base+rva),got,N)&&!std::memcmp(got,want,N);
}
static bool Install() noexcept {
    if(retired||!retail_build112::Ready(g_base)||!services59c5::counterHook||!g_reaction_dispatch50||!g_submit)return false;if(candidateHook){ // C29: reinstall after a failed disable: hook still live, so report ready only with enabled restored
        const bool live=candidateHook->is_enabled();if(live)enabled.store(true,std::memory_order_release);return live;}
    constexpr unsigned char entry[]{0x40,0x55,0x53,0x56,0x57,0x41,0x54,0x41,0x55,0x41,0x56,0x48,0x8D,0xAC,0x24,0x50,0xFF,0xFF,0xFF};
    constexpr unsigned char call[]{0xE8,0x00,0x24,0x7D,0xFF};
    constexpr unsigned char evaluation[]{0xE8,0xE0,0x60,0xAE,0x00};
    constexpr unsigned char type[]{0x48,0x8D,0x05,0x33,0x40,0xE3,0x02};
    constexpr unsigned char managerLoad[]{0x48,0x8B,0x82,0x00,0x02,0x00,0x00};
    constexpr unsigned char managerStore[]{0x48,0x89,0x47,0x40};
    constexpr unsigned char rowStore[]{0x48,0x89,0x48,0x58};
    if(!Pin(0xC55BB4,entry)||!Pin(0x14837AF,call)||!Pin(0xC55C8F,evaluation)||!Pin(0xC5564E,type)||
       !Pin(0xC5588E,managerLoad)||!Pin(0xC558D1,managerStore)||!Pin(0xC55D11,rowStore))return false;
    using namespace DetourModKit::hook;Options options{};options.fail_if_already_hooked=true;
    auto hook=inline_at(InlineRequest{"kcd2vr_c27_npc_defense_candidate_receipt",Target{DetourModKit::Address{g_base+0xC55BB4}},options},CandidateDetour);
    if(!hook)return false;candidateHook=std::move(*hook);candidateOriginal=candidateHook->original<CandidateFn>();
    if(!candidateOriginal||!candidateHook->enable()){candidateHook.reset();candidateOriginal=nullptr;return false;}enabled.store(true,std::memory_order_release);return true;
}
static void Uninstall() noexcept {
    enabled.store(false,std::memory_order_release);
    if(!candidateHook)return;if(candidateHook->is_enabled()&&!candidateHook->disable())return;
    if(candidateHook->is_enabled())return;
    // Entered callbacks may still call original; no inline rundown is supplied.
    candidateHook->release();candidateHook.reset();retired=true; // keep original process-live; no reinstall
}
}
