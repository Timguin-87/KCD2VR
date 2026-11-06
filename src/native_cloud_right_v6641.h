#pragma once
#include <cstdint>

namespace kcd2vr::native_cloud_right_v6641 {

// V66.41: give the RIGHT-eye volumetric-cloud history textures real GPU memory.
// Retail CVolumetricCloudsStage creates texture OBJECTS for both eyes (Init 0x180741FD0),
// but its per-frame resize/update (vtable slot, 0x1809B94E8) passes allowCreate=(eye==0)
// to the render-target helper 0x181E142E0, which returns without allocating when that
// flag is 0. The stage's active-gate (0x180578998) only checks LEFT textures, so the
// RIGHT cloud pass runs into unbacked targets and draws nothing.
// Exact-build/prologue gated; calls the same retail helper with the same format/flags.
bool Install() noexcept;
void Uninstall() noexcept;
bool Installed() noexcept;
// V66.42 A/B: when OFF, the cloud stage gate returns false for the RIGHT eye only
// (retail behaviour: no RIGHT clouds). Textures stay allocated. F4 toggles.
void SetRightCloudsEnabled(bool on) noexcept;

} // namespace kcd2vr::native_cloud_right_v6641
