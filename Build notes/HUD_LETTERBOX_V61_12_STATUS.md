# HUD / letterbox status after V61.10 runtime test

Retired:
- HUD.xml fixed safe-area rectangles: moved/clipped the movie instead of scaling its content.
- `fc_setRatioPreset` early-return patch: too broad; start-menu text vanished and bars were not correctly removed.

Do not patch these again by offsets.

Next safe targets in current KCD2 `hud.gfx`:
- `_xscale`, `_yscale`
- `g_layoutWidth`, `g_layoutOffsetX`
- `SCREEN_WIDTH`, `SCREEN_HEIGHT`, `resolutionScale`
- `ActionCursorCross`, `CombatCursorCross`
- `ratioStrips` visibility only

The next UI patch should preserve the full Scaleform stage and change internal transforms/anchors rather than the UIElements viewport rectangle.
