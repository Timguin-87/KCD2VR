#pragma once
namespace kcd2vr::native_taa_pair_epoch_v6661 {
// V66.62: register-preserving PostAA history getter stub. The native caller relies on
// RDX surviving both getter calls, so ordinary C++ detouring is explicitly forbidden.
bool Install() noexcept;
void Uninstall() noexcept;
bool Installed() noexcept;
void SetEnabled(bool enabled) noexcept;
bool Enabled() noexcept;
void LogStats(const char* reason) noexcept;
}
