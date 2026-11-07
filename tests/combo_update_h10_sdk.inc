// Generated verbatim production SDK callbacks/installation. Host SDK context/patch creation leaves only.
namespace combo59c17 {
// Exact retail register seams. SDK-specific hook construction stays outside the
// extracted production policy prefix; host tests call the same policy functions.
std::optional<DetourModKit::hook::Hook> g_cleanup_mid_h10,g_hit_read_mid_h10;
std::atomic<bool> g_update_hooks_ready_h10{false};
static void CleanupMidH10(DetourModKit::hook::MidContext& ctx) noexcept {
    if(!g_update_hooks_ready_h10.load(std::memory_order_acquire)||!g_ready.load(std::memory_order_acquire)||g_teardown_pending19.load(std::memory_order_acquire))return;
    ObserveOrdinaryCleanupH10(reinterpret_cast<const void*>(DetourModKit::hook::gpr(ctx,DetourModKit::hook::Gpr::Rdi)),std::uint64_t(GetTickCount64()));
}
static void HitReadMidH10(DetourModKit::hook::MidContext& ctx) noexcept {
    (void)SyncCB2();
    using namespace DetourModKit::hook;
    if(!g_update_hooks_ready_h10.load(std::memory_order_acquire))return;
    const auto trigger=gpr(ctx,Gpr::Rdi);
    if(!trigger||(trigger!=g_update_continuation_h10.trigger&&trigger!=g_cb2.trigger))return;
    const auto result=gpr(ctx,Gpr::Rax);if(result&0xFF)return;
    const auto stack=stack_pointer(ctx);
    const auto caller=Rd<std::uintptr_t>(reinterpret_cast<const void*>(stack),0x28,0);
    const auto now=std::uint64_t(GetTickCount64());const auto* actor=reinterpret_cast<const void*>(gpr(ctx,Gpr::Rsi));
    if(KeepUpdateHitReadH10(reinterpret_cast<const void*>(trigger),actor,caller,now))
        gpr(ctx,Gpr::Rax)=(result&~std::uintptr_t(0xFF))|1; // read only; native480 remains false
    else if(KeepReadCB2(reinterpret_cast<const void*>(trigger),actor,caller,now)){ // CB2: same read keep, per real combo step
        g_cb2_read_kept.fetch_add(1,std::memory_order_relaxed);gpr(ctx,Gpr::Rax)=(result&~std::uintptr_t(0xFF))|1;}
}
static bool DropMidH10(std::optional<DetourModKit::hook::Hook>& hook) noexcept {
    if(!hook)return true;
    if(hook->is_enabled()&&!hook->disable())return false;
    if(hook->is_enabled())return false;hook.reset();return true;
}
static bool DropUpdateContinuationH10() noexcept {
    g_update_hooks_ready_h10.store(false,std::memory_order_release);
    const bool read=DropMidH10(g_hit_read_mid_h10),cleanup=DropMidH10(g_cleanup_mid_h10);return read&&cleanup;
}
static bool ArmUpdateContinuationH10() noexcept {
    if(!retail_build112::Ready(g_base))return false;
    if(g_cleanup_mid_h10||g_hit_read_mid_h10)return false;
    struct Pin {std::uintptr_t rva;const unsigned char* bytes;std::size_t size;};
    constexpr unsigned char cleanup[]{0x48,0x8D,0x8F,0x80,0x04,0x00,0x00,0x33,0xD2,0xE8,0xF0,0x57,0xDF,0xFF};
    constexpr unsigned char read[]{0x48,0x81,0xC1,0x80,0x04,0x00,0x00,0x48,0x8B,0x01,0xFF,0x50,0x08,0x84,0xC0,0x74,0x0E};
    constexpr unsigned char caller[]{0x48,0x8B,0xCB,0xE8,0x42,0x13,0x00,0x00,0x84,0xC0,0x75,0xEE};
    constexpr unsigned char property[]{0x38,0x51,0x08,0x74,0x32,0x48,0x8B,0x59,0x10,0x88,0x51,0x08};
    for(const auto& p:{Pin{0x93A236,cleanup,sizeof(cleanup)},Pin{0x909B01,read,sizeof(read)},Pin{0x908746,caller,sizeof(caller)},Pin{0x72FA49,property,sizeof(property)}}){
        unsigned char actual[32]{};if(!SehCopy146(reinterpret_cast<const void*>(g_base+p.rva),actual,p.size)||std::memcmp(actual,p.bytes,p.size))return false;}
    using namespace DetourModKit::hook;Options options{};options.fail_if_already_hooked=true;
    auto cleanupHook=mid_at(MidRequest{"kcd2vr_combo_ordinary_cleanup_hit",Target{DetourModKit::Address{g_base+0x93A236}},options},&CleanupMidH10);
    if(!cleanupHook)return false;g_cleanup_mid_h10=std::move(*cleanupHook);
    auto readHook=mid_at(MidRequest{"kcd2vr_combo_earned_update_hit_read",Target{DetourModKit::Address{g_base+0x909B0E}},options},&HitReadMidH10);
    if(!readHook){(void)DropUpdateContinuationH10();return false;}g_hit_read_mid_h10=std::move(*readHook);
    if(!g_cleanup_mid_h10->enable()||!g_hit_read_mid_h10->enable()){(void)DropUpdateContinuationH10();return false;}
    g_update_hooks_ready_h10.store(true,std::memory_order_release);return true;
}
}
