# KCD2VR V36 Multi-Theory Stereo Sweep

Observer-only source build. No CVar, camera, projection, render-target, return-value, or CSystem flag writes.

## Why V36
V35 showed CSystem+0x598 is a generic system-state predicate, not a credible stereo master switch. V36 deliberately tests multiple renderer-native hypotheses in one run.

## Theory A — Volumetric cloud stereo path
Reads the recovered backing global for `r_VolumetricCloudsStereoReprojection` at WHGame+0x50DBCD8. Static DLL analysis shows `$VolCloudLeftEye` is registered at WHGame+0x742413 and its handle is stored at owner offset +0xA0.

Markers:
- `VOLCLOUD-STEREO-V36`

Interpretation:
- reprojection=1 confirms another retained/active renderer stereo feature layer.
- the LeftEye handle anchor remains a target for later consumer tracing.

## Theory B — Engine singleton/global object sweep
Scans WHGame+0x492D780..+0x492D900 for pointers to MSVC RTTI-bearing engine objects. This window contains known CSystem/C3DEngine globals and may expose renderer/stereo-provider singletons without guessing old SDK vtable slots.

Markers:
- `ENGINE-GLOBAL-SWEEP-V36`

## Theory C — CStandardGraphicsPipeline embedded-object sweep
On first pipeline dispatch, scans the first 0x800 bytes of the live pipeline object for pointers to RTTI-bearing objects. This can expose embedded stage/provider objects that are not global singletons.

Markers:
- `PIPELINE-OBJECT-SWEEP-V36`

## Theory D — hidden eye/pass enum sweep
For the first 16 CStandardGraphicsPipeline dispatches, snapshots the first 0x2000 bytes before and after execution and compares consecutive pre-dispatch snapshots. Logs changed bytes when either side is a small enum-like value (0..3).

Markers:
- `PIPELINE-STATE-V36 kind=inter-pre`
- `PIPELINE-STATE-V36 kind=intra-dispatch`

This is intended to discover current-eye / stereo-pass / render-mode selectors without knowing their field offsets.

## Retained controls
V33 direct renderer stereo backing values and V35 CSystem semantic probe remain present as controls. CSystem+0x598 is not forced and is considered retired as a stereo candidate unless new evidence contradicts that conclusion.
