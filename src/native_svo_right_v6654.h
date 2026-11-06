#pragma once
#include <cstdint>

namespace kcd2vr::native_svo_right_v6654 {

// V66.54: RIGHT eye gets its own SVO (voxel GI) history textures.
//
// Retail keeps two sets of SVO history textures (names SVO_* / SV1_*) and swaps them in
// CSvoGiStage::Update (0x180FF33EC). The graphics pipeline calls Update once per PASS,
// and native stereo runs two passes per frame, so each eye's "previous frame" was the
// OTHER eye's picture. In addition the SVO reprojection block (site B 0x18057B764) is
// guarded by "already done this frame number", so the RIGHT pass reused the LEFT eye's
// reprojection. Result: SVO lighting of one eye pasted into the other -> flicker with
// visible depth, worse close up (headset-confirmed root cause: e_svoTI_Apply=0 fixes it).
//
// This module keeps a second, RIGHT-only set of those textures (created with the
// game's own helper 0x180578644 under unique names), swaps it in for the RIGHT pass
// only, lets the RIGHT pass compute its own reprojection, and keeps the previous
// view-projection matrix (stage+0xA0) per eye. No extra rendering: both eyes already
// run the SVO screen passes in retail. Needs V66.49 (per-eye previous camera) ON.
bool Install() noexcept;
bool Installed() noexcept;
void SetEnabled(bool on) noexcept;   // F8 A/B; default ON
bool Enabled() noexcept;
// Call on the render thread right after the RIGHT pipeline returns.
void OnRightPassEnd() noexcept;
void LogStats(const char* why) noexcept;

} // namespace kcd2vr::native_svo_right_v6654
