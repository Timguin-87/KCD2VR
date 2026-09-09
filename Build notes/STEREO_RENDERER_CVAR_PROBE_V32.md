# V32 — DLL-guided renderer stereo CVar probe

V31 proved `g_stereoFrameworkEnable` is registered at runtime with default 1 and its backing storage becomes 1. V32 therefore does not write that gate.

Static `WHGame.dll` analysis locates the renderer stereo registration block around `WHGame+0x10178xx..0x10179xx`. V32 hooks both CryEngine CVar registration helpers and captures live backing storage plus returned CVar objects for:

- `r_stereoScaleCoefficient`
- `r_StereoStrength`
- `r_StereoEyeDist`
- `r_StereoScreenDist`
- `r_StereoNearGeoScale`
- `r_StereoHudScreenDist`
- `r_StereoGammaAdjustment`
- `r_StereoEnableMgpu`
- `r_StereoFlipEyes`

It also samples captured values from the live graphics-pipeline thread. This is read-only: no CVar, camera, projection, render-target, or GPU writes.

Expected markers:

- `STEREO-RENDERER-V32 float-enter/float-leave`
- `STEREO-RENDERER-V32 int-enter/int-leave`
- `STEREO-RENDERER-V32 live`
- `STEREO-RENDERER-V32 summary`

The returned CVar object vtable and qwords are logged so a subsequent probe can identify the CVar implementation and reader path if the values themselves do not expose the missing stereo mode/provider gate.
