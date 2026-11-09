#pragma once
#include <cstdint>

namespace kcd2vr::native_dlss_stereo_d82 {
// Uses WHGame's exported NGX API and the game's command lists/resources. It does
// not load another SDK, change jitter, replace projection, or run DLSS twice per
// eye. The second feature owns the RIGHT eye's temporal state.
enum class Fault : unsigned {
    None, MissingCreate, MissingTwin, Capacity, TwinCreate, AliasedHandle,
    ViewIdentity, Parameters, DuplicateEye, MissingLeft, Evaluate, RestoreReset,
    Release, Shutdown, ResetMismatch
};
struct Status {
    bool installed{},enabled{},ready{};
    Fault fault{Fault::None};
    unsigned livePairs{};
    std::uint64_t creates{},twins{},leftEvaluations{},rightEvaluations{},pairs{},
        refused{},resets{},resetCuts{},unscoped{},releases{},shutdowns{};
    std::uint64_t lastEpoch{};
    std::uint32_t lastFrame{};
};
// Call on the bootstrap worker as soon as WHGame is loaded, before the first
// native DLSS CreateFeature. A late install does not guess creation parameters.
bool Install() noexcept;
bool Installed() noexcept;
void SetEnabled(bool enabled) noexcept;
// R149 alternate-eye (AFR) DLSS. One live eye is rendered per game frame; the
// paired NGX handles keep separate per-eye histories fed only by that eye's
// live frames. Motion vectors are rebuilt for the same eye's previous live frame
// (N -> N-2) from depth and the exact CRenderView camera banks. The stale eye
// is warped later by XR composition from its post-DLSS image; it never enters NGX.
void SetAlternateEyeMode(bool enabled) noexcept;
// 0 = reset history every eye frame, 1 = same-eye camera reprojection (default),
// 2 = raw game motion vectors (diagnostic only: they point at the other eye).
void SetAlternateEyeMotionMode(int mode) noexcept;
int AlternateEyeMotionMode() noexcept;
// Captured resources/hooks are ready to enter Stereo, including re-entry from
// Mono. This is distinct from routing being enabled and from PairReady.
bool ReadyForStereo() noexcept;
bool PairReady(std::uint64_t epoch,std::uint32_t frame) noexcept;
// Called only after the native RIGHT pipeline returns, never from a GPU queue
// callback. A reset cut authorizes another pair attempt, never output acceptance.
bool RecoverableResetCut(std::uint64_t epoch,std::uint32_t frame,std::uintptr_t pipeline) noexcept;
bool Faulted() noexcept;
Status GetStatus() noexcept;
void LogStatus(const char* reason) noexcept;
// Stop routing but retain hooks until native ReleaseFeature/Shutdown1 retires
// the paired handles at the game's GPU lifetime boundary.
void Stop() noexcept;
// R149 alternate-eye frame dump: captures the next `frames` live AFR eye frames
// (DLSS inputs, rebuilt vectors, DLSS output, cameras, identities) plus the XR
// composition of the same frames into KCD2VR_AER_DUMP\<time>\ beside the mod DLL.
// delayMs lets a menu button close before capture starts. Returns false while a
// previous dump is still running or alternate-eye DLSS is not active.
bool RequestAerFrameDump(unsigned frames,unsigned delayMs) noexcept;
bool AerFrameDumpBusy() noexcept;
// Game DIRECT queue render tick (Mod::OnRenderTick): fences finished captures
// and hands them to the background writer. Cheap when no dump is pending.
// presentSubmitted: this tick follows the batch that carried the game PRESENT.
void AerFrameDumpRenderTick(void* directQueue,bool presentSubmitted) noexcept;
}
