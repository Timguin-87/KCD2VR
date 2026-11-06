#include "diagnostics/log_sink_d48.h"
#include "recursive_view_probe.h"
#include "debug_trace.h"

#include <DetourModKit/logger.hpp>
#include <windows.h>
#include <intrin.h>
#include <cstdio>
#include <cstddef>
#include <cstring>
#include <utility>

namespace kcd2vr {

using DetourModKit::log;
namespace hook = DetourModKit::hook;

namespace {
constexpr std::uintptr_t kGetOrCreateRenderViewRva = 0x43EB60ull;
constexpr std::uintptr_t kCreateRecursivePassRva = 0x2525414ull;
constexpr std::uintptr_t kRendererGlobalRva = 0x492D908ull;

std::uintptr_t WhBase() noexcept
{
    return reinterpret_cast<std::uintptr_t>(GetModuleHandleA("WHGame.dll"));
}

__forceinline std::uintptr_t CallerRva() noexcept
{
    const auto b = WhBase();
    const auto a = reinterpret_cast<std::uintptr_t>(_ReturnAddress());
    return (b && a >= b) ? (a - b) : 0;
}

std::uintptr_t SafeReadPtr(const void* p, std::ptrdiff_t off) noexcept
{
    if (!p) return 0;
    __try { return *reinterpret_cast<const std::uintptr_t*>(reinterpret_cast<std::uintptr_t>(p) + off); }
    __except (EXCEPTION_EXECUTE_HANDLER) { return 0; }
}

std::uint32_t SafeReadU32(const void* p, std::ptrdiff_t off) noexcept
{
    if (!p) return 0xFFFFFFFFu;
    __try { return *reinterpret_cast<const std::uint32_t*>(reinterpret_cast<std::uintptr_t>(p) + off); }
    __except (EXCEPTION_EXECUTE_HANDLER) { return 0xFFFFFFFFu; }
}

float SafeReadF32(const void* p, std::ptrdiff_t off) noexcept
{
    if (!p) return 0.0f;
    __try { return *reinterpret_cast<const float*>(reinterpret_cast<std::uintptr_t>(p) + off); }
    __except (EXCEPTION_EXECUTE_HANDLER) { return 0.0f; }
}

std::uintptr_t VtableRva(const void* p) noexcept
{
    const auto vt = SafeReadPtr(p, 0);
    const auto b = WhBase();
    return (b && vt >= b && vt < b + 0x6000000ull) ? (vt - b) : 0;
}

void* SafeReadRendererGlobal(std::uintptr_t wh) noexcept
{
    if (!wh) return nullptr;
    void* renderer = nullptr;
    __try {
        renderer = *reinterpret_cast<void**>(wh + kRendererGlobalRva);
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        renderer = nullptr;
    }
    return renderer;
}

const char* ViewTypeName(int t) noexcept
{
    switch (t) {
    case 0: return "Normal";
    case 1: return "Recursive";
    case 2: return "Shadow";
    case 3: return "BillboardGen";
    default: return "Unknown";
    }
}
} // namespace

RecursiveViewProbe::GetOrCreateRenderViewFunc RecursiveViewProbe::s_get_or_create_original = nullptr;
RecursiveViewProbe::CreateRecursivePassFunc RecursiveViewProbe::s_create_recursive_original = nullptr;
std::optional<hook::Hook> RecursiveViewProbe::s_get_or_create_hook;
std::optional<hook::Hook> RecursiveViewProbe::s_create_recursive_hook;
std::atomic<std::uint64_t> RecursiveViewProbe::s_get_calls{0};
std::atomic<std::uint64_t> RecursiveViewProbe::s_recursive_passes{0};
std::atomic<std::uint64_t> RecursiveViewProbe::s_recursive_requests{0};

bool RecursiveViewProbe::Install()
{
    const auto wh = WhBase();
    if (!wh) {
        KCD2VR_DIAG_LOG(Bug,error,"V45-RECURSIVE: WHGame.dll unavailable");
        return false;
    }

    s_get_calls.store(0, std::memory_order_release);
    s_recursive_passes.store(0, std::memory_order_release);
    s_recursive_requests.store(0, std::memory_order_release);

    // Runtime control: prove WHGame+0x492D908 really is the renderer object
    // owning the public-style GetOrCreateRenderView(EViewType) vslot +0x1E8.
    void* renderer = SafeReadRendererGlobal(wh);
    {
        char m[256]{};
        std::snprintf(m, sizeof(m),
            "V45-RECURSIVE renderer-global=%p vt_rva=0x%llX getOrCreate=WHGame+0x%llX viewTypes={0:Normal,1:Recursive,2:Shadow,3:BillboardGen}",
            renderer, static_cast<unsigned long long>(VtableRva(renderer)),
            static_cast<unsigned long long>(kGetOrCreateRenderViewRva));
        debug_trace::Write(m);
    }

    hook::InlineRequest get_req{
        "kcd2vr_v45_get_or_create_render_view",
        hook::Target{DetourModKit::Address{wh + kGetOrCreateRenderViewRva}},
        hook::Options{}
    };
    auto get_hook = hook::inline_at(std::move(get_req), &RecursiveViewProbe::GetOrCreateRenderViewDetour);
    if (!get_hook) {
        KCD2VR_DIAG_LOG(Bug,error,"V45-RECURSIVE: GetOrCreateRenderView hook install failed");
        return false;
    }
    s_get_or_create_hook = std::move(*get_hook);
    s_get_or_create_original = s_get_or_create_hook->original<GetOrCreateRenderViewFunc>();
    if (!s_get_or_create_hook->enable()) {
        KCD2VR_DIAG_LOG(Bug,error,"V45-RECURSIVE: GetOrCreateRenderView hook enable failed");
        s_get_or_create_hook.reset();
        s_get_or_create_original = nullptr;
        return false;
    }

    hook::InlineRequest pass_req{
        "kcd2vr_v45_create_recursive_pass",
        hook::Target{DetourModKit::Address{wh + kCreateRecursivePassRva}},
        hook::Options{}
    };
    auto pass_hook = hook::inline_at(std::move(pass_req), &RecursiveViewProbe::CreateRecursivePassDetour);
    if (!pass_hook) {
        KCD2VR_DIAG_LOG(Bug,error,"V45-RECURSIVE: recursive pass hook install failed");
        (void)s_get_or_create_hook->disable();
        s_get_or_create_hook.reset();
        s_get_or_create_original = nullptr;
        return false;
    }
    s_create_recursive_hook = std::move(*pass_hook);
    s_create_recursive_original = s_create_recursive_hook->original<CreateRecursivePassFunc>();
    if (!s_create_recursive_hook->enable()) {
        KCD2VR_DIAG_LOG(Bug,error,"V45-RECURSIVE: recursive pass hook enable failed");
        s_create_recursive_hook.reset();
        s_create_recursive_original = nullptr;
        (void)s_get_or_create_hook->disable();
        s_get_or_create_hook.reset();
        s_get_or_create_original = nullptr;
        return false;
    }

    debug_trace::Write("V45-RECURSIVE armed: observing CryEngine EViewType::Recursive render-to-texture path; no engine state writes");
    return true;
}

void RecursiveViewProbe::Uninstall()
{
    if (s_create_recursive_hook) { (void)s_create_recursive_hook->disable(); s_create_recursive_hook.reset(); }
    s_create_recursive_original = nullptr;
    if (s_get_or_create_hook) { (void)s_get_or_create_hook->disable(); s_get_or_create_hook.reset(); }
    s_get_or_create_original = nullptr;
}


std::uint64_t RecursiveViewProbe::RecursivePassCount() noexcept
{
    return s_recursive_passes.load(std::memory_order_acquire);
}

std::uint64_t RecursiveViewProbe::RecursiveRequestCount() noexcept
{
    return s_recursive_requests.load(std::memory_order_acquire);
}

void* __fastcall RecursiveViewProbe::GetOrCreateRenderViewDetour(void* renderer, int view_type) noexcept
{
    const auto n = s_get_calls.fetch_add(1, std::memory_order_relaxed) + 1;
    const auto caller = CallerRva();
    if (view_type == 1)
        s_recursive_requests.fetch_add(1, std::memory_order_relaxed);
    void* ret = s_get_or_create_original ? s_get_or_create_original(renderer, view_type) : nullptr;
    const auto actual_type = SafeReadU32(ret, 0x14);

    if (view_type == 1 || n <= 12 || (n % 600ull) == 0) {
        char m[320]{};
        std::snprintf(m, sizeof(m),
            "V45-GET-RENDERVIEW call=%llu requested=%d(%s) caller=0x%llX renderer=%p renderer_vt=0x%llX ret=%p ret_vt=0x%llX actualType=%u tid=%lu",
            static_cast<unsigned long long>(n), view_type, ViewTypeName(view_type),
            static_cast<unsigned long long>(caller), renderer,
            static_cast<unsigned long long>(VtableRva(renderer)), ret,
            static_cast<unsigned long long>(VtableRva(ret)), actual_type,
            static_cast<unsigned long>(GetCurrentThreadId()));
        debug_trace::Write(m);
    }
    return ret;
}

void* __fastcall RecursiveViewProbe::CreateRecursivePassDetour(
    void* out_pass, std::uint32_t thread_id, void* camera, std::uint32_t render_flags) noexcept
{
    const auto n = s_recursive_passes.fetch_add(1, std::memory_order_relaxed) + 1;
    const auto caller = CallerRva();
    const float x = SafeReadF32(camera, 0x0C);
    const float y = SafeReadF32(camera, 0x1C);
    const float z = SafeReadF32(camera, 0x2C);

    void* ret = s_create_recursive_original
        ? s_create_recursive_original(out_pass, thread_id, camera, render_flags)
        : out_pass;

    const auto render_view = reinterpret_cast<void*>(SafeReadPtr(out_pass, 0x20));
    const auto view_type = SafeReadU32(render_view, 0x14);
    const auto source = reinterpret_cast<void*>(SafeReadPtr(render_view, 0x38));

    if (n <= 64 || (n % 300ull) == 0) {
        char m[384]{};
        std::snprintf(m, sizeof(m),
            "V45-RECURSIVE-PASS call=%llu caller=0x%llX out=%p thread=%u flags=0x%X camera=%p pos=(%.3f,%.3f,%.3f) renderView=%p viewType=%u source=%p tid=%lu",
            static_cast<unsigned long long>(n), static_cast<unsigned long long>(caller),
            out_pass, thread_id, render_flags, camera,
            static_cast<double>(x), static_cast<double>(y), static_cast<double>(z),
            render_view, view_type, source, static_cast<unsigned long>(GetCurrentThreadId()));
        debug_trace::Write(m);
    }
    return ret;
}

} // namespace kcd2vr
