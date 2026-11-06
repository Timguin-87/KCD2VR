#pragma once

namespace kcd2vr::hud_xml_mod {

struct Settings {
    float scale = 0.85f;
    int offset_x = 0;
    int offset_y = 0;
};

// Writes/updates a normal KCD2 mod at <game>\\Mods\\KCD2VR_HUD_Runtime.
// The retail game loads HUD.xml from the generated stored (uncompressed) PAK
// on the next process start. This function deliberately does not attempt an
// unsafe live FlashUI reload.
bool WriteRuntimeHudMod(const Settings& settings) noexcept;

// Removes the generated HUD PAK + manifest only. Existing third-party HUD mods
// are never touched. Restart KCD2 after reset for the original HUD to return.
bool ResetRuntimeHudMod() noexcept;

} // namespace kcd2vr::hud_xml_mod
