#pragma once

#include <DetourModKit/hook.hpp>
#include <atomic>
#include <cstdint>
#include <optional>

namespace kcd2vr {

// V45: low-noise proof of CryEngine's existing secondary render-to-texture path.
// Static analysis maps EViewType as: 0 Normal, 1 Recursive, 2 Shadow, 3 BillboardGen.
// WHGame+0x2525414 explicitly asks the renderer for type 1 while building the
// pass used by CD3D9Renderer::FX_DrawToRenderTarget (water-reflection path).
class RecursiveViewProbe {
public:
    static bool Install();
    static void Uninstall();

    static bool IsRecursiveView(const void* render_view) noexcept;
    static std::uint64_t RecursivePassCount() noexcept;
    static std::uint64_t RecursiveRequestCount() noexcept;

private:
    using GetOrCreateRenderViewFunc = void*(__fastcall*)(void* renderer, int view_type);
    using CreateRecursivePassFunc = void*(__fastcall*)(void* out_pass, std::uint32_t thread_id, void* camera, std::uint32_t render_flags);

    static void* __fastcall GetOrCreateRenderViewDetour(void* renderer, int view_type) noexcept;
    static void* __fastcall CreateRecursivePassDetour(void* out_pass, std::uint32_t thread_id, void* camera, std::uint32_t render_flags) noexcept;

    static GetOrCreateRenderViewFunc s_get_or_create_original;
    static CreateRecursivePassFunc s_create_recursive_original;
    static std::optional<DetourModKit::hook::Hook> s_get_or_create_hook;
    static std::optional<DetourModKit::hook::Hook> s_create_recursive_hook;
    static std::atomic<std::uint64_t> s_get_calls;
    static std::atomic<std::uint64_t> s_recursive_passes;
    static std::atomic<std::uint64_t> s_recursive_requests;
};

} // namespace kcd2vr
