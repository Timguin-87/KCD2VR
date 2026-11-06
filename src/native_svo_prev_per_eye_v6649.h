#pragma once
#include <cstdint>

namespace kcd2vr::native_svo_prev_per_eye_v6649 {

// V66.49: per-eye "previous camera" for KCD2's SVO (voxel GI / specular) reprojection.
// Retail keeps the previous view/projection as FUNCTION-LOCAL STATIC matrices in the two
// only callers of the reprojection helper 0x181DF83A0 and overwrites them with the current
// camera on EVERY call. Native stereo calls them twice per frame (LEFT, RIGHT), so each eye's
// "previous" was the OTHER eye's current camera (70 mm away) -> SVO temporal flicker in both
// eyes, gone at zero IPD and in true mono. This keeps one copy per eye and swaps it in/out
// around each call (plain 256-byte memory copies; no engine function is called).
bool Install() noexcept;
bool Installed() noexcept;
void SetEnabled(bool on) noexcept;   // A/B switch (F8); default ON
bool Enabled() noexcept;
void LogStats() noexcept;

} // namespace kcd2vr::native_svo_prev_per_eye_v6649
