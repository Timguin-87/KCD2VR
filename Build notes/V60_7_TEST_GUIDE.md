# KCD2VR V60.7 F8 Compositor Isolation

Press **F8** once. F6 is forced off only during the 35-second test and restored after.

All five phases keep **KCD2 cl_fov = 100**, **source FOV = 100**, **zoom = 1**, **UV binocular = 0**. Only compositor mapping/submitted FOV changes.

1. **A RAWMAP / SUBMIT .75** — current V60.6-style behavior. Source rays use raw Quest eye FOV, OpenXR projection submits asymmetry .75.
2. **B MATCHEDMAP / SUBMIT .75** — new consistency test. Source rays use the same .75-adjusted FOV that is submitted to OpenXR.
3. **C RAWMAP / SUBMIT 1.00** — raw Quest mapping and raw Quest submitted FOV; no mismatch.
4. **D MATCHED COMMON 0.00** — source map and submission both use centered/common FOV. Diagnostic only.
5. **E RAW NORMALIZED COPY** — no tangent FOV mapping; immersive projection-layer control. It will not be geometrically correct, but should expose whether the tangent stage is what moves the whole raster/UI.

Watch a fixed HUD/UI element and world straight lines. Report which phase has the least uniform UI push and least geometry warp.

Useful log markers: `V60.7-F8-BEGIN`, `V60.7-MONO-BLIT`, `matchedSubmitFov=YES`.
