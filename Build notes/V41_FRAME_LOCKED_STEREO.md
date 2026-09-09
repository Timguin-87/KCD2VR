# V42 - Frame-Locked Stereo Pivot

V42 retires D3D12 PRESENT as the source of left/right eye identity.

## Why
V39/V40.2 showed multiple PRESENT transitions can occur inside one game/render frame, and the same backbuffer resource can be tagged L then R. That makes PRESENT-driven AFR eye identity unreliable and makes IPD tuning meaningless.

## New frame epoch
V42 hooks the already-mapped unique ViewCamera publication point:

- WHGame+0x7F0AE0 (`CView -> CSystem ViewCamera` publish)
- ABI: `void(__fastcall*)(void* system, void* camera)`
- one frame serial / eye assignment is established there
- all subsequent PRESENT transitions inherit the current frame eye
- PRESENT never advances the eye

Markers:
- `V42-FRAME-SYNC armed`
- `V42-FRAME-PUBLISH serial=... eye=L/R`
- `V42-PRESENT-TAG serial=... frame=... eye=L/R`

This is still a sequential/AFR proof mode, not final native stereo. It is now tied to the camera/render epoch rather than presentation noise.

## Native renderer correlation
A second lean hook observes `CStandardGraphicsPipeline::Execute` at WHGame+0x57278C and correlates each pipeline dispatch with the V42 frame epoch, CRenderView pointer, selector/mode, record count, eye, and TID.

Marker:
- `V42-NATIVE-DISPATCH ...`

This replaces broad V29-V36 runtime archaeology for the next native-stereo decision. It does not re-enter the renderer and does not claim native stereo yet.

## Modes
- 0 Mono bridge
- 1 FrameLocked-AFR (new V42 path)
- 2 RendererSequential-PROBE (mono compositor; pipeline correlation remains active)
- 3 NativeStereo-PROBE (mono compositor; pipeline correlation remains active)

Insert cycles modes. Home toggles final compositor eye swap.

## Defaults
- VrStereoExperiment=1
- VrAfrIpdScale=1.0
- VrSwapEyes=false
- VrPresentationMode=1 (Fit, never Crop by default)
- VrPresentationZoom=1.0
- EnableLegacyStereoProbes=false

Negative `VrAfrIpdScale` values down to -3 are now accepted as a diagnostic sign inversion.

## Engine.pak findings retained for later native compatibility
Engine.pak confirms engine-supported CVars including:
- `r_width`, `r_height`
- `r_SuperResolution_Mode`
- `r_SuperResolution_AMD_FSR_QualityMode`
- `r_SuperResolution_AMD_FSR_CustomResolutionScaleWH`
- `e_SkipInFrustumOptimizationWH=1` in `Config/CVarOverrides/Cutscene.cfg`

These are documented only; V42 does not force them.
