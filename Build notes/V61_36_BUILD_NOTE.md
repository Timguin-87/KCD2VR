# KCD2VR V61.36 CLEAN MODES

This source branch removes diagnostic hot-path work from the normal runtime.

Presentation switches in the old VR menu:

- **OLD MONO** — legacy/pre-F8 geometry, no Center-Y correction.
- **CORRECTED MONO** — current XR-derived cyclopean projection with the proven +0.080 Center-Y trim foundation. This is the default.
- **PSEUDO .90** — corrected geometry plus fixed 0.90 pseudo stereo.
- **PSEUDO .90 + DEPTH** — same .90 pseudo stereo with a small depth-weighted modulation. The `Depth stereo strength` slider is 0.00–0.20, default 0.05. It is intentionally a small modulation, not full reprojection.

Performance cleanup:

- synchronous `KCD2VR-debug.log` fallback trace compiled to a no-op;
- FRIK/final-pose diagnostic hooks are not installed;
- F4/F8/F9 test toggles are removed from the runtime path;
- per-draw D3D12 depth hooks are not installed; depth mode uses only DSV creation + OMSetRenderTargets bind tracking and exact source-resolution/aspect matching;
- in-game INI save is disabled;
- failed HUD-scale control is hidden until the exact live FlashUI MovieClip transform ABI is recovered.

Dialogue/inventory correction:

The V61.33 transient `cl_fov` writer is retired. Dialogue/inventory no longer pump or restore `cl_fov`; game/source FOV remain fixed. Presentation zoom/framing is user-owned.

V61.12 gameplay-only culling remains unchanged. F6 remains the optional virtual screen.

Source/static edited only in this environment; not Windows/MSVC or headset verified.
