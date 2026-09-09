# KCD2VR V61.32 — Combined release-candidate controls / menu / pseudo-projection source

Status: **source-only**. This package has not been Windows/MSVC runtime-verified in this environment.

Baseline: the user's corrected `KCD2VR-V61.30-ALL-HYPOTHESES-SOURCE-DXGUID-FIX.zip`. The corrected `CMakeLists.txt` / `dxguid` link fix is preserved unchanged.

## Immutable behavior preserved

- V61.12 exact gameplay-`CView` culling synchronization is unchanged. The HMD matrix is temporary around the gameplay frustum builder and immediately restored; auxiliary cameras and cull FOV remain untouched.
- GOOD MONO remains the submission baseline: one KCD2 source image may feed both eyes while OpenXR retains independent per-eye pose/FOV.
- The 1920x1800 KCD2 source baseline is not forced by this build.
- Retired stereo routes stay retired: mode-2/Shadow, AFR/Insert/Home, `+0x818/+0x828`, CSystem `+0x598`, old vcam/zero-camera and renderer re-entry are not revived.
- OperatorQueue pose writes remain retired.

## Hotkeys

### F6 — virtual screen restored

F6 once toggles the existing LOCAL/world-locked virtual screen ON; F6 again returns to immersive projection.

While the virtual screen or inventory comfort screen is active:
- hold **Right Grip**;
- Right Stick Y adjusts distance;
- Right Stick X adjusts physical screen scale.

Default flat-screen scale is reduced from 1.58 to **1.42** as a first approximately-10%-smaller framing candidate after headset feedback that inventory/dialogue still felt slightly zoomed. It remains live-adjustable and persisted.

### F8 — Auto Projection (replaces dead StereoL/R producer test)

The V61.30 exact-source `StereoL`/`StereoR` producer hypothesis is retired after runtime evidence reported `binds=0`, `uniqueRT=0`, `sameEpochPairs=0` in repeated tests.

F8 now toggles automatic source-projection correction. It:
1. reads the exact gameplay `CView` VFOV already proven by V61.28/30;
2. reads the current OpenXR left/right eye tangents;
3. reads the actual captured KCD2 source aspect;
4. derives the enclosing cyclopean source coverage and vertical optical center;
5. adjusts ordinary `cl_fov` through its CVar only until measured gameplay VFOV covers the derived target;
6. feeds the **measured actual gameplay VFOV** into the currently selected GOOD MONO or safe pseudo presentation;
7. applies the derived Center-Y in the presentation path.

There is no fixed 106-degree target anymore. A small 1-degree automatic coverage margin remains, with user trim on top.

Manual correction remains available in the **FOV / Projection** menu tab:
- Manual Game FOV: 70–130 degrees;
- F8 Auto FOV trim: -10 to +10 degrees;
- Presentation Zoom: 0.95–1.05;
- F8 optical-center trim: -0.08 to +0.08.

Zero trim / Zoom 1.00 is the first-test baseline.

### F9 — read first, write second

F9 now cycles:
1. **OFF**
2. **READ** — final-skin Henry-pose correlation only; zero pose setters/writes
3. **READ+WRITE** — keeps correlation active and permits the existing validated gate-2 temporary hand pulse immediately before the final consumer, then restores it
4. back to **OFF**

The second-stage hand pulse is retained only as a diagnostic proof. The previous V61.30 runtime already showed successful hand APPLY/RESTORE events with no visible hand movement, so this is not presented as solved VRIK and no stronger/repeated ownership hypothesis is introduced.

### F4 — focus-driven pseudo only

F4 now tests the useful part of V61.30 without the failed depth-wrapper archaeology:
- reads `Dof_User_FocusDistance`, `Dof_FocusDistance`, or `FocusDistance` when available;
- maps near/far focus distance to smoothly changing pseudo strength;
- uses the safe matched pseudo presentation;
- retains fixed 8 ms rotation filtering.

The V61.30 Normal G-buffer 0x100 wrapper/resource scan is **retired and never armed** by F4. V61.30 runtime reached very high stage/MRT counts with zero candidates/depth hits.

F4 and F8 are intentionally compatible: F8 can supply source FOV/optical-center correction while F4 changes only pseudo strength.

## VR menu

The V61.31 parchment/book accordion and right-Touch laser are merged onto the corrected V61.30 baseline. The right page now has eight tabs:

1. Resolution
2. Render
3. FOV / Projection
4. UI
5. Head position
6. Controls
7. Dialogue
8. Optimized Defaults

The 185 ms open/close animation and exact right-Touch ray pointer remain.

### Resolution

Manual Apply only. Presets remain:
- 960x900
- 1440x1350
- 1920x1800
- 2400x2250
- 2880x2700
- 3840x3600
- 1248x1344 portrait
- 1872x2016 portrait
- 2496x2688 portrait

Apply writes `r_CustomResWidth`, `r_CustomResHeight`, then `r_CustomResPreview=1` with max-size/readback checks.

### Render

- Mono: GOOD MONO
- Fake Stereo: current safe compositor pseudo `.95`
- Native Stereo: visible but locked; no retired stereo path is revived

F8 Auto Projection is independent of this selector and can provide source correction for Mono or Fake Stereo.

### UI

This is deliberately labelled/documented as **virtual-screen scale**, not true KCD2 Scaleform HUD scaling. True HUD root-MovieClip scaling remains future work.

### Optimized Defaults

- Default / VR Optimized profile choice
- FPS lock slider: 60–120, proposed VR marker 90
- View distance slider: 30–300, proposed VR marker 120
- Apply

The View Distance Apply writes these three ordinary KCD2 CVars to the same value:
- `e_ViewDistRatio`
- `e_ViewDistRatioVegetation`
- `e_MergedMeshesViewDistRatio`

The existing reversible VR graphics snapshot/profile is retained; profile disable restores its captured graphics snapshot when available.

## Native Quest Touch control pass

The provisional Win32 input bridge is changed toward gamepad-like KCD2 combat while preserving the user's working locomotion and bow aim:

- Left Stick: movement/strafe unchanged
- Left Stick click: sprint
- Left Trigger: block/guard (RMB)
- Right Trigger: held native attack (LMB); KCD2 owns tap/hold/heavy timing
- Right Grip + Right Stick: smooth two-axis attack-direction / bow-aim path at the already-tested 50% mouse-camera rate
- Right Stick inner range: reduced smooth turn (55% of the prior full-rate path)
- Right Stick outer horizontal >=0.88: one latched large horizontal gesture, reset below 0.58; intended to encourage KCD2 opponent switching without rapid cycling. This remains a mouse-bridge fallback, not a proven native opponent-switch action injection.
- Right Stick click: crouch
- Right Stick Up: jump when the directional modifier is not active
- Right B short: weapon slot swap 1/2
- Right B hold >=0.5 s: provisional draw/holster by tapping the current weapon slot
- Left Y short: horse/companion context (`X`)
- Left Y hold >=0.5 s: held companion/dog `X`
- Left Grip + Left Stick: existing quick-action fallback layer
- A: interact/select (`E`)

### Bow zoom guard

Bow aim sensitivity itself is unchanged.

Because no verified semantic bow-equipped state owner has been proven yet, the zoom guard is intentionally conservative/heuristic:
- it arms only during the existing Right-Grip directional/aim gesture;
- it performs **zero writes** unless the exact gameplay VFOV drops by more than 2 degrees from the grip-entry baseline;
- if such a drop occurs while F8 Auto Projection is OFF, it raises ordinary `cl_fov` toward the baseline and restores the original game FOV on grip release;
- if F8 Auto Projection is ON, F8's own projection/FOV feedback owns this problem and the separate guard stays out of the way.

Config: `EnableBowZoomGuard=true`.

## New / changed INI settings

```ini
VrAutoFovTrimDegrees = 0.0
VrAutoCenterYTrim = 0.0
EnableBowZoomGuard = true
VrNativeUiScale = 1.42
```

## Static audit performed here

- corrected V61.30 `CMakeLists.txt` with `dxguid` is preserved byte-identical;
- `camera_hook.cpp/.h`, `frame_sync.cpp`, `device_hook.cpp`, and `depth_probe.cpp` remain byte-identical to the corrected V61.30 baseline;
- modified C++ files pass delimiter/scope-balance checks;
- there are no active `StereoProducerProbe` or F4 `DepthProbe::Install/SetEnabled` calls in `mod.cpp`;
- no Windows/MSVC compiler/runtime verification is claimed.
