#pragma once
#include <cstdint>
// D85 UPSCALERS: XeSS per-eye contexts behind the reviewed D82 aggregate.
namespace kcd2vr::native_xess_stereo_d85 {
bool Install() noexcept;
bool Installed() noexcept;
void SetEnabled(bool enabled) noexcept;
bool ReadyForStereo() noexcept;
bool PairReady(std::uint64_t epoch, std::uint32_t frame) noexcept;
void Stop() noexcept;
void LogStatus(const char* reason) noexcept;
}
