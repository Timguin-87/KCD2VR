# V46.1 — CryEngine CustomRes + FOV + Recursive View Matrix

## Why this exists

V46 proved the one-launch matrix architecture. V46.1 adds the two engine-side mono hypotheses that can be tested without inventing a second camera:

1. **CryEngine CustomRes** — render the scene at the HMD-like 13:14 aspect ratio internally while leaving the desktop backbuffer alone.
2. **Game FOV** — request a wide symmetric `cl_fov` and measure the actual FOV published in KCD2's `CCamera` (`float[12]`, +0x30) rather than assuming the CVar reached the camera.

It keeps the V45 type-1 Recursive View observer as the native-stereo candidate.

## Static WHGame.dll evidence used

This build does not type commands or require `autoexec.cfg`.

The exact runtime console interface was recovered from the current WHGame.dll:

- global `IConsole*`: `WHGame+0x492D8A8`
- `IConsole::GetCVar(const char*)`: vslot `+0xB8`
- `ICVar::GetIVal()`: vslot `+0x10`
- `ICVar::GetFVal()`: vslot `+0x20`
- `ICVar::Set(int)`: vslot `+0x38`
- `ICVar::Set(float)`: vslot `+0x40`

KCD2's own code at the `r_Width` / `r_Height` runtime update path obtains those CVars through `+0xB8` and calls `+0x38` with the stored width/height values. A separate renderer path calls `+0x40` with a float CVar value.

Custom-resolution registration is live in this build. Static xrefs show:

- `r_CustomResWidth`
- `r_CustomResHeight`
- `r_CustomResPreview`
- `r_CustomResMaxSize`

`r_CustomResMaxSize` registers with a default of 4096, so V46.1 deliberately stays below it.

## One F8 cycle: 10 × 5 seconds

1. `BASELINE_FIT`
2. `BASELINE_STRETCH`
3. `BASELINE_CROP`
4. `CUSTOMRES_1248x1344`
5. `CUSTOMRES_1248x1344_FOV110`
6. `CUSTOMRES_1872x2016_FOV110`
7. `CUSTOMRES_1872x2016_XR_FOV`
8. `EYE_ROUTE_CANONICAL`
9. `EYE_ROUTE_SWAPPED`
10. `RECURSIVE_VIEW_OBSERVER`

All stereo experiments remain disabled during the matrix. The headset receives mono until a real second renderer exists.

## What is measured

Each phase logs:

- requested and read-back `r_CustomResWidth/Height/Preview`
- current `r_CustomResMaxSize`
- requested/read-back `cl_fov`
- actual published `CCamera` FOV in radians/degrees
- desktop PRESENT resource dimensions
- exact-size D3D12 resources matching the requested CustomRes dimensions
- whether those exact-size resources enter/leave `D3D12_RESOURCE_STATE_RENDER_TARGET`
- OpenXR valid-frame snapshot count
- OpenXR-FOV compositor use
- canonical/final eye swap routing markers
- Recursive View request/pass/pipeline/render-target counters

## Important interpretation

`CUSTOMRES_RENDER_ACTIVE` is stronger than a CVar read-back. It means an exact-size D3D12 texture matching the requested CustomRes dimensions was observed participating in a render-target transition.

`CAMERA_FOV_CHANGED` in the completion summary means the published camera's +0x30 FOV moved materially from the baseline while the 110-degree phase was active. A `cl_fov=110` read-back alone is not counted as success.

`RECURSIVE_FULL_CHAIN` still means the engine-owned secondary-render candidate reached type-1 `CRenderView`, `CStandardGraphicsPipeline`, and a D3D12 render-target transition.

## Safety / restore

The F8 cycle snapshots the original CustomRes values and `cl_fov` before changing anything. After the 50-second sequence it restores those original engine values and returns the VR bridge to mono Fit, swap off.

No `CRenderView` is manually constructed. No dangerous `0x4E6924` / `0x4E7E64` call is made. No `0x653940` ABI is touched.
