// Exact retail ordinary receiver C5235C. Read-only ancestry, not a new hit producer.
constexpr std::uintptr_t kFistReceiverH12=0xC5235Cull;
constexpr unsigned char kFistReceiverEntryH12[]{0x48,0x89,0x5C,0x24,0x18,0x55,0x56,0x57,0x41,0x56,0x41,0x57,0x48,0x8D,0x6C,0x24,0xB0};
using FistReceiverFnH12=void(__fastcall*)(const void*,void*);
static std::optional<DetourModKit::hook::Hook> g_fist_receiver_hook_h12;
static FistReceiverFnH12 g_fist_receiver_original_h12=nullptr;
static std::array<unsigned char,sizeof(kFistReceiverEntryH12)> g_fist_receiver_owned_h12{};
static std::uintptr_t g_fist_receiver_base_h12=0;
static bool g_fist_receiver_uncertain_h12=false;
#if defined(_MSC_VER) && !defined(__try)
static int FistReceiverExceptionH12(const kcd2vr::fist_action_h12::Scope* prior) noexcept {
    kcd2vr::fist_action_h12::exceptionRestore(prior);return EXCEPTION_CONTINUE_SEARCH;
}
static void FistReceiverOriginalCallH12(const void* receiver,void* details) {
    const auto* prior=kcd2vr::fist_action_h12::Scope::live?kcd2vr::fist_action_h12::Scope::live->prior:nullptr;
    // Restore borrowed TLS without swallowing/changing the native exception.
    // The existing caller's own exception policy remains responsible for it.
    __try {if(g_fist_receiver_original_h12)g_fist_receiver_original_h12(receiver,details);}
    __except(FistReceiverExceptionH12(prior)) {}
}
#else
static void FistReceiverOriginalCallH12(const void* receiver,void* details) {
    if(g_fist_receiver_original_h12)g_fist_receiver_original_h12(receiver,details);
}
#endif
static void __fastcall FistReceiverDetourH12(const void* receiver,void* details) {
    const bool allowed=!g_fist_receiver_uncertain_h12&&g_fist_receiver_base_h12==g_base&&retail_build112::Ready(g_base)&&
        !diagnostics::lab::NativeReference();
    kcd2vr::fist_action_h12::run(reinterpret_cast<std::uintptr_t>(receiver),reinterpret_cast<std::uintptr_t>(details),
        g_base,GetTickCount64(),allowed,
        [](std::uintptr_t p,void* out,std::size_t n) noexcept {return SehCopy146(reinterpret_cast<const void*>(p),out,n);},
        [&](){FistReceiverOriginalCallH12(receiver,details);});
}
// An installed patch is accepted only when it is OUR exact current compared
// patch and original trampoline. A foreign jump is never a valid contract.
static FistReceiverFnH12 FistReceiverOriginalH12() noexcept {
    std::array<unsigned char,sizeof(kFistReceiverEntryH12)> entry{};
    if(g_fist_receiver_uncertain_h12||!g_base||g_fist_receiver_base_h12!=g_base||!retail_build112::Ready(g_base)||
       !g_fist_receiver_hook_h12||!g_fist_receiver_hook_h12->is_enabled()||!g_fist_receiver_original_h12||
       !SehCopy146(reinterpret_cast<const void*>(g_base+kFistReceiverH12),entry.data(),entry.size())||entry!=g_fist_receiver_owned_h12)return nullptr;
    return g_fist_receiver_original_h12;
}
static FistReceiverFnH12 FistReceiverCallableH12() noexcept {
    if(FistReceiverOriginalH12())return &FistReceiverDetourH12;
    // Hook unavailable: existing exact unpatched weapon live path stays valid.
    // Boxing receives no false receiver scope and therefore cannot borrow it.
    unsigned char entry[sizeof(kFistReceiverEntryH12)]{};
    if(g_fist_receiver_hook_h12||g_fist_receiver_original_h12||!g_base||!retail_build112::Ready(g_base)||
       !SehCopy146(reinterpret_cast<const void*>(g_base+kFistReceiverH12),entry,sizeof(entry))||
       std::memcmp(entry,kFistReceiverEntryH12,sizeof(entry)))return nullptr;
    return reinterpret_cast<FistReceiverFnH12>(g_base+kFistReceiverH12);
}
static bool ReleaseFistReceiverH12() noexcept {
    kcd2vr::fist_action_h12::published.store(0,std::memory_order_release);
    if(g_fist_receiver_hook_h12){
        if(!g_fist_receiver_hook_h12->disable()){
            // The patch may remain live. Keep its original function and owner
            // alive so callbacks continue forwarding; never destroy uncertainty.
            g_fist_receiver_uncertain_h12=true;
            diagnostic_trace::Write("H12 FIST_RECEIVER stop=UNCERTAIN retained=HOOK_AND_ORIGINAL contact=DISABLED");return false;
        }
        g_fist_receiver_hook_h12.reset();
    }
    g_fist_receiver_original_h12=nullptr;g_fist_receiver_base_h12=0;g_fist_receiver_uncertain_h12=false;
    return true;
}
