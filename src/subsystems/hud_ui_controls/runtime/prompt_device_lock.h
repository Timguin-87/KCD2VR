#pragma once
#include <cstdint>

namespace kcd2vr::prompt_device_lock {

// Exact-build, fail-closed prompt-device bridge. This intentionally affects
// KCD2's UI classification and balanced cursor lifecycle; it does not pretend keyboard
// SendInput is native XInput. Device 2 is the game's Xbox/gamepad prompt family
// on the supported retail build, corroborated by the supplied XboxPromptLock mod.
bool Install() noexcept;
void Uninstall() noexcept;
bool Installed() noexcept;
// Saved art selector: 2 Xbox / 4 PlayStation. Native semantic family stays Xbox2.
// ForcedDevice reports that semantic family; it is not the saved art selector.
void SetForcedDevice(int device) noexcept;
int ForcedDevice() noexcept;
// Pass native requested device through only as a held-pointer fallback when
// the mixed-cursor bridge is unavailable. Never unlock merely for Theater.
void SetSuspended(bool suspended) noexcept;
bool Suspended() noexcept;
bool MixedCursorReady() noexcept;
// Request a cursor-hide transition on the next natural native input callback.
void NotifyNavigation() noexcept;
void WriteCursorSnapshot() noexcept;
// Queue a bounded glyph-art refresh on the native VM owner. No console Lua.
bool RequestFlashStyleD56(int device) noexcept;
// Copied station/palm state only; native UI work runs on the existing VM client.
void PublishMinigamePromptsD80(bool active,std::uint64_t lifetime,unsigned phase,
    bool openPalm,std::uint64_t now,bool alchemy) noexcept;

} // namespace kcd2vr::prompt_device_lock
