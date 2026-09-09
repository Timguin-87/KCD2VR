# UI mod findings — V60.1

Source material inspected from the user-provided mods.

## `letterboxremover.pak`

Contains only `Libs/UI/hud.gfx`. The decompressed Scaleform/GFx movie contains HUD/dialogue symbols and ActionScript names including:

- `mc_interactiveDialog`
- `dialogWindow`
- `ratioStrips`
- `cinematicRatio`
- `scaleMode`, `noScale`
- screen width/height bookkeeping

This strongly indicates letterbox/cinematic framing is implemented in the Scaleform HUD movie, not only in the 3D renderer.

## Sleek Modular HUD 2.0.1

The core variants also contain only `Libs/UI/hud.gfx`. Comparing the Standard and Standard + No Letterbox variants shows exactly one ActionScript DoAction block changes. The decompressed movie exposes KCD2-specific names:

- `g_RatioPreset`
- `fc_setRatioPreset`
- `RATIO_PRESET_GAME`
- `RATIO_PRESET_APSE`
- `RATIO_PRESET_CUTSCENE`
- `RATIO_PRESET_DIALOG`
- `RATIO_PRESET_DIALOG_EXTENSION`
- `RATIO_PRESET_MAIN_MENU`
- `CINEMATIC_RATIO`
- `resolutionScale`
- `SCREEN_WIDTH`, `SCREEN_HEIGHT`
- `updateDialogPosition`

The No Letterbox variant differs from the default in only 12 bytes inside that ActionScript block. Among the changed numeric pushes are two `0.5 -> 0.0` changes and two `0.0 -> -70.0` changes, plus two one-byte opcode/value changes. This is strong evidence that KCD2's letterbox/dialog framing is controlled by a small ratio-preset routine inside `hud.gfx`.

## `hudinventoryrework(1).pak`

This package contains UI textures but no `.gfx` movie, so it is useful for HUD artwork/atlas clues but not for changing layout/projection logic.

## Consequence for VR

There are now two separate suspects for the "tiny dialogue/inventory window":

1. transient 3D camera FOV publication — V60.1 tests a compositor FOV guard for this;
2. Scaleform ratio-preset / viewport logic inside `Libs/UI/hud.gfx` — if the guard does not solve it, this is the next target.

Do not blindly patch the 12 byte offsets into a different `hud.gfx`; offsets are build/mod-version specific. The safer next step is to identify the KCD2 stock ActionScript function corresponding to `fc_setRatioPreset` and patch or intercept its dialog/cutscene preset semantically.
