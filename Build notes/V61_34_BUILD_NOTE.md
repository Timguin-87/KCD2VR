# KCD2VR V61.34 — calibration + generic depth-candidate probe

Source-only combined test build. No Windows/MSVC or headset runtime verification was possible in this environment.

## What this build is for

V61.34 deliberately retires the V61.31/V61.33 accordion menu for testing and restores the older two-page calibration menu. The release menu work is preserved in V61.33 but is not used here.

The goal is to lock down the new corrected mono/pseudo projection, calibrate user-preferred framing, test a wider native-HUD shrink range, and replace the failed narrow F4 wrapper-depth scan with a read-only generic D3D12 depth-candidate observer.

## Hotkeys

- **F4** — generic D3D12 depth-candidate observer ON/OFF. First press resets/arms the observer. Play/look around for ~10–20 seconds. Second press stops it and writes a ranked candidate summary to `KCD2VR-debug.log`.
- **F6** — optional LOCAL/world-locked virtual screen. Unchanged purpose; not used automatically for inventory/dialogue.
- **F7** — old two-page VR calibration menu.
- **F8** — corrected OpenXR projection/Center-Y correction toggle. It starts **ON** in this calibration build.
- **F9** — final-pose correlation READ-ONLY ON/OFF. The temporary CPoseData write proof is retired.

## F8 / projection changes

The successful V61.33 headset setting is promoted:

- default `VrAutoCenterYTrim = +0.080`
- default game FOV target = 106 degrees
- default source/view mapper FOV = 106 degrees
- F8 starts ON

V61.34 no longer continuously chases or overwrites `cl_fov`. This is intentional so the old menu can tune:

1. **Game FOV** — KCD2 `cl_fov`, committed with the visible APPLY button.
2. **Source FOV calibration** — compositor/view mapper FOV, live and independent.
3. **VR zoom / framing** — live compositor zoom.
4. **F8 vertical Center-Y trim** — live `-0.120 .. +0.120`; menu shows both trim and resulting effective Center-Y.

F8 still derives the headset optical-center correction from live OpenXR eye FOVs and the selected source/view mapper FOV. The same corrected projection is used by both MONO and the existing safe pseudo modes.

## HUD scale test

`VrNativeUiScale` is now a user-facing inverse shrink control:

- `1.00` => 100% HUD root scale
- `1.20` => 80%
- `1.50` => 50% (**default**)
- `1.80` => 20%

The old menu exposes the full `1.00 .. 1.80` range. Internally the semantic HUD request maps `rootScale = 2.0 - control` and is clamped to 0.20..1.00.

The persisted value is attempted once after a real live HUD object has been observed, and again when the slider is released. This remains an inferred FlashUI/console ABI and is not claimed runtime-proven beyond prior user observation.

## F4 generic depth-candidate observer

The old V61.30 G-buffer wrapper scan remains retired. V61.34 instead broadens observation at the D3D12 API level:

- hooks `ID3D12Device::CreateDepthStencilView` to map DSV descriptor handles to real `ID3D12Resource*` objects;
- observes the DSV bound through `OMSetRenderTargets`;
- hooks `DrawInstanced` and `DrawIndexedInstanced` with an extremely small disabled fast path;
- while F4 is armed, counts draws against the currently bound depth resource;
- records resource dimensions/format/flags;
- ranks on F4-off by screen-size match, aspect-ratio match, draw count, then bind count;
- excludes the mod's own VR/menu command lists;
- performs **no resource transitions, clears, copies, shader changes, or resource-content writes**.

This is conceptually modeled on the robust "observe many depth candidates and rank them" strategy used by generic post-processing injectors, but it is original KCD2VR instrumentation, not ReShade code.

If a plausible scene-depth resource is proven, a later build can sample/copy it safely and use depth only to make a **small, clamped modulation** of the existing pseudo disparity. Full DIBR/reprojection is not the target.

## VRIK status

The CPoseData write route is retired after the visible-negative V61.30/V61.33 tests. V61.34 F9 is read-only correlation only. The intended VRIK plan remains the previously agreed late/final character-pose post-animation route (Cyberpunk/Crysis-style ownership), with the existing controller-driven two-bone arm solution applied at the actual visible pose owner once that owner is recovered.

## Safety / preserved baseline

- V61.12 gameplay-only culling implementation unchanged.
- GOOD MONO OpenXR submission semantics retained.
- F6 remains optional virtual screen only.
- no mode-2/Shadow stereo, AFR, `+0x818/+0x828`, CSystem `+0x598`, renderer re-entry, OperatorQueue writes, or CPoseData write reactivation.
- corrected CMake `dxguid` linkage preserved.
