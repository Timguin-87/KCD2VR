# V59.0 Build Note

## Purpose
Calibrate the unexpected V58.9 phase-D stereoscopic percept by varying only the submitted OpenXR eye-FOV asymmetry around a common cyclopean camera origin.

## Implementation
`XrManager::SetMonoFovAsymmetryScale(float)` controls interpolation around the two-eye mean FOV:

`eyeFov = commonFov + scale * (runtimeEyeFov - commonFov)`

- `+1`: original runtime eye asymmetry (V58.9 phase D)
- `0`: centered/common FOV
- negative: reverses the binocular asymmetry polarity

No additional renderer re-entry, native eye writes, or skeleton writes are enabled.

## Important interpretation
A positive headset report of depth in this mode proves a binocular disparity generated at projection submission, not independent scene parallax. Do not label it native stereo until distinct L/R scene resources exist.
