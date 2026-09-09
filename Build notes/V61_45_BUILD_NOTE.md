# KCD2VR V61.45 — POSE + UI CONSOLIDATION

This branch is based on V61.43 and deliberately avoids re-opening retired camera/UI experiments.

## Tracking / frame identity
- Keeps V61.12 exact-gameplay-CView culling synchronization.
- Culling-only pose application no longer advances the rendered PRESENT pose identity.
- PublishViewCamera preferentially reuses the exact recent culling-pose matrix for the same camera.
- Normal gameplay now exposes the already-existing pose source modes through INI:
  - 0 CACHED_XR
  - 1 SYNC_PREDICTED
  - 2 SYNC_NOW (test default)
  - 3 SYNC_PREDICTED_OFFSET
- `VrPosePredictionOffsetMs` applies only to mode 3.

## Dialogue / inventory
- Restores the solved V60.1/V61.36 rule: normal game/source FOV stays fixed.
- Retires the V61.38 transient `cl_fov` pump.
- Dialogue behavior remains ForcePOV=1, NoCameras=0.

## UI
- Retires the non-working generic live HUD-scale percentage controls from the VR menu.
- The old subtitle percentage slider was also invalid: `fc_setSubtitleSize` is a size mode, not 50..150 percent.
- See `OPTIONAL_UI_MODS/README.md` for the two testable PAK routes.

## VRIK
- Keeps the V61.43 semantic ownership ladder opt-in and OFF by default.
