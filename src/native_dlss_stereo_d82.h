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
}
