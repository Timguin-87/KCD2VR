# Native Stereo Injection V17 — fallback ladder

Goal: stop burning one runtime launch per hypothesis.

V17 uses one stereo-source ladder at the proven WHGame+0x866F6C selected camera-pair builder:

1. `fresh-xr`: sane two-eye `xrLocateViews` pair <=80 ms old.
2. `cached-xr`: last-known-good sane pair <=500 ms old.
3. `relaxed-xr`: successful raw two-view result with sane 40–90 mm separation <=750 ms old, even if strict view-state bits are incomplete.
4. `fixed-64mm`: deterministic -32/+32 mm diagnostic split if all XR-derived tiers fail.

Absolute HMD pose is still owned by the existing camera hook. V17 only injects relative eye separation.

New diagnostics:
- `XR-VIEWS-V17`: raw xrLocateViews result, viewCount, flags, positions, raw IPD, plausibility.
- `NATIVE-PAIR-V17 apply`: chosen source tier, IPD, offsets, cache age and raw XR metadata.
- `NATIVE-PAIR-V17 post`: generated WHGame output state after the builder.

Recommended first run:
- DiagnosticRendering=false
- EnableNativeEyeWrite=true

Interpretation:
- `source=fresh-xr/cached-xr/relaxed-xr`: headset-derived eye separation reached the engine.
- `source=fixed-64mm`: XR extraction failed, but the KCD2 injection path was still exercised.
- nonzero `q2` with fixed-64mm reproduces V14's proven builder response.
