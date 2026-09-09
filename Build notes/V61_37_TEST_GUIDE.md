# V61.37 headset check

This is not a diagnostic-log build. Use the old F7 menu and judge the result visually.

## View modes

Start with CORRECTED MONO. Confirm the V61.36/F8-corrected geometry is still good.

Then compare:
- PSEUDO .90
- DEPTH PSEUDO .70-1.00

For DEPTH PSEUDO, begin at Depth Influence 30%. Try 0%, 30%, 60%, then 100% briefly. At 100%, close geometry is allowed to approach 0.70 while far/background stays at 1.00. The intended useful range is likely lower than 100%.

If the depth relationship appears backwards (far scenery gets the strongest change instead of nearby geometry), report that specifically; the depth convention can be flipped without changing the architecture.

## HUD

Move LIVE HUD scale and release the slider. Test 100%, 70%, then 50%. This is session-only and is not written to the INI.

## Dialogue text

Enter a dialogue with visible subtitles. Move Dialogue/subtitle text and release. Try 100%, 75%, and 125%. This targets HUD SetSubtitleSize, not the 3D view or source FOV.

## Dialogue view

The build must not pump or restore cl_fov on dialogue entry/exit. The projection FOV should remain your selected game/source calibration. Any remaining dialogue close-up is a separate zoom/framing problem, not an FOV correction path.

## Performance

No FRIK/final-output write probes, generic draw counting, or high-frequency fallback file trace are active. Runtime INI saving remains disabled.
