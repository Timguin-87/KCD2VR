#pragma once

#include <atomic>
#include <cstdint>
#include <optional>
#include <DetourModKit/hook.hpp>

namespace kcd2vr {

// V61.30: lazy-installed, read-only Normal CSceneGBufferStage depth probe.
// The hooks are installed only after F4 is first pressed. SetEnabled(false)
// leaves a cheap pass-through detour so we never live-unhook renderer code.
class DepthProbe final {
public:
    static bool Install();
    static void Uninstall();
    static void SetEnabled(bool enabled) noexcept;
    static bool Enabled() noexcept;


private:
    using SceneGBufferStageExecuteFunc = void(__fastcall*)(void* self);
    using GBufferMrtSetupFunc = void(__fastcall*)(void* pass, void* depth,
        void* r8_resource, void* r9_resource, void* stack5_resource, void* stack6_resource);

    static void __fastcall SceneGBufferStageExecuteDetour(void* self) noexcept;
    static void __fastcall GBufferMrtSetupDetour(void* pass, void* depth,
        void* r8_resource, void* r9_resource, void* stack5_resource, void* stack6_resource) noexcept;

    static SceneGBufferStageExecuteFunc s_stage_original;
    static GBufferMrtSetupFunc s_mrt_original;
    static std::optional<DetourModKit::hook::Hook> s_stage_hook;
    static std::optional<DetourModKit::hook::Hook> s_mrt_hook;
    static std::atomic<bool> s_enabled;
    static std::atomic<std::uint64_t> s_stage_calls;
    static std::atomic<std::uint64_t> s_mrt_calls;
    static std::atomic<std::uint64_t> s_resource_candidates;
    static std::atomic<std::uint64_t> s_depth_resource_hits;
};

} // namespace kcd2vr
