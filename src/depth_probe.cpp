#include "depth_probe.h"
#include "debug_trace.h"

#include <d3d12.h>
#include <dxgiformat.h>
#include <windows.h>
#include <cstdio>
#include <cstring>
#include <utility>

namespace kcd2vr {

namespace hook = DetourModKit::hook;

namespace {
constexpr std::uintptr_t kSceneGBufferStageExecuteRva = 0x7BB6B0ull;
constexpr std::uintptr_t kGBufferMrtSetupRva = 0x7BC700ull;
constexpr std::size_t kWrapperScanBytes = 0x100;

thread_local void* g_tls_stage = nullptr;
thread_local void* g_tls_crv = nullptr;

std::uintptr_t WhBase() noexcept {
    return reinterpret_cast<std::uintptr_t>(GetModuleHandleA("WHGame.dll"));
}

std::uintptr_t SafeReadPtr(const void* p, std::ptrdiff_t off) noexcept {
    if (!p) return 0;
    __try { return *reinterpret_cast<const std::uintptr_t*>(reinterpret_cast<std::uintptr_t>(p) + off); }
    __except (EXCEPTION_EXECUTE_HANDLER) { return 0; }
}

std::uint32_t SafeReadU32(const void* p, std::ptrdiff_t off) noexcept {
    if (!p) return 0xFFFFFFFFu;
    __try { return *reinterpret_cast<const std::uint32_t*>(reinterpret_cast<std::uintptr_t>(p) + off); }
    __except (EXCEPTION_EXECUTE_HANDLER) { return 0xFFFFFFFFu; }
}

bool IsReadableAddress(const void* p) noexcept {
    if (!p) return false;
    MEMORY_BASIC_INFORMATION mbi{};
    if (!VirtualQuery(p, &mbi, sizeof(mbi))) return false;
    if (mbi.State != MEM_COMMIT || (mbi.Protect & PAGE_GUARD) || (mbi.Protect & PAGE_NOACCESS)) return false;
    const DWORD prot = mbi.Protect & 0xFFu;
    return prot == PAGE_READONLY || prot == PAGE_READWRITE || prot == PAGE_WRITECOPY ||
           prot == PAGE_EXECUTE_READ || prot == PAGE_EXECUTE_READWRITE || prot == PAGE_EXECUTE_WRITECOPY;
}

bool ModuleNameForAddress(const void* p, char* out, std::size_t cap) noexcept {
    if (!p || !out || cap == 0) return false;
    out[0] = '\0';
    HMODULE mod = nullptr;
    if (!GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                            GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                            reinterpret_cast<LPCSTR>(p), &mod) || !mod)
        return false;
    char path[MAX_PATH]{};
    if (!GetModuleFileNameA(mod, path, MAX_PATH)) return false;
    const char* base = std::strrchr(path, '\\');
    base = base ? base + 1 : path;
    std::snprintf(out, cap, "%s", base);
    return true;
}

bool LooksLikeD3D12ComObject(void* p, char* module, std::size_t module_cap) noexcept {
    if (!p || !IsReadableAddress(p)) return false;
    const auto vt = SafeReadPtr(p, 0);
    if (!vt || !IsReadableAddress(reinterpret_cast<void*>(vt))) return false;
    const auto fn0 = SafeReadPtr(reinterpret_cast<void*>(vt), 0);
    if (!fn0) return false;
    char m[64]{};
    if (!ModuleNameForAddress(reinterpret_cast<void*>(fn0), m, sizeof(m))) return false;
    if (module && module_cap) std::snprintf(module, module_cap, "%s", m);
    return std::strstr(m, "D3D12") || std::strstr(m, "d3d12") ||
           std::strstr(m, "D3D12Core") || std::strstr(m, "d3d12core");
}

bool TryDescribeResource(void* candidate, D3D12_RESOURCE_DESC& desc) noexcept {
    if (!candidate) return false;
    bool ok = false;
    __try {
        auto* r = reinterpret_cast<ID3D12Resource*>(candidate);
        desc = r->GetDesc();
        ok = desc.Dimension >= D3D12_RESOURCE_DIMENSION_BUFFER &&
             desc.Dimension <= D3D12_RESOURCE_DIMENSION_TEXTURE3D &&
             desc.Width > 0;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) { ok = false; }
    return ok;
}

bool IsDepthFormat(DXGI_FORMAT f) noexcept {
    switch (f) {
        case DXGI_FORMAT_D32_FLOAT:
        case DXGI_FORMAT_D24_UNORM_S8_UINT:
        case DXGI_FORMAT_D16_UNORM:
        case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
        case DXGI_FORMAT_R32_TYPELESS:
        case DXGI_FORMAT_R24G8_TYPELESS:
        case DXGI_FORMAT_R16_TYPELESS:
        case DXGI_FORMAT_R32G8X24_TYPELESS:
            return true;
        default: return false;
    }
}

struct TraceResult { std::uint32_t candidates{0}; std::uint32_t depth_hits{0}; };

TraceResult TraceDepthWrapper(void* depth, std::uint64_t call) noexcept {
    TraceResult result{};
    if (!depth) {
        char m[192]{};
        std::snprintf(m, sizeof(m), "V61.30-F4 DEPTH-RESOURCE-WAIT call=%llu viewType=Normal wrapper=null",
            static_cast<unsigned long long>(call));
        debug_trace::Write(m);
        return result;
    }

    // The WHGame argument is a renderer wrapper, not known to be ID3D12Resource.
    // Scan only the first 0x100 bytes and only inspect pointer fields whose COM
    // vtable resolves into D3D12. No AddRef, no resource state changes, no writes.
    for (std::size_t off = 0; off < kWrapperScanBytes; off += sizeof(void*)) {
        void* p = reinterpret_cast<void*>(SafeReadPtr(depth, static_cast<std::ptrdiff_t>(off)));
        if (!p) continue;
        char mod[64]{};
        if (!LooksLikeD3D12ComObject(p, mod, sizeof(mod))) continue;
        D3D12_RESOURCE_DESC d{};
        if (!TryDescribeResource(p, d)) continue;

        ++result.candidates;
        const bool depth_like = (d.Flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL) != 0 || IsDepthFormat(d.Format);
        if (depth_like) ++result.depth_hits;
        char m[640]{};
        std::snprintf(m, sizeof(m),
            "%s call=%llu wrapper=%p resource=%p wrapperOff=0x%zX module=%s dim=%u size=%llux%u array=%u mips=%u format=%u samples=%u flags=0x%X",
            depth_like ? "V61.30-F4 DEPTH-RESOURCE-FOUND" : "V61.30-F4 DEPTH-CANDIDATE",
            static_cast<unsigned long long>(call), depth, p, off, mod,
            static_cast<unsigned>(d.Dimension), static_cast<unsigned long long>(d.Width),
            d.Height, d.DepthOrArraySize, d.MipLevels, static_cast<unsigned>(d.Format),
            d.SampleDesc.Count, static_cast<unsigned>(d.Flags));
        debug_trace::Write(m);
        if (result.candidates >= 4) break;
    }

    if (result.candidates == 0) {
        char m[280]{};
        std::snprintf(m, sizeof(m),
            "V61.30-F4 DEPTH-RESOURCE-WAIT call=%llu wrapper=%p wrapperVt=%p scanBytes=0x%zX",
            static_cast<unsigned long long>(call), depth,
            reinterpret_cast<void*>(SafeReadPtr(depth, 0)), kWrapperScanBytes);
        debug_trace::Write(m);
    }
    return result;
}
} // namespace

DepthProbe::SceneGBufferStageExecuteFunc DepthProbe::s_stage_original = nullptr;
DepthProbe::GBufferMrtSetupFunc DepthProbe::s_mrt_original = nullptr;
std::optional<hook::Hook> DepthProbe::s_stage_hook;
std::optional<hook::Hook> DepthProbe::s_mrt_hook;
std::atomic<bool> DepthProbe::s_enabled{false};
std::atomic<std::uint64_t> DepthProbe::s_stage_calls{0};
std::atomic<std::uint64_t> DepthProbe::s_mrt_calls{0};
std::atomic<std::uint64_t> DepthProbe::s_resource_candidates{0};
std::atomic<std::uint64_t> DepthProbe::s_depth_resource_hits{0};

bool DepthProbe::Install() {
    if (s_stage_hook && s_mrt_hook) return true;
    const auto wh = WhBase();
    if (!wh) return false;
    s_enabled.store(false, std::memory_order_release);

    hook::InlineRequest stage_req{"kcd2vr_v6130_depth_gbuffer_stage",
        hook::Target{DetourModKit::Address{wh + kSceneGBufferStageExecuteRva}}, hook::Options{}};
    auto sh = hook::inline_at(std::move(stage_req), &DepthProbe::SceneGBufferStageExecuteDetour);
    if (!sh) return false;
    s_stage_hook = std::move(*sh);
    s_stage_original = s_stage_hook->original<SceneGBufferStageExecuteFunc>();
    if (!s_stage_hook->enable()) { s_stage_hook.reset(); s_stage_original = nullptr; return false; }

    hook::InlineRequest mrt_req{"kcd2vr_v6130_depth_mrt_setup",
        hook::Target{DetourModKit::Address{wh + kGBufferMrtSetupRva}}, hook::Options{}};
    auto mh = hook::inline_at(std::move(mrt_req), &DepthProbe::GBufferMrtSetupDetour);
    if (!mh) { (void)s_stage_hook->disable(); s_stage_hook.reset(); s_stage_original = nullptr; return false; }
    s_mrt_hook = std::move(*mh);
    s_mrt_original = s_mrt_hook->original<GBufferMrtSetupFunc>();
    if (!s_mrt_hook->enable()) {
        s_mrt_hook.reset(); s_mrt_original = nullptr;
        (void)s_stage_hook->disable(); s_stage_hook.reset(); s_stage_original = nullptr;
        return false;
    }
    debug_trace::Write("V61.30-F4 DEPTH-PROBE installed DISABLED; lazy F4 gate; read-only Normal GBuffer wrapper scan");
    return true;
}

void DepthProbe::SetEnabled(bool enabled) noexcept {
    if (enabled) {
        s_stage_calls.store(0, std::memory_order_release);
        s_mrt_calls.store(0, std::memory_order_release);
        s_resource_candidates.store(0, std::memory_order_release);
        s_depth_resource_hits.store(0, std::memory_order_release);
    }
    s_enabled.store(enabled, std::memory_order_release);
    debug_trace::Write(enabled
        ? "V61.30-F4 DEPTH-PROBE ON: read-only GBuffer depth wrapper/resource discovery"
        : "V61.30-F4 DEPTH-PROBE OFF: hooks remain installed as disabled pass-through until shutdown");
}

bool DepthProbe::Enabled() noexcept { return s_enabled.load(std::memory_order_acquire); }

void DepthProbe::Uninstall() {
    s_enabled.store(false, std::memory_order_release);
    if (s_mrt_hook) { (void)s_mrt_hook->disable(); s_mrt_hook.reset(); }
    s_mrt_original = nullptr;
    if (s_stage_hook) { (void)s_stage_hook->disable(); s_stage_hook.reset(); }
    s_stage_original = nullptr;
}

void __fastcall DepthProbe::SceneGBufferStageExecuteDetour(void* self) noexcept {
    if (!s_enabled.load(std::memory_order_acquire)) {
        if (s_stage_original) s_stage_original(self);
        return;
    }
    s_stage_calls.fetch_add(1, std::memory_order_relaxed);
    void* prev_stage = g_tls_stage;
    void* prev_crv = g_tls_crv;
    g_tls_stage = self;
    g_tls_crv = reinterpret_cast<void*>(SafeReadPtr(self, 0x28));
    if (s_stage_original) s_stage_original(self);
    g_tls_stage = prev_stage;
    g_tls_crv = prev_crv;
}

void __fastcall DepthProbe::GBufferMrtSetupDetour(void* pass, void* depth,
    void* r8_resource, void* r9_resource, void* stack5_resource, void* stack6_resource) noexcept {
    if (s_enabled.load(std::memory_order_acquire)) {
        const auto n = s_mrt_calls.fetch_add(1, std::memory_order_relaxed) + 1;
        const auto view_type = SafeReadU32(g_tls_crv, 0x14);
        // Keep the hot-path logging intentionally sparse. First 8 Normal calls
        // establish the wrapper layout; then one sample every 600 calls.
        if (g_tls_stage && g_tls_crv && view_type == 0u && (n <= 8 || (n % 600ull) == 0)) {
            const TraceResult tr = TraceDepthWrapper(depth, n);
            if (tr.candidates) s_resource_candidates.fetch_add(tr.candidates, std::memory_order_relaxed);
            if (tr.depth_hits) s_depth_resource_hits.fetch_add(tr.depth_hits, std::memory_order_relaxed);
        }
    }
    if (s_mrt_original) s_mrt_original(pass, depth, r8_resource, r9_resource, stack5_resource, stack6_resource);
}

} // namespace kcd2vr
