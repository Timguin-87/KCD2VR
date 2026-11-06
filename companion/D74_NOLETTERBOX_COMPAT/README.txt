OPTIONAL HUD MOVIE COMPATIBILITY ADDON

This is only for the supported replacement HUD movie, not a general HUD patch.
The normal main companion already contains compact notifications and callbacks.
Use this addon only when that specific replacement would overwrite them.
The packaged manifest and startup validator identify its compatibility target.

Place its Mods subfolder alongside the main companion. If maintaining a manual
mod-order file, preserve the other entries and load this addon after the main
companion and the matching HUD replacement. Check the native startup log for the
actual mount order. No installer here changes another mod or the game settings.

It overrides one HUD movie, retains its original code and other leaves, and adds
the same glyph and ingredient-name callbacks as the main companion. The DLL owns
station-local prompt scaling; this file cannot replace the DLL. Full-field masks
and cinematic geometry stay unchanged.

This compatibility layout has not been tested in a headset. Keep it optional.
