# KCD2VR theory sweep

This build combines the previous read-only renderer archaeology into one launch.
It does **not** enable stereo CVars, alter 3MonHack, or call unknown renderer APIs.
Keep `DiagnosticRendering=false`.

The sweep runs after KCD2's D3D12 DIRECT queue is captured and before OpenXR starts.
It tests five theories sequentially:

1. **Surviving CryEngine stereo registration**
   - recovers the containing help strings around `dual rendering` / `stereo mode`
   - xrefs the real help-string starts
   - validates `r_stereoScaleCoefficient` and `r_StereoGammaAdjustment`
   - dumps the common float CVar registration routine

2. **Stereo backing storage is consumed at runtime**
   - confirms current float values
   - uses a broader RIP-relative decoder than the previous probe, including no-REX integer forms and common SSE forms

3. **Dormant 3Mon code leads to a common view/render dispatcher**
   - validates the known 3Mon function and CVar refs
   - enumerates direct calls around both known parent callers
   - logs `COMMON-DISPATCH-CANDIDATE` for call targets shared by both parent regions

4. **KCD2 retains a secondary-view / render-to-texture path**
   - searches WHGame data for RenderToTexture, DynTexSource, RenderView, RenderScene,
     SecondaryView, MultiView, recursive/reflection/portal-view and render-mask terminology
   - xrefs each unique containing string start back into executable code

5. **Known renderer/view clues have non-registration consumers**
   - checks ShowRenderTarget, duplicate-frame-on-camera-change, reprojection, motion vectors,
     render-job mask, frustum optimization, recursion and portals.

## What to send back

Send `KCD2VR-debug.log` from one normal launch. Main menu is enough for the static sweep;
loading into the world is still useful to verify OpenXR/camera stability afterward.

High-value prefixes:
- `THEORY-SWEEP COMMON-DISPATCH-CANDIDATE`
- `THEORY-SWEEP secondary-string`
- `THEORY-SWEEP xref-summary`
- `THEORY-SWEEP backing`
