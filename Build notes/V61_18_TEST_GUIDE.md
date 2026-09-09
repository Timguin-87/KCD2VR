# KCD2VR V61.18 — HUD priority cleanup

Source-only branch.

## Changes
- Removed the obsolete ROT B*D / D*B menu control. Runtime is fixed to the normal order.
- Removed the World / head translation slider and fixes head translation scale at 1.00.
- Game FOV target default = 95. Source FOV calibration default = 95.
- V61.17 HUD discovery wiring fixed: the read-only engine-global RTTI scan now runs during a normal VR session and no longer depends on the optional stereo observer suite.
- Stereo work remains passive; no native-stereo writes are enabled.

## What to test
Run into gameplay for 10–20 seconds, then send KCD2VR-debug log. Search for:

`V61.17-HUD-FLASH-DISCOVERY candidate ... CFlashUI`

or the `ENGINE-GLOBAL-SWEEP-V36` lines around 0x492D900..0x492D980.

If CFlashUI resolves, the next patch can safely target live HUD scale/X/Y and cursor X/Y with RESET.
