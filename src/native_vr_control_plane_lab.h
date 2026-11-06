#pragma once
#include <cstdint>

namespace kcd2vr::native_vr_control_plane_lab {

struct Status {
    int mode{};
    bool supported_build{};
    bool three_mon_hook_installed{};
    std::uint64_t three_mon_calls{};
    std::uintptr_t three_mon_last_self{};
    std::uintptr_t three_mon_last_caller_rva{};
    float matrix_1820{};
    float matrix_1834{};
    float matrix_1840{};
    float matrix_1850{};
    bool bridge_active{};
    float bridge_openxr_ipd{};
    float bridge_original_eye_dist{};
    float bridge_written_eye_dist{};
    bool flow_process_hooks_installed{};
    std::uint64_t read_process_calls{};
    std::uint64_t stereo_process_calls{};
    int read_last_event{-1};
    int stereo_last_event{-1};
    std::uintptr_t read_last_self{};
    std::uintptr_t stereo_last_self{};
};

// V65.28: read-only dormant-stereo FlowNode ABI lab. 3Mon/IPD writes remain retired.
// Rungs 3-4 observe the exact retail ReadStereoParameters/StereoParameters ProcessEvent
// methods recovered from RTTI/vtables. Activation remains locked until input ABI/state is exact.
bool Initialize() noexcept;
void Shutdown() noexcept;
void SetMode(int mode) noexcept;
void Cycle() noexcept;
void Tick() noexcept;
int Mode() noexcept;
const char* ModeName(int mode) noexcept;
Status GetStatus() noexcept;

} // namespace kcd2vr::native_vr_control_plane_lab
