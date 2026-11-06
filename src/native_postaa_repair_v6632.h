#pragma once
#include <cstdint>

namespace kcd2vr::native_postaa_repair_v6632 {

// Reconstruct the two right-eye PostAA history slots that retail KCD2's
// CPostAAStage::Resize deliberately clears after creating the two primary histories.
// Exact-build/prologue gated. The engine owns the resulting textures through the
// original stage slots; uninstall only removes the detour.
bool Install() noexcept;
void Uninstall() noexcept;
bool Installed() noexcept;

} // namespace kcd2vr::native_postaa_repair_v6632
