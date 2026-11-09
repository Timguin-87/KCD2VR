#pragma once
#include <cstdint>
#include <dxgiformat.h>

struct ID3D12GraphicsCommandList;
struct ID3D12Resource;

// R154 ALTERNATE-EYE STALE-EYE WARP (modelled on the SnowRunner VR mod's warp ladder).
//
// AER renders ONE eye per game frame. The other (stale) eye shows its own last
// live post-DLSS image, brought to the live frame's pose. Tiers, each containing
// the one before it:
//   0 Rotation       HMD + game camera rotation (the R144 blit-shader warp).
//   1 6DoF           + the camera's TRANSLATION, reprojected through the stale
//                    eye's own depth from its last live frame (head lean, walking,
//                    riding, the eye separation itself).
//   2 MotionVectors  + every moving surface moved by the motion the game measured
//                    for it: the live eye's motion vectors point at the previous
//                    render, which under AER IS the stale eye's image. Surfaces
//                    the live eye cannot see keep their own last motion.
// Pixels no source reaches (disocclusions) take the background beside them at its
// rotation + 6DoF warp, then the full backward 6DoF warp, then rotation alone.
//
// Data is captured at the live eye's DLSS evaluate (render thread, game command
// list, before NGX) and consumed by the XR blit, which receives a per-pixel offset
// map and samples the stale image ONCE. Records are matched by the XR pose frame
// the present was tagged with; any mismatch falls back to the rotation warp.
namespace kcd2vr::aer_warp_r154 {

enum : int { kRotation = 0, kSixDof = 1, kMotionVectors = 2 };
void SetMode(int mode) noexcept;   // clamped 0..2; default 2
int Mode() noexcept;
void SetDebugTint(bool on) noexcept; // tint the stale eye by the tier that moved each pixel
bool DebugTint() noexcept;
void SetTimestampFrequency(std::uint64_t ticksPerSecond) noexcept;

// CryEngine CCamera: Matrix34 rotation rows (columns right/forward/up), position.
struct Camera {
    double rot[9]{};
    double pos[3]{};
    float tanX{}, tanY{}, nearZ{}, farZ{};
};

struct LiveEyeInputs {
    unsigned eye{};
    std::uint32_t renderFrame{};   // CRenderView frame (render clock)
    std::uint64_t poseFrame{};     // XR pose frame of the camera (0 = unknown)
    float eyeOffsetWorld{};        // signed offset of this eye along camera right, world units
    bool eyeOffsetKnown{};
    ID3D12Resource* depth{};       DXGI_FORMAT depthView{};  unsigned depthX{}, depthY{};
    ID3D12Resource* motion{};      DXGI_FORMAT motionView{}; unsigned mvX{}, mvY{}, mvW{}, mvH{};
    float mvScaleX{1.0f}, mvScaleY{1.0f};
    unsigned renderW{}, renderH{};
    int depthMode{-1};             // 0 reversed, 1 standard, 2 linear/far, 3 linear metres
    bool modelValid{};             // R149 static-camera agreement holds for depthMode
    bool camerasOk{};
    Camera cur{}, prev{};          // CRenderView current / previous banks
};
// Render thread, inside the NGX evaluate hook, before the native evaluation.
// Records compute work into the game's command list; inputs are shader-readable.
void OnLiveEyeEvaluate(ID3D12GraphicsCommandList* list, const LiveEyeInputs& in) noexcept;

struct StaleWarpRequest {
    int staleEye{};
    std::uint64_t stalePoseFrame{};  // pose frame of the cached stale image
    std::uint64_t livePoseFrame{};   // pose frame of the live present (the declared pose)
};
struct StaleWarpResult {
    ID3D12Resource* offsets{};       // R32G32B32A32_FLOAT, PIXEL_SHADER_RESOURCE state
    const char* reason{"unset"};
    int tierMode{};                  // the mode actually used (1 or 2)
};
// XR blit (render thread). Records compute into the blit command list and returns
// the offset map for the stale eye's draw slot t3. False = use the rotation warp.
bool WarpStaleEye(ID3D12GraphicsCommandList* list, const StaleWarpRequest& req, StaleWarpResult& out) noexcept;

// Periodic housekeeping from the XR thread: status line and releasing the GPU
// memory once the warp has not run for a few seconds.
void Tick() noexcept;
}
