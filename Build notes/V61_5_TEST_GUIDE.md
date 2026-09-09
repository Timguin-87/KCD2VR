# V61.5 GOOD MONO + PSEUDO STEREO

Purpose: return to the single-image pseudo-stereo branch after the retained CryEngine native stereo path proved inactive.

## Baseline
- MONO preserves the V61.4 LEGACY_GOOD_MONO path: same KCD2 source image in both eyes, normal per-eye OpenXR poses and asymmetric FOVs.
- MONO bypasses cyclopean/common-center processing and pseudo UV shifts.
- Native stereo CVar ladder, hardware watchpoints, AFR and DUAL diagnostics are not part of this test.
- TP/KCD2Tools FOV/cull widening is forced off even if an older INI still contains VrCullFrustumScale=1.25.

## F8 / Stereo Test button
Each press selects one preset and emits a Windows confirmation beep:
1. PS1 asymmetry 0.75
2. PS2 asymmetry 1.00
3. PS6 asymmetry 0.875
4. PS4 asymmetry 0.75 + UV shift 0.002
5. PS5 asymmetry 0.75 + UV shift 0.004
6. MONO (legacy-good hard gate)

The log marker is `V61.5-PSEUDO-SELECT`. There is no timed ladder; compare each mode at your own pace.

This is still single-image pseudo stereo, not geometric dual-render stereo.
