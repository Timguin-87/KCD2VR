#pragma once
#include <cstdint>
namespace kcd2vr::native_stereo_preflight {
// D82: only the executing native pipeline thread can identify an upscaler eye.
// The accessor never guesses from the last/global eye or changes render state.
struct UpscalerView {
    std::uint64_t epoch{};
    std::uint32_t frame{},eye{};
    std::uintptr_t pipeline{};
};
bool InUpscalerView() noexcept;
bool CurrentUpscalerView(UpscalerView& out) noexcept;
// V66.18 retains this public API/filename. V66.15 already proved the real native
// L/R path, so F9 can now arm stereo-design rungs directly from a normal launch.
// F11 remains available as a separate diagnostic, but it is no longer a prerequisite.
bool Start() noexcept;
void Stop() noexcept;
void Poll() noexcept;
bool Busy() noexcept;
// Pinned six-hook diagnostic; conflicting legacy experiments must remain disabled.
// -1 armed, 1 active (including native calls), 0 done/idle, -2 stopped/refused/faulted.
int Phase() noexcept;
// True only after the F11 proof itself reached CPU-EYE1-RETURNED and restored state.
bool Eye1Verified() noexcept;
unsigned Eye1Outcome() noexcept;
// F9 stage 1 = one-shot real-IPD camera pair CPU witness.
// F9 stage 2 = preserved-pair witness.
// F9 stage 3 = bounded burst.
// F9 stage 4 = continuous live stereo until the user stops it.
bool StartStereoDesign(int stage, float ipd_meters) noexcept;
int LastRunKind() noexcept; // 1=F11 proof, 2=F9 real-IPD witness, 3=F9 capture witness
bool LastRunPassed() noexcept;
int LastDesignStagePassed() noexcept;
std::uint64_t LastDesignPairSerial() noexcept;
// V66.48 diagnostic: render both eye passes from the center camera (IPD 0).
}
