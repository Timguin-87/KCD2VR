# Stereo V61.21 static progress

## Correction: VolCloudLeftEye
Static disassembly around WHGame+0x742413 shows the string `$VolCloudLeftEye` is passed to a resource-registration helper and the resulting handle is stored at owner+0xA0. Neighboring registrations are `$VolCloudMinTmp`, `$VolCloudDensityTmp`, and `$VolCloudShadowTmp` at +0x98/+0xA8/+0xB0. Therefore this anchor proves retained stereo-aware volumetric-cloud resources, but **does not prove a scene-eye selector**.

## CSystem gate
WHGame+0x1A736D0 is exactly `movzx eax, byte ptr [rcx+0x598]; ret`. Prior semantic/runtime work classified this as a generic CSystem state predicate with enormous non-stereo call volume. It remains retired as a stereo master-switch candidate.

## Retained stereo framework
`g_stereoFrameworkEnable` exists with help text `Enables the processing of the game stereo framework. (2=extra debug output)`, is registered with default 1, and prior runtime probes observed its backing storage as 1. The old renderer stereo parameter block (`r_StereoEyeDist`, strength, screen distance, HUD distance, etc.) is also live on graphics-pipeline dispatches.

## Pipeline differential
V36 captured two stable Normal CRenderViews alternating through `CStandardGraphicsPipeline`. Small pipeline-state fields at +0x818 and +0x828 also changed on alternating calls. This is useful correlation, but not enough to label those views left/right; they may be temporal/ping-pong state.

## Next static target
Trace actual reads/writes of +0x818/+0x828 and search their surrounding functions for projection/view setup. The success condition is one of:
1. a 0/1 eye index that controls projection/camera construction,
2. a same-tick two-pass scene dispatch with distinct projections,
3. an engine stereo-provider interface equivalent to UEVR's `IStereoRendering` hook point.
No runtime force/write should be added before that lineage is proven.
