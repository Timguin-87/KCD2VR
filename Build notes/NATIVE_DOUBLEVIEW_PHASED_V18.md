# KCD2VR V18 — phased broad double-view/stereo experiment

Purpose: return to the V7 boundary (`WHGame+0x865550`) that visibly produced a doubled desktop image, while keeping V17 raw OpenXR diagnostics and the exact `0x866F6C` pair builder as observer-only.

With `EnableNativeEyeWrite=true`, one correlated main-4E6C mode-2 child automatically runs three phases from the first matching broad camera-build call:

1. **0–4 seconds — `legacy-v7-base`**: exact V7 behavior. Shift base camera banks `+0x8EE8` and `+0x9508` both +0.030 local-right for the duration of `0x865550`.
2. **4–8 seconds — `legacy-v7-selected`**: use selector `(CRenderView+0x8EE0)&1` and shift the selected indexed A/B banks both +0.030 local-right.
3. **8 seconds onward — stereo source ladder**: same selected indexed A/B banks get left/right half-IPD offsets. Source ladder remains fresh XR -> cached XR -> relaxed XR -> fixed 64 mm diagnostic fallback.

`WHGame+0x866F6C` is observer-only in V18, so it cannot add a second write. It logs the generated pair-builder state after the broad intervention.

Key markers:
- `CRV-DOWNSTREAM EXPERIMENT-V18`
- `NATIVE-BROAD-V18 apply= ... phase=legacy-v7-base`
- `NATIVE-BROAD-V18 apply= ... phase=legacy-v7-selected`
- `NATIVE-BROAD-V18 apply= ... phase=fresh-xr|cached-xr|relaxed-xr|fixed-64mm`
- `NATIVE-PAIR-V18 write-disabled`
- `NATIVE-PAIR-V18 post`
- `XR-VIEWS-V17` (raw XR logger retained from V17 source)

Packaged test settings:
- `DiagnosticRendering=false`
- `EnableNativeEyeWrite=true`

Interpretation:
- If only phase 1 recreates visible doubling, the exact legacy base-bank mutation is the important mechanism.
- If phases 1 and 2 both double, the broader `0x865550` lifetime/boundary matters more than bank indexing.
- If phase 3 produces stereo-like separation/depth, the broad boundary is a viable native-stereo injection point.
- If all phases are visually clean but pair output changes, V7's old doubling depended on another difference in the historical build and should be diffed further rather than assuming `0x865550` alone is sufficient.
