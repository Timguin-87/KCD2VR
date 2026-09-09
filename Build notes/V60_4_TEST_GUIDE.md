# V60.4 Test Guide — Frustum Culling + Independent FOV + Controls

Build ID: `FRUSTUM-CULL-MANUAL-FOV-CONTROLS-20260905-H`

## What changed

- PS3 DIRECT is retired because it produced double vision.
- PS6 replaces that slot: tangent/OpenXR, asymmetry `0.875`, UV offset `0`.
- Right Grip + Right Stick is now smooth two-axis mouse aim at 50% of the normal camera mouse rate.
- Left Y SHORT swaps weapon slots 1/2.
- Left Y HOLD >= 0.5 s taps the current weapon slot again as the provisional draw/holster toggle.
- Game FOV and VR Source FOV are now independent.
- New `Culling frustum expansion` slider: 1.00–1.60, default 1.25.

## Culling test first

Use the monitor view because the culling defect is visible there before OpenXR presentation.

1. Keep a fixed camera position where a roof/tree/wall disappears near the side.
2. Set Cull = 1.00. Turn slowly until the object disappears.
3. Try 1.15, 1.25, 1.35, 1.50.
4. The desired result is later/no disappearance with **no visible FOV change** on the monitor.

The implementation temporarily widens `CCamera + 0x30` only while
`CCamera::UpdateFrustumPlanes` executes, then restores the original camera FOV.
The log line is:

`V60.4-CULL-FRUSTUM ... fovDeg=... widenedDeg=... scale=... restored=true`

If the field is not plausible/readable it fails closed and logs `skipped`.

## FOV / zoom test

`KCD2 game FOV` changes what the game renders.
`VR source-FOV calibration` changes only the compositor's assumption about that image.

Start with:
- game FOV: 105 or 110
- source calibration: 95
- zoom: 1.00
- PS1 or PS2

Then change only Source FOV: 90, 95, 100, 105, 110.
Look for the value where straight geometry and head rotation look least warped.
Do not change game FOV at the same time.

Once a source calibration is chosen, change only game FOV to test whether additional
scene coverage improves the result.

## Pseudo modes

- MONO
- PS1 = asym 0.75, UV 0
- PS2 = asym 1.00, UV 0
- PS6 = asym 0.875, UV 0
- PS4 = asym 0.75, UV -0.002
- PS5 = asym 0.75, UV -0.004

## Controls

- Hold Right Grip and move Right Stick: smooth X/Y bow aim at 50% normal mouse-camera rate.
- Left Y short: swap 1/2.
- Left Y hold >= 0.5 s: draw/holster current slot (provisional same-slot toggle).

No MSVC compiler is available in the packaging environment; source/static checks only.
