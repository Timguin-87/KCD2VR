#pragma once
namespace kcd2vr::startup_s15 {
// After LoadConfig, offer next-launch settings on a separate GUI thread.
// Never block renderer bootstrap, invoke in DllMain, or resize a live renderer.
bool ApplyPendingBeforeConfig() noexcept; // one allowlisted merge before any loaded settings writer
void FirstRun() noexcept;
// R118: write the built-in default KCD2VR.ini beside the mod when none exists (never over an existing file).
// Returns core::default_ini_r118::Result as int. Called before ApplyPendingBeforeConfig / LoadConfig.
int EnsureDefaultIniR118() noexcept;
}
