# KCD2VR / Medieval Immersion Simulator V58.4 — angular-correct mono diagnostic

V58.4 supersedes V58.3 for headset testing. V58.3 tried to restore the old 130-degree projection baseline, but user feedback clarified that the old baseline was itself still binocular/too narrow.

## What V58.4 changes

- Keeps the V58.3 startup `cl_fov` refresh, dedicated-XR F7 menu path, corrected left-stick menu navigation, and input-injection telemetry.
- Defaults the legacy fallback source VFOV to 100 degrees instead of restoring 130.
- **F5 toggles the key A/B test:**
  - ON (default): source VFOV follows the live published KCD2 `ViewCamera` FOV; the tangent remap does not globally shrink the entire image just to cover an asymmetric HMD edge; uncovered peripheral rays edge-hold.
  - OFF: legacy manual source VFOV plus the old global no-black coverage clamp.
- The center of the image is therefore allowed to keep the camera's actual angular scale in LIVE mode. This is a temporary mono-bridge diagnostic, not true stereo.

## First test

1. Launch and judge intro/main menu/gameplay without touching F5 or the KCD2 FOV slider.
2. In gameplay, press F5 once to compare LEGACY, then again to return LIVE. Each switch beeps and logs `V58.4-ANGULAR-MODE`.
3. Report which mode has the less binocular/nose-close world scale and whether LIVE shows objectionable stretched/frozen peripheral edges.
4. Then open F7 and check that the duplicate monitor layer is gone and left-stick navigation works.

Useful log markers: `V58.4-ANGULAR-SYNC`, `V58.4-MONO-BLIT`, `V58.4-ANGULAR-MODE`, `V58.3-INJECT`, `V58.3-FOV-REFRESH`.

## Architectural limit

The gameplay bridge still copies one KCD2 mono backbuffer into both OpenXR eyes. Even if LIVE mode fixes world scale, it cannot create binocular disparity or correct per-eye occlusion. True stereo still requires distinct engine-rendered left/right scene resources.
