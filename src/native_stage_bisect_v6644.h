#pragma once
#include <cstdint>

namespace kcd2vr::native_stage_bisect_v6644 {

// V66.44 RIGHT-pass stage elimination. True mono is clean; stereo flickers in BOTH eyes,
// so a RIGHT-pass stage overwrites shared state the LEFT pass uses next frame.
// Each candidate stage can be skipped for the RIGHT pass only (selector 1), through the
// engine's own "stage inactive" path (IsStageActive gate = vtable slot 9) or, where the
// gate is too small to hook safely (<16 bytes), the stage's virtual Execute (slot 4).
// LEFT is never touched. RIGHT will look wrong for the skipped stage; watch LEFT.
bool Install() noexcept;
int  StageCount() noexcept;                 // individual stages (ShadowMask is index 0)
const char* StageName(int index) noexcept;
void LogStats(const char* reason) noexcept; // per-stage RIGHT skip counts since last call

} // namespace kcd2vr::native_stage_bisect_v6644
