# KCD2VR V39 — Alternate-Frame Stereo causality build

V37 proved KCD2's final D3D12 backbuffer can be presented through OpenXR. V39 keeps that path and adds the first active binocular render experiment.

## What V39 does

- `VrStereoExperiment=0`: proven V38/V37 mono bridge.
- `VrStereoExperiment=1`: alternate-frame stereo (AFR).
- The camera frustum hook adds an eye-local X offset of `-IPD/2` for left frames and `+IPD/2` for right frames, using OpenXR's live IPD and `VrAfrIpdScale`.
- Each game backbuffer that reaches PRESENT is tagged L or R.
- The completed backbuffer is copied into a persistent D3D12 cache for that eye.
- OpenXR receives the latest cached L and R frames together.
- If AFR fails, V39 falls back to the known-good mono bridge, then to diagnostic clear.

This is intentionally a causality experiment. It does not yet inject OpenXR asymmetric per-eye projection, and the two eyes are one game frame apart in time. The success criterion is visible binocular disparity/depth, not final image quality.

## Recommended first test

```ini
[Settings]
DiagnosticRendering = false
EnableVrOutputBridge = true
VrStereoExperiment = 1
VrAfrIpdScale = 1.0
VrRenderScale = 1.0
VrPresentationMode = 2
VrPresentationZoom = 1.0
```

If depth is too strong/weak, try `VrAfrIpdScale=0.5` or `1.5`. Set `VrStereoExperiment=0` to return instantly to the V38 mono path.

## Important markers

- `V39-STEREO config`
- `V39-AFR-PRESENT serial=... eye=L/R`
- `V39-AFR cache-created`
- `V39-AFR-BLIT ... cacheValid=11`
- `V39-AFR stereo blit failed; falling back to V38 mono bridge`

Once `cacheValid=11`, both independent eye caches have been populated.

## Limitations / next step

AFR is temporally offset and still uses KCD2's desktop projection. If it proves real depth, the next step is to locate/inject the consumed projection fields with the actual `XrFovf` for each eye, then move from AFR to same-frame dual rendering.
