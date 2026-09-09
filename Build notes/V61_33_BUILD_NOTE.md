# KCD2VR V61.33 — Native HUD / immersive zoom corrected combined source

Status: **source-only / not Windows-MSVC runtime verified in this environment**.

Baseline: V61.32 combined working tree, itself based on the user's corrected V61.30 `DXGUID-FIX` source. `CMakeLists.txt` keeps `dxguid`.

## Why V61.33 exists

V61.32 incorrectly conflated the F6 virtual-screen size with the native KCD2 HUD/UI scale. Runtime feedback established that inventory and dialogue were already being used successfully in **immersive GOOD MONO**, without F6. V61.33 separates those systems.

## Hotkeys

- **F4** — engine-focus-driven global pseudo test only. The V61.30 G-buffer wrapper scan is retired after `candidates=0 / depthHits=0`.
- **F6** — explicit/manual LOCAL virtual screen again. It is not automatically entered for inventory, dialogue, or pause.
- **F8** — automatic XR-derived source projection/cyclopean coverage + vertical optical-center correction. Small user trims remain available; no fixed 106-degree target.
- **F9** — `OFF -> READ -> READ+WRITE -> OFF`. READ performs correlation/observation only. READ+WRITE permits the existing validated temporary final-skin hand pulse. This is still a diagnostic ownership proof, not VRIK.

## Native HUD scale — UI tab

The UI tab now represents **native HUD scale**, not F6 screen size.

Target:

- UI element: `HUD`
- logical MovieClip: `MasterSwitch` (instance `all` in the shipped HUD XML)
- semantic CryEngine operation: `UIAction.SetScale`
- slider range: `0.60x .. 1.20x`, default `1.00x`

The request is gated on a **recent HUD object passively observed from the game's own FlashUI lookup**. No HUD pointer is guessed.

This source uses a deliberately isolated experimental console/script bridge because the exact retail KCD2 C++ MovieClip transform ABI has not yet been statically proven. KCD2's `IConsole::GetCVar` is already proven at vslot `+0xB8`. Public CryEngine-family ordering places `GetVariable`, `GetVariable(float)`, then `ExecuteString`, giving a **candidate** relative slot `+0xD0`. The candidate target must be executable and the call is protected by SEH. The scale command is user-triggered by releasing the UI slider; default startup does not execute it.

Command form:

```text
#UIAction.SetScale("HUD",-1,"MasterSwitch",{x=S,y=S,z=1.0})
```

Important: this path is **not yet runtime proven in retail KCD2**. Retail Lua-console execution may also be restricted. Failure must be treated as evidence to resolve the direct MovieClip transform ABI, not as a reason to revive HUD.xml viewport scaling or guess child vtable offsets.

The old HUD.xml width/height path remains retired as a true-scale solution because it clipped/reframed content rather than scaling the root display tree.

## F6 virtual-screen scale is separate

F6 keeps its own `VrFlatScreenScale` (default `1.58`) and right-grip/right-stick distance/size calibration. Moving the native HUD slider never changes F6 geometry.

## Immersive inventory/dialogue/bow zoom reduction

Inventory/dialogue remain in immersive GOOD MONO. Touch inventory/pause actions no longer toggle the F6/theater state.

New setting:

```ini
VrTransientZoomRetain=0.40
```

When KCD2 publishes a temporary camera FOV more than 5 degrees narrower than the stable source FOV, and F8 is not owning projection feedback, V61.33 retains only the configured fraction of that zoom delta. Default `0.40` follows the headset feedback: keep 40% of the native zoom, suppress 60%. The correction uses ordinary `cl_fov`, restores the prior value on exit, and is logged. This same mechanism is intended to suppress the unwanted bow zoom while leaving the existing bow aim sensitivity unchanged.

The previous right-grip-only bow zoom heuristic remains hard-disabled in V61.33 so the two guards cannot fight.

## Menu / controls retained from V61.32

Eight accordion tabs remain:

1. Resolution
2. Render
3. FOV / Projection
4. UI
5. Head position
6. Controls
7. Dialogue
8. Optimized Defaults

Optimized Defaults retains Default / VR Optimized, FPS 60–120, and unified view-distance 30–300 for `e_ViewDistRatio`, `e_ViewDistRatioVegetation`, and `e_MergedMeshesViewDistRatio`.

Touch controls retain the proposed native-like pass: LT block, RT attack/hold, left-stick locomotion/strafe, reduced right-stick turning + latched hard-deflection target switch, contextual right-grip + right-stick direction/bow aim, B weapon family, Y horse/companion family. Bow aim sensitivity itself is unchanged.

## Explicitly not revived

- mode-2 / Shadow stereo
- AFR stereo
- `+0x818/+0x828` stereo probing
- CSystem `+0x598`
- StereoL/R exact-source F8 producer test
- OperatorQueue pose writes
- blind renderer/pipeline re-entry
- old vcam / zero-camera route
- HUD.xml rectangle scaling as a true HUD scale
- guessed MovieClip child `+0x168`

## Immutable baseline

V61.12 gameplay-only culling synchronization is unchanged: temporary HMD matrix only around the gameplay frustum builder, immediate restore, auxiliary cameras untouched, cull FOV untouched. GOOD MONO submission remains available. Source baseline remains 1920x1800 unless the user explicitly applies another resolution.
