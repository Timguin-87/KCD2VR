// V65.99-REAUDIT. Included inside native_stereo_outer_lab.cpp's anonymous namespace.
// Exact supplied retail binary only. Diagnostic, NOT a complete stereo renderer.
// Default: read-only new probes; historical F11 modes 2/3/4 retain their existing writes.
// V66.01: selector hypothesis is controlled by the command-scoped F11 sequence.
// Environment settings are ignored. Placement remains AFTER command selector=0.
// Nonempty RANGE counters are requested render-item ranges, NOT verified GPU draws.

using V6599DrawItemsFn = void(__fastcall*)(void*,void*,std::uint8_t,int,int);
constexpr std::uintptr_t kV6599ExecuteRva=0x7C2110ull;
constexpr std::uintptr_t kV6599OpaqueRva=0x7C24B8ull;
constexpr std::uintptr_t kV6599DrawItemsRva=0x7729CCull;
constexpr std::uintptr_t kV6599CrvVtableRva=0x3DF25C0ull;
constexpr unsigned char kV6599ExecuteBytes[]={0x48,0x89,0x5C,0x24,0x20,0x55,0x56,0x57,0x41,0x54,0x41,0x55,0x41,0x56,0x41,0x57,0x48,0x8B,0xEC,0x48,0x83,0xEC,0x30,0x48};
constexpr unsigned char kV6599OpaqueBytes[]={0x40,0x53,0x55,0x56,0x57,0x41,0x54,0x41,0x55,0x41,0x56,0x41,0x57,0x48,0x83,0xEC,0x48,0x4C,0x8B,0x61,0x28,0x4C,0x8B,0xE9};
constexpr unsigned char kV6599DrawItemsBytes[]={0x44,0x89,0x4C,0x24,0x20,0x55,0x53,0x56,0x57,0x41,0x56,0x48,0x8D,0x6C,0x24,0xC0,0x48,0x81,0xEC,0x40,0x01,0x00,0x00,0x48};
std::optional<DetourModKit::hook::Hook> g_v6599_execute_hook, g_v6599_opaque_hook, g_v6599_draw_hook;
SceneGBufferUpdateFn g_v6599_execute_original=nullptr, g_v6599_opaque_original=nullptr;
V6599DrawItemsFn g_v6599_draw_original=nullptr;
std::atomic<bool> g_v6599_selector_probe{false};
std::atomic<std::uint64_t> g_v6599_execute_total{0},g_v6599_opaque_total{0};
struct V6599Scope { void* command; void* crv; std::uint64_t serial; };
thread_local V6599Scope g_v6599_scope{};
thread_local unsigned g_v6599_opaque_depth=0;
thread_local std::uint64_t g_v6599_execute_count=0,g_v6599_opaque_count=0;
thread_local std::uint64_t g_v6599_range_count=0,g_v6599_nonempty_count=0,g_v6599_requested_items=0;
thread_local std::uint64_t g_v6599_upload_count=0,g_v6599_last_upload_hash=0;
thread_local void* g_v6599_last_upload_owner=nullptr;

bool V6599LogNow(std::uint64_t n) noexcept {
    if(g_v6601_owned.load(std::memory_order_acquire))
        return g_v6601_command_active && (g_v6601_progress.commands<16u || g_v6601_progress.commands%30u==0u);
    return V6601EffectiveDiag()>0 &&
        (g_v6595_replay_guard || n<=24ull || (n%120ull)==0ull);
}
void V6599ObserveUpload(void* owner,void* records,std::uint32_t stride,std::uint32_t inputCount,
                       std::uint32_t dispatchCount,bool forced,bool ok) noexcept {
    ++g_v6599_upload_count;
    g_v6599_last_upload_owner=owner;
    g_v6599_last_upload_hash=(records && stride==0x470u)?SafeFnv1a64(reinterpret_cast<const unsigned char*>(records),0x470ull):0ull;
    if(V6599LogNow(g_v6599_upload_count)) Tracef("V65.99-DRAWPATH CB-UPLOAD epoch=%llu serial=%llu replay=%d scopeCrv=%p owner=%p inputCount=%u dispatchCount=%u forcedBlock1=%d ok=%d hash=%016llX NOTE=CPU-upload-not-GPU-binding",
        (unsigned long long)g_v6599_upload_count,(unsigned long long)g_v6599_scope.serial,g_v6595_replay_guard?1:0,
        g_v6599_scope.crv,owner,unsigned(inputCount),unsigned(dispatchCount),forced?1:0,ok?1:0,(unsigned long long)g_v6599_last_upload_hash);
}
#include "native_stereo_path_v6600.inc"

void DisableV6599DrawHooks() noexcept {
    DisableV6600NativeHooks();
    if(g_v6599_draw_hook){(void)g_v6599_draw_hook->disable();g_v6599_draw_hook.reset();}
    if(g_v6599_opaque_hook){(void)g_v6599_opaque_hook->disable();g_v6599_opaque_hook.reset();}
    if(g_v6599_execute_hook){(void)g_v6599_execute_hook->disable();g_v6599_execute_hook.reset();}
    g_v6599_draw_original=nullptr;g_v6599_opaque_original=nullptr;g_v6599_execute_original=nullptr;
    g_v6599_selector_probe.store(false,std::memory_order_release);
}
