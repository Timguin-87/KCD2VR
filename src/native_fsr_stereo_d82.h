#pragma once
#include <cstdint>
namespace kcd2vr::native_fsr_stereo_d82 {
bool Install() noexcept;
bool Installed() noexcept;
void SetEnabled(bool enabled) noexcept;
bool ReadyForStereo() noexcept;
bool PairReady(std::uint64_t epoch, std::uint32_t frame) noexcept;
bool RecoverableResetCut(std::uint64_t epoch,std::uint32_t frame,std::uintptr_t pipeline) noexcept;
void Stop() noexcept;
void LogStatus(const char* reason) noexcept;
}
