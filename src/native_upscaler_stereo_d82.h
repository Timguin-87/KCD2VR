#pragma once
#include <cstdint>
namespace kcd2vr::native_upscaler_stereo_d82 {
// Install on the startup worker, outside DllMain. This only intercepts the
// game's existing vendor APIs; it never initializes a second device or SDK.
void InstallEarly(bool stereoWanted) noexcept;
void SetMode(bool stereo, int nativeTechnology) noexcept;
void SetTechnologyReader(int (*reader)() noexcept) noexcept;
bool CanEnterStereo(int nativeTechnology) noexcept;
// Native keeps its existing path. Every bound epoch requires an unchanged
// technology; temporal modes additionally require matching successful L/R
// evaluations from the same native pair epoch and game frame.
bool PairReady(std::uint64_t epoch, std::uint32_t frame) noexcept;
// Exact failed-attempt status for continuous-session recovery, not publication.
bool RecoverableResetCut(std::uint64_t epoch,std::uint32_t frame,std::uintptr_t pipeline) noexcept;
bool FrameReady(std::uint32_t frame) noexcept;
std::uint64_t CurrentEpoch() noexcept;
// Stored with the exact published textures, then copied into the XR cache.
// Native0 keeps its established reuse policy; temporal caches cannot outlive
// their stereo epoch, technology, enabled state, or backend readiness.
struct PairOwner { std::uint64_t epoch{}; int technology{0}; };
int BoundTechnology() noexcept;
bool MayPresent(PairOwner owner) noexcept;
struct Publication { std::uint64_t serial{},epoch{};std::uint32_t frame{};int technology{-1}; };
// Actual queue publication only; context creation or CVar read-back never calls this.
void NotifyPublished(std::uint64_t epoch,std::uint32_t frame) noexcept;
Publication LastPublication() noexcept;
std::uint64_t PublicationSerial() noexcept;
bool TemporalActive() noexcept;
void BeginEpoch(std::uint64_t epoch) noexcept;
// Withdraw an affected temporal attempt after an unconfirmed native mutation.
// Does not stop/free SDK work and cannot invalidate a newer concurrently bound epoch.
void EndEpoch(std::uint64_t expectedEpoch) noexcept;
void Stop() noexcept;
void LogStatus(const char* reason) noexcept;
}
