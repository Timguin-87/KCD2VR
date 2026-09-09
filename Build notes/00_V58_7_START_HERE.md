# KCD2VR V58.7 — Crysis Crop + World-Fixed F6

This is a SOURCE package. Build it with the same Windows/MSVC + vcpkg + DetourModKit setup used for V58.6.

## Purpose

V58.7 removes the global mono coverage squeeze that was introduced in V46.6 to hide the lower black border. That clamp made the FOV slider effectively stop changing the image below the Quest coverage threshold and contributed to the wrapped/binocular presentation.

The new default follows the Crysis VR workaround more closely:

1. Read the actual published KCD2 `ViewCamera` vertical FOV.
2. Read the real asymmetric OpenXR eye FOVs.
3. Preserve the source angular scale and crop each eye from that symmetric source instead of globally shrinking the whole source image.
4. Do not edge-hold/stretched-copy missing source pixels.
5. Keep startup in F6 until the published KCD2 source frustum is actually wide enough to cover the current HMD FOV.

This remains MONO duplicated into both eyes. It is not true native stereo.

## V58.7 changes

### Real F6 world quad
- F6 is now an `XrCompositionLayerQuad` in `LOCAL` space.
- Full head position + orientation are captured once.
- The screen pose no longer follows later headset movement.
- HMD rotation and position injection into the KCD2 source camera are suspended while F6 is active.
- The game image is copied to the quad without the old yaw/pitch counter-warp.

### Automatic startup F6
- Startup begins in F6 automatically.
- The mod computes the symmetric source VFOV required to contain the current asymmetric OpenXR views.
- The immersive switch requires:
  - a real published KCD2 camera FOV,
  - valid OpenXR view FOVs,
  - source VFOV >= required VFOV + 1 degree,
  - a real camera/present frame,
  - 100 ms continuous stability.
- Pressing F6 manually cancels this startup automation.

For the Quest 3 values seen in the logs, KCD2 at about 140 horizontal FOV has been publishing about 113.08 degrees vertical, which should be enough. If KCD2 stays too narrow, V58.7 intentionally stays on the fixed F6 screen rather than globally zooming the immersive image to hide a missing edge.

### Honest mono OpenXR pose
The copied frame comes from one KCD2 camera. V58.7 therefore submits both projection views from one common center/head pose instead of claiming that the duplicated pixels were rendered from two cameras separated by the physical IPD. Each eye still keeps its own asymmetric OpenXR FOV.

### F5
- Default: `CRYSIS_CROP` — published KCD2 VFOV, exact angular scale, no global clamp, no edge-hold.
- F5: `LEGACY_CLAMP` — old V58.6 global no-black safety squeeze for comparison only.
- The F7 source-VFOV slider is now explicitly labeled as a legacy/F5-only control.

### Head smoothing
- Fixed 8 ms remains the default.
- F4 still toggles adaptive only as a diagnostic. The previous headset test reported adaptive as much worse.
- The more important V58.7 jitter change is the common-center mono projection pose.

### Controls
V58.6 control swap is retained:
- Left Y: forward/back.
- Left X: smooth horizontal turn.
- L3: sprint.
- Right X: strafe.
- Right stick up: jump.
- R3: crouch.
- Right trigger + right stick: attack direction.
- Right grip remains reserved.

## Test order

1. Set KCD2 FOV to **140** for the first run and do not touch the F7 legacy VFOV slider.
2. Watch intro/menu/loading in automatic F6. Turn and lean your head: the virtual screen should remain fixed in one place.
3. Note exactly when the automatic switch to immersive view happens.
4. In immersive mode, check:
   - world scale / remaining zoom,
   - far left edge in left eye and far right edge in right eye,
   - whether a lower black border appears,
   - whether head jitter is reduced.
5. Press F5 once for `LEGACY_CLAMP`, compare for 5–10 seconds, then press F5 again to return to `CRYSIS_CROP`.
6. F4 is optional. Fixed 8 ms is the intended default.
7. Test the swapped locomotion layout from V58.6.

## Important logs

Please send the debug log after the run. The useful V58.7 markers are:

- `V58.7-STARTUP-F6`
- `V58.7-F6-QUAD`
- `V58.7-F6-LAYER`
- `V58.7-F6-CAMERA`
- `V58.7-MONO-SYNC`
- `V58.7-MONO-BLIT`
- `V58.7-MONO-POSE`

`V58.7-MONO-SYNC` reports `publishedVFOV`, `requiredCropVFOV`, and their margin. That tells us directly whether a black edge should be mathematically possible.

## Safety / scope

- No native stereo re-entry was enabled.
- No projection-matrix write was added.
- No WHGame skeleton/IK write was added.
- No ViGEm backend yet.
- F7 remains the existing simple XR menu; illustrated book UI is still a later task.
