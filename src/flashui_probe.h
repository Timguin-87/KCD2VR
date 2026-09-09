#pragma once
namespace kcd2vr::flashui_probe {
bool Install() noexcept;
void Uninstall() noexcept;
// V61.25 live HUD proof: toggles only CombatCursorCross through the same
// FlashUI element virtuals used by WHGame. Uses the most recently observed HUD.
bool SetCombatCursorVisible(bool visible) noexcept;
// V62.4 generic retail HUD visibility helper for declared MovieClips.
bool SetHudMovieClipVisible(const char* name, bool visible) noexcept;
// Recent passive HUD observation gate retained for cursor archaeology.
bool HasRecentHud() noexcept;

// V62.2 retail-native HUD transform path. Uses the exact WHGame FlashUI GetElement
// call sequence and IFlashVariableObject Get/SetDisplayInfo virtuals recovered from
// retail WHGame.dll. No console Lua, PAK, HUD.xml or hud.gfx replacement required.
bool ApplyVrLayout(bool enabled,
                   float subtitleScale, float subtitleY,
                   float dialogueScale, float dialogueY, float dialogueInsetPct,
                   float hintsScale, float hintsInsetPct, float hintsY) noexcept;
}
